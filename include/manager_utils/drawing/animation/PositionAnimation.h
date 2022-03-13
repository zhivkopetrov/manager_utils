#ifndef MANAGER_UTILS_POSITIONANIMATION_H_
#define MANAGER_UTILS_POSITIONANIMATION_H_

// System headers
#include <cstdint>

// Other libraries headers
#include "utils/ErrorCode.h"

// Own components headers
#include "manager_utils/drawing/animation/AnimationBase.h"

class PositionAnimation : public AnimationBase {
 public:
  PositionAnimation();
  virtual ~PositionAnimation() = default;

  PositionAnimation(PositionAnimation&& movedOther);
  PositionAnimation& operator=(PositionAnimation&& movedOther);

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
   *
   *         NOTE2: this function reset current animation direction to
   *                the initial value set by the .configure() function.
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
   *  @param const Point            - end position for the animation
   *  @param const uint16_t         - number of steps for the animation to
   *  @param AnimationEndCb *       - animation End callback
   *                                  get from startPos to endPos
   *  @param const PosAnimType      - ONE_DIRECTIONAL or BI_DIRECTIONAL
   *                                  animation type
   *  @param const AnimType         - FINITE or INFINITE animation type
   *  @param const uint16_t         - number of animation repeats
   *                                  if(AnimType::FINITE is used)
   *
   *  @return ErrorCode             - error code
   * */
  ErrorCode configure(const AnimBaseConfig& cfg, const Point& endPos,
                    const uint16_t numberOfSteps,
                    AnimationEndCb* endCb = nullptr,
                    const PosAnimType posAnimDir = PosAnimType::BI_DIRECTIONAL,
                    const AnimType animType = AnimType::FINITE,
                    const uint16_t numberOfRepeats = 1);

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

  /** @brief used to acquire the current direction of movement
   *                                                  for the animation
   *
   *  @return AnimDir - the current direction of animation movement
   * */
  AnimDir getCurrDirection() const { return _currAnimDir; }

  /** @brief used when user is requesting a animation internals reset
   *         for the following reason:
   *         - The animations is active but is in some intermediate state;
   *         - The position of animation needs to be changed without the
   *                                      need of animation to be stopped;
   *         - Animations logic needs to be reset
   *                                       (_numOfSteps and _currAnimDir);
   * */
  void forceInternalsReset();

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

  /** Holds internal animation direction
   *                              (ONE_DIRECTIONAL or BI_DIRECTIONAL)
   * */
  PosAnimType _posAnimDir;

  /** Holds numbers of repeats if animation is of type FINITE */
  uint16_t _numOfRepeats;

  /** Holds original numbers of repeats if animation is of type FINITE,
   *  so the variable can be restored if .reset() method is invoked.
   * */
  uint16_t _origNumOfRepeats;

  /** Holds how many steps the animations need to go
   *                              from startPos to endPos and vice-versa
   * */
  uint16_t _numOfSteps;

  /** Holds original number of steps for the animation so the variable
   *                       can be restored if .reset() method is invoked.
   * */
  uint16_t _origNumOfSteps;

  /** Holds the current animation direction (FORWARD or BACKWARD) */
  AnimDir _currAnimDir;

  /** Holds animation movement for single step by the X axis */
  int32_t _stepX;

  /** Holds animation movement for single step by the X axis */
  int32_t _stepY;

  /** Holds the last movement for the animation if division
   *  had a remainder for the X axis.
   *  Example: Animation need to move 503 pixels for 5 steps.
   *           This means 5 steps with 100 pixels each with remainderX for
   *           3 pixels, which will be gradually added on each step.
   *  */
  int32_t _remainderX;

  /** Holds the last movement for the animation if division
   *  had a remainder for the Y axis.
   *  Example: Animation need to move 305 pixels for 3 steps.
   *           This means 5 steps with 101 pixels each with remainderY for
   *           2 pixels, , which will be gradually added on each step.
   *  */
  int32_t _remainderY;
};

#endif /* MANAGER_UTILS_POSITIONANIMATION_H_ */
