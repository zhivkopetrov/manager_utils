// Corresponding header
#include "manager_utils/drawing/animation/AnimationBase.h"

// System headers
#include <utility>

// Other libraries headers
#include "utils/Log.h"

// Own components headers
#include "manager_utils/drawing/Fbo.h"
#include "utils/LimitValues.h"

AnimationBase::AnimationBase()
    : _img(nullptr), _endCb(nullptr), _isVisible(true), _isCfgComplete(false) {}

AnimationBase::~AnimationBase() noexcept {
  if (AnimImageType::INTERNAL == _cfg.animImageType) {
    if (nullptr != _img) {
      delete _img;
      _img = nullptr;
    }
  }
}

AnimationBase::AnimationBase(AnimationBase&& movedOther)
    : TimerClient(std::move(movedOther)),
      _endCb(nullptr),
      _isVisible(true),
      _isCfgComplete(false) {
  // take ownership of resources
  _cfg = movedOther._cfg;
  _endCb = movedOther._endCb;
  _img = movedOther._img;
  _isVisible = movedOther._isVisible;
  _isCfgComplete = movedOther._isCfgComplete;

  // ownership of resource should be taken from moved instance
  movedOther.resetConfigInternal();
}

AnimationBase& AnimationBase::operator=(AnimationBase&& movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    // take ownership of resources
    _cfg = movedOther._cfg;
    _endCb = movedOther._endCb;
    _img = movedOther._img;
    _isVisible = movedOther._isVisible;
    _isCfgComplete = movedOther._isCfgComplete;

    // explicitly invoke TimerClient move assignment operator
    TimerClient::operator=(std::move(movedOther));

    // ownership of resource should be taken from moved instance
    movedOther.resetConfigInternal();
  }

  return *this;
}

ErrorCode AnimationBase::configureInternal(const AnimBaseConfig& cfg,
                                           AnimationEndCb* endCb) {
  _cfg = cfg;

  _endCb = endCb;

  if (AnimImageType::INTERNAL == cfg.animImageType) {
    if (0 == _cfg.rsrcId) {
      LOGERR("Error, rsrcId not provided for animation!");
      return ErrorCode::FAILURE;
    }

    _img = new Image;
    _img->create(_cfg.rsrcId);
  } else {
    _img = cfg.externalImage;
  }

  if (Points::UNDEFINED != _cfg.startPos) {
    _img->setPosition(_cfg.startPos);
  } else {
    _cfg.startPos = _img->getPosition();
  }

  if (AnimDir::UNKNOWN == _cfg.animDirection) {
    LOGERR("Error, animation with rsrcId: %#16lX has AnimDir::UNKNOWN, "
           "which is forbidden. Consider using AnimDir::FORWARD or "
           "AnimDir::BACKWARD", _cfg.rsrcId);
    return ErrorCode::FAILURE;
  }

  if (INIT_INT32_VALUE == _cfg.timerId) {
    LOGERR("Error, timerId not provided for animation with rsrcId: %#16lX",
           _cfg.rsrcId);
    return ErrorCode::FAILURE;
  }

  if (20 > _cfg.timerInterval) {
    LOGERR("Error, timeInterval of %ld is forbidden. The minimum "
           "possible timerInterval is 20ms", _cfg.timerInterval);
    return ErrorCode::FAILURE;
  }

  return ErrorCode::SUCCESS;
}

void AnimationBase::resetConfigInternal() {
  if (AnimImageType::INTERNAL == _cfg.animImageType) {
    if (nullptr != _img) {
      _img->destroy();
      delete _img;
      _img = nullptr;
    }
  }

  _cfg.reset();
  _endCb = nullptr;
  _isVisible = true;
  _isCfgComplete = false;
}

bool AnimationBase::isAnimationActive() const {
  if (false == _isCfgComplete) {
    return false;
  }

  if (isActiveTimerId(_cfg.timerId)) {
    return true;
  }

  return false;
}

void AnimationBase::draw() const {
  if (_isVisible) {
    _img->draw();
  }
}

void AnimationBase::drawOnFbo(Fbo& fbo) const {
  if (_isVisible) {
    fbo.addWidget(*_img);
  }
}

void AnimationBase::setAnimOpacity(const int32_t opacity) {
  _img->setOpacity(opacity);
}

void AnimationBase::activateAnimationAlphaModulation() {
  _img->activateAlphaModulation();
}

void AnimationBase::setPosition(const int32_t x, const int32_t y) {
  _img->setPosition(x, y);
  _cfg.startPos.x = x;
  _cfg.startPos.y = y;
}

void AnimationBase::setX(const int32_t x) {
  _img->setX(x);
  _cfg.startPos.x = x;
}

void AnimationBase::setY(const int32_t y) {
  _img->setY(y);
  _cfg.startPos.y = y;
}

void AnimationBase::setPosition(const Point& pos) {
  _img->setPosition(pos);
  _cfg.startPos = pos;
}

void AnimationBase::moveRight(const int32_t dX) {
  _img->moveRight(dX);
  _cfg.startPos.x += dX;
}

void AnimationBase::moveLeft(const int32_t dX) {
  _img->moveLeft(dX);
  _cfg.startPos.x -= dX;
}

void AnimationBase::moveUp(const int32_t dY) {
  _img->moveUp(dY);
  _cfg.startPos.y -= dY;
}

void AnimationBase::moveDown(const int32_t dY) {
  _img->moveDown(dY);
  _cfg.startPos.y += dY;
}

void AnimationBase::setFrame(const int32_t frameIdx) {
  _img->setFrame(frameIdx);
}

void AnimationBase::setAnimCropRectangle(const Rectangle& cropRect) {
  _img->setCropRect(cropRect);
}
