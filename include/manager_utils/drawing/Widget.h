#ifndef MANAGER_UTILS_WIDGET_H_
#define MANAGER_UTILS_WIDGET_H_

// System headers
#include <cstdint>

// Other libraries headers
#include "utils/class/NonCopyable.h"

// Own components headers
#include "sdl_utils/drawing/DrawParams.h"

// Forward Declarations

/* Common class for graphical Textures.
 * All graphical textures must inherit from Widget */
class Widget: public NonCopyable {
public:
  Widget();

  Widget(Widget &&movedOther);
  Widget& operator=(Widget &&movedOther);

  /** @brief apply (draw) the widget to the currently active back buffer
   * */
  void draw() const;

  /** @brief set flip type (mirror widget)
   *
   *  @param const WidgetFlipType - /NONE, VERTICAL, HORIZONTAL, etc.../
   * */
  void setFlipType(const WidgetFlipType flipType) {
    _drawParams.widgetFlipType = flipType;
  }

  /** @brief used to set crop rectangle.
   *         NOTE: this method affects Widget's frame rectangle.
   *
   *  @param const Rectangle & - crop rectangle
   * */
  void setCropRect(const Rectangle &cropRect);

  /** @brief used to reset crop rectangle.
   *
   *         NOTE: this method resets crop rectangle and
   *               restores Widget's original frame rectangle.
   *         NOTE2: if Widget already has an active crop enabled and
   *                you try to .setCrop() again -> initially
   *                first .resetCrop() is called and after that
   *                                      .setCrop() for the new request.
   * */
  void resetCrop();

  /** @brief used to check whether we have crop activated.
   *
   *  @returns bool - has crop or not
   * */
  bool hasCrop() const {
    return _drawParams.hasCrop;
  }

  /** @brief used to enabled alpha modulation (Widget transparency)
   * */
  void activateAlphaModulation();

  /** @brief used to deactivate  alpha modulation (Widget transparency)
   *         for the Widget
   * */
  void deactivateAlphaModulation();

  /** @brief used to activate scaling for the Widget
   * */
  void activateScaling();

  /** @brief used to deactivate scaling for the Widget
   * */
  void deactivateScaling();

  /** @brief used to determine whether scaling is active for the Widget
   *
   *  @return bool - scaling is ON or OFF
   *  */
  bool isScalingActive() const {
    return _drawParams.hasScaling;
  }

  /** @brief used to set maximum width for horizontal scaling
   *
   *         NOTE: crop will be recalculated if Widget current
   *                      scaled width is bigger than provided maxWidth
   *
   *  @param const int32_t - maximum horizontal scaling width
   * */
  void setMaxScalingWidth(const int32_t maxWidth);

  /** @brief used to set maximum height for vertical scaling
   *
   *         NOTE: crop will be recalculated if Widget current
   *                      scaled width is bigger than provided maxHeight
   *
   *  @param const int32_t - maximum horizontal scaling height
   * */
  void setMaxScalingHeight(const int32_t maxHeight);

  /** @brief used to set widget scaled width value.
   *
   *         NOTE: this function requires activateScaling() to be already
   *                                             invoked in order to work.
   *
   *  @param const int32_t - new scaled width of the image
   * */
  void setScaledWidth(const int32_t width);

  /** @brief used to set widget scaled height value.
   *
   *         NOTE: this function requires activateScaling() to be already
   *                                             invoked in order to work.
   *
   *  @param const int32_t - new scaled height of the image
   * */
  void setScaledHeight(const int32_t height);

  /** @brief used to scale Widget to his X axis
   *         NOTE: this function requires activateScaling() to be already
   *                                             invoked in order to work.
   *
   *         NOTE2: scaleX must be in boundaries (0.0 - 1.0) non-inclusive
   *
   *  @param const double - scale X
   * */
  void setScaleX(const double scaleX);

  /** @brief used to scale Widget to his Y axis
   *         NOTE: this function requires activateScaling() to be already
   *                                             invoked in order to work.
   *
   *         NOTE2: scaleY must be in range (0.0 - 1.0) non-inclusive
   *
   *  @param const double - scale Y
   * */
  void setScaleY(const double scaleY);

  /** @brief used to scale Widget to both his X and Y axis
   *         NOTE: this function requires activateScaling() to be already
   *                                             invoked in order to work.
   *
   *         NOTE2: scale must be in range (0.0 - 1.0)  non-inclusive
   *
   *  @param const double - scale (for both X and Y axis)
   * */
  void setScale(const double scale);

  /**
   * @brief used to scale Widget to both its X and Y axis but keeps the
   * scaling pivot point to the center of the widget thus making the
   * widget appear to be perfectly center scaled.
   *
   *         NOTE: this function requires activateScaling() to be already
   *                                             invoked in order to work.
   *
   *         NOTE2: scale must be in range (0.0 - 1.0)  non-inclusive
   *
   * @param const double  - scale (for both X and Y axis)
   * @param const Point & - the original position of the widget before
   * scaling occured
   **/
  void setScaleCentered(const double scale, const Point &startPos);

  /** @brief used to scale Widget to both his X and Y axis
   *         NOTE: this function requires activateScaling() to be already
   *                                             invoked in order to work.
   *
   *         NOTE2: scaleX and scaleY must be in range (0.0 - 1.0)
   *                                                        non-inclusive
   *
   *  @param const double - scale X
   *  @param const double - scale Y
   * */
  void setScaleXY(const double scaleX, const double scaleY);

  /** @brief used to set widget position.
   *
   *  @param const int32_t - new X coordinate
   *  @param const int32_t - new Y coordinate
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void setPosition(const int32_t x, const int32_t y);

  /** @brief used to set widget position.
   *
   *  @param const Point & - new coordinates
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void setPosition(const Point &pos);

  /** @brief used to set widget X coordinate.
   *
   *  @param const int32_t - new X coordinate
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void setX(const int32_t x);

  /** @brief used to set widget Y coordinate.
   *
   *  @param const int32_t - new Y coordinate
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void setY(const int32_t y);

  /** @brief used to move widget vertically.
   *
   *  @param const int32_t - Y coordinate offset
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void moveDown(const int32_t y);

  /** @brief used to move widget vertically.
   *
   *  @param const int32_t - Y coordinate offset
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void moveUp(const int32_t y);

  /** @brief used to move widget horizontally.
   *
   *  @param const int32_t - X coordinate offset
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void moveLeft(const int32_t x);

  /** @brief used to move widget horizontally.
   *
   *  @param const int32_t - X coordinate offset
   *         NOTE: the position is relative to the current monitorWindow.
   * */
  void moveRight(const int32_t x);

  /** @brief used to set widget width value.
   *
   *  @param const int32_t - new width of the whole resource
   *         NOTE: the width of the widget is originally set on
   *         resource load up. If you invoke this function you will
   *         override the value.
   *         NOTE2: this function modifies full image width
   *         (not current frame width).
   * */
  void setImageWidth(const int32_t width) {
    _imageWidth = width;
  }

  /** @brief used to set widget height value.
   *
   *  @param const int32_t - new height of the whole resource
   *         NOTE: the height of the widget is originally set on
   *         resource load up. If you invoke this function you will
   *         override the value.
   *         NOTE2: this function modifies full image height
   *         (not current frame width).
   * */
  void setImageHeight(const int32_t height) {
    _imageHeight = height;
  }

  /** @brief used to set frame width value.
   *
   *  @param const int32_t - new width of the frame
   *         NOTE: the width of the frame is originally set on
   *         resource load up. If you invoke this function you will
   *         override the value.
   *         NOTE2: this function modifies frame image width
   *         (not full image width).
   * */
  void setFrameWidth(const int32_t width);

  /** @brief used to set frame height value.
   *
   *  @param const int32_t - new height of the frame
   *         NOTE: the height of the frame is originally set on
   *         resource load up. If you invoke this function you will
   *         override the value.
   *         NOTE2: this function modifies frame image height
   *         (not full image width).
   * */
  void setFrameHeight(const int32_t height);

  /** @brief used to set frame Rectangle value.
   *
   *  @param const Rectangle & - new frame rectangle
   *         NOTE: the frame rectangle is originally set on
   *         resource load up. If you invoke this function you will
   *         override the value.
   *         NOTE2: this function modifies the frame rectangle
   *         (not full image rectangle).
   *
   *         WARNING: this method can override an already activated
   *                  scaling dimensions
   * */
  void setFrameRect(const Rectangle &rect);

  /** @brief used to set the coordinates of the point around which
   *                                           rotation will be performed
   *
   *  @param const int32_t - rotation center X coordinate
   *  @param const int32_t - rotation center Y coordinate
   *         NOTE: if rotation center is not set - rotation be done
   *                 around _drawParams.width / 2, _drawParams.height / 2.
   * */
  void setRotationCenter(const int32_t x, const int32_t y) {
    _drawParams.rotCenter.x = x;
    _drawParams.rotCenter.y = y;
  }

  /** @brief used to set the coordinates of the point around which
   *                                           rotation will be performed
   *
   *  @param const Point - rotation center coordinates
   *         NOTE: if rotation center is not set - rotation be done
   *                 around _drawParams.width / 2, _drawParams.height / 2.
   * */
  void setRotationCenter(const Point &pos) {
    _drawParams.rotCenter = pos;
  }

  /** @brief used to set the coordinates of the point around which
   *                                           rotation will be performed
   *
   *  @param const RotationCenterType - rotation center coordinates around
   *           options TOP_LEFT, ORIG_CENTER, SCALED_CENTER
   * */
  void setPredefinedRotationCenter(const RotationCenterType rotCenterType);

  /** @brief used to set absolute rotation angle
   *
   *  @param const double - rotation angle
   *         NOTE: (+) positive values rotates clockwise
   *               (-) negative values rotates anti-clockwise
   * */
  void setRotation(const double rotationAngle) {
    _drawParams.angle = rotationAngle;
  }

  /** @brief used to set Widget opacity values from (0 - 255),
   *                      respectively from ZERO_OPACITY to FULL OPACITY
   *
   *  @param const int32_t - new Widget opacity
   * */
  void setOpacity(const int32_t opacity);

  /** @brief used to rotate widget relative to the current widget
   *                                                      rotation angle
   *
   *  @param const double - rotation angle
   *         NOTE: (+) positive values rotates clockwise
   *               (-) negative values rotates anti-clockwise
   * */
  void rotate(const double angle);

  /** @brief used to get the coordinates of the point around which
   *                                           rotation will be performed
   *
   *  @return const RotationCenterType - rotation center coordinates around
   *           options TOP_LEFT, ORIG_CENTER, SCALED_CENTER
   *
   *  @return Point - rotation center
   * */
  Point getPredefinedRotationCenter(
      const RotationCenterType rotCenterType) const;

  /** @brief used acquire Widget opacity value
   *
   *  @return int32_t - current Widget opacity
   * */
  int32_t getOpacity() const {
    return _drawParams.opacity;
  }

  /** @brief used to acquire absolute rotation angle of the widget
   *
   *  @return double - rotation angle
   *         NOTE: (+) positive values rotates clockwise
   *               (-) negative values rotates anti-clockwise
   * */
  double getRotation() const {
    return _drawParams.angle;
  }

  /** @brief used to get the currently set widget flip type (mirror widget)
   *
   *  @return WidgetFlipType - /NONE, VERTICAL, HORIZONTAL, etc.../
   * */
  WidgetFlipType getFlipType() const {
    return _drawParams.widgetFlipType;
  }

  /** @brief used to get absolute image coordinates
   *
   *  @returns Point - absolute image coordinates
   * */
  Point getPosition() const {
    return _drawParams.pos;
  }

  /** @brief used to get absolute image X coordinate
   *
   *  @returns int32_t - absolute image X coordinate
   * */
  int32_t getX() const {
    return _drawParams.pos.x;
  }

  /** @brief used to get absolute image Y coordinate
   *
   *  @returns int32_t - absolute image Y coordinate
   * */
  int32_t getY() const {
    return _drawParams.pos.y;
  }

  /** @brief used to get frame width dimension
   *
   *  @returns int32_t - frame width dimension
   * */
  int32_t getFrameWidth() const {
    return _origFrameRect.w;
  }

  /** @brief used to get frame height dimension
   *
   *  @returns int32_t - frame height dimension
   * */
  int32_t getFrameHeight() const {
    return _origFrameRect.h;
  }

  /** @brief used to get frame width dimension
   *
   *         NOTE: if crop is not enabled -
   *                                     original frame width is returned
   *
   *  @returns int32_t - frame width dimension
   * */
  int32_t getCroppedFrameWidth() const {
    return _drawParams.frameRect.w;
  }

  /** @brief used to get frame height dimension
   *
   *         NOTE: if crop is not enabled -
   *                                    original frame height is returned
   *
   *  @returns int32_t - frame height dimension
   * */
  int32_t getCroppedFrameHeight() const {
    return _drawParams.frameRect.h;
  }

  /** @brief used to get frame rectangle
   *
   *  @returns int32_t - frame rectangle
   * */
  Rectangle getFrameRect() const {
    return _drawParams.frameRect;
  }

  /** @brief used to get Image/Widget bounding rectangle
   *  WARNING: even if crop is enabled -> original Image Rectangle is
   *                                                            returned.
   *
   *  @returns Rectangle - Image/Widget bounding rectangle
   * */
  Rectangle getImageRect() const {
    return Rectangle(_drawParams.pos, _origFrameRect.w, _origFrameRect.h);
  }

  /** @brief used to get Image/Widget bounding scaled rectangle
   *  WARNING: even scaling is not activated
   *           a 0 width/height rectangle will be returned
   *
   *  @returns Rectangle - Image/Widget scaled bounding rectangle
   * */
  Rectangle getScaledRect() const {
    return Rectangle(_drawParams.pos, _drawParams.scaledWidth,
        _drawParams.scaledHeight);
  }

  /** @brief used to get absolute cropped image rectangle
   *
   *         WARNING: if crop is disabled - original image rectangle is
   *                                                            returned.
   *
   *  @returns Rectangle - absolute cropped image rectangle
   * */
  Rectangle getCropRect() const {
    return
        _drawParams.hasCrop ?
            _drawParams.frameCropRect :
            Rectangle(_drawParams.pos, _drawParams.frameRect.w,
                _drawParams.frameRect.h);
  }

  // Gets image dimensions
  // Warning this gives the full image dimension

  /** @brief used to get full image width dimension
   *         NOTE: not to be mistaken with getFrameWidth()
   *
   *  @returns int32_t - full image width
   * */
  int32_t getImageWidth() const {
    return _imageWidth;
  }

  /** @brief used to get full image height dimension
   *         NOTE: not to be mistaken with getFrameHeight()
   *
   *  @returns int32_t - full image height
   * */
  int32_t getImageHeight() const {
    return _imageHeight;
  }

  /** @brief used to acquire the widget scaled width value.
   *
   *         NOTE: this function requires activateScaling() to be already
   *                                             invoked in order to work.
   *
   *  @return int32_t - the scaled width of the image
   * */
  int32_t getScaledWidth() const;

  /** @brief used to acquire the widget scaled height value.
   *
   *         NOTE: this function requires activateScaling() to be already
   *                                             invoked in order to work.
   *
   *  @return int32_t - the scaled height of the image
   * */
  int32_t getScaledHeight() const;

  /** @brief used to acquire the widget scaled width value.
   *
   *         NOTE: this function requires activateScaling() to be already
   *                                             invoked in order to work.
   *
   *  @return int32_t - the maximum scaled width of the image
   * */
  int32_t getMaxScalingWidth() const;

  /** @brief used to acquire the widget scaled height value.
   *
   *         NOTE: this function requires activateScaling() to be already
   *                                             invoked in order to work.
   *
   *  @return int32_t - the maximum scaled height of the image
   * */
  int32_t getMaxScalingHeight() const;

  /** @brief used for making a copy of the current Widget DrawParams
   *
   *  @param DrawParams - current Widget DrawParams
   * */
  DrawParams getDrawParams() const {
    return _drawParams;
  }

  /** @brief used to determine whether the widget is created or not
   *
   *  @return bool - is Widget created or not
   * */
  bool isCreated() const {
    return _isCreated;
  }

  /** @brief used to hide widget (so it not be unnecessary drawn
   *                                       when draw() method is called)
   * */
  void hide() {
    _isVisible = false;
  }

  /** @brief used to show the widget (so it will be drawn
   *                                       when draw() method is called)
   * */
  void show() {
    _isVisible = true;
  }

  /** @brief used to determine whether the widget is hidden or not
   *
   *  @return bool - is Widget hidden or not
   * */
  bool isVisible() const {
    return _isVisible;
  }

protected:
  /** @brief used to reset all Widget internal data. This function should
   *         be used when Widget is being moved (e.g. move constructed or
   *         move assigned) or when Widget is being destroyed
   *         (e.g. Image::destroy() or Text::destroy())
   * */
  void reset();

  // Draw parameters needed for the renderer to perform a draw call
  DrawParams _drawParams;

  // flag to determine if Widget is created in order not to create it twice
  bool _isCreated;

  // flag to determine if Widget is visible or not
  bool _isVisible;

  /** @brief used to determine whether alpha modulation
   *         (Widget transparency) is enabled for the Widget
   * */
  bool _isAlphaModulationEnabled;

  /** Widget full dimensions
   *      NOTE: > for images/sprites those hold the whole resource
   *              dimensions (combined width/height of all sprites
   *                                                 in the sprite sheet)
   *
   *            > for Texts those hold the total Image surface
   *                                                     width and height
   * */
  int32_t _imageWidth;
  int32_t _imageHeight;

private:
  /** @brief used to apply crop
   * */
  void applyCrop();

  /** @brief used to apply crop over the scaled Widget dimensions
   * */
  void applyScaledCrop();

  /** Holds maximum scaling dimension for the image
   *                                              (if scaling is enabled)
   *  */
  int32_t _maxScalingWidth;
  int32_t _maxScalingHeight;

  /** Holds scaleFactor
   *                (_origFrameRect.w/h / _drawParams.scaledWidth/Height)
   * */
  double _scaleXFactor;
  double _scaleYFactor;

  /** Used to hold crop boundaries for Widget
   */
  Rectangle _cropRectangle;

  /** Holds the original value of frame rectangle so it can be restored
   *  if crop is reset
   * */
  Rectangle _origFrameRect;
};

#endif /* MANAGER_UTILS_WIDGET_H_ */
