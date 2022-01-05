// Corresponding header
#include "manager_utils/sound/Sound.h"

// C system headers

// C++ system headers
#include <utility>

// Other libraries headers

// Own components headers
#include "manager_utils/managers_base/SoundMgrBase.h"
#include "utils/Log.h"

// move constructor
Sound::Sound(Sound&& movedOther) : SoundWidget(std::move(movedOther)) {}

// move assignment operator implementation
Sound& Sound::operator=(Sound&& movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    // implicitly invoke SoundWidget move assignment operator
    SoundWidget::operator=(std::move(movedOther));
  }

  return *this;
}

Sound::~Sound() {
  // attempt to destroy SoundWidget only if it
  // was first created and not destroyed
  if (true == _isCreated && false == _isDestroyed) {
    destroy();
  }
}

void Sound::destroy() {
  if (_isDestroyed) {
    LOGERR(
        "Warning, trying to destroy already destroyed Sound with "
        "_rsrcId: %#16lX",
        _rsrcId);
    return;
  }

  // sanity check
  if (nullptr != gSoundMgrBase) {
    // check for auto-closing of music
    gSoundMgrBase->trySelfStopChunk(_rsrcId, _soundLevel);
  }

  // invoke base destroy
  SoundWidget::destroy();
}

void Sound::setVolume(const SoundLevel soundLevel) {
  if (SoundLevel::UNKNOWN == soundLevel) {
    LOGERR(
        "Error, UNKNOWN soundLevel value detected. "
        "Will not change volume value.");
    return;
  } else  // it is valid SoundLevel value
  {
    _soundLevel = soundLevel;
    gSoundMgrBase->setChunkVolume(_rsrcId, _soundLevel);
  }
}

void Sound::play(const int32_t loops) {
  gSoundMgrBase->playChunk(_rsrcId, loops, _endCb);
}

void Sound::playWithPanning(const int32_t loops, const int32_t leftVolume,
                            const int32_t rightVolume) {
  constexpr int32_t UINT8_T_MAX = 255;
  if (0 > leftVolume || UINT8_T_MAX < leftVolume) {
    LOGERR(
        "Invalid leftVolume param: %d provided. "
        "Volume must be in range 0 - 255(inclusive)",
        leftVolume);

    return;
  }

  if (0 > rightVolume || UINT8_T_MAX < rightVolume) {
    LOGERR(
        "Invalid rightVolume param: %d provided. "
        "Volume must be in range 0 - 255(inclusive)",
        rightVolume);

    return;
  }

  gSoundMgrBase->playChunkWithPanning(
      _rsrcId, loops, static_cast<uint8_t>(leftVolume),
      static_cast<uint8_t>(rightVolume), _endCb);
}

void Sound::stop() { gSoundMgrBase->stopChunk(_rsrcId); }

bool Sound::isPlaying() const { return gSoundMgrBase->isChunkPlaying(_rsrcId); }

bool Sound::isPaused() const { return gSoundMgrBase->isChunkPaused(_rsrcId); }

void Sound::pause() { gSoundMgrBase->pauseChunk(_rsrcId); }

void Sound::resume() { gSoundMgrBase->resumeChunk(_rsrcId); }
