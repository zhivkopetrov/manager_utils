#ifndef MANAGER_UTILS_MUSIC_H_
#define MANAGER_UTILS_MUSIC_H_

/*
 * Music.h
 *
 *  @brief A class for playing sounds.
 *
 *         Definitions:
 *              > Music:
 *                       - Only 1 music can be played/paused/rewind at a time;
 *                       - Music is usually a bigger piece of sound;
 *                       - Music is not loaded into memory - it is
 *                                                         constantly buffered;
 */

// System headers
#include <cstdint>

// Other libraries headers

// Own components headers
#include "manager_utils/sound/SoundWidget.h"

// Forward declarations

class Music : public SoundWidget {
 public:
  Music() = default;
  virtual ~Music() noexcept;

  Music(Music&& movedOther);
  Music& operator=(Music&& movedOther);

  //================ START SoundWidget related functions =================

  virtual void destroy() override;

  /** @brief used to set sound volume for the Music
   *         NOTE: this function does not return error code
   *                                              for performance reasons
   *
   *  @param const SoundLevel - input sound level
   * */
  virtual void setVolume(const SoundLevel soundLevel) override;

  /** @brief used to play sound under the Music
   *
   *  @param const int32_t - number of repeats (-1 for
   *                                              endless loop /~65000/ )
   * */
  virtual void play(const int32_t loops = 0) override;

  /** @brief used to stop the Music if it's playing.
   * */
  virtual void stop() override;

  /** @brief used to determine whether the Music is playing
   *
   *  @return bool - is Music playing or not
   * */
  virtual bool isPlaying() const override;

  /** @brief used to determine whether the Music is paused
   *
   *  @return bool - is Music paused or not
   * */
  virtual bool isPaused() const override;

  /** @brief used to pause a Music
   * */
  virtual void pause() override;

  /** @brief used to resume a Music(that was previously paused)
   * */
  virtual void resume() override;

  //================= END SoundWidget related functions ==================

  /** @brief used to load music to the global gSoundMgr
   *
   *         REMINDER: Only 1 music can be loaded/played at a time.
   * */
  void loadMusic();

  /** @brief used to unload music from the global gSoundMgr
   *
   *         REMINDER: Only 1 music can be loaded/played at a time.
   * */
  void unloadMusic();

  /** @brief used to rewind(start from begging) the Music
   *
   *         NOTE: Music must first be paused.
   * */
  void rewind();
};

#endif /* MANAGER_UTILS_MUSIC_H_ */
