#ifndef MANAGER_UTILS_ROTATIONANIMATION_H_
#define MANAGER_UTILS_ROTATIONANIMATION_H_

// System headers
#include <cstdint>

// Other libraries headers
#include "utils/ErrorCode.h"

// Own components headers
#include "manager_utils/drawing/animation/AnimationBase.h"

// Forward declarations

class RotationAnimation : public AnimationBase {
 public:
  RotationAnimation();

  RotationAnimation(RotationAnimation&& movedOther);
  RotationAnimation& operator=(RotationAnimation&& movedOther);

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
   *  @param const AnimBaseConfig & - animation configuration
   *  @param const double           - rotation angle step
   *  @param AnimationEndCb *       - animation End callback
   *  @param const Point            - rotation Center of image
   *                                     (relative to the image position)
   *  @param const PosAnimType      - ONE_DIRECTIONAL or BI_DIRECTIONAL
   *                                 animation type
   *  @param const AnimType         - FINITE or INFINITE animation type
   *  @param const double           - total rotation angle until animation stops
   *
   *  @return ErrorCode             - error code
   * */
  ErrorCode configure(const AnimBaseConfig& cfg, const double rotationAngleStep,
                    AnimationEndCb* endCb = nullptr,
                    const Point& rotationCenter = Points::UNDEFINED,
                    const PosAnimType posAnimDir = PosAnimType::BI_DIRECTIONAL,
                    const AnimType animType = AnimType::FINITE,
                    const double totalRotationAngle = FULL_ROTATION_ANGLE);

  /** @brief used to swap the direction of movement for the animation.
   *
   *         NOTE: this function is only for animations of:
   *               > PosAnimType::ONE_DIRECTIONAL
   *               > AnimType::FINITE
   *
   *         NOTE2: this function may only be invoked if the animation
   *                is idle (either stopped or finished)
   * */
  void swapDirection();

  /** @brief used to advance the rotation animation with a certain
   *         rotation angle. This method can be used to give the animation
   *         starting rotation angle different than the default one.
   *
   *         NOTE: this method can only be invoked if the animation is
   *               not yet started.
   *
   *         NOTE 2: the value of advanceAngle should be clamped between
   *                 ZERO_ANGLE_VALUE and config's totalRotationAngle
   *
   *         NOTE 3: the value of rotationAngle should be correctly
   *                 divisible  by config's rotatationAngleStep
   *                 Example: rotatationAngleStep = 2.0
   *                          advanceAngle = 6.0 (Correct)
   *                          advanceAngle = 6.4 (Incorrect)
   *
   *  @param const double - advanceAngle to advance the animation with
   * */
  void advance(const double advanceAngle);

  /** @brief used to acquire the current direction of movement
   *                                                  for the animation
   *
   *  @return AnimDir - the current direction of animation movement
   * */
  AnimDir getCurrDirection() const { return _currAnimDir; }

  double getRotationAngleStep() const { return _rotAngleStep; }

  double getRotationAngleLeft() const {
    return _totalRotAngle - _currRotAngle;
  }

  void slowDown(const double value) {
    (_rotAngleStep - value >= 0) ? _rotAngleStep -= value : _rotAngleStep = 0;
  }

  void speedUp(const double value) { _rotAngleStep += value; }

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

  /** @brief used to determine whether AnimationEndCb::onAnimationCycle()
   *                 callback should be called or not (if handler is set)
   * */
  void checkForAnimCycleCallback();

  /** @brief used to completely reset AnimationBase internal variables
   * */
  void resetConfigInternal();

  /** Holds internal animation direction
   *                              (ONE_DIRECTIONAL or BI_DIRECTIONAL)
   * */
  PosAnimType _posAnimDir;

  /** Holds internal animation type (FINITE or INFINITE) */
  AnimType _animType;

  /** Holds the current animation direction (FORWARD or BACKWARD) */
  AnimDir _currAnimDir;

  /** Holds animation rotation step per frame */
  double _rotAngleStep;

  /** Holds current animation rotation (since start of animation) */
  double _currRotAngle;

  /** Holds total animation rotation (if such is provided) */
  double _totalRotAngle;

  /** Used to determine when a full animation cycle has passed */
  double _animCycleWatchAngle;
};

#endif /* MANAGER_UTILS_ROTATIONANIMATION_H_ */
