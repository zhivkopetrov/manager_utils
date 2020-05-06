#ifndef MANAGER_UTILS_ANIMATIONENDCB_HPP_
#define MANAGER_UTILS_ANIMATIONENDCB_HPP_

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

// C system headers

// C++ system headers
#include <cstdint>

// Other libraries headers

// Own components headers

class AnimationEndCb {
 public:
  AnimationEndCb() = default;
  virtual ~AnimationEndCb() = default;

  /** @brief this function is invoked on complete cycle of the
   *         concrete animation if(isRepetative flag is set to true).
   *         NOTE: this function will not be invoked if animation is
   *               stopped manually (AnimationBase::stop)
   *
   *  @return int32_t - user defined error code
   * */
  virtual int32_t onAnimationCycle() { return 0; }

  /** @brief this function is invoked on complete animation end
   *         (all iterations of the animations) of the concrete animation
   *         (isRepetative flag is set to false).
   *         NOTE: this function will not be invoked if animation is
   *               stopped manually (AnimationBase::stop)
   *
   *  @return int32_t - user defined error code
   * */
  virtual int32_t onAnimationEnd() { return 0; }
};

#endif /* MANAGER_UTILS_ANIMATIONENDCB_HPP_ */
