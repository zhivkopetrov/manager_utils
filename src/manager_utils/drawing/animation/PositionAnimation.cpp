// Corresponding header
#include "manager_utils/drawing/animation/PositionAnimation.h"

// C system headers

// C++ system headers
#include <utility>

// Other libraries headers

// Own components headers
#include "manager_utils/drawing/animation/AnimationEndCb.hpp"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

// default constructor
PositionAnimation::PositionAnimation()
    : _animType(AnimType::UNKNOWN),
      _posAnimDir(PosAnimType::UNKNOWN),
      _numOfRepeats(0),
      _origNumOfRepeats(0),
      _numOfSteps(0),
      _origNumOfSteps(0),
      _currAnimDir(AnimDir::UNKNOWN),
      _stepX(0),
      _stepY(0),
      _remainderX(0),
      _remainderY(0) {}

// move constructor
PositionAnimation::PositionAnimation(PositionAnimation&& movedOther)
    : AnimationBase(
          std::move(movedOther))  // invoke AnimationBase move constructor
{
  // take ownership of resources
  _animType = movedOther._animType;
  _posAnimDir = movedOther._posAnimDir;
  _numOfRepeats = movedOther._numOfRepeats;
  _origNumOfRepeats = movedOther._origNumOfRepeats;
  _numOfSteps = movedOther._numOfSteps;
  _origNumOfSteps = movedOther._origNumOfSteps;
  _currAnimDir = movedOther._currAnimDir;
  _stepX = movedOther._stepX;
  _stepY = movedOther._stepY;
  _remainderX = movedOther._remainderX;
  _remainderY = movedOther._remainderY;

  // ownership of resource should be taken from moved instance
  movedOther.resetConfigInternal();
}

// move assignment operator implementation
PositionAnimation& PositionAnimation::operator=(
    PositionAnimation&& movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    // take ownership of resources
    _animType = movedOther._animType;
    _posAnimDir = movedOther._posAnimDir;
    _numOfRepeats = movedOther._numOfRepeats;
    _origNumOfRepeats = movedOther._origNumOfRepeats;
    _numOfSteps = movedOther._numOfSteps;
    _origNumOfSteps = movedOther._origNumOfSteps;
    _currAnimDir = movedOther._currAnimDir;
    _stepX = movedOther._stepX;
    _stepY = movedOther._stepY;
    _remainderX = movedOther._remainderX;
    _remainderY = movedOther._remainderY;

    // explicitly invoke AnimationBase move assignment operator
    AnimationBase::operator=(std::move(movedOther));

    // ownership of resource should be taken from moved instance
    movedOther.resetConfigInternal();
  }

  return *this;
}

int32_t PositionAnimation::configure(const AnimBaseConfig& cfg,
                                     const Point& endPos,
                                     const uint16_t numberOfSteps,
                                     AnimationEndCb* endCb,
                                     const PosAnimType posAnimDir,
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
    if (AnimDir::BACKWARD == _cfg.animDirection ||
        AnimDir::UNKNOWN == _cfg.animDirection) {
      LOGERR(
          "Error, Position animation direction could only be "
          "of type FORWARD. Configuration failed.");

      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    if (0 == numberOfSteps) {
      LOGERR(
          "Error, Position animation numberOfSteps can not be 0."
          "Configuration failed.");
      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    if (PosAnimType::ONE_DIRECTIONAL == posAnimDir &&
        AnimType::INFINITE == animType) {
      LOGERR(
          "Error, Position animation of type ONE_DIRECTIONAL could not"
          " be of type INFINITE. Configuration failed.");

      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    if (PosAnimType::ONE_DIRECTIONAL == posAnimDir && 1 != numberOfRepeats) {
      LOGERR(
          "Error, Position animation of type ONE_DIRECTIONAL could not"
          " have numberOfRepeats different than 1. Configuration "
          "failed.");

      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    _animType = animType;

    _numOfRepeats = numberOfRepeats;
    _origNumOfRepeats = numberOfRepeats;

    _numOfSteps = numberOfSteps;
    _origNumOfSteps = numberOfSteps;

    _posAnimDir = posAnimDir;

    const int32_t TOTAL_MOVEMENT_X = endPos.x - _cfg.startPos.x;

    _remainderX = TOTAL_MOVEMENT_X % numberOfSteps;
    _stepX = (TOTAL_MOVEMENT_X - _remainderX) / numberOfSteps;

    const int32_t TOTAL_MOVEMENT_Y = endPos.y - _cfg.startPos.y;

    _remainderY = TOTAL_MOVEMENT_Y % numberOfSteps;
    _stepY = (TOTAL_MOVEMENT_Y - _remainderY) / numberOfSteps;

    _currAnimDir = _cfg.animDirection;
  }

  if (SUCCESS == err) {
    _isCfgComplete = true;
  } else  // FAILURE == true
  {
    LOGERR("PositionAnimation configuration failed!");
    resetConfigInternal();
  }

  return err;
}

void PositionAnimation::resetConfigInternal() {
  AnimationBase::resetConfigInternal();

  _animType = AnimType::UNKNOWN;
  _posAnimDir = PosAnimType::UNKNOWN;
  _numOfRepeats = 0;
  _origNumOfRepeats = 0;
  _numOfSteps = 0;
  _origNumOfSteps = 0;
  _currAnimDir = AnimDir::UNKNOWN;
  _stepX = 0;
  _stepY = 0;
  _remainderX = 0;
  _remainderY = 0;
}

void PositionAnimation::start() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, PositionAnimation could not be started, because "
        "configuration is incomplete. Consider using "
        "PositionAnimation::configure() first");
    return;
  }

  TimerGroup timerGroup = _cfg.isTimerPauseble ? TimerGroup::INTERRUPTIBLE
                                               : TimerGroup::NON_INTERRUPTIBLE;

  startTimer(_cfg.timerInterval, _cfg.timerId, TimerType::PULSE, timerGroup);
}

void PositionAnimation::stop() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, PositionAnimation could not be stopped, because "
        "configuration is incomplete. Consider using "
        "PositionAnimation::configure() first");
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

void PositionAnimation::reset() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, PositionAnimation could not be reset, because "
        "configuration is incomplete. Consider using "
        "PositionAnimation::configure() first");
    return;
  }

  if (isActiveTimerId(_cfg.timerId)) {
    stopTimer(_cfg.timerId);
  }

  _numOfRepeats = _origNumOfRepeats;
  _numOfSteps = _origNumOfSteps;
  _currAnimDir = _cfg.animDirection;
  _img->setPosition(_cfg.startPos);
}

void PositionAnimation::onTimeout(const int32_t timerId) {
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
  } else {
    LOGERR(
        "Invalid timer Id: %d from PositionAnimation with rsrcId: "
        "%#16lX",
        timerId, _cfg.rsrcId);
  }
}

void PositionAnimation::executeFiniteForward() {
  --_numOfSteps;

  // move to X axis
  _img->moveRight(_stepX);

  // move to Y axis
  _img->moveDown(_stepY);

  // animation has reached it's end
  if (0 == _numOfSteps) {
    // check for X axis remainder
    if (_remainderX) {
      _img->moveRight(_remainderX);
    }

    // check for Y axis remainder
    if (_remainderY) {
      _img->moveDown(_remainderY);
    }

    if (PosAnimType::ONE_DIRECTIONAL == _posAnimDir) {
      /** Decrease remaining animation repetitions only if it's
       *                                       ONE_DIRECTIONAL animation
       *
       * BI_DIRECTIONAL animation decrement their timers on
       * return (on executeFinishBackward end)
       * */
      if (0 < _numOfRepeats) {
        --_numOfRepeats;
      }

      if (isActiveTimerId(_cfg.timerId)) {
        stopTimer(_cfg.timerId);
      }

      // If there is an callback attached -> execute it
      if (nullptr != _endCb) {
        _endCb->onAnimationCycle();
        _endCb->onAnimationEnd();
      }
    } else  // PosAnimType::BI_DIRECTIONAL == _posAnimDir
    {
      // swap direction to backwards
      _currAnimDir = AnimDir::BACKWARD;
      _numOfSteps = _origNumOfSteps;
    }
  }
}

void PositionAnimation::executeFiniteBackward() {
  --_numOfSteps;

  // move to X axis
  _img->moveLeft(_stepX);

  // move to Y axis
  _img->moveUp(_stepY);

  // animation has reached it's end
  if (0 == _numOfSteps) {
    // check for X axis remainder
    if (_remainderX) {
      _img->moveLeft(_remainderX);
    }

    // check for Y axis remainder
    if (_remainderY) {
      _img->moveUp(_remainderY);
    }

    if (PosAnimType::ONE_DIRECTIONAL == _posAnimDir) {
      /** Decrease remaining animation repetitions only if it's
       *                                       ONE_DIRECTIONAL animation
       *
       * BI_DIRECTIONAL animation decrement their timers on
       * return (on executeFinishBackward end)
       * */
      if (0 < _numOfRepeats) {
        --_numOfRepeats;  // decrease remaining animation repetitions
      }

      if (isActiveTimerId(_cfg.timerId)) {
        stopTimer(_cfg.timerId);
      }

      // If there is an callback attached -> execute it
      if (nullptr != _endCb) {
        _endCb->onAnimationCycle();
        _endCb->onAnimationEnd();
      }
    } else  // PosAnimType::BI_DIRECTIONAL == _posAnimDir
    {
      if (0 < _numOfRepeats) {
        --_numOfRepeats;  // decrease remaining animation repetitions
      }

      // If there is an callback attached -> execute it
      if (nullptr != _endCb) {
        _endCb->onAnimationCycle();
      }

      // check whether there is number of repeats set
      if (0 == _numOfRepeats) {
        /* Manually reset the animation so if someone try to stop it
         * from outside the onAnimationEnd() will not be executed twice
         * */
        reset();

        // If there is an callback attached -> execute it
        if (nullptr != _endCb) {
          _endCb->onAnimationEnd();
        }
      } else  // 0 != _numOfRepeats
      {
        // swap direction to forward
        _currAnimDir = AnimDir::FORWARD;
        _numOfSteps = _origNumOfSteps;
      }
    }
  }
}

void PositionAnimation::executeInfiniteForward() {
  --_numOfSteps;

  // move to X axis
  _img->moveRight(_stepX);

  // move to Y axis
  _img->moveDown(_stepY);

  // animation has reached it's end
  if (0 == _numOfSteps) {
    // check for X axis remainder
    if (_remainderX) {
      _img->moveRight(_remainderX);
    }

    // check for Y axis remainder
    if (_remainderY) {
      _img->moveDown(_remainderY);
    }

    // swap direction to backwards
    _currAnimDir = AnimDir::BACKWARD;
    _numOfSteps = _origNumOfSteps;
  }
}

void PositionAnimation::executeInfiniteBackward() {
  --_numOfSteps;

  // move to X axis
  _img->moveLeft(_stepX);

  // move to Y axis
  _img->moveUp(_stepY);

  // animation has reached it's end
  if (0 == _numOfSteps) {
    // check for X axis remainder
    if (_remainderX) {
      _img->moveLeft(_remainderX);
    }

    // check for Y axis remainder
    if (_remainderY) {
      _img->moveUp(_remainderY);
    }

    // If there is an callback attached -> execute it
    if (nullptr != _endCb) {
      _endCb->onAnimationCycle();
    }

    // swap direction to forward
    _currAnimDir = AnimDir::FORWARD;
    _numOfSteps = _origNumOfSteps;
  }
}

void PositionAnimation::swapDirection() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, PositionAnimation::swapDirection() could not be "
        "performed, because configuration is incomplete. Consider using "
        "PositionAnimation::configure() first");

    return;
  }

  if (PosAnimType::BI_DIRECTIONAL == _posAnimDir) {
    LOGERR(
        "Error, PositionAnimation::swapDirection() could not be "
        "performed, because is configured to be of type BI_DIRECTIONAL");

    return;
  }

  if (AnimType::INFINITE == _animType) {
    LOGERR(
        "Error, PositionAnimation::swapDirection() could not be "
        "performed, because is configured to be of type INFINITE");

    return;
  }

  if (isActiveTimerId(_cfg.timerId)) {
    LOGERR(
        "Error, PositionAnimation::swapDirection() could not be "
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

  _numOfSteps = _origNumOfSteps;
}

void PositionAnimation::forceInternalsReset() {
  _numOfRepeats = _origNumOfRepeats;
  _numOfSteps = _origNumOfSteps;
  _currAnimDir = _cfg.animDirection;
  _img->setPosition(_cfg.startPos);
}
