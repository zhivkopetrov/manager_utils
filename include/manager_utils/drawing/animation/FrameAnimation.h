#ifndef MANAGER_UTILS_FRAMEANIMATION_H_
#define MANAGER_UTILS_FRAMEANIMATION_H_

// System headers
#include <cstdint>

// Other libraries headers
#include "utils/ErrorCode.h"

// Own components headers
#include "manager_utils/drawing/animation/AnimationBase.h"

class FrameAnimation : public AnimationBase {
 public:
  FrameAnimation();

  FrameAnimation(FrameAnimation&& movedOther);
  FrameAnimation& operator=(FrameAnimation&& movedOther);

  //============== START AnimationBase related functions =================

  /** @brief used to start the loaded/configured animation.
   * */
  void start() final;

  /** @brief used to manually stop a loaded/configured animation.
   * */
  void stop() final;

  /** @brief used to reset a loaded/configured animation internals so it
   *         can be started again from the start without the need of
   *                                                         reconfigure.
   *         NOTE: this function does not reset visibility.
   *               If your animation was previously hidden with
   *               AnimationBase::hideAnimation() you need to manually
   *               invoke AnimationBase::showAnimation().
   * */
  void reset() final;

  /** @brief TimerClient Timer tick implementation
   * */
  void onTimeout(const int32_t timerId) final;

  //=============== END AnimationBase related functions ==================

  /** @brief used to configure FrameAnimation specific internals.
   *         NOTE: you can reset the animation internals by calling
   *         configure again.
   *
   *  @param const AnimBaseConfig& - animation configuration
   *  @param AnimationEndCb*       - animation End callback
   *  @param const AnimType        - FINITE or INFINITE animation type
   *  @param const uint16_t        - the start frame index for every
   *                                 animation iteration after the 1st one
   *  @param const uint16_t        - number of animation repeats
   *                                 if(AnimType::FINITE is used)
   *
   *  @return ErrorCode            - error code
   * */
  ErrorCode configure(const AnimBaseConfig& cfg,
                      AnimationEndCb* endCb = nullptr,
                      const AnimType animType = AnimType::INFINITE,
                      const uint16_t repeatIndex = 0,
                      const uint16_t numberOfRepeats = 1);

  /** @brief used to swap the direction of movement for the animation.
   *
   *         NOTE: this function is only for animations type:
   *               > AnimType::FINITE
   *
   *         NOTE2: this function may only be invoked if the animation
   *                is idle (either stopped or finished)
   * */
  void swapDirection();

  /** @brief used to acquire the current direction of movement
   *                                                  for the animation
   *
   *  @return AnimDir - the current direction of animation movement
   * */
  AnimDir getCurrDirection() const { return _cfg.animDirection; }

  /** @brief used to explicitly set the first frame of the FrameAnimation.
   *         This can be used in order to use the animation as a simple
   *                                             image when drawing it.
   *
   *         NOTE: this method can only be invoked if the animation is
   *                                               currently not running.
   * */
  void setFirstFrame();

  /** @brief used to explicitly set the last frame of the FrameAnimation.
   *         This can be used in order to use the animation as a simple
   *                                             image when drawing it.
   *
   *         NOTE: this method can only be invoked if the animation is
   *                                               currently not running.
   * */
  void setLastFrame();

 private:
  /** @brief used to set the next logical frame for
   *                                           finite forward animation
   * */
  void executeFiniteForward();

  /** @brief used to set the next logical frame for
   *                                           finite backward animation
   * */
  void executeFiniteBackward();

  /** @brief used to set the next logical frame for
   *                                           infinite forward animation
   * */
  void executeInfiniteForward();

  /** @brief used to set the next logical frame for
   *                                           infinite backward animation
   * */
  void executeInfiniteBackward();

  /** @brief used to completely reset AnimationBase internal variables
   * */
  void resetConfigInternal();

  /** Holds internal animation type (FINITE or INFINITE) */
  AnimType _animType;

  /** Holds internal repeat index for the animation (if such is set).
   *      Example: we have INFINITE animation with 45 frames
   *               and repeat index set to 15.
   *
   *               On first iteration of the animation cycle frames from
   *               0 to 44 are iterated.
   *
   *               On every other animation cycles
   *               frames from 15 to 44 will be iterated.
   * */
  uint16_t _repeatIndex;

  /** Holds numbers of repeats if animation is of type FINITE */
  uint16_t _numOfRepeats;

  /** Holds original numbers of repeats if animation is of type FINITE,
   *  so the variable can be restored if .reset() method is invoked.
   * */
  uint16_t _origNumOfRepeats;

  /** Holds total image frame count. This is a micro optimisation, in
   *  order to bypass non-stop invoking of Image::getFrameCount() .
   *  */
  int32_t _origFrameCount;
};

#endif /* MANAGER_UTILS_FRAMEANIMATION_H_ */
