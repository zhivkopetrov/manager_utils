#include "manager_utils/input/ButtonBase.h"

// C system headers

// C++ system headers
#include <utility>

// Other libraries headers

// Own components headers
#include "sdl_utils/input/InputEvent.h"
#include "utils/Unused.h"
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

void ButtonBase::onLeave(const InputEvent& e) {
  _buttonTexture.setFrame(UNCLICKED);

  UNUSED(e);
}

void ButtonBase::onReturn(const InputEvent& e) {
  _buttonTexture.setFrame(CLICKED);

  UNUSED(e);
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

void ButtonBase::setEventCaptureRect(const Rectangle& rect) {
  _isCaptureEventRectSet = true;
  _captureEventRect = rect;

  /** User defined capture event rectangle is set ->
   * set TouchEntityEventRect to point to it.
   * */
  touchEntityEventRect = &_captureEventRect;
}

/** @brief used to reset user defined capture event rectangle. By doing
 *         so the default image frame rectangle will be used
 * */
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
