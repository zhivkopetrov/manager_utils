#ifndef MANAGER_UTILS_BUTTONBASE_H_
#define MANAGER_UTILS_BUTTONBASE_H_

/*
 * Button.h
 *
 *  Brief: The class should be inherited by every new individual Button class
 *         Example:
 *         #include "ButtonBase.h"
 *         class ExitButton : public ButtonBase
 *         {
 *              virtual void handleEvent(const InputEvent& e) override
 *              {
 *                  //some concrete implementation
 *              }
 *
 *              virtual void onLeave() override
 *              {
 *                  //some concrete implementation
 *              }
 *
 *              virtual void onReturn() override
 *              {
 *                  //some concrete implementation
 *              }
 *         };
 *
 *         NOTE: overriding onLeave() and onReturn() functions are optional.
 *               If it's not overridden the default implementation will be used.
 */

// System headers

// Other libraries headers
#include "sdl_utils/input/TouchEntity.h"
#include "utils/class/NonCopyable.h"

// Own components headers
#include "manager_utils/drawing/Image.h"

// Forward declarations
class InputEvent;

class ButtonBase : public TouchEntity, public NonCopyable {
 public:
  ButtonBase();

  ButtonBase(ButtonBase&& movedOther);
  ButtonBase& operator=(ButtonBase&& movedOther);

  virtual ~ButtonBase() noexcept = default;

  /** @brief used to handle user input event down the event hierarchy
   *
   *  @param const Event & - user input event
   * */
  virtual void handleEvent(const InputEvent& e) = 0;

  //=============== START TouchEntity related functions ==================

  /** @brief used to check whether event is inside
   *                                       current frameRectangle or not
   *         NOTE: concrete implementation of TouchEntity function
   *
   *  @returns bool     - isInside or not
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

  /** @brief used to lock this TouchEntity for incoming InputEvent
   * */
  void lockInput() override {
    _isInputUnlocked = false;
    _buttonTexture.setFrame(DISABLED);
  }

  /** @brief used to unlock this TouchEntity for incoming InputEvent
   * */
  void unlockInput() override {
    _isInputUnlocked = true;
    _buttonTexture.setFrame(UNCLICKED);
  }

  //================ END TouchEntity related functions ===================

  //================== START Image related functions =====================

  /** @brief used to create _buttonTexture resource. This function must
   *         be called in order to operate will be resource.
   *         The Image itself only handles draw specific data,
   *         not the actual Surface/Texture.
   *   This function does not return error code for performance reasons
   *
   *  @param const uint64_t - unique resource ID
   * */
  void create(const uint64_t rsrcId);

  /** @brief used to destroy underlying button texture
   * */
  void destroyTexture();

  void draw() const;

  /** @brief used to set _buttonTexture position.
   *
   *  @param const int32_t - new X coordinate
   *  @param const int32_t - new Y coordinate
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void setPosition(const int32_t x, const int32_t y);

  /** @brief used to set _buttonTexture position.
   *
   *  @param const Point - new coordinates
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void setPosition(const Point& pos);

  /** @brief used to set _buttonTexture X coordinate.
   *
   *  @param const int32_t - new X coordinate
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void setX(const int32_t x);

  /** @brief used to set _buttonTexture Y coordinate.
   *
   *  @param const int32_t - new Y coordinate
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void setY(const int32_t y);

  /** @brief used to set Image Rect to point to a specific frame Rect.
   *         Even if Image does not have sprites it is still valid to
   *         call setFrame(0)
   *
   *  @param const int32_t - input frame index
   * */
  void setFrame(const int32_t frameIndex);

  /** @brief used to set next valid frame index.
   *         NOTE: if maxFrames are reached frame 0 is set.
   * */
  void setNextFrame();

  /** @brief used to set previous valid frame index.
   *         NOTE: if -1 index is reached frame _maxFrames - 1 is set.
   * */
  void setPrevFrame();

  /** @brief used to set the _buttonTexture opacity values from (0 - 255),
   *                      respectively from ZERO_OPACITY to FULL OPACITY
   *
   *  @param const int32_t - new Widget opacity
   * */
  void setOpacity(const int32_t opacity);

  /** @brief used to enabled alpha modulation (Widget transparency)
   * */
  void activateAlphaModulation();

  /** @brief used to deactivate  alpha modulation (Widget transparency)
   *         for the Widget
   * */
  void deactivateAlphaModulation();

  /** @brief used to move _buttonTexture vertically.
   *
   *  @param const int32_t - Y coordinate offset
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void moveDown(const int32_t y);

  /** @brief used to move _buttonTexture vertically.
   *
   *  @param const int32_t - Y coordinate offset
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void moveUp(const int32_t y);

  /** @brief used to move _buttonTexture horizontally.
   *
   *  @param const int32_t - X coordinate offset
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void moveLeft(const int32_t x);

  /** @brief used to move _buttonTexture horizontally.
   *
   *  @param const int32_t - X coordinate offset
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void moveRight(const int32_t x);

  bool isCreated() const { return _buttonTexture.isCreated(); }

  const Image &getButtonTexture() const { return _buttonTexture; };
  Image &getButtonTextureMutable() { return _buttonTexture; };

  /** @brief used to get absolute _buttonTexture coordinates
   *
   *  @returns Point - absolute _buttonTexture coordinates
   *
   *  WARNING: even if user defined capture event rectangle is present,
   *           the position of the _originalEventRect are returned.
   * */
  Point getPosition() const {
    return Point(_originalEventRect.x, _originalEventRect.y);
  }

  /** @brief used to get absolute _buttonTexture X coordinate
   *
   *  @returns int32_t - absolute _buttonTexture X coordinate
   *
   *  WARNING: even if user defined capture event rectangle is present,
   *           the X coordinate of the _originalEventRect is returned.
   * */
  int32_t getX() const { return _originalEventRect.x; }

  /** @brief used to get absolute _buttonTexture Y coordinate
   *
   *  @returns int32_t - absolute _buttonTexture Y coordinate
   *
   *  WARNING: even if user defined capture event rectangle is present,
   *           the Y coordinate of the _originalEventRect is returned.
   * */
  int32_t getY() const { return _originalEventRect.y; }

  /** @brief used to get width _buttonTexture dimension
   *
   *  @returns int32_t - _buttonTexture width
   *
   *  WARNING: even if user defined capture event rectangle is present,
   *           the width of the _originalEventRect is returned.
   * */
  int32_t getWidth() const { return _originalEventRect.w; }

  /** @brief used to get height _buttonTexture dimension
   *
   *  @returns int32_t - _buttonTexture height
   *
   *  WARNING: even if user defined capture event rectangle is present,
   *           the height of the _originalEventRect is returned.
   * */
  int32_t getHeight() const { return _originalEventRect.h; }

  /** @brief used to obtain currently set frame index
   *
   *  @param int32_t - frame index
   * */
  int32_t getFrame() const { return _buttonTexture.getFrame(); }

  /** @brief used to obtain maximum frame count
   *
   *  @param int32_t - frame index
   * */
  int32_t getFrameCount() const {
    return _buttonTexture.getFrameCount();
  }

  /** @brief used to get Button bounding rectangle
   *
   *  @returns Rectangle - Button bounding rectangle
   * */
  Rectangle getButtonRect() const {
    return _buttonTexture.getImageRect();
  }

  /** @brief used to get Button event rectangle
   *
   *  @returns Rectangle - Button _originalEventRect rectangle or
   *                     - Button _captureEventRect rectangle if set
   * */
  Rectangle getButtonEventRect() const;

  /** @brief used to manually add an additional frame
   *                                               to the _buttonTexture
   *          Constrains: the frame rectangle must be from the
   *                       same source file image as the original frames
   *
   *  @const Rectangle & - new frame Rectangle dimensions
   * */
  void addFrame(const Rectangle& rectFrame);

  /** @brief used to hide the button widget (so it not be
   *                     unnecessary drawn when draw() method is called)
   * */
  void hide() { _buttonTexture.hide(); }

  /** @brief used to show the button widget (so it will be drawn
   *                                       when draw() method is called)
   * */
  void show() { _buttonTexture.show(); }

  /** @brief used to determine whether the Button's widget
   *                                                    is hidden or not
   *
   *  @return bool - is Button widget hidden or not
   * */
  bool isVisible() const { return _buttonTexture.isVisible(); }

  //=================== END Image related functions ======================

  /** @brief used to set user defined capture event rectangle. If such
   *         is not provided the default image frame rectangle is used
   *
   *  @param const Rectangle & - user provided capture event rectangle
   *
   *  WARNING: _captureEventRect is absolute. It is not affected by moving
   *  functions (e.g. setPosition/setX/setY/moveLeft/moveRight etc... )
   * */
  void setEventCaptureRect(const Rectangle& rect);

  /** @brief used to reset user defined capture event rectangle. By doing
   *         so the default image frame rectangle will be used
   * */
  void resetEventCaptureRect();

  /** @brief used to check whether user defined capture event rectangle
   *                                                               is set.
   *
   *  @return bool - user provided capture event rectangle is set or not
   * */
  bool isEventCaptureRectSet() const { return _isCaptureEventRectSet; }

 protected:
  /** Define ButtonStates in order to use _someButton.setFrame(CLICKED)
   *  instead of _someButton.setFrame(1)
   * */
  enum ButtonStates { UNCLICKED = 0, CLICKED, DISABLED };

 private:
  /** Holds the graphical button representation texture
   * */
  Image _buttonTexture;

  /** Holds original capture event rectangle(_buttonTexture frame
   *                                                         dimensions).
   * */
  Rectangle _originalEventRect;

  /** Holds user defines capture event rectangle. If such is not provided
   *  the default _buttonTexture frame rectangle is used.
   *
   *  WARNING: _captureEventRect is absolute. It is not affected by moving
   *  functions (e.g. setPosition/setX/setY/moveLeft/moveRight etc... )
   * */
  Rectangle _captureEventRect;

  /** Used to check whether _captureEventRect is set or not.
   *
   *  NOTE: this bool variable can be skipped, but it's better to check
   *  if(_isCaptureEventRectSet) rather than
   *  if(Rectangle::UNDEFINED == _captureEventRect), which internally is
   *  8 integers comparison
   * */
  bool _isCaptureEventRectSet;
};

#endif /* MANAGER_UTILS_BUTTONBASE_H_ */
