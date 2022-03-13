#ifndef MANAGER_UTILS_SOUNDWIDGETENDCB_H_
#define MANAGER_UTILS_SOUNDWIDGETENDCB_H_

// System headers
#include <cstdint>

// Other libraries headers
#include "utils/ErrorCode.h"

// Own components headers

// Forward declarations

class SoundWidgetEndCb {
 public:
  SoundWidgetEndCb() = default;
  virtual ~SoundWidgetEndCb() noexcept = default;

  /** @brief this function is invoked on complete sound end
   *                                          (all loops of the sound)
   *
   *         NOTE: this function will be invoked if sound is
   *               stopped manually (Sound::stop)
   *
   *  @return ErrorCode - user defined error code
   * */
  virtual ErrorCode onSoundWidgetEnd() {
    return ErrorCode::SUCCESS;
  }
};

#endif /* MANAGER_UTILS_SOUNDWIDGETENDCB_H_ */
