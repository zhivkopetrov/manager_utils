#ifndef MANAGER_UTILS_SOUND_H_
#define MANAGER_UTILS_SOUND_H_

/*
 * Sound.h
 *
 *  @brief A class for playing sounds.
 *
 *         Definitions:
 *              > Chunks/Sounds:
 *                       - Big numbers of sounds can be played simultaneously
 *                                  (check SoundMgr::_SUPPORTED_SOUND_CHANNELS);
 *
 *                       - Chunks/Sounds are loaded into memory;
 *
 *         Important note:
 *             Multiple Sound instances of the same sound resource share
 *             a common sound resource.
 *             Applying business logic on such cases may lead to
 *             unwanted behavior.
 *             If you need many instances of the same sound resource, which
 *             requires constant business logic such as invoking of
 *             .setVolume()/ .isPlaying()/ .isPaused() it's better to copy
 *             the sound resource and bind each instance of the Sound class
 *             to it's own sound resource.
 */

// System headers
#include <cstdint>

// Other libraries headers

// Own components headers
#include "manager_utils/sound/SoundWidget.h"

class Sound : public SoundWidget {
 public:
  Sound() = default;
  virtual ~Sound() noexcept;

  Sound(Sound&& movedOther);
  Sound& operator=(Sound&& movedOther);

  //================ START SoundWidget related functions =================

  /** @brief used to destroy a Sound. In order to use the
   *                           Sound again - it needs to be re-created.
   *                                      (Invoking of .create() method)
   * */
  void destroy() override;

  /** @brief used to set sound volume for the Sound
   *         NOTE: this function does not return error code
   *                                              for performance reasons
   *
   *  @param const SoundLevel - input sound level
   * */
  void setVolume(const SoundLevel soundLevel) override;

  /** @brief used to play sound under the Sound
   *
   *  @param const int32_t - number of repeats (-1 for
   *                                              endless loop /~65000/ )
   * */
  void play(const int32_t loops = 0) override;

  /** @brief used to stop the Sound if it's playing.
   * */
  void stop() override;

  /** @brief used to determine whether the Sound is playing
   *
   *  @return bool - is playing or not
   * */
  bool isPlaying() const override;

  /** @brief used to determine whether the Sound is paused
   *
   *  @return bool - is paused or not
   * */
  bool isPaused() const override;

  /** @brief used to pause a Sound
   * */
  void pause() override;

  /** @brief used to pause a Sound(that was previously paused)
   * */
  void resume() override;

  //================= END SoundWidget related functions ==================

  /** @brief used to play the Sound with panning. The left volume and
   *         right volume are specified as integers between 0 and 255,
   *                                    quietest to loudest, respectively.
   *
   *         NOTE: for real panning effect consider using
   *                      playWithPanning(loops, left, 255 - left);
   *
   *  @param const int32_t - specific channel Id
   *  @param const int32_t - left volume value
   *  @param const int32_t - right volume value
   *
   *  @return int32_t      - error code
   * */
  void playWithPanning(const int32_t loops, const int32_t leftVolume,
                       const int32_t rightVolume);
};

#endif /* MANAGER_UTILS_SOUND_H_ */
