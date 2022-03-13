#ifndef MANAGER_UTILS_SCROLLER_H_
#define MANAGER_UTILS_SCROLLER_H_

// System headers
#include <cstdint>

// Other libraries headers
#include "sdl_utils/input/TouchEntity.h"
#include "utils/class/NonCopyable.h"
#include "utils/drawing/Point.h"
#include "utils/drawing/Rectangle.h"
#include "utils/ErrorCode.h"

// Own components headers

// Forward Declarations
class InputEvent;

enum class ScrollerType : uint8_t { UNKNOWN = 0, VERTICAL, HORIZONTAL, MIXED };

class Scroller : public TouchEntity, public NonCopyable {
 public:
  Scroller();

  Scroller(Scroller&& movedOther);
  Scroller& operator=(Scroller&& movedOther);

  //=============== START TouchEntity related functions ==================

  /** @brief used to check whether event is inside
   *                                       current frameRectangle or not
   *         NOTE: concrete implementation of TouchEntity function
   *
   *  @returns bool - isInside or not
   * */
  bool containsEvent(const InputEvent& e) final;

  /** @brief invoked when event was PRESSED inside this TouchEntity
   *         and event leaves the borders of the TouchEntity when no
   *         RELEASE action was performed inside of the same TouchEntity.
   *         NOTE: concrete implementation of TouchEntity function
   *
   *  @param const InputEvent & - engine's InputEvent
   * */
  void onLeave(const InputEvent& e) override;

  /** @brief invoked when event was PRESSED inside this TouchEntity
   *         and event leaves the borders of the TouchEntity when no
   *         RELEASE action was performed inside of the same TouchEntity.
   *         While still action is PRESSED we then again return in the
   *         borders of this TouchEntity.
   *
   *  @param const InputEvent & - engine's InputEvent
   * */
  void onReturn(const InputEvent& e) override;

  /** @brief called when the scroller receives a TouchEvent::Release
   * */
  virtual void onScrollerRelease(const InputEvent& e) = 0;

  /** @brief used to lock this TouchEntity for incoming InputEvent
   * */
  void lockInput() override { _isInputUnlocked = false; }

  /** @brief used to unlock this TouchEntity for incoming InputEvent
   * */
  void unlockInput() override { _isInputUnlocked = true; }

  //================ END TouchEntity related functions ===================

  /** @brief used to initialize the starting position of the scroller
   *         so not unnecessary check if(Point::UNDEFINES != _prevPos)
   *         is done everytime.
   *
   *  @param const Rectangle &  - the scroller boundary rectangle
   *  @param const Point        - the start positon of the scroller
   *  @param const ScrollerType - scroller type
   *
   *  @return ErrorCode         - error code
   * */
  ErrorCode init(const Rectangle& boundaryRect, const Point& startPos,
                 const ScrollerType scrollerType = ScrollerType::VERTICAL);

  /** @brief used to acquire relative X coordinate movement since
   *                                                           last frame
   *
   *  @return int32_t - relative X coordinate movement
   * */
  int32_t getMovementX() const { return _movementX; }

  /** @brief used to acquire relative Y coordinate movement since
   *                                                           last frame
   *
   *  @return int32_t - relative Y coordinate movement
   * */
  int32_t getMovementY() const { return _movementY; }

  /** @brief used to process user input
   *
   *   @param const InputEvent & - user input event
   * */
  virtual void handleEvent(const InputEvent& e);

 protected:
  /** used to internally reset scroller pos and X/Y axis movement
   * */
  void resetScroller();

  ScrollerType _scrollerType;

  Point _prevPos;

  int32_t _movementX;
  int32_t _movementY;

 private:
  void resetInternalVariables();

  Rectangle _boundaryRect;
};

#endif /* MANAGER_UTILS_SCROLLER_H_ */
