#include "manager_utils/input/ButtonBase.h"

// C system headers

// C++ system headers
#include <utility>

// Other libraries headers

// Own components headers
#include "sdl_utils/input/InputEvent.h"
#include "utils/Log.h"

// default constructor
ButtonBase::ButtonBase()
    : _originalEventRect(Rectangle::UNDEFINED),
      _captureEventRect(Rectangle::UNDEFINED),
      _isCaptureEventRectSet(false) {}

// move constructor
ButtonBase::ButtonBase(ButtonBase&& movedOther) {
  // take ownership of resources
  _buttonTexture = std::move(movedOther._buttonTexture);
  _originalEventRect = movedOther._originalEventRect;
  _captureEventRect = movedOther._captureEventRect;
  _isCaptureEventRectSet = movedOther._isCaptureEventRectSet;
  _isInputUnlocked = movedOther._isInputUnlocked;

  if (!_isCaptureEventRectSet) {
    touchEntityEventRect = &_originalEventRect;
  } else {
    touchEntityEventRect = &_captureEventRect;
  }

  // ownership of resource should be taken from moved instance
  movedOther._originalEventRect = Rectangle::UNDEFINED;
  movedOther._captureEventRect = Rectangle::UNDEFINED;
  movedOther._isCaptureEventRectSet = false;
  movedOther.touchEntityEventRect = nullptr;
  movedOther._isInputUnlocked = true;
}

// move assignment operator
ButtonBase& ButtonBase::operator=(ButtonBase&& movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    // take ownership of resources
    _buttonTexture = std::move(movedOther._buttonTexture);
    _originalEventRect = movedOther._originalEventRect;
    _captureEventRect = movedOther._captureEventRect;
    _isCaptureEventRectSet = movedOther._isCaptureEventRectSet;
    _isInputUnlocked = movedOther._isInputUnlocked;

    if (!_isCaptureEventRectSet) {
      touchEntityEventRect = &_originalEventRect;
    } else {
      touchEntityEventRect = &_captureEventRect;
    }

    // ownership of resource should be taken from moved instance
    movedOther._originalEventRect = Rectangle::UNDEFINED;
    movedOther._captureEventRect = Rectangle::UNDEFINED;
    movedOther._isCaptureEventRectSet = false;
    movedOther.touchEntityEventRect = nullptr;
    movedOther._isInputUnlocked = true;
  }

  return *this;
}

bool ButtonBase::containsEvent(const InputEvent& e) {
  // you can not click what your don't see. Logical isn't it?
  if (!_buttonTexture.isVisible()) {
    return false;
  }

  if (!isInputUnlocked())  // sanity check
  {
    LOGERR(
        "Warning, you should not call this function when your "
        "TouchEntity is locked. Consider using "
        "TouchEntity::isInputUnlocked() first.");

    return false;
  }

  const Rectangle &usedRect =
      _isCaptureEventRectSet ? _captureEventRect : _originalEventRect;

  const bool isInside = Rectangle::isPointInRect(e.pos, usedRect);
  if (isInside && TouchEvent::TOUCH_PRESS == e.type) {
    e.setLastClicked(this);
  }
  return isInside;
}

void ButtonBase::onLeave([[maybe_unused]]const InputEvent& e) {
  _buttonTexture.setFrame(UNCLICKED);
}

void ButtonBase::onReturn([[maybe_unused]]const InputEvent& e) {
  _buttonTexture.setFrame(CLICKED);
}

void ButtonBase::create(const uint64_t rsrcId) {
  /** _buttonTexture does not return error code for performance reasons.
   *  If something is wrong there should be at least 10 red messages on the
   *  terminal when binary is ran. */
  _buttonTexture.create(rsrcId);

  _originalEventRect.x = _buttonTexture.getX();
  _originalEventRect.y = _buttonTexture.getY();
  _originalEventRect.w = _buttonTexture.getFrameWidth();
  _originalEventRect.h = _buttonTexture.getFrameHeight();

  touchEntityEventRect = &_originalEventRect;
}

void ButtonBase::destroyTexture() {
  _buttonTexture.destroy();
}

void ButtonBase::draw() {
  _buttonTexture.draw();
}

void ButtonBase::setPosition(const int32_t x, const int32_t y) {
  _originalEventRect.x = x;
  _originalEventRect.y = y;
  _buttonTexture.setPosition(x, y);
}
void ButtonBase::setPosition(const Point& pos) {
  _originalEventRect.x = pos.x;
  _originalEventRect.y = pos.y;
  _buttonTexture.setPosition(pos);
}

void ButtonBase::setX(const int32_t x) {
  _originalEventRect.x = x;
  _buttonTexture.setX(x);
}

void ButtonBase::setY(const int32_t y) {
  _originalEventRect.y = y;
  _buttonTexture.setY(y);
}

void ButtonBase::setFrame(const int32_t frameIndex) {
  _buttonTexture.setFrame(frameIndex);
}

void ButtonBase::setNextFrame() {
  _buttonTexture.setNextFrame();
}

void ButtonBase::setPrevFrame() {
  _buttonTexture.setPrevFrame();
}

void ButtonBase::moveDown(const int32_t y) {
  _originalEventRect.y += y;
  _buttonTexture.moveDown(y);
}

void ButtonBase::moveUp(const int32_t y) {
  _originalEventRect.y -= y;
  _buttonTexture.moveUp(y);
}

void ButtonBase::ButtonBase::moveLeft(const int32_t x) {
  _originalEventRect.x -= x;
  _buttonTexture.moveLeft(x);
}

void ButtonBase::moveRight(const int32_t x) {
  _originalEventRect.x += x;
  _buttonTexture.moveRight(x);
}

void ButtonBase::setEventCaptureRect(const Rectangle& rect) {
  _isCaptureEventRectSet = true;
  _captureEventRect = rect;

  /** User defined capture event rectangle is set ->
   * set TouchEntityEventRect to point to it.
   * */
  touchEntityEventRect = &_captureEventRect;
}

void ButtonBase::resetEventCaptureRect() {
  _isCaptureEventRectSet = false;
  _captureEventRect = Rectangle::UNDEFINED;

  /** User defined capture event rectangle is reset ->
   * set TouchEntityEventRect to point to _originalEventRect.
   * */
  touchEntityEventRect = &_originalEventRect;
}

Rectangle ButtonBase::getButtonEventRect() const {
  if (_isCaptureEventRectSet) {
    return _captureEventRect;
  }

  return _originalEventRect;
}

void ButtonBase::addFrame(const Rectangle& rectFrame) {
  _buttonTexture.addFrame(rectFrame);
}
