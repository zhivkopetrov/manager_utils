// Corresponding header
#include "manager_utils/managers/SoundMgr.h"

// C system headers

// C++ system headers
#include <cstring>
#include <vector>
#include <mutex>

// Other libraries headers

// Own components headers
#include "manager_utils/managers/RsrcMgr.h"
#include "manager_utils/sound/SoundWidgetEndCb.hpp"
#include "sdl_utils/input/InputEvent.h"
#include "sdl_utils/sound/SoundMixer.h"
#include "utils/data_type/EnumClassUtils.hpp"
#include "utils/debug/FunctionTracer.hpp"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

SoundMgr* gSoundMgr = nullptr;

static std::vector<int32_t> finishedChannels;
static std::mutex gSoundMutex;

SoundMgr::SoundMgr()
    : _panningMap(nullptr),
      _usedChannels(nullptr),
      _usedChannelsEndCb(nullptr),
      _music(nullptr),
      _loadedMusicRsrcId(0),
      _systemSoundLevel(SoundLevel::NONE) {}

SoundMgr::~SoundMgr() {
  TRACE_ENTRY_EXIT;

  deinit();
}

int32_t SoundMgr::init() {
  TRACE_ENTRY_EXIT;

  if (SUCCESS !=
      SoundMixer::allocateSoundChannels(SUPPORTED_SOUND_CHANNELS)) {
    LOGERR("Error in allocateSoundChannels() for requestedChannels: %d",
           SUPPORTED_SOUND_CHANNELS);
    return FAILURE;
  }

  if (SUCCESS !=
      SoundMixer::setCallbackOnChannelFinish(onChannelFinished)) {
    LOGERR("Error in setCallbackOnChannelFinish()");
    return FAILURE;
  }

  _panningMap = new int32_t[SUPPORTED_SOUND_CHANNELS];

  if (nullptr == _panningMap) {
    LOGERR("Error, bad alloc for _panningMap -> Terminating ...");
    return FAILURE;
  }

  // initialize all values to 0
  memset(_panningMap, 0, sizeof(int32_t) * SUPPORTED_SOUND_CHANNELS);

  _usedChannels = new uint64_t[SUPPORTED_SOUND_CHANNELS];

  if (nullptr == _usedChannels) {
    LOGERR("Error, bad alloc for _usedChannels -> Terminating ...");
    return FAILURE;
  }

  // initialize all values to 0
  memset(_usedChannels, 0, sizeof(uint64_t) * SUPPORTED_SOUND_CHANNELS);

  _usedChannelsEndCb = new SoundWidgetEndCb*[SUPPORTED_SOUND_CHANNELS];

  if (nullptr == _usedChannelsEndCb) {
    LOGERR("Error, bad alloc for _usedChannelsEndCb -> Terminating...");
    return FAILURE;
  }

  for (int32_t i = 0; i < SUPPORTED_SOUND_CHANNELS; ++i) {
    _usedChannelsEndCb[i] = nullptr;
  }

  return SUCCESS;
}

int32_t SoundMgr::recover() { return SUCCESS; }

void SoundMgr::deinit() {
  TRACE_ENTRY_EXIT;

  for (int32_t i = 0; i < SUPPORTED_SOUND_CHANNELS; ++i) {
    _usedChannelsEndCb[i] = nullptr;
  }

  // stop Music if it's playing
  SoundMixer::stopMusic();

  // stop chunks if there are any playing
  SoundMixer::stopAllChannels();

  /** NOTE: the memory freeing should occur last, because
   *        some function use the _panningMap and _usedChannels
   * */

  // sanity check
  if (nullptr != _panningMap) {
    delete[] _panningMap;
    _panningMap = nullptr;
  }

  // sanity check
  if (nullptr != _usedChannels) {
    delete[] _usedChannels;
    _usedChannels = nullptr;
  }

  // sanity check
  if (nullptr != _usedChannelsEndCb) {
    delete[] _usedChannelsEndCb;
    _usedChannelsEndCb = nullptr;
  }
}

const char* SoundMgr::getName() { return "SoundMgr"; }

void SoundMgr::process() {
  std::vector<int> finishedChannelsCopy;

  gSoundMutex.lock();
  std::swap(finishedChannelsCopy, finishedChannels);
  gSoundMutex.unlock();

  for (const int32_t channel : finishedChannelsCopy) {
    resetChannel(channel);
  }
}

void SoundMgr::handleEvent([[maybe_unused]]const InputEvent& e) {
}

void SoundMgr::setGlobalVolume(const SoundLevel soundLevel) {
  _systemSoundLevel = soundLevel;

  SoundMixer::setAllChannelsVolume(getEnumValue(_systemSoundLevel));

  // check if music is already loaded
  if (nullptr != _music) {
    // since music is playing on a different dedicated channel ->
    // it's sound needs to be set manually
    SoundMixer::setMusicVolume(getEnumValue(_systemSoundLevel));
  }
}

void SoundMgr::increaseGlobalVolume() {
  ++_systemSoundLevel;

  SoundMixer::setAllChannelsVolume(getEnumValue(_systemSoundLevel));

  // check if music is already loaded
  if (nullptr != _music) {
    // since music is playing on a different dedicated channel ->
    // it's sound needs to be set manually
    SoundMixer::setMusicVolume(getEnumValue(_systemSoundLevel));
  }
}

void SoundMgr::changeOSVolume(const int32_t soundLevel) {
  if ((0 > soundLevel) || (100 < soundLevel)) {
    LOGERR(
        "Error, invalid soundLevel: %d provided! ::changeOSVolume() "
        "expects values in range [0, 100]",
        soundLevel);
  } else {
    std::string command("amixer -D pulse sset Master ");
    command.append(std::to_string(soundLevel)).append("%");

    FILE* fp = popen(command.c_str(), "w");

    if (!fp) {
      LOGERR("Error, pipe command %s could not be executed. Reason: %s",
             command.c_str(), strerror(errno));
    } else {
      if (SUCCESS != pclose(fp)) {
        LOGERR("Error, pclose() failed. Reason: %s", strerror(errno));
      }
    }
  }
}

int32_t SoundMgr::loadMusic(const uint64_t rsrcId) {
  // check if music is already loaded
  if (nullptr != _music) {
    LOGERR(
        "Music is already loaded. Two simultaneously loaded musics is"
        "forbidden. Unload the currently loaded music with "
        "SoundMgr::unloadMusic() and then load your music");
    return FAILURE;
  }

  _loadedMusicRsrcId = rsrcId;

  gRsrcMgr->getMusicSound(rsrcId, _music);

  if (nullptr == _music) {
    LOGERR("gRsrcMgr->getMusicSound() failed for rsrcId: %#16lX", rsrcId);
  } else {
    // music sound needs to be changed with system sound level on load,
    // because SoundMixer::setAllChannelsVolume() does not affect
    // audio channel associated with music, if such is not loaded
    SoundMixer::setMusicVolume(getEnumValue(_systemSoundLevel));
  }

  return SUCCESS;
}

void SoundMgr::trySelfUnloadMusic(const uint64_t rsrcId,
                                      const SoundLevel soundLevel) {
  // check if there is music loaded in the first place
  // second check if the music that requests closing is the same as the
  // currently playing one
  if (nullptr != _music && _loadedMusicRsrcId == rsrcId) {
    const SoundData* soundData = nullptr;

    if (SUCCESS != gRsrcMgr->getSoundData(rsrcId, soundData)) {
      LOGERR("gRsrcMgr->getSoundData() failed for rsrcId: %#16lX", rsrcId);
    } else {
      // only request change if value was change during run-time
      if (soundLevel != soundData->soundLevel) {
        setMusicVolume(soundData->soundLevel);
      }
    }

    // call the sound API to stop the music if it was playing
    if (SoundMixer::isMusicPlaying()) {
      const int32_t channelId = findAssociatedChannel(rsrcId);

      if (INVALID_CHANNEL_ID != channelId) {
        /** Since the Music is about to be destroyed by it's destructor
         * assure that it has no soundCallback attached, because
         * sounds callbacks are executed on the next engine process()
         * cycle when the widget will already be destroyed
         * */
        _usedChannelsEndCb[channelId] = nullptr;
      }

      SoundMixer::stopMusic();
    }

    // reset variables
    _music = nullptr;
    _loadedMusicRsrcId = 0;
  }
}

void SoundMgr::setMusicVolume(const SoundLevel soundLevel) {
  // check if music is loaded
  if (nullptr == _music) {
    LOGERR(
        "Music is not loaded. Consider using "
        "SoundMgr::loadMusic() first");
    return;
  }

  if (SoundLevel::UNKNOWN == soundLevel) {
    LOGERR(
        "Error, UNKNOWN soundLevel value detected. "
        "Will not change volume value.");
    return;
  }

  SoundMixer::setMusicVolume(getEnumValue(soundLevel));
}

SoundLevel SoundMgr::getMusicVolume() const {
  // check if music is loaded
  if (nullptr == _music) {
    LOGERR(
        "Music is not loaded. Consider using SoundMgr::loadMusic() "
        "first. Returning SoundLevel::UNKNOWN.");
    return SoundLevel::UNKNOWN;
  }

  return toEnum<SoundLevel>(SoundMixer::getMusicVolume());
}

void SoundMgr::playLoadedMusic(const int32_t loops,
                                   SoundWidgetEndCb* endCb) {
  // check if music is loaded
  if (nullptr == _music) {
    LOGERR(
        "Music is not loaded. Consider using SoundMgr::loadMusic() "
        "first.");
    return;
  }

  if (MUSIC_RESERVED_CHANNEL_ID == SoundMixer::playMusic(_music, loops)) {
    _usedChannels[MUSIC_RESERVED_CHANNEL_ID] = _loadedMusicRsrcId;
    _usedChannelsEndCb[MUSIC_RESERVED_CHANNEL_ID] = endCb;
  } else {
    LOGERR(
        "SDL_Mixer failed to play into the requested sound "
        "channelId: %d",
        MUSIC_RESERVED_CHANNEL_ID);
  }
}

void SoundMgr::stopLoadedMusic() {
  // check if music is loaded in the first place
  if (nullptr == _music) {
    return;
  }

  if (false == SoundMixer::isMusicPlaying()) {
    return;
  }

  // call the sound API to stop the music
  SoundMixer::stopMusic();

  const int32_t channelId = findAssociatedChannel(_loadedMusicRsrcId);

  if (INVALID_CHANNEL_ID != channelId) {
    // call music callback (if such is set)
    if (nullptr != _usedChannelsEndCb[channelId]) {
      _usedChannelsEndCb[channelId]->onSoundWidgetEnd();
      // do not reset the callback variable - it might be used again
    }
  }
}

bool SoundMgr::isMusicPlaying() const {
  return SoundMixer::isMusicPlaying();
}

bool SoundMgr::isMusicPaused() const { return SoundMixer::isMusicPaused(); }

void SoundMgr::pauseMusic() { SoundMixer::pauseMusic(); }

void SoundMgr::resumeMusic() { SoundMixer::resumeMusic(); }

void SoundMgr::rewindMusic() { SoundMixer::rewindMusic(); }

void SoundMgr::setChunkVolume(const uint64_t rsrcId,
                                  const SoundLevel soundLevel) {
  if (SoundLevel::UNKNOWN == soundLevel) {
    LOGERR(
        "Error, UNKNOWN soundLevel value detected. "
        "Will not change volume value.");
    return;
  }

  Mix_Chunk* chunk = nullptr;

  gRsrcMgr->getChunkSound(rsrcId, chunk);

  if (nullptr == chunk) {
    LOGERR("Error in gRsrcMgr->getChunkSound() for chunk: %#16lX", rsrcId);
  } else  // it is valid chunk
  {
    SoundMixer::setChunkVolume(chunk, getEnumValue(soundLevel));
  }
}

SoundLevel SoundMgr::getChunkVolume(const uint64_t rsrcId) const {
  Mix_Chunk* chunk = nullptr;

  gRsrcMgr->getChunkSound(rsrcId, chunk);

  if (nullptr == chunk) {
    LOGERR(
        "Error in getChunkSound() for rsrcId: %#16lX. "
        "Returning SoundLevel::UNKNOWN.",
        rsrcId);

    return SoundLevel::UNKNOWN;
  }

  return toEnum<SoundLevel>(SoundMixer::getChunkVolume(chunk));
}

void SoundMgr::playChunk(const uint64_t rsrcId, const int32_t loops,
                             SoundWidgetEndCb* endCb) {
  Mix_Chunk* chunk = nullptr;

  gRsrcMgr->getChunkSound(rsrcId, chunk);

  if (nullptr == chunk) {
    LOGERR("Error in getChunkSound() for rsrcId: %#16lX. "
           "Chunk could not be played!", rsrcId);
    return;
  }

  const int32_t channelId = getNextFreeChannel();
  if (INVALID_CHANNEL_ID == channelId) {
    LOGERR(
        "Error, maximum sound supported channels count %d is "
        "reached. Sound will with rsrcId: %#16lX will not be "
        "played. Increase the number of maximum sound supported "
        "channels _SUPPORTED_SOUND_CHANNELS",
        SUPPORTED_SOUND_CHANNELS, rsrcId);
    return;
  }

  if (channelId != SoundMixer::playChunk(chunk, channelId, loops)) {
    LOGERR("SDL_Mixer failed to play into the requested sound channelId: %d",
           channelId);
    return;
  }

  _usedChannels[channelId] = rsrcId;
  _usedChannelsEndCb[channelId] = endCb;
}

void SoundMgr::playChunkWithPanning(const uint64_t rsrcId,
                                        const int32_t loops,
                                        const uint8_t leftVolume,
                                        const uint8_t rightVolume,
                                        SoundWidgetEndCb* endCb) {
  Mix_Chunk* chunk = nullptr;

  gRsrcMgr->getChunkSound(rsrcId, chunk);
  if (nullptr == chunk) {
    LOGERR("Error in getChunkSound() for rsrcId: %#16lX. "
           "Chunk could not be played!", rsrcId);
    return;
  }

  const int32_t channelId = getNextFreeChannel();
  if (INVALID_CHANNEL_ID == channelId) {
    LOGERR(
        "Error, maximum sound supported channels count %d is "
        "reached. Sound will with rsrcId: %#16lX will not be "
        "played. Increase the number of maximum sound supported "
        "channels _SUPPORTED_SOUND_CHANNELS",
        SUPPORTED_SOUND_CHANNELS, rsrcId);
    return;
  }

  if (channelId != SoundMixer::playChunk(chunk, channelId, loops)) {
    LOGERR("SDL_Mixer failed to play into the requested sound "
           "channelId: %d", channelId);
    return;
  }

  _usedChannels[channelId] = rsrcId;
  _usedChannelsEndCb[channelId] = endCb;

  if (SUCCESS != setChannelPanning(channelId, leftVolume, rightVolume)) {
    LOGERR("Error in setChannelPanning() for channel: %d leftVolume: %hhu, "
           "rightVolume: %hhu", channelId, leftVolume, rightVolume);
  }
}

void SoundMgr::stopChunk(const uint64_t rsrcId) {
  const int32_t channelId = findAssociatedChannel(rsrcId);

  if (INVALID_CHANNEL_ID == channelId) {
    LOGERR("Error, rsrcId: %#16lX is not associated with a valid sound "
           "channel! SoundMgr::stopChunk() will not take effect",
        rsrcId);
    return;
  }

  // call the sound API to stop channel
  SoundMixer::stopChannel(channelId);
}

void SoundMgr::trySelfStopChunk(const uint64_t rsrcId,
                                    const SoundLevel soundLevel) {
  const int32_t channelId = findAssociatedChannel(rsrcId);
  if (INVALID_CHANNEL_ID == channelId) {
    return;
  }

  const SoundData* soundData = nullptr;
  if (SUCCESS != gRsrcMgr->getSoundData(rsrcId, soundData)) {
    LOGERR("gRsrcMgr->getSoundData() failed for rsrcId: %#16lX", rsrcId);
    return;
  }

  // only request change if value was change during run-time
  if (soundLevel != soundData->soundLevel) {
    Mix_Chunk* chunk = nullptr;

    gRsrcMgr->getChunkSound(rsrcId, chunk);

    if (nullptr == chunk) {
      LOGERR("Error in getChunkSound() for rsrcId: %#16lX", rsrcId);
    } else {
      // reset sound to it's original value
      SoundMixer::setChunkVolume(chunk, getEnumValue(soundData->soundLevel));
    }
  }

  /** Since the widget is about to be destroyed by it's destructor
   * assure that it has no soundCallback attached, because
   * sounds callbacks are executed on the next engine process() cycle
   * when the widget will already be destroyed
   * */
  _usedChannelsEndCb[channelId] = nullptr;

  // call the sound API to stop the channel
  SoundMixer::stopChannel(channelId);
}

bool SoundMgr::isChunkPlaying(const uint64_t rsrcId) const {
  const int32_t channelId = findAssociatedChannel(rsrcId);

  // not associated channel found -> chunk is not playing
  if (INVALID_CHANNEL_ID == channelId) {
    return false;
  }

  return isChannelPlaying(channelId);
}

bool SoundMgr::isChunkPaused(const uint64_t rsrcId) const {
  const int32_t channelId = findAssociatedChannel(rsrcId);

  if (INVALID_CHANNEL_ID == channelId) {
    LOGERR(
        "Error, rsrcId: %#16lX is not associated with a valid sound "
        "channel! SoundMgr::isChunkPaused() will not take effect. "
        "Returning false",
        rsrcId);

    return false;
  }

  return isChannelPaused(channelId);
}

void SoundMgr::pauseChunk(const uint64_t rsrcId) {
  const int32_t channelId = findAssociatedChannel(rsrcId);

  if (INVALID_CHANNEL_ID == channelId) {
    LOGERR(
        "Error, rsrcId: %#16lX is not associated with a valid sound "
        "channel! SoundMgr::pauseChunk() will not take effect.",
        rsrcId);

    return;
  }

  pauseChannel(channelId);
}

void SoundMgr::resumeChunk(const uint64_t rsrcId) {
  const int32_t channelId = findAssociatedChannel(rsrcId);

  if (INVALID_CHANNEL_ID == channelId) {
    LOGERR(
        "Error, rsrcId: %#16lX is not associated with a valid sound "
        "channel! SoundMgr::resumeChunk() will not take effect",
        rsrcId);

    return;
  }

  resumeChannel(channelId);
}

bool SoundMgr::isChannelPlaying(const int32_t channel) const {
  if (0 > channel || SUPPORTED_SOUND_CHANNELS <= channel) {
    LOGERR(
        "Warning, invalid channel provided: %d. Max number of"
        " supported sound channels currently supported: %d",
        channel, SUPPORTED_SOUND_CHANNELS);

    return false;
  }

  return SoundMixer::isChannelPlaying(channel);
}

bool SoundMgr::isChannelPaused(const int32_t channel) const {
  if (0 > channel || SUPPORTED_SOUND_CHANNELS <= channel) {
    LOGERR(
        "Warning, invalid channel provided: %d. Max number of"
        " supported sound channels currently supported: %d",
        channel, SUPPORTED_SOUND_CHANNELS);

    return false;
  }

  return SoundMixer::isChannelPaused(channel);
}

void SoundMgr::resumeChannel(const int32_t channel) {
  if (0 > channel || SUPPORTED_SOUND_CHANNELS <= channel) {
    LOGERR(
        "Warning, invalid channel provided: %d. Max number of"
        " supported sound channels currently supported: %d",
        channel, SUPPORTED_SOUND_CHANNELS);

    return;
  }

  SoundMixer::resumeChannel(channel);
}

void SoundMgr::pauseChannel(const int32_t channel) {
  if (0 > channel || SUPPORTED_SOUND_CHANNELS <= channel) {
    LOGERR(
        "Warning, invalid channel provided: %d. Max number of"
        " supported sound channels currently supported: %d",
        channel, SUPPORTED_SOUND_CHANNELS);

    return;
  }

  SoundMixer::pauseChannel(channel);
}

int32_t SoundMgr::setChannelPanning(const int32_t channel,
                                        const uint8_t leftVolume,
                                        const uint8_t rightVolume) {
  int32_t err = SUCCESS;

  if (0 > channel || SUPPORTED_SOUND_CHANNELS <= channel) {
    LOGERR(
        "Warning, invalid channel provided: %d. Max number of"
        " supported sound channels currently supported: %d",
        channel, SUPPORTED_SOUND_CHANNELS);

    err = FAILURE;
  }

  if (SUCCESS == err) {
    if (SUCCESS !=
        SoundMixer::setChannelPanning(channel, leftVolume, rightVolume)) {
      LOGERR(
          "Error in setChannelPanning() for channel: %d leftVolume: "
          "%hhu, rightVolume: %hhu",
          channel, leftVolume, rightVolume);

      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    // mark that the current channel has requested panning, so later on
    // the panning could be automatically reseted
    _panningMap[channel] = 1;
  }

  return err;
}

int32_t SoundMgr::resetChannelPanning(const int32_t channel) {
  int32_t err = SUCCESS;

  if (0 > channel || SUPPORTED_SOUND_CHANNELS <= channel) {
    LOGERR(
        "Warning, invalid channel provided: %d. Max number of"
        " supported sound channels currently supported: %d",
        channel, SUPPORTED_SOUND_CHANNELS);
    err = FAILURE;
  }

  if (SUCCESS == err) {
    if (SUCCESS != SoundMixer::setChannelPanning(channel, 255, 255)) {
      LOGERR(
          "Error in setChannelPanning() for channel: %d, "
          "leftVolume: 255, rightVolume: 255",
          channel);

      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    // unmark the current channel as panned
    _panningMap[channel] = 0;
  }

  return err;
}

int32_t SoundMgr::findAssociatedChannel(const uint64_t rsrcId) const {
  // keep in mind that (if not used correctly by the developer) there
  // might be more than one associated channel for a provided rsrcId
  for (int32_t i = 0; i < SUPPORTED_SOUND_CHANNELS; ++i) {
    if (rsrcId == _usedChannels[i]) {
      // channel found, end the search
      return i;
    }
  }

  // return invalid Id if channel is not found
  return INVALID_CHANNEL_ID;
}

int32_t SoundMgr::getNextFreeChannel() const {
  int32_t channelId = INVALID_CHANNEL_ID;

  // NOTE: the channelId 0 is reserved for Music
  for (int32_t i = 1; i < SUPPORTED_SOUND_CHANNELS; ++i) {
    if (0 == _usedChannels[i]) {
      channelId = i;

      break;
    }
  }

  return channelId;
}

void SoundMgr::resetChannel(const int32_t channel) {
  // check if channel was using panning
  if (_panningMap[channel]) {
    // if so -> request panning reset
    if (SUCCESS != resetChannelPanning(channel)) {
      LOGERR("Error in resetChannelPanning for channel: %d", channel);
    }
  }

  // check if end handler was set
  if (nullptr != _usedChannelsEndCb[channel]) {
    // if so -> request execute the callback
    _usedChannelsEndCb[channel]->onSoundWidgetEnd();

    _usedChannelsEndCb[channel] = nullptr;
  }

  _usedChannels[channel] = 0;
}

void SoundMgr::onChannelFinished(const int32_t channel) {
  std::lock_guard<std::mutex> lock(gSoundMutex);
  finishedChannels.emplace_back(channel);
}
