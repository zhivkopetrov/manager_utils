// Corresponding header
#include "manager_utils/input/Scroller.h"

// C system headers

// C++ system headers
#include <utility>

// Other libraries headers

// Own components headers
#include "sdl_utils/input/InputEvent.h"
#include "utils/LimitValues.hpp"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

// default constructor
Scroller::Scroller()
    : _scrollerType(ScrollerType::UNKNOWN),
      _prevPos(Point::UNDEFINED),
      _movementX(INIT_INT32_VALUE),
      _movementY(INIT_INT32_VALUE),
      _boundaryRect(Rectangle::ZERO) {
}

// move constructor
Scroller::Scroller(Scroller&& movedOther)
    : TouchEntity(std::move(movedOther)),
      _prevPos(Point::UNDEFINED),
      _movementX(0),
      _movementY(0),
      _boundaryRect(Rectangle::ZERO) {
  // take ownership of resources
  _scrollerType = movedOther._scrollerType;
  _boundaryRect = movedOther._boundaryRect;
  _prevPos = movedOther._prevPos;
  _movementX = movedOther._movementX;
  _movementY = movedOther._movementY;

  // ownership of resource should be taken from moved instance
  movedOther.resetInternalVariables();
}

// move assignment operator
Scroller& Scroller::operator=(Scroller&& movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    // take ownership of resources
    _scrollerType = movedOther._scrollerType;
    _boundaryRect = movedOther._boundaryRect;
    _prevPos = movedOther._prevPos;
    _movementX = movedOther._movementX;
    _movementY = movedOther._movementY;

    // explicitly invoke TouchEntity's move assignment operator
    TouchEntity::operator=(std::move(movedOther));

    // ownership of resource should be taken from moved instance
    movedOther.resetInternalVariables();
  }

  return *this;
}

int32_t Scroller::init(const Rectangle& boundaryRect, const Point& startPos,
                       const ScrollerType scrollerType) {
  _boundaryRect = boundaryRect;
  _prevPos = startPos;
  _scrollerType = scrollerType;

  _movementX = 0;
  _movementY = 0;

  touchEntityEventRect = &_boundaryRect;

  return SUCCESS;
}

bool Scroller::containsEvent(const InputEvent& e) {
  if (!isInputUnlocked())  // sanity check
  {
    LOGERR(
        "Warning, you should not call this function when your "
        "TouchEntity is locked. Consider using "
        "TouchEntity::isInputUnlocked() first.");

    return false;
  }

  const bool isInside = _boundaryRect.isPointInRect(e.pos);

  // set last clicked TouchEntity
  if (isInside && TouchEvent::TOUCH_PRESS == e.type) {
    e.setLastClicked(this);
  }

  return isInside;
}

void Scroller::handleEvent(const InputEvent& e) {
  /** Remember position on PRESS in order to do further movement calculations
   *  if there are new DRAG events */
  if (TouchEvent::TOUCH_PRESS == e.type) {
    _prevPos = e.pos;
  } else if (TouchEvent::TOUCH_DRAG == e.type) {
    if (ScrollerType::HORIZONTAL == _scrollerType) {
      if (e.pos.x != _prevPos.x) {
        _movementX = e.pos.x - _prevPos.x;
        _prevPos.x = e.pos.x;
      }
    } else if (ScrollerType::VERTICAL == _scrollerType) {
      if (e.pos.y != _prevPos.y) {
        _movementY = e.pos.y - _prevPos.y;
        _prevPos.y = e.pos.y;
      }
    } else  // ScrollerType::MIXED == _scrollerType
    {
      if (e.pos != _prevPos) {
        _movementX = e.pos.x - _prevPos.x;
        _movementY = e.pos.y - _prevPos.y;
        _prevPos = e.pos;
      }
    }
  } else if (TouchEvent::TOUCH_RELEASE == e.type) {
    onScrollerRelease(e);
    resetScroller();
  }
}

void Scroller::onLeave([[maybe_unused]]const InputEvent& e) {
  resetScroller();
}

void Scroller::onReturn(const InputEvent& e) { _prevPos = e.pos; }

void Scroller::resetScroller() {
  _prevPos = Point::UNDEFINED;

  _movementX = 0;
  _movementY = 0;
}

void Scroller::resetInternalVariables() {
  _scrollerType = ScrollerType::UNKNOWN;
  _boundaryRect = Rectangle::ZERO;
  _prevPos = Point::UNDEFINED;
  _movementX = INIT_INT32_VALUE;
  _movementY = INIT_INT32_VALUE;
}
