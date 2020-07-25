// Corresponding header
#include "manager_utils/drawing/animation/PulseAnimation.h"

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
PulseAnimation::PulseAnimation()
    : _currScale(MIN_SCALE_FACTOR),
      _scaleStep(MIN_SCALE_FACTOR),
      _minScale(MIN_SCALE_FACTOR),
      _origNumOfShrinkSteps(0),
      _currShrinkStep(0),
      _origNumOfRepeats(0),
      _numOfRepeats(0),
      _origImgWidth(0),
      _origImgHeight(0),
      _animType(AnimType::UNKNOWN),
      _currAnimDir(AnimDir::UNKNOWN),
      _alignType(PulseAlignType::UNKNOWN) {}

// move constructor
PulseAnimation::PulseAnimation(PulseAnimation&& movedOther)
    : AnimationBase(
          std::move(movedOther))  // invoke AnimationBase move constructor
{
  // take ownership of resources
  _currScale = movedOther._currScale;
  _minScale = movedOther._minScale;
  _scaleStep = movedOther._scaleStep;
  _origNumOfShrinkSteps = movedOther._origNumOfShrinkSteps;
  _currShrinkStep = movedOther._currShrinkStep;
  _origNumOfRepeats = movedOther._origNumOfRepeats;
  _numOfRepeats = movedOther._numOfRepeats;
  _origImgWidth = movedOther._origImgWidth;
  _origImgHeight = movedOther._origImgHeight;
  _animType = movedOther._animType;
  _currAnimDir = movedOther._currAnimDir;
  _alignType = movedOther._alignType;

  // ownership of resource should be taken from moved instance
  movedOther.resetConfigInternal();
}

// move assignment operator implementation
PulseAnimation& PulseAnimation::operator=(PulseAnimation&& movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    // take ownership of resources
    _currScale = movedOther._currScale;
    _minScale = movedOther._minScale;
    _scaleStep = movedOther._scaleStep;
    _origNumOfShrinkSteps = movedOther._origNumOfShrinkSteps;
    _currShrinkStep = movedOther._currShrinkStep;
    _origNumOfRepeats = movedOther._origNumOfRepeats;
    _numOfRepeats = movedOther._numOfRepeats;
    _animType = movedOther._animType;
    _currAnimDir = movedOther._currAnimDir;
    _alignType = movedOther._alignType;

    // explicitly invoke AnimationBase move assignment operator
    AnimationBase::operator=(std::move(movedOther));

    // ownership of resource should be taken from moved instance
    movedOther.resetConfigInternal();
  }

  return *this;
}

int32_t PulseAnimation::configure(const AnimBaseConfig& cfg,
                                  const double minScale,
                                  const uint8_t numberOfShrinkSteps,
                                  const PulseAlignType alignType,
                                  AnimationEndCb* endCb,
                                  const AnimType animType,
                                  const uint16_t numberOfRepeats) {
  int32_t err = SUCCESS;

  if (SUCCESS != AnimationBase::configureInternal(cfg, endCb)) {
    LOGERR(
        "Error, AnimationBase::configureInternal() failed for rsrcId: "
        "%#16lX",
        cfg.rsrcId);

    err = FAILURE;
  }

  if (SUCCESS == err) {
    _animType = animType;

    _currAnimDir = cfg.animDirection;

    _minScale = minScale;

    _currShrinkStep = numberOfShrinkSteps;

    _origNumOfShrinkSteps = numberOfShrinkSteps;

    _numOfRepeats = numberOfRepeats;

    _alignType = alignType;

    _origImgWidth = _img->getFrameWidth();
    _origImgHeight = _img->getFrameHeight();

    if (MIN_SCALE_FACTOR > minScale || MAX_SCALE_FACTOR < minScale) {
      LOGERR(
          "Error configuration not complete. Reason: Invalid minScale "
          "param provided: %f. Consider using a value in "
          "range %f - %f(non-inclusive)",
          minScale, MIN_SCALE_FACTOR, MAX_SCALE_FACTOR);

      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    if (FloatingPointUtils::areAlmostEqual(MIN_SCALE_FACTOR, minScale) ||
        FloatingPointUtils::areAlmostEqual(MAX_SCALE_FACTOR, minScale)) {
      LOGERR(
          "Error configuration not complete. Reason: Invalid minScale "
          "param provided: %f. Consider using a value in "
          "range %f - %f(non-inclusive)",
          minScale, MIN_SCALE_FACTOR, MAX_SCALE_FACTOR);

      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    if (MAX_SHRINK_STEPS < numberOfShrinkSteps || 0 >= numberOfShrinkSteps) {
      LOGERR(
          "Error configuration not complete. Reason: invalid value "
          "%hhu for numberOfShrinkSteps. Valid range is "
          "1 - 200(inclusive)",
          numberOfShrinkSteps);

      err = FAILURE;
    } else {
      _scaleStep = (MAX_SCALE_FACTOR - _minScale) / numberOfShrinkSteps;
    }
  }

  if (SUCCESS == err) {
    if ((AnimDir::BACKWARD == cfg.animDirection) &&
        (PulseAlignType::TOP_LEFT == _alignType)) {
      LOGERR(
          "Error configuration not complete. Reason: PulseAnimation "
          "could not be of type AnimDir::BACKWARD and "
          "PulseAlignType::TOP_LEFT at the same time. Consider using "
          "PulseAlignType::CENTER");

      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    _isCfgComplete = true;
    _img->activateScaling();

    if (AnimDir::FORWARD == _cfg.animDirection) {
      _currScale = MAX_SCALE_FACTOR;
      _img->setScale(_currScale);
    } else  // AnimDir::BACKWARD == _cfg.animDirection
    {
      _currScale = _minScale;
      _img->setScale(_currScale);

      if (PulseAlignType::CENTER == _alignType) {
        centerImage();
      }
    }
  } else  // FAILURE == true
  {
    LOGERR("PulseAnimation configuration failed!");
    resetConfigInternal();
  }

  return err;
}

void PulseAnimation::start() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, PulseAnimation could not be started, because "
        "configuration is incomplete. Consider using "
        "PulseAnimation::configure() first");
    return;
  }

  TimerGroup timerGroup = _cfg.isTimerPauseble ? TimerGroup::INTERRUPTIBLE
                                               : TimerGroup::NON_INTERRUPTIBLE;

  startTimer(_cfg.timerInterval, _cfg.timerId, TimerType::PULSE, timerGroup);
}

void PulseAnimation::stop() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, PulseAnimation could not be stopped, because "
        "configuration is incomplete. Consider using "
        "PulseAnimation::configure() first");
    return;
  }

  /** User requested animation stop -> execute onAnimationEnd()
   * callback if there is an active AnimationEndCb handler set.
   * */
  if (nullptr != _endCb) {
    /** In order to call onAnimationEnd() callback we need to check whether
     *  the animation has been started at all (e.g. someone could call
     *  PulseAnimation::stop() before it was started. In this case the
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

void PulseAnimation::reset() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, PulseAnimation could not be reset, because "
        "configuration is incomplete. Consider using "
        "PulseAnimation::configure() first");
    return;
  }

  if (isActiveTimerId(_cfg.timerId)) {
    stopTimer(_cfg.timerId);
  }

  double resetScale = 0.0;

  if (AnimDir::FORWARD == _cfg.animDirection) {
    resetScale = MAX_SCALE_FACTOR;
  } else  // AnimDir::BACKWARD == _cfg.animDirection
  {
    resetScale = _minScale;
  }

  _img->setScale(resetScale);

  if (PulseAlignType::CENTER == _alignType) {
    centerImage();
  }

  _currScale = resetScale;
  _currShrinkStep = _origNumOfShrinkSteps;
  _numOfRepeats = _origNumOfRepeats;
  _currAnimDir = _cfg.animDirection;
}

void PulseAnimation::onTimeout(const int32_t timerId) {
  if (timerId == _cfg.timerId) {
    if (AnimType::FINITE == _animType) {
      if (AnimDir::FORWARD == _currAnimDir) {
        executeFiniteForward();
      } else  // AnimDirection::BACKWARD == _currAnimDir
      {
        executeFiniteBackward();
      }
    } else  // INFINITE animations
    {
      if (AnimDir::FORWARD == _currAnimDir) {
        executeInfiniteForward();
      } else  // AnimDirection::BACKWARD == _currAnimDir
      {
        executeInfiniteBackward();
      }
    }

    if (PulseAlignType::CENTER == _alignType) {
      centerImage();
    }
  } else {
    LOGERR(
        "Invalid timer Id: %d from PulseAnimation with rsrcId: "
        "%#16lX",
        timerId, _cfg.rsrcId);
  }
}

void PulseAnimation::executeFiniteForward() {
  --_currShrinkStep;
  _currScale -= _scaleStep;
  _img->setScale(_currScale);

  // animation has reached it's end
  if (0 == _currShrinkStep) {
    // animations should only decrease it's on backward end repetitions

    if (nullptr != _endCb) {
      _endCb->onAnimationCycle();
    }

    // manually set the value to _minScale
    // to get rid of accumulated rounding error
    _currScale = _minScale;

    // swap direction of PulseAnimation from shrinking to expanding
    _currAnimDir = AnimDir::BACKWARD;
    _currShrinkStep = _origNumOfShrinkSteps;
  }
}

void PulseAnimation::executeFiniteBackward() {
  --_currShrinkStep;
  _currScale += _scaleStep;
  _img->setScale(_currScale);

  // animation has restarted
  if (0 == _currShrinkStep) {
    if (0 < _numOfRepeats) {
      --_numOfRepeats;  // decrease remaining animation repetitions
    }

    if (nullptr != _endCb) {
      _endCb->onAnimationCycle();
    }

    // check whether there is number of repeats set
    if (0 == _numOfRepeats) {
      /** Manually reset the animation so if someone try to stop it from
       * outside the onAnimationEnd() will not be executed twice
       * */
      reset();

      // If there is an callback attached -> execute it
      if (nullptr != _endCb) {
        _endCb->onAnimationEnd();
      }
    } else  // 0 != _numOfRepeats
    {
      // manually set the value to _minScale
      // to get rid of accumulated rounding error
      _currScale = MAX_SCALE_FACTOR;

      // swap direction of PulseAnimation from expanding to shrinking
      _currAnimDir = AnimDir::FORWARD;
      _currShrinkStep = _origNumOfShrinkSteps;
    }
  }
}

void PulseAnimation::executeInfiniteForward() {
  --_currShrinkStep;
  _currScale -= _scaleStep;
  _img->setScale(_currScale);

  // animation has restarted
  if (0 == _currShrinkStep) {
    // If there is an callback attached -> execute it
    if (nullptr != _endCb) {
      _endCb->onAnimationCycle();
    }

    // manually set the value to _minScale
    // to get rid of accumulated rounding error
    _currScale = _minScale;

    // swap direction of PulseAnimation from shrinking to expanding
    _currAnimDir = AnimDir::BACKWARD;
    _currShrinkStep = _origNumOfShrinkSteps;
  }
}

void PulseAnimation::executeInfiniteBackward() {
  --_currShrinkStep;
  _currScale += _scaleStep;
  _img->setScale(_currScale);

  // animation has restarted
  if (0 == _currShrinkStep) {
    // If there is an callback attached -> execute it
    if (nullptr != _endCb) {
      _endCb->onAnimationCycle();
    }

    // manually set the value to _minScale
    // to get rid of accumulated rounding error
    _currScale = MAX_SCALE_FACTOR;

    // swap direction of PulseAnimation from expanding to shrinking
    _currAnimDir = AnimDir::FORWARD;
    _currShrinkStep = _origNumOfShrinkSteps;
  }
}

void PulseAnimation::centerImage() {
  const int32_t DIFF_X = (_origImgWidth - _img->getScaledWidth()) >> 1;
  const int32_t DIFF_Y = (_origImgHeight - _img->getScaledHeight()) >> 1;

  _img->setPosition(_cfg.startPos.x + DIFF_X, _cfg.startPos.y + DIFF_Y);
}

void PulseAnimation::resetConfigInternal() {
  AnimationBase::resetConfigInternal();

  _currScale = MAX_SCALE_FACTOR;
  _minScale = 0.0;
  _scaleStep = 0.0;
  _origNumOfShrinkSteps = 0;
  _currShrinkStep = 0;
  _origNumOfRepeats = 0;
  _numOfRepeats = 0;
  _origImgWidth = 0, _origImgHeight = 0, _animType = AnimType::UNKNOWN;
  _currAnimDir = AnimDir::UNKNOWN;
  _alignType = PulseAlignType::UNKNOWN;
}
