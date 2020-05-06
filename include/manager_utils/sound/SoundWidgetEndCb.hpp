#ifndef MANAGER_UTILS_SOUNDWIDGETENDCB_HPP_
#define MANAGER_UTILS_SOUNDWIDGETENDCB_HPP_

// C system headers

// C++ system headers
#include <cstdint>

// Other libraries headers

// Own components headers

// Forward declarations

class SoundWidgetEndCb {
 public:
  SoundWidgetEndCb() = default;
  virtual ~SoundWidgetEndCb() = default;

  /** @brief this function is invoked on complete sound end
   *                                          (all loops of the sound)
   *
   *         NOTE: this function will be invoked if sound is
   *               stopped manually (Sound::stop)
   *
   *  @return int32_t - user defined error code
   * */
  virtual int32_t onSoundWidgetEnd() { return 0; }
};

#endif /* MANAGER_UTILS_SOUNDWIDGETENDCB_HPP_ */
