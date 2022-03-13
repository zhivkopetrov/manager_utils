#ifndef MANAGER_UTILS_PULSEANIMATION_H_
#define MANAGER_UTILS_PULSEANIMATION_H_

// System headers
#include <cstdint>

// Other libraries headers
#include "utils/ErrorCode.h"

// Own components headers
#include "manager_utils/drawing/animation/AnimationBase.h"

// Forward declarations

enum class PulseAlignType : uint8_t {
  TOP_LEFT = 0,
  CENTER = 1,

  UNKNOWN = 255
};

class PulseAnimation : public AnimationBase {
 public:
  PulseAnimation();

  PulseAnimation(PulseAnimation&& movedOther);
  PulseAnimation& operator=(PulseAnimation&& movedOther);

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
   *  @param const double          - minScale - animations shrinks from
   *                                 scale 1.0 to scale minScale. After
   *                                 that from scale minScale to 1.0
   *  @param const uint8_t         - number of steps for reaching from
   *                                 scale 1.0 to scale minScale
   *                                 (takes values in range 1 - 200)
   *  @param AnimationEndCb*       - animation End callback
   *  @param const AnimType        - FINITE or INFINITE animation type
   *  @param const uint16_t        - number of animation repeats
   *                                 if(AnimType::FINITE is used)
   *
   *  @return ErrorCode            - error code
   * */
  ErrorCode configure(const AnimBaseConfig& cfg, const double minScale,
                      const uint8_t numberOfShrinkSteps,
                      const PulseAlignType alignType = PulseAlignType::CENTER,
                      AnimationEndCb* endCb = nullptr,
                      const AnimType animType = AnimType::INFINITE,
                      const uint16_t numberOfRepeats = 1);

  /**
   * @brief Sets the number of repeats for the pulse animation by hand
   * @param numberOfRepeats - the number of full pulses the animation must
   * perform.
   **/
  void setNumberOfRepeats(const uint16_t numberOfRepeats) {
    _numOfRepeats = numberOfRepeats;
  }

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

  /**
   * @brief set the center position of the image if
   *                                 PulseAlignType::CENTERED is provided
   */
  void centerImage();

  /** @brief used to completely reset AnimationBase internal variables
   * */
  void resetConfigInternal();

  enum InternalDefines {
    MAX_SHRINK_STEPS = 200
  };

  /** Holds current scale for the animation.
   *  NOTE: scale 1.0 is the max. Scale 0.0 is the min.
   * */
  double _currScale;

  /** Holds the scaleStep calculated by minScale / numberOfShrinkSteps
   *  set by the configure() method
   *  */
  double _scaleStep;

  /** Holds the minScale set by the configure() method */
  double _minScale;

  /** Holds original numbers of shrink/expand steps if animation is of
   *  type FINITE, so the variable can be restored
   *                                       if .reset() method is invoked.
   * */
  uint8_t _origNumOfShrinkSteps;

  /** Holds current shrink step for the animation. */
  uint8_t _currShrinkStep;

  /** Holds original numbers of repeats if animation is of type FINITE,
   *  so the variable can be restored if .reset() method is invoked.
   * */
  uint16_t _origNumOfRepeats;

  /** Holds numbers of repeats if animation is of type FINITE */
  uint16_t _numOfRepeats;

  /** Holds original image width if animation alignment is
   *                                                     of type CENTERED
   * */
  int32_t _origImgWidth;

  /** Holds original image height if animation alignment is
   *                                                     of type CENTERED
   * */
  int32_t _origImgHeight;

  /** Holds internal animation type (FINITE or INFINITE) */
  AnimType _animType;

  /** Holds the current animation direction (FORWARD or BACKWARD) */
  AnimDir _currAnimDir;

  /** Holds the animation pulse movement type (TOP_LEFT or CENTERED) */
  PulseAlignType _alignType;
};

#endif /* MANAGER_UTILS_PULSEANIMATION_H_ */
