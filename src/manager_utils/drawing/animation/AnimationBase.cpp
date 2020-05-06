// Corresponding header
#include "manager_utils/drawing/animation/AnimationBase.h"

// C system headers

// C++ system headers
#include <cstdlib>
#include <utility>

// Other libraries headers

// Own components headers
#include "manager_utils/drawing/SpriteBuffer.h"
#include "utils/Log.h"

// default constructor
AnimationBase::AnimationBase()
    : _endCb(nullptr), _isVisible(true), _isCfgComplete(false) {}

// move constructor
AnimationBase::AnimationBase(AnimationBase&& movedOther)
    : TimerClient(std::move(movedOther)),
      _endCb(nullptr),
      _isVisible(true),
      _isCfgComplete(false) {
  // take ownership of resources
  _cfg = movedOther._cfg;
  _endCb = movedOther._endCb;
  _img = std::move(movedOther._img);
  _isVisible = movedOther._isVisible;
  _isCfgComplete = movedOther._isCfgComplete;

  // ownership of resource should be taken from moved instance
  movedOther.resetConfigInternal();
}

// move assignment operator implementation
AnimationBase& AnimationBase::operator=(AnimationBase&& movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    // take ownership of resources
    _cfg = movedOther._cfg;
    _endCb = movedOther._endCb;
    _img = std::move(movedOther._img);
    _isVisible = movedOther._isVisible;
    _isCfgComplete = movedOther._isCfgComplete;

    // explicitly invoke TimerClient move assignment operator
    TimerClient::operator=(std::move(movedOther));

    // ownership of resource should be taken from moved instance
    movedOther.resetConfigInternal();
  }

  return *this;
}

int32_t AnimationBase::configureInternal(const AnimBaseConfig& cfg,
                                         AnimationEndCb* endCb) {
  resetConfigInternal();

  _cfg = cfg;

  _endCb = endCb;

  _img.create(_cfg.rsrcId);

  if (Point::UNDEFINED != _cfg.startPos) {
    _img.setPosition(_cfg.startPos);
  } else {
    _cfg.startPos = _img.getPosition();
  }

  if (0 == _cfg.rsrcId) {
    LOGERR("Error, rsrcId not provided for animation!");
    return EXIT_FAILURE;
  }

  if (AnimDir::UNKNOWN == _cfg.animDirection) {
    LOGERR(
        "Error, animation with rsrcId: %#16lX has AnimDir::UNKNOWN, "
        "which is forbidden. Consider using AnimDir::FORWARD or "
        "AnimDir::BACKWARD",
        _cfg.rsrcId);
    return EXIT_FAILURE;
  }

  if (0 == _cfg.timerId) {
    LOGERR(
        "Error, timerId not provided for animation with rsrcId: "
        "%#16lX",
        _cfg.rsrcId);
    return EXIT_FAILURE;
  }

  if (20 > _cfg.timerInterval) {
    LOGERR(
        "Error, timeInterval of %ld is forbidden. The minimum "
        "possible timerInterval is 20ms",
        _cfg.timerInterval);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void AnimationBase::resetConfigInternal() {
  _cfg.reset();
  _img.destroy(); /* Clear previous image (if any) */
  _endCb = nullptr;
  _isVisible = true;
  _isCfgComplete = false;
}

bool AnimationBase::isAnimationActive() const {
  bool isActive = false;

  if (false == _isCfgComplete) {
    LOGERR(
        "Error, Animation could not be checked for active status, "
        "because configuration is incomplete. Consider using "
        "Animation::configure() first");
  }

  if (isActiveTimerId(_cfg.timerId)) {
    isActive = true;
  }

  return isActive;
}

void AnimationBase::draw() {
  if (_isVisible) {
    _img.draw();
  }
}

void AnimationBase::drawOnSpriteBuffer(SpriteBuffer& spriteBuffer) {
  if (_isVisible) {
    spriteBuffer.addWidget(_img);
  }
}
