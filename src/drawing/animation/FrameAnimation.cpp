// Corresponding header
#include "manager_utils/drawing/animation/FrameAnimation.h"

// C system headers

// C++ system headers
#include <utility>

// Other libraries headers

// Own components headers
#include "manager_utils/drawing/animation/AnimationEndCb.hpp"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

// default constructor
FrameAnimation::FrameAnimation()
    : _animType(AnimType::UNKNOWN),
      _repeatIndex(0),
      _numOfRepeats(0),
      _origNumOfRepeats(0),
      _origFrameCount(0) {}

// move constructor
FrameAnimation::FrameAnimation(FrameAnimation&& movedOther)
    : AnimationBase(
          std::move(movedOther))  // invoke AnimationBase move constructor
{
  // take ownership of resources
  _animType = movedOther._animType;
  _repeatIndex = movedOther._repeatIndex;
  _numOfRepeats = movedOther._numOfRepeats;
  _origNumOfRepeats = movedOther._origNumOfRepeats;
  _origFrameCount = movedOther._origFrameCount;

  // ownership of resource should be taken from moved instance
  movedOther.resetConfigInternal();
}

// move assignment operator implementation
FrameAnimation& FrameAnimation::operator=(FrameAnimation&& movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    // take ownership of resources
    _animType = movedOther._animType;
    _repeatIndex = movedOther._repeatIndex;
    _numOfRepeats = movedOther._numOfRepeats;
    _origNumOfRepeats = movedOther._origNumOfRepeats;
    _origFrameCount = movedOther._origFrameCount;

    // explicitly invoke AnimationBase move assignment operator
    AnimationBase::operator=(std::move(movedOther));

    // ownership of resource should be taken from moved instance
    movedOther.resetConfigInternal();
  }

  return *this;
}

int32_t FrameAnimation::configure(const AnimBaseConfig& cfg,
                                  AnimationEndCb* endCb,
                                  const AnimType animType,
                                  const uint16_t repeatIndex,
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
    if (AnimDir::BACKWARD == _cfg.animDirection) {
      _img->setFrame(_img->getFrameCount() - 1);  // set last frame
    }

    _origFrameCount = _img->getFrameCount();

    _animType = animType;

    _repeatIndex = repeatIndex;

    _numOfRepeats = numberOfRepeats;
    _origNumOfRepeats = numberOfRepeats;

    if (_repeatIndex >= _origFrameCount) {
      LOGERR(
          "Error configuration not complete. Reason: bad repeatIndex: "
          "%hu, because totalImageFrames: %d",
          _repeatIndex, _img->getFrameCount());
      err = FAILURE;
    }
  }

  if (SUCCESS == err) {
    _isCfgComplete = true;
  } else  // FAILURE == true
  {
    LOGERR("FrameAnimation configuration failed!");
    resetConfigInternal();
  }

  return err;
}

void FrameAnimation::swapDirection() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, FrameAnimation::swapDirection() could not be performed, "
        "because configuration is incomplete. Consider using "
        "PositionAnimation::configure() first");
    return;
  }

  if (AnimType::INFINITE == _animType) {
    LOGERR(
        "Error, FrameAnimation::swapDirection() could not be "
        "performed, because it is configured to be of type INFINITE");
    return;
  }

  if (isActiveTimerId(_cfg.timerId)) {
    LOGERR(
        "Error, FrameAnimation::swapDirection() could not be "
        "performed, because animation is still running. Wait for it to "
        "finish and then invoke .swapDirection() method.");

    return;
  }

  // swap current animation direction
  if (AnimDir::FORWARD == _cfg.animDirection) {
    _cfg.animDirection = AnimDir::BACKWARD;
    _img->setFrame(_img->getFrameCount() - 1);  // set last frame
  } else if (AnimDir::BACKWARD == _cfg.animDirection) {
    _cfg.animDirection = AnimDir::FORWARD;
    _img->setFrame(0);  // set first frame
  }
}

void FrameAnimation::setFirstFrame() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, setFirstFrame for FrameAnimation could not be performed,"
        " because configuration is incomplete. Consider using "
        "FrameAnimation::configure() first");

    return;
  }

  if (isActiveTimerId(_cfg.timerId)) {
    LOGERR(
        "Error, .setFirstFrame() can not be invoked when "
        "the animation is running");
    return;
  }

  if (AnimDir::FORWARD == _cfg.animDirection) {
    _img->setFrame(0);
  } else  // AnimDir::BACKWARD == _cfg.animDirection
  {
    _img->setFrame(_origFrameCount - 1);
  }
}

void FrameAnimation::setLastFrame() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, setLastFrame for FrameAnimation could not be performed, "
        "because configuration is incomplete. Consider using "
        "FrameAnimation::configure() first");

    return;
  }

  if (isActiveTimerId(_cfg.timerId)) {
    LOGERR(
        "Error, .setLastFrame() can not be invoked when "
        "the animation is running");

    return;
  }

  if (AnimDir::FORWARD == _cfg.animDirection) {
    _img->setFrame(_origFrameCount - 1);
  } else  // AnimDir::BACKWARD == _cfg.animDirection
  {
    _img->setFrame(0);
  }
}

void FrameAnimation::resetConfigInternal() {
  AnimationBase::resetConfigInternal();

  _animType = AnimType::UNKNOWN;
  _repeatIndex = 0;
  _numOfRepeats = 0;
  _origNumOfRepeats = 0;
  _origFrameCount = 0;
}

void FrameAnimation::start() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, FrameAnimation could not be started, because "
        "configuration is incomplete. Consider using "
        "FrameAnimation::configure() first");

    return;
  }

  TimerGroup timerGroup = _cfg.isTimerPauseble ? TimerGroup::INTERRUPTIBLE
                                               : TimerGroup::NON_INTERRUPTIBLE;

  startTimer(_cfg.timerInterval, _cfg.timerId, TimerType::PULSE, timerGroup);
}

void FrameAnimation::stop() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, FrameAnimation could not be stopped, because "
        "configuration is incomplete. Consider using "
        "FrameAnimation::configure() first");

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
    if (AnimDir::FORWARD == _cfg.animDirection) {
      if (isActiveTimerId(_cfg.timerId)) {
        _endCb->onAnimationEnd();
      }
    } else  // AnimType::BACKWARD == _cfg.animDirection
    {
      if (isActiveTimerId(_cfg.timerId)) {
        _endCb->onAnimationEnd();
      }
    }
  }

  // stop animation timer
  if (isActiveTimerId(_cfg.timerId)) {
    stopTimer(_cfg.timerId);
  }
}

void FrameAnimation::reset() {
  if (false == _isCfgComplete) {
    LOGERR(
        "Error, FrameAnimation could not be reset, because "
        "configuration is incomplete. Consider using "
        "FrameAnimation::configure() first");
    return;
  }

  if (isActiveTimerId(_cfg.timerId)) {
    stopTimer(_cfg.timerId);
  }

  _numOfRepeats = _origNumOfRepeats;

  if (AnimDir::FORWARD == _cfg.animDirection) {
    _img->setFrame(0);  // set first frame
  } else               // AnimType::BACKWARD == _cfg.animDirection
  {
    _img->setFrame(_origFrameCount - 1);  // set last frame
  }
}

void FrameAnimation::onTimeout(const int32_t timerId) {
  if (timerId == _cfg.timerId) {
    if (AnimType::FINITE == _animType) {
      if (AnimDir::FORWARD == _cfg.animDirection) {
        executeFiniteForward();
      } else  // AnimDirection::BACKWARD == cfg.animDirection
      {
        executeFiniteBackward();
      }
    } else  // INFINITE animations
    {
      if (AnimDir::FORWARD == _cfg.animDirection) {
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

void FrameAnimation::executeFiniteForward() {
  _img->setNextFrame();

  // animation has ended
  if (_origFrameCount - 1 == _img->getFrame()) {
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
      if (0 != _repeatIndex) {
        // there is repeatIndex set -> use it
        _img->setFrame(_repeatIndex);
      }
    }
  }
}

void FrameAnimation::executeFiniteBackward() {
  _img->setPrevFrame();

  // animation has restarted
  if (0 == _img->getFrame()) {
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
      if (0 != _repeatIndex) {
        // there is repeatIndex set -> use it
        _img->setFrame(_origFrameCount - _repeatIndex);
      }
    }
  }
}

void FrameAnimation::executeInfiniteForward() {
  _img->setNextFrame();

  // animation has restarted
  if (_origFrameCount - 1 == _img->getFrame()) {
    // If there is an callback attached -> execute it
    if (nullptr != _endCb) {
      _endCb->onAnimationCycle();
    }

    if (0 != _repeatIndex) {
      // there is repeatIndex set -> use it
      _img->setFrame(_repeatIndex);
    }
  }
}

void FrameAnimation::executeInfiniteBackward() {
  _img->setPrevFrame();

  // animation has ended
  if (0 == _img->getFrame()) {
    // If there is an callback attached -> execute it
    if (nullptr != _endCb) {
      _endCb->onAnimationCycle();
    }

    if (0 != _repeatIndex) {
      // there is repeatIndex set -> use it
      _img->setFrame(_origFrameCount - _repeatIndex);
    }
  }
}
