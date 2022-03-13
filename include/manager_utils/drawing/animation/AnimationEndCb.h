#ifndef MANAGER_UTILS_ANIMATIONENDCB_H_
#define MANAGER_UTILS_ANIMATIONENDCB_H_

/*
 * AnimationEndCb.hpp
 *
 *  @brief AnimationEndCb is attached to every animation that inherits
 *         AnimationBase class. On complete cycle of concrete animation
 *         this function will be invoked.
 *
 *         Developer can implement their own AnimationEndCb class by
 *         inheriting AnimationEndCb and overriding methods:
 *              > virtual int32_t onAnimationCycle();
 *              > virtual int32_t onAnimationEnd();
 *
 *         If one or both of these methods are not overridden the default
 *         empty methods will be used.
 *
 */

// System headers
#include <cstdint>

// Other libraries headers
#include "utils/ErrorCode.h"

// Own components headers

class AnimationEndCb {
 public:
  AnimationEndCb() = default;
  virtual ~AnimationEndCb() noexcept = default;

  /** @brief this function is invoked on complete cycle of the
   *         concrete animation if(isRepetative flag is set to true).
   *         NOTE: this function will not be invoked if animation is
   *               stopped manually (AnimationBase::stop)
   *
   *  @return ErrorCode - user defined error code
   * */
  virtual ErrorCode onAnimationCycle() {
    return ErrorCode::SUCCESS;
  }

  /** @brief this function is invoked on complete animation end
   *         (all iterations of the animations) of the concrete animation
   *         (isRepetative flag is set to false).
   *         NOTE: this function will not be invoked if animation is
   *               stopped manually (AnimationBase::stop)
   *
   *  @return ErrorCode - user defined error code
   * */
  virtual ErrorCode onAnimationEnd() {
    return ErrorCode::SUCCESS;
  }
};

#endif /* MANAGER_UTILS_ANIMATIONENDCB_H_ */
