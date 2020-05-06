#ifndef MANAGER_UTILS_SCROLLER_H_
#define MANAGER_UTILS_SCROLLER_H_

// C system headers

// C++ system headers
#include <cstdint>

// Other libraries headers

// Own components headers
#include "sdl_utils/input/TouchEntity.h"
#include "utils/drawing/Point.h"
#include "utils/drawing/Rectangle.h"

// Forward Declarations
class InputEvent;

enum class ScrollerType : uint8_t { UNKNOWN = 0, VERTICAL, HORIZONTAL, MIXED };

class Scroller : public TouchEntity {
 public:
  Scroller();
  virtual ~Scroller() = default;

  // move constructor
  Scroller(Scroller&& movedOther);

  // move assignment operator implementation
  Scroller& operator=(Scroller&& movedOther);

  // forbid the copy constructor and copy assignment operator
  Scroller(const Scroller& other) = delete;
  Scroller& operator=(const Scroller& other) = delete;

  //=============== START TouchEntity related functions ==================

  /** @brief used to check whether event is inside
   *                                       current frameRectangle or not
   *         NOTE: concrete implementation of TouchEntity function
   *
   *  @param Graphics & - graphics instance
   *
   *  @returns bool     - isInside or not
   * */
  virtual bool containsEvent(const InputEvent& e) override final;

  /** @brief invoked when event was PRESSED inside this TouchEntity
   *         and event leaves the borders of the TouchEntity when no
   *         RELEASE action was performed inside of the same TouchEntity.
   *         NOTE: concrete implementation of TouchEntity function
   *
   *  @param const InputEvent & - engine's InputEvent
   * */
  virtual void onLeave(const InputEvent& e) override;

  /** @brief invoked when event was PRESSED inside this TouchEntity
   *         and event leaves the borders of the TouchEntity when no
   *         RELEASE action was performed inside of the same TouchEntity.
   *         While still action is PRESSED we then again return in the
   *         borders of this TouchEntity.
   *
   *  @param const InputEvent & - engine's InputEvent
   * */
  virtual void onReturn(const InputEvent& e) override;

  /** @brief called when the scroller receives a TouchEvent::Release
   * */
  virtual void onScrollerRelease(const InputEvent& e) = 0;

  /** @brief used to lock this TouchEntity for incoming InputEvent
   * */
  virtual void lockInput() override { _isInputUnlocked = false; }

  /** @brief used to unlock this TouchEntity for incoming InputEvent
   * */
  virtual void unlockInput() override { _isInputUnlocked = true; }

  //================ END TouchEntity related functions ===================

  /** @brief used to initialize the starting position of the scroller
   *         so not unnecessary check if(Point::UNDEFINES != _prevPos)
   *         is done everytime.
   *
   *  @param const Rectangle &  - the scroller boundary rectangle
   *  @param const Point        - the start positon of the scroller
   *  @param const ScrollerType - scroller type
   *
   *  @return int32_t           - error code
   * */
  int32_t init(const Rectangle& boundaryRect, const Point& startPos,
               const ScrollerType scrollerType = ScrollerType::VERTICAL);

  /** @brief used to acquire relative X coordinate movement since
   *                                                           last frame
   *
   *  @return int32_t - relative X coordinate movement
   * */
  inline int32_t getMovementX() const { return _movementX; }

  /** @brief used to acquire relative Y coordinate movement since
   *                                                           last frame
   *
   *  @return int32_t - relative Y coordinate movement
   * */
  inline int32_t getMovementY() const { return _movementY; }

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
