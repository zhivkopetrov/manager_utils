// Corresponding header
#include "manager_utils/drawing/animation/RotationAnimation.h"

// C system headers

// C++ system headers
#include <utility>

// Other libraries headers

// Own components headers
#include "manager_utils/drawing/animation/AnimationEndCb.hpp"
#include "utils/data_type/FloatingPointUtils.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

// default constructor
RotationAnimation::RotationAnimation()
    : _posAnimDir(PosAnimType::UNKNOWN),
      _animType(AnimType::UNKNOWN),
      _currAnimDir(AnimDir::UNKNOWN),
      _rotAngleStep(ZERO_ANGLE),
      _currRotAngle(ZERO_ANGLE),
      _totalRotAngle(ZERO_ANGLE),
      _animCycleWatchAngle(ZERO_ANGLE) {}

// move constructor
RotationAnimation::RotationAnimation(RotationAnimation&& movedOther)
    : AnimationBase(
          std::move(movedOther))  // invoke AnimationBase move constructor
{
  // take ownership of resources
  _posAnimDir = movedOther._posAnimDir;
  _animType = movedOther._animType;
  _currAnimDir = movedOther._currAnimDir;
  _rotAngleStep = movedOther._rotAngleStep;
  _currRotAngle = movedOther._currRotAngle;
  _totalRotAngle = movedOther._totalRotAngle;
  _animCycleWatchAngle = movedOther._animCycleWatchAngle;

  // ownership of resource should be taken from moved instance
  movedOther.resetConfigInternal();
}

// move assignment operator implementation
RotationAnimation& RotationAnimation::operator=(
    RotationAnimation&& movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    // take ownership of resources
    _posAnimDir = movedOther._posAnimDir;
    _animType = movedOther._animType;
    _currAnimDir = movedOther._currAnimDir;
    _rotAngleStep = movedOther._rotAngleStep;
    _currRotAngle = movedOther._currRotAngle;
    _totalRotAngle = movedOther._totalRotAngle;
    _animCycleWatchAngle = movedOther._animCycleWatchAngle;

    // explicitly invoke AnimationBase move assignment operator
    AnimationBase::operator=(std::move(movedOther));

    // ownership of resource should be taken from moved instance
    movedOther.resetConfigInternal();
  }

  return *this;
}

int32_t RotationAnimation::configure(const AnimBaseConfig& cfg,
                                     const double rotationAngleStep,
                                     AnimationEndCb* endCb,
                                     const Point& rotationCenter,
                                     const PosAnimType posAnimDir,
                                     const AnimType animType,
                                     const double totalRotationAngle) {
  int32_t err = SUCCESS;

  if (SUCCESS != AnimationBase::configureInternal(cfg, endCb)) {
    LOGERR(
        "Error, AnimationBase::configureInternal() failed for rsrcId: "
        "%#16lX",
        cfg.rsrcId);

    err = FAILURE;
  }

  if (SUCCESS == err) {
    _posAnimDir = posAnimDir;
    _animType = animType;
    _rotAngleStep = rotationAngleStep;
    _totalRotAngle = totalRotationAngle;

    /** _currAnimDir start always as AnimDir::FORWARD, because:
     *
     *  In case of cfg.animDirection::FORWARD - rotation starts from
     *  ZERO_ANGLE to totalRonAngle (while adding positive values
     *                   in the first iteration/before swap of directions/)
     *
     *  In case of cfg.animDirection::BACKWARDS - rotation starts from
     *  -_totalRotAngle to ZERO_ANGLE (while adding positive values
     *                   in the first iteration/before swap of directions/)
     * */
    _currAnimDir = AnimDir::FORWARD;

    if (Point::UNDEFINED != rotationCenter) {
      _img->setRotationCenter(rotationCenter);
    }

    if (ZERO_ANGLE > _rotAngleStep) {
      LOGERR(
          "Error configuration not complete. Reason: negative "
          "rotationAngleStep provided: %f. Consider using a positive"
          "rotationAngleStep value with AnimDir::BACKWARD.",
          _rotAngleStep);

      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    if (PosAnimType::ONE_DIRECTIONAL == posAnimDir &&
        AnimType::INFINITE == animType) {
      LOGERR(
          "Error, Rotation animation of type ONE_DIRECTIONAL could not"
          " be of type INFINITE. Configuration failed.");

      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    if (FloatingPointUtils::areAlmostEqual(ZERO_ANGLE, _rotAngleStep) ||
        FloatingPointUtils::areAlmostEqual(FULL_ROTATION_ANGLE,
                                           _rotAngleStep)) {
      LOGERR(
          "Error configuration not complete. Reason: zero or close to "
          "zero rotationAngleStep provided: %f. Consider using an "
          "angle > %f and angle < %f degrees.",
          _rotAngleStep, ZERO_ANGLE, FULL_ROTATION_ANGLE);

      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    if (FULL_ROTATION_ANGLE < _rotAngleStep) {
      LOGERR(
          "Error configuration not complete. Reason: too big "
          "rotationAngleStep detected: %f\nConsider using an angle "
          "> %f and angle < %f degrees.",
          _rotAngleStep, ZERO_ANGLE, FULL_ROTATION_ANGLE);

      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    if (ZERO_ANGLE > totalRotationAngle) {
      LOGERR(
          "Error configuration not complete. Reason: negative "
          "totalRotationAngle provided: %f. Consider using a positive"
          "totalRotationAngle value with AnimDir::BACKWARD.",
          _totalRotAngle);

      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    if (AnimType::FINITE == _animType &&
        FloatingPointUtils::areAlmostEqual(ZERO_ANGLE,
                                           totalRotationAngle)) {
      LOGERR(
          "Error configuration not complete. Reason: AnimType::FINITE "
          "provided with totalRotationAngle = %f. Consider using a"
          "totalRotationAngle value different than %f or change the"
          "animation type to AnimType::INFINITE.",
          ZERO_ANGLE, ZERO_ANGLE);

      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    // when initial animation direction is set to backwards
    if (AnimDir::BACKWARD == _cfg.animDirection) {
      _img->rotate(-_totalRotAngle);
      _currRotAngle = -_totalRotAngle;
      _animCycleWatchAngle = -_totalRotAngle;
    }
  }

  if (SUCCESS == err) {
    _isCfgComplete = true;
  } else  // FAILURE == true
  {
    LOGERR("RotationAnimation configuration failed!");

    resetConfigInternal();
  }

  return err;
}

void RotationAnimation::resetConfigInternal() {
  AnimationBase::resetConfigInternal();

  _posAnimDir = PosAnimType::UNKNOWN;
  _animType = AnimType::UNKNOWN;
  _currAnimDir = AnimDir::UNKNOWN;
  _rotAngleStep = ZERO_ANGLE;
  _currRotAngle = ZERO_ANGLE;
  _totalRotAngle = ZERO_ANGLE;
  _animCycleWatchAngle = ZERO_ANGLE;
}

void RotationAnimation::start() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, RotationAnimation could not be started, because "
        "configuration is incomplete. Consider using "
        "RotationAnimation::configure() first");
    return;
  }

  TimerGroup timerGroup = _cfg.isTimerPauseble ? TimerGroup::INTERRUPTIBLE
                                               : TimerGroup::NON_INTERRUPTIBLE;

  startTimer(_cfg.timerInterval, _cfg.timerId, TimerType::PULSE, timerGroup);
}

void RotationAnimation::stop() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, RotationAnimation could not be stopped, because "
        "configuration is incomplete. Consider using "
        "RotationAnimation::configure() first");
    return;
  }

  /** User requested animation stop -> execute onAnimationEnd()
   * callback if there is an active AnimationEndCb handler set.
   * */
  if (nullptr != _endCb) {
    /** In order to call onAnimationEnd() callback we need to check whether
     *  the animation has been started at all (e.g. someone could call
     *  FrameAnimation::stop() before it was started. In this case the
     *  onAnimationEnd() callback should not be invoked)
     *
     *  For this reason we check if the timer was started.
     *  */
    if (isActiveTimerId(_cfg.timerId)) {
      _endCb->onAnimationEnd();
    }
  }

  // stop animation timer
  if (isActiveTimerId(_cfg.timerId)) {
    stopTimer(_cfg.timerId);
  }
}

void RotationAnimation::reset() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, RotationAnimation could not be reset, because "
        "configuration is incomplete. Consider using "
        "RotationAnimation::configure() first");
    return;
  }

  if (isActiveTimerId(_cfg.timerId)) {
    stopTimer(_cfg.timerId);
  }

  double resetAngle = 0.0;

  if (AnimDir::FORWARD == _currAnimDir) {
    resetAngle = ZERO_ANGLE;
  } else  // AnimDir::BACKWARD == _currAnimDir
  {
    resetAngle = -_totalRotAngle;
  }

  _img->setRotation(resetAngle);
  _currRotAngle = resetAngle;
  _animCycleWatchAngle = resetAngle;
  _currAnimDir = AnimDir::FORWARD;
}

void RotationAnimation::onTimeout(const int32_t timerId) {
  if (timerId == _cfg.timerId) {
    if (AnimType::FINITE == _animType) {
      if (AnimDir::FORWARD == _currAnimDir) {
        executeFiniteForward();
      } else  // AnimDirection::BACKWARD == cfg.animDirection
      {
        executeFiniteBackward();
      }
    } else  // INFINITE animations
    {
      if (AnimDir::FORWARD == _currAnimDir) {
        executeInfiniteForward();
      } else  // AnimDirection::BACKWARD == cfg.animDirection
      {
        executeInfiniteBackward();
      }
    }
  } else {
    LOGERR(
        "Invalid timer Id: %d from RotationAnimation with rsrcId: "
        "%#16lX",
        timerId, _cfg.rsrcId);
  }
}

void RotationAnimation::advance(const double advanceAngle) {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, RotationAnimation::advance() could not be invoked, "
        "because configuration is incomplete. Consider using "
        "RotationAnimation::configure() first");
    return;
  }

  if (isActiveTimerId(_cfg.timerId)) {
    LOGERR(
        "Error, RotationAnimation::advance() could not be "
        "invoked, because animation is still running. Wait for it to "
        "finish and then invoke .advance() method.");

    return;
  }

  if (!((ZERO_ANGLE < advanceAngle) && (advanceAngle < _totalRotAngle))) {
    LOGERR(
        "Error, RotationAnimation::advance() could not be invoked, "
        "because provided advanveAngle: %f is not clamped between %f and"
        " _totalRotAngle: %f",
        advanceAngle, ZERO_ANGLE, _totalRotAngle);

    return;
  }

  const double FP_RATIO = advanceAngle / _rotAngleStep;
  const uint32_t INT_RATIO = static_cast<uint32_t>(FP_RATIO);

  if (!FloatingPointUtils::areAlmostEqual(FP_RATIO,
                                          static_cast<double>(INT_RATIO))) {
    LOGERR(
        "Error, RotationAnimation::advanceAngle() called with "
        "advanceAngle: %f, which is not exact divisible by config's "
        "_rotAngleStep: %f",
        advanceAngle, _rotAngleStep);

    return;
  }

  _img->rotate(advanceAngle);

  _currRotAngle += advanceAngle;

  // Check if there is an active AnimationEndCb handler set.
  if (nullptr != _endCb) {
    _animCycleWatchAngle += advanceAngle;

    checkForAnimCycleCallback();
  }
}

void RotationAnimation::swapDirection() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, RotationAnimation::swapDirection() could not be "
        "performed, because configuration is incomplete. Consider using "
        "PositionAnimation::configure() first");

    return;
  }

  if (PosAnimType::BI_DIRECTIONAL == _posAnimDir) {
    LOGERR(
        "Error, RotationAnimation::swapDirection() could not be "
        "performed, because is configured to be of type BI_DIRECTIONAL");

    return;
  }

  if (AnimType::INFINITE == _animType) {
    LOGERR(
        "Error, RotationAnimation::swapDirection() could not be "
        "performed, because is configured to be of type INFINITE");

    return;
  }

  if (isActiveTimerId(_cfg.timerId)) {
    LOGERR(
        "Error, RotationAnimation::swapDirection() could not be "
        "performed, because animation is still running. Wait for it to "
        "finish and then invoke .swapDirection() method.");

    return;
  }

  // swap current animation direction
  if (AnimDir::FORWARD == _currAnimDir) {
    _currAnimDir = AnimDir::BACKWARD;
  } else if (AnimDir::BACKWARD == _currAnimDir) {
    _currAnimDir = AnimDir::FORWARD;
  }
}

void RotationAnimation::executeFiniteForward() {
  _img->rotate(_rotAngleStep);

  _currRotAngle += _rotAngleStep;

  // Check if there is an active AnimationEndCb handler set.
  if (nullptr != _endCb) {
    _animCycleWatchAngle += _rotAngleStep;

    checkForAnimCycleCallback();
  }

  if (AnimDir::FORWARD == _cfg.animDirection) {
    /* Check for total rotation angle bypassed */
    if (_currRotAngle >= _totalRotAngle) {
      // If there is an callback attached -> execute it
      if (nullptr != _endCb) {
        _endCb->onAnimationEnd();
      }

      /** Manually reset the animation so if someone try to stop it from
       * outside the onAnimationEnd() will not be executed twice
       * */
      reset();
    }
  } else  // AnimDir::BACKWARD == _cfg.animDirection
  {
    /* Check for total rotation angle bypassed */
    if (ZERO_ANGLE <= _currRotAngle) {
      /** Manually reset the animation so if someone try to stop it from
       * outside the onAnimationEnd() will not be executed twice
       * */
      reset();

      // If there is an callback attached -> execute it
      if (nullptr != _endCb) {
        _endCb->onAnimationEnd();
      }
    }
  }
}

void RotationAnimation::executeFiniteBackward() {
  _img->rotate(-_rotAngleStep);

  _currRotAngle -= _rotAngleStep;

  // Check if there is an active AnimationEndCb handler set.
  if (nullptr != _endCb) {
    _animCycleWatchAngle -= _rotAngleStep;

    checkForAnimCycleCallback();
  }

  if (AnimDir::FORWARD == _cfg.animDirection) {
    /* Check for complete animation end */
    if (ZERO_ANGLE >= _currRotAngle) {
      /** Manually reset the animation so if someone try to stop it from
       * outside the onAnimationEnd() will not be executed twice
       * */
      reset();

      // If there is an callback attached -> execute it
      if (nullptr != _endCb) {
        _endCb->onAnimationEnd();
      }
    }
  } else  // AnimDir::BACKWARD == _cfg.animDirection
  {
    /* Check for complete animation end */
    if (_currRotAngle <= -_totalRotAngle) {
      /** Manually reset the animation so if someone try to stop it from
       * outside the onAnimationEnd() will not be executed twice
       * */
      reset();

      // If there is an callback attached -> execute it
      if (nullptr != _endCb) {
        _endCb->onAnimationEnd();
      }
    }
  }
}

void RotationAnimation::executeInfiniteForward() {
  _img->rotate(_rotAngleStep);

  _currRotAngle += _rotAngleStep;

  // Check if there is an active AnimationEndCb handler set.
  if (nullptr != _endCb) {
    _animCycleWatchAngle += _rotAngleStep;

    checkForAnimCycleCallback();
  }

  if (AnimDir::FORWARD == _cfg.animDirection) {
    /* Check for total rotation angle bypassed */
    if (_currRotAngle >= _totalRotAngle) {
      /** Manually set the animation internals, but do not stop the timer.
       * Simply change the direction and continue the animation.
       *
       * This is explicitly needed to prevent floating point
       * rounding errors.
       * */
      _img->setRotation(_totalRotAngle);

      _currRotAngle = _totalRotAngle;
      _animCycleWatchAngle = _totalRotAngle;

      // swap direction to backward
      _currAnimDir = AnimDir::BACKWARD;
    }
  } else  // AnimDir::BACKWARD == _cfg.animDirection
  {
    /* Check for total rotation angle bypassed */
    if (ZERO_ANGLE <= _currRotAngle) {
      /** Manually set the animation internals, but do not stop the timer.
       * Simply change the direction and continue the animation.
       *
       * This is explicitly needed to prevent floating point
       * rounding errors.
       * */
      _img->setRotation(ZERO_ANGLE);

      _currRotAngle = ZERO_ANGLE;
      _animCycleWatchAngle = ZERO_ANGLE;

      // swap direction to backward
      _currAnimDir = AnimDir::BACKWARD;
    }
  }
}

void RotationAnimation::executeInfiniteBackward() {
  _img->rotate(-_rotAngleStep);

  _currRotAngle -= _rotAngleStep;

  // Check if there is an active AnimationEndCb handler set.
  if (nullptr != _endCb) {
    _animCycleWatchAngle -= _rotAngleStep;

    checkForAnimCycleCallback();
  }

  if (AnimDir::FORWARD == _cfg.animDirection) {
    /* Check for complete animation end */
    if (ZERO_ANGLE >= _currRotAngle) {
      /** Manually set the animation internals, but do not stop the timer.
       * Simply change the direction and continue the animation.
       *
       * This is explicitly needed to prevent floating point
       * rounding errors.
       * */
      _img->setRotation(ZERO_ANGLE);

      _currRotAngle = ZERO_ANGLE;
      _animCycleWatchAngle = ZERO_ANGLE;

      // swap direction to forward
      _currAnimDir = AnimDir::FORWARD;
    }
  } else  // AnimDir::BACKWARD == _cfg.animDirection
  {
    /* Check for complete animation end */
    if (_currRotAngle <= -_totalRotAngle) {
      /** Manually set the animation internals, but do not stop the timer.
       * Simply change the direction and continue the animation.
       *
       * This is explicitly needed to prevent floating point
       * rounding errors.
       * */
      _img->setRotation(-_totalRotAngle);

      _currRotAngle = -_totalRotAngle;
      _animCycleWatchAngle = -_totalRotAngle;

      // swap direction to forward
      _currAnimDir = AnimDir::FORWARD;
    }
  }
}

void RotationAnimation::checkForAnimCycleCallback() {
  if (-FULL_ROTATION_ANGLE > _animCycleWatchAngle) {
    /** If _animCycleWatchAngle == ZERO_ANGLE this means the
     *  animation has not started yet -> no need to execute
     *  onAnimationCycle() callback
     * */
    if (false == FloatingPointUtils::areAlmostEqual(ZERO_ANGLE,
                                                    _animCycleWatchAngle)) {
      _animCycleWatchAngle += FULL_ROTATION_ANGLE;

      // full animation cycle has passed -> execute onAnimationCycle() cb
      _endCb->onAnimationCycle();
    }
  } else if (FULL_ROTATION_ANGLE < _animCycleWatchAngle) {
    /** If _animCycleWatchAngle == ZERO_ANGLE this means the
     *  animation has not started yet -> no need to execute
     *  onAnimationCycle() callback
     * */
    if (false == FloatingPointUtils::areAlmostEqual(ZERO_ANGLE,
                                                    _animCycleWatchAngle)) {
      _animCycleWatchAngle -= FULL_ROTATION_ANGLE;

      // full animation cycle has passed -> execute onAnimationCycle() cb
      _endCb->onAnimationCycle();
    }
  }
}
