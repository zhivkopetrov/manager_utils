#ifndef MANAGER_UTILS_SOUNDMGR_H_
#define MANAGER_UTILS_SOUNDMGR_H_

// System headers
#include <cstdint>

// Other libraries headers
#include "resource_utils/defines/SoundDefines.h"

// Own components headers
#include "manager_utils/managers/MgrBase.h"

// Forward declarations
class InputEvent;
class SoundWidgetEndCb;
typedef struct _Mix_Music Mix_Music;

class SoundMgr final : public MgrBase {
 public:
  SoundMgr();
  virtual ~SoundMgr() noexcept;

  //================= START MgrBase related functions ====================

  /** @brief used to initialize the current manager.
   *         NOTE: this is the first function that will be called.
   *
   *  @return ErrorCode - error code
   * */
  ErrorCode init() override;

  /** @brief used to recover the current manager.
   *         NOTE: this function that will be called if init() passed
   *                                                       successfully.
   *         NOTE2: recover() function will be called after the
   *                successful pass of init() function, even if system
   *                was shutdown correctly.
   *
   *  @return ErrorCode - error code
   * */
  ErrorCode recover() override;

  /** @brief used to deinitialize the current manager.
   * */
  void deinit() override;

  /** @brief used to process the current manager (poll him on every
   *         engine cycle so the managers can do any internal updates, if
   *                                                     such are needed).
   * */
  void process() override;

  /** @brief captures user inputs (if any)
   *
   *  @param const InputEvent & - user input event
   * */
  void handleEvent(const InputEvent& e) override;

  /** @brief returns the name of the current manager
   *
   *  @return const char * - current manager name
   * */
  const char* getName() override;

  //================== END MgrBase related functions =====================

  /** @brief used to get the global sound level for the whole system
   *
   *  @return SoundLevel - the system sound level
   * */
  SoundLevel getGlobalVolumeLevel() const { return _systemSoundLevel; }

  //=================== START Music related functions ====================

  /** @brief used to load music sound from rsrcId so it can
   *                                                 later on be played
   *
   *         NOTE: there could be only 1 loaded/played music at a time
   *
   *  @param const uint64_t     - specific resource ID
   *
   *  @return ErrorCode         - error code
   * */
  ErrorCode loadMusic(const uint64_t rsrcId);

  /** @brief used to unload a currently loaded music resource if the
   *         provided music ID is the same as the currently loaded one.
   *
   *         NOTE: This function is invoked from ~Music() destructor in
   *               order to automatically stop the playing music and
   *               unload it.
   *               For example: we have a opened game with music playing
   *               in the background. The user request game exit.
   *               The game::deinit() is called and the music
   *                                                     must be stopped.
   *
   *  @param const uint64_t   - specific resource ID
   *  @param const SoundLevel - current sound level
   * */
  void trySelfUnloadMusic(const uint64_t rsrcId, const SoundLevel soundLevel);

  /** @brief used to set currently loaded music volume
   *         NOTE: this function does not return error code
   *                                              for performance reasons
   *
   *  @param const SoundLevel - input sound level
   * */
  void setMusicVolume(const SoundLevel soundLevel);

  /** @brief used to acquire currently set music volume
   *
   *  @return SoundLevel - currently loaded music sound level
   * */
  SoundLevel getMusicVolume() const;

  /** @brief used to play currently loaded music
   *         NOTE: this function does not return error code
   *                                              for performance reasons
   *
   *  @param const int32_t      - number of repeats (-1 for
   *                                              endless loop /~65000/ )
   *  @param SoundWidgetEndCb * - user defined sound end callback
   * */
  void playLoadedMusic(const int32_t loops, SoundWidgetEndCb* endCb);

  /** @brief used to stop currently loaded music
   * */
  void stopLoadedMusic();

  /** @brief used to determine whether the music is currently loaded
   *
   *  @return bool - music is loaded or not
   * */
  bool isMusicLoaded() const { return nullptr != _music; }

  /** @brief used to determine whether the music is currently playing
   *
   *  @return bool - music is currently playing or not
   * */
  bool isMusicPlaying() const;

  /** @brief used to determine whether the music is currently paused
   *
   *  @return bool - music is paused playing or not
   * */
  bool isMusicPaused() const;

  /** @brief used to pause the currently loaded music
   * */
  void pauseMusic();

  /** @brief used to resume(from pause) the currently loaded music
   * */
  void resumeMusic();

  /** @brief used to rewind(start from begging) the currently loaded music
   *
   *         NOTE: music must first be paused.
   * */
  void rewindMusic();

  //==================== END Music related functions =====================

  //=================== START Chunk related functions ====================

  /** @brief used to set music volume of a specific sound chunk
   *         NOTE: this function does not return error code
   *                                              for performance reasons
   *
   *  @param const uint64_t   - unique resource ID
   *  @param const SoundLevel - input sound level
   * */
  void setChunkVolume(const uint64_t rsrcId, const SoundLevel soundLevel);

  /** @brief used to acquire currently set sound chunk volume
   *         NOTE: this function does not return error code
   *                                              for performance reasons
   *
   *  @param const uint64_t - unique resource ID
   *
   *  @return SoundLevel    - currently loaded music sound level
   * */
  SoundLevel getChunkVolume(const uint64_t chunkId) const;

  /** @brief used to play a specific sound chunk
   *         NOTE: this function does not return error code
   *                                              for performance reasons
   *
   *  @param const uint64_t     - unique resource ID
   *  @param const int32_t      - number of repeats (-1 for
   *                                              endless loop /~65000/ )
   *  @param SoundWidgetEndCb * - user defined sound end callback
   * */
  void playChunk(const uint64_t rsrcId, const int32_t loops,
                 SoundWidgetEndCb* endCb);

  /** @brief used to play a specific sound chunk with panning on for
   *         it's associated channel. The left volume and right volume
   *         are specified as integers between 0 and 255,
   *                                  quietest to loudest, respectively.
   *
   *         NOTE: for real panning effect consider using
   *               playChunkWithPanning(rsrcId, loops, left, 255 - left);
   *
   *         NOTE2: this function does not return error code
   *                                              for performance reasons
   *
   *  @param const uint64_t     - unique resource ID
   *  @param const int32_       - number of repeats (-1 for
   *                                              endless loop /~65000/ )
   *  @param const uint8_t      - left volume value
   *  @param const uint8_t      - right volume value
   *  @param SoundWidgetEndCb * - user defined sound end callback
   * */
  void playChunkWithPanning(const uint64_t rsrcId, const int32_t loops,
                            const uint8_t leftVolume, const uint8_t rightVolume,
                            SoundWidgetEndCb* endCb);

  /** @brief used to stop a specific sound chunk from playing
   *         NOTE: this function does not return error code
   *                                              for performance reasons
   *
   *  @param const uint64_t - unique resource ID
   * */
  void stopChunk(const uint64_t rsrcId);

  /** @brief used to restore a loaded sound chunk internal data when it's
   *                                                about to be destroyed.
   *
   *         NOTE: This function is invoked from ~Sound() destructor in
   *               order to automatically stop the playing sound and
   *               restore it's original internal data.
   *               For example: we have a opened game with sound playing
   *               in some moment. The user request game exit.
   *               The game::deinit() is called and the sound chunk
   *                                                     must be stopped.
   *
   *         NOTE2: this function does not return error code
   *                                              for performance reasons
   *
   *  @param const uint64_t   - specific resource ID
   *  @param const SoundLEvel - current sound level
   * */
  void trySelfStopChunk(const uint64_t rsrcId, const SoundLevel soundLevel);

  /** @brief used to determine whether a sound chunk is currently playing
   *         NOTE: this function does not return error code
   *                                              for performance reasons
   *
   *  @param const uint64_t - unique resource ID
   *
   *  @return bool - sound chunk is currently playing or not
   * */
  bool isChunkPlaying(const uint64_t rsrcId) const;

  /** @brief used to determine whether a sound chunk is currently paused
   *         NOTE: this function does not return error code
   *                                              for performance reasons
   *
   *  @param const uint64_t - unique resource ID
   *
   *  @return bool - sound chunk is currently paused or not
   * */
  bool isChunkPaused(const uint64_t rsrcId) const;

  /** @brief used to pause a sound chunk
   *         NOTE: this function does not return error code
   *                                              for performance reasons
   *
   *  @param const uint64_t - unique resource ID
   * */
  void pauseChunk(const uint64_t rsrcId);

  /** @brief used to resume a sound chunk(that was previously paused)
   *         NOTE: this function does not return error code
   *                                              for performance reasons
   *
   *  @param const uint64_t - unique resource ID
   * */
  void resumeChunk(const uint64_t rsrcId);
  //==================== END Chunk related functions =====================

 protected:
  /** @brief used to set global volume for the whole system
   *         NOTE: internally this method changed the sound for all
   *                                          SUPPORTED_SOUND_CHANNELS.
   * */
  void setGlobalVolume(const SoundLevel soundLevel);

  /** @brief used to set global volume for the whole system
   *         NOTE: internally this method changed the sound for all
   *                                          SUPPORTED_SOUND_CHANNELS.
   *
   *         NOTE2: if _systemSoundLevel == SoundLevel::VERY_HIGH ->
   *                   _systemSoundLevel is set to SoundLevel::NONE
   * */
  void increaseGlobalVolume();

  /** @brief used to increase the hardware sound levels (direct OS call)
   *
   *  @param const int32_t - requested sound level (in percent)
   *
   *         NOTE: method only accepts value in range [0, 100] inclusive
   * */
  void changeOSVolume(const int32_t soundLevel);

 private:
  //================== START Channel related functions ===================

  /** @brief used to set the panning of a channel. The left volume and
   *         right volume are specified as integers between 0 and 255,
   *                                    quietest to loudest, respectively.
   *
   *         NOTE: for real panning effect consider using
   *                      setChannelPanning(channel, left, 255 - left);
   *
   *  @param const int32_t - specific channel Id
   *  @param const uint8_t - left volume value
   *  @param const uint8_t - right volume value
   *
   *  @return ErrorCode    - error code
   * */
  ErrorCode setChannelPanning(const int32_t channel, const uint8_t leftVolume,
                              const uint8_t rightVolume);

  /** @brief used to reset the panning of a channel.
   *
   *  @param const int32_t - specific channel Id
   *
   *  @return ErrorCode    - error code
   * */
  ErrorCode resetChannelPanning(const int32_t channel);

  /** @brief used to determine whether a sound channel is
   *                                                    currently playing
   *
   *  @param const int32_t - specific channel ID
   *
   *  @return bool - sound channel is currently playing or not
   * */
  bool isChannelPlaying(const int32_t channel) const;

  /** @brief used to determine whether a sound channel is
   *                                                    currently paused
   *
   *  @param const int32_t - specific channel ID
   *
   *  @return bool - sound channel is currently paused or not
   * */
  bool isChannelPaused(const int32_t channel) const;

  /** @brief used to pause a sound channel
   *
   *  @param const int32_t - specific channel ID
   * */
  void pauseChannel(const int32_t channel);

  /** @brief used to resume a sound channel(that was previously paused)
   *
   *  @param const int32_t - specific channel ID
   * */
  void resumeChannel(const int32_t channel);

  /** @brief used to determine the associated sound chunk
   *                      unique resource ID bound to the sound channel
   *
   *  @param const uint64_t - specific channel ID
   *
   *  @return int32_t       - specific channel ID (-1 for INVALID_CHANNEL)
   * */
  int32_t findAssociatedChannel(const uint64_t rsrcId) const;

  /** @brief used to acquire a free sound channel
   *
   *  @return int32_t - specific channel ID (-1 for INVALID_CHANNEL)
   *
   *          NOTE: the channelId 0 is reserved for Music
   * */
  int32_t getNextFreeChannel() const;

  /** @brief used to reset channel (if it was occupied)
   *         NOTE: if there is SoundEndHandler attached to this channel -
   *               the endHandler is first invoked
   *               if there is panning set on this channel - the panning
   *               is reset
   *
   *  @param const int32_t specific channel ID
   * */
  void resetChannel(const int32_t channel);

  //=================== END Channel related functions ====================

  /** @brief a callback for when a sound (music or chunk) is
   *                          finished playing on it's associated channel
   *
   *  @param const int32_t - channel that has finished playing
   * */
  static void onChannelFinished(const int32_t channel);

  enum InternalDefines {
    MUSIC_RESERVED_CHANNEL_ID = 0,
    INVALID_CHANNEL_ID = -1,

    /* Holds maximum supported sound channels (number of sounds that
     *                                    can be played simultaneously)
     * */
    SUPPORTED_SOUND_CHANNELS = 20
  };

  /* Used to mark current sound channel that has requested panning in
   * order to reset the panning, when the associated sound behind the
   * channel has finished playing.
   * */
  int32_t* _panningMap;

  /** Used to map rsrcId to with his currently used
   *                                       by the SDL_Mixer audio channel
   * */
  uint64_t* _usedChannels;

  /** Used to map SoundWidgetEndCb with his currently used
   *                                       by the SDL_Mixer audio channel
   * */
  SoundWidgetEndCb** _usedChannelsEndCb;

  /* Holds the loaded music(if such).
   * Note: there can be only 1 music loaded and playing simultaneously
   * */
  Mix_Music* _music;

  /* Holds the unique resource ID bound to the loaded Mix_Music */
  uint64_t _loadedMusicRsrcId;

  /* Holds the global system level of sound */
  SoundLevel _systemSoundLevel;
};

extern SoundMgr* gSoundMgr;

#endif /* MANAGER_UTILS_SOUNDMGR_H_ */
