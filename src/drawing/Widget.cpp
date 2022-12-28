// Corresponding header
#include "manager_utils/drawing/Widget.h"

// System headers
#include <cstring>

// Other libraries headers
#include "sdl_utils/drawing/GeometryUtils.h"
#include "utils/data_type/EnumClassUtils.h"
#include "utils/data_type/FloatingPointUtils.h"
#include "utils/Log.h"

// Own components headers
#include "manager_utils/managers/DrawMgr.h"

namespace {
constexpr auto MAX_SCALE_FACTOR_INTERNAL = MAX_SCALE_FACTOR + 0.01;
}

#define OVERRIDE_SCALING_ON_NEW_FRAME_RECT 0

Widget::Widget()
    : _isCreated(false),
      _isVisible(true),
      _isAlphaModulationEnabled(false),
      _imageWidth(0),
      _imageHeight(0),
      _maxScalingWidth(0),
      _maxScalingHeight(0),
      _scaleXFactor(MIN_SCALE_FACTOR),
      _scaleYFactor(MIN_SCALE_FACTOR),
      _cropRectangle(Rectangles::ZERO),
      _origFrameRect(Rectangles::ZERO) {}

Widget::Widget(Widget &&movedOther) {
  // take ownership of resources
  _drawParams = movedOther._drawParams;
  _isCreated = movedOther._isCreated;
  _isVisible = movedOther._isVisible;
  _isAlphaModulationEnabled = movedOther._isAlphaModulationEnabled;
  _imageWidth = movedOther._imageHeight;
  _imageHeight = movedOther._imageHeight;
  _maxScalingWidth = movedOther._maxScalingWidth;
  _maxScalingHeight = movedOther._maxScalingHeight;
  _scaleXFactor = movedOther._scaleXFactor;
  _scaleYFactor = movedOther._scaleYFactor;
  _cropRectangle = movedOther._cropRectangle;
  _origFrameRect = movedOther._origFrameRect;

  // ownership of resource should be taken from moved instance
  movedOther.reset();
}

Widget &Widget::operator=(Widget &&movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    // take ownership of resources
    _drawParams = movedOther._drawParams;
    _isCreated = movedOther._isCreated;
    _isVisible = movedOther._isVisible;
    _isAlphaModulationEnabled = movedOther._isAlphaModulationEnabled;
    _imageWidth = movedOther._imageHeight;
    _imageHeight = movedOther._imageHeight;
    _maxScalingWidth = movedOther._maxScalingWidth;
    _maxScalingHeight = movedOther._maxScalingHeight;
    _scaleXFactor = movedOther._scaleXFactor;
    _scaleYFactor = movedOther._scaleYFactor;
    _cropRectangle = movedOther._cropRectangle;
    _origFrameRect = movedOther._origFrameRect;

    // ownership of resource should be taken from moved instance
    movedOther.reset();
  }

  return *this;
}

void Widget::draw() const {
  if (!_isCreated) {
    LOGERR(
        "Error, widget with rsrcId: %" PRIu64" not created!", _drawParams.rsrcId);
  } else if (_isVisible) {
    gDrawMgr->addDrawCmd(_drawParams);
  }
}

void Widget::reset() {
  _drawParams.reset();
  _isCreated = false;
  _isVisible = true;
  _isAlphaModulationEnabled = false;
  _imageHeight = 0;
  _maxScalingWidth = 0;
  _maxScalingHeight = 0;
  _scaleXFactor = MIN_SCALE_FACTOR;
  _scaleYFactor = MIN_SCALE_FACTOR;

  /** Widget Rectangle::ZERO is chosen over Rectangle::UNDEFINED,
   * because Rectangle::UNDEFINED is a valid draw rectangle.
   * */
  _cropRectangle = Rectangles::ZERO;
  _origFrameRect = Rectangles::ZERO;
}

void Widget::activateAlphaModulation() {
  if (_isAlphaModulationEnabled) {
    LOGERR(
        "Error, alhaModulation is already enabled for Widget with "
        "rsrcId: %" PRIu64". Will not try to activate it twice.",
        _drawParams.rsrcId);

    return;
  }

  _isAlphaModulationEnabled = true;

  const BlendMode blendMode = BlendMode::BLEND;

  // NOTE: for third parameter either rsrcId(uint64_t) or
  //      uniqueContainerId(int32_t) so the bigger size is chosen
  //      for the buffer population
  uint8_t data[sizeof(_drawParams.widgetType) + sizeof(blendMode) +
               sizeof(_drawParams.rsrcId)];
  uint64_t populatedBytes = 0;

  memcpy(data, &_drawParams.widgetType, sizeof(_drawParams.widgetType));
  populatedBytes += sizeof(_drawParams.widgetType);

  memcpy(data + populatedBytes, &blendMode, sizeof(blendMode));
  populatedBytes += sizeof(blendMode);

  if (WidgetType::IMAGE == _drawParams.widgetType) {
    memcpy(data + populatedBytes, &_drawParams.rsrcId,
           sizeof(_drawParams.rsrcId));
    populatedBytes += sizeof(_drawParams.rsrcId);
  } else  // WidgetType::TEXT or WidgetType::SPRITE_BUFFER
  {
    //textId and and spriteBufferId are the same since they are in a union
    memcpy(data + populatedBytes, &_drawParams.textId,
           sizeof(_drawParams.textId));
    populatedBytes += sizeof(_drawParams.textId);
  }

  gDrawMgr->addRendererCmd(RendererCmd::CHANGE_TEXTURE_BLENDMODE, data,
                           populatedBytes);
}

void Widget::deactivateAlphaModulation() {
  if (!_isAlphaModulationEnabled) {
    LOGERR(
        "Error, alhaModulation was not enabled for Widget with "
        "rsrcId: %" PRIu64". Will not try to deactivate it.",
        _drawParams.rsrcId);

    return;
  }

  _isAlphaModulationEnabled = false;

  const BlendMode blendMode = BlendMode::NONE;

  // NOTE: for third parameter either rsrcId(uint64_t) or
  //      uniqueContainerId(int32_t) so the bigger size is chosen
  //      for the buffer population
  uint8_t data[sizeof(_drawParams.widgetType) + sizeof(blendMode) +
               sizeof(_drawParams.rsrcId)];
  uint64_t populatedBytes = 0;

  memcpy(data, &_drawParams.widgetType, sizeof(_drawParams.widgetType));
  populatedBytes += sizeof(_drawParams.widgetType);

  memcpy(data + populatedBytes, &blendMode, sizeof(blendMode));
  populatedBytes += sizeof(blendMode);

  if (WidgetType::IMAGE == _drawParams.widgetType) {
    memcpy(data + populatedBytes, &_drawParams.rsrcId,
           sizeof(_drawParams.rsrcId));
    populatedBytes += sizeof(_drawParams.rsrcId);
  } else  // WidgetType::TEXT or WidgetType::SPRITE_BUFFER
  {
    //textId and and spriteBufferId are the same since they are in a union
    memcpy(data + populatedBytes, &_drawParams.textId,
           sizeof(_drawParams.textId));
    populatedBytes += sizeof(_drawParams.textId);
  }

  gDrawMgr->addRendererCmd(RendererCmd::CHANGE_TEXTURE_BLENDMODE, data,
                           populatedBytes);
}

void Widget::activateScaling() {
  if (_drawParams.hasScaling) {
    LOGERR(
        "Warning, Scaling is already active for Widget with rsrcId: "
        "%" PRIu64". Current _drawParams.scaledWidth: %d and "
        "_drawParams.scaledHeight: %d will be overridden by their "
        "default values -> width: %d, height: %d ",
        _drawParams.rsrcId, _drawParams.scaledWidth, _drawParams.scaledHeight,
        _origFrameRect.w, _origFrameRect.h);
  }

  _drawParams.hasScaling = true;

  _drawParams.scaledWidth = _origFrameRect.w;
  _drawParams.scaledHeight = _origFrameRect.h;

  _scaleXFactor = MAX_SCALE_FACTOR;
  _scaleYFactor = MAX_SCALE_FACTOR;
}

void Widget::deactivateScaling() {
  _drawParams.hasScaling = false;

  _drawParams.scaledWidth = 0;
  _drawParams.scaledHeight = 0;

  _scaleXFactor = MIN_SCALE_FACTOR;
  _scaleYFactor = MIN_SCALE_FACTOR;

  // if crop is enabled -> recalculate non scaled crop
  if (_drawParams.hasCrop) {
    applyCrop();
  }
}

void Widget::setFrameWidth(const int32_t width) {
  if (0 == width) {
    LOGERR(
        "Error, setFrameWidth() for Widget with rsrcId: "
        "%" PRIu64" with value 0 is forbidden.",
        _drawParams.rsrcId);
    return;
  }

  _drawParams.frameRect.w = width;
  _origFrameRect.w = width;

  if (_drawParams.hasScaling) {
    // check if maxScalingWidth is set
    if (_maxScalingWidth && _maxScalingWidth < width) {
      _drawParams.scaledWidth = _maxScalingWidth;
    } else {
      _drawParams.scaledWidth = width;
    }
  }

  if (_drawParams.hasCrop) {
    if (_drawParams.hasScaling) {
      applyScaledCrop();
    } else {
      applyCrop();
    }
  }
}

void Widget::setFrameHeight(const int32_t height) {
  if (0 == height) {
    LOGERR(
        "Error, setFrameHeight() for Widget with rsrcId: "
        "%" PRIu64" with value 0 is forbidden.",
        _drawParams.rsrcId);
    return;
  }

  _drawParams.frameRect.h = height;
  _origFrameRect.h = height;

  if (_drawParams.hasScaling) {
    // check if maxScalingHeight is set
    if (_maxScalingHeight && _maxScalingHeight < height) {
      _drawParams.scaledHeight = _maxScalingHeight;
    } else {
      _drawParams.scaledHeight = height;
    }
  }

  if (_drawParams.hasCrop) {
    if (_drawParams.hasScaling) {
      applyScaledCrop();
    } else {
      applyCrop();
    }
  }
}

void Widget::setMaxScalingWidth(const int32_t maxWidth) {
  if (0 == maxWidth) {
    LOGERR(
        "Error, setMaxScalingWidth() for Widget with rsrcId: "
        "%" PRIu64" with value 0 is forbidden.",
        _drawParams.rsrcId);
    return;
  }

  _maxScalingWidth = maxWidth;

  // if scaling is turned on -> it needs to be reevaluated
  if (_drawParams.hasScaling) {
    if (_drawParams.scaledWidth > _maxScalingWidth) {
      _drawParams.scaledWidth = _maxScalingWidth;

      // if scaledWidth is changed we need to reevaluate the crop
      //                                              if it is enabled
      if (_drawParams.hasCrop) {
        applyScaledCrop();
      }
    }
  }
}

void Widget::setMaxScalingHeight(const int32_t maxHeight) {
  if (0 == maxHeight) {
    LOGERR(
        "Error, setMaxScalingHeight() for Widget with rsrcId: "
        "%" PRIu64" with value 0 is forbidden.",
        _drawParams.rsrcId);
    return;
  }

  _maxScalingHeight = maxHeight;

  // if scaling is turned on -> it needs to be reevaluated
  if (_drawParams.hasScaling) {
    if (_drawParams.scaledHeight > _maxScalingHeight) {
      _drawParams.scaledHeight = _maxScalingHeight;

      // if scaledWidth is changed we need to reevaluate the crop
      //                                              if it is enabled
      if (_drawParams.hasCrop) {
        applyScaledCrop();
      }
    }
  }
}

void Widget::setScaledWidth(const int32_t width) {
  if (!_drawParams.hasScaling) {
    LOGERR(
        "Error, scaling is not turned on for Widget with rsrcId: %" PRIu64
        ". .setScaledWidth() with param %d, will not be performed",
        _drawParams.rsrcId, width);
    return;
  }
  if (0 == width) {
    LOGERR("Error, setScaledWidth() with value 0 is forbidden.");
    return;
  }

  // check if maxScalingHeight is set
  if (_maxScalingWidth && _maxScalingWidth < width) {
    _drawParams.scaledWidth = _maxScalingWidth;
  } else { //_maxScaling is not set -> simply change the value
    _drawParams.scaledWidth = width;
    _scaleXFactor =
        static_cast<double>(_origFrameRect.w / _drawParams.scaledWidth);
  }

  // if crop is enabled -> it needs to be reevaluated
  if (_drawParams.hasCrop) {
    applyScaledCrop();
  }
}

void Widget::setScaledHeight(const int32_t height) {
  if (!_drawParams.hasScaling) {
    LOGERR(
        "Error, scaling is not turned on for Widget with rsrcId: %" PRIu64
        ". .setScaledHeight() with param %d, will not be performed",
        _drawParams.rsrcId, height);
    return;
  }
  if (0 == height) {
    LOGERR("Error, setScaledHeight() with value 0 is forbidden.");
    return;
  }

  // check if maxScalingHeight is set
  if (_maxScalingHeight && _maxScalingHeight < height) {
    _drawParams.scaledHeight = _maxScalingHeight;
  } else { //_maxScaling is not set -> simply change the value
    _drawParams.scaledHeight = height;
    _scaleYFactor =
        static_cast<double>(_origFrameRect.h / _drawParams.scaledHeight);
  }

  // if crop is enabled -> it needs to be reevaluated
  if (_drawParams.hasCrop) {
    applyScaledCrop();
  }
}

void Widget::setScaleX(const double scaleX) {
  if (!_drawParams.hasScaling) {
    LOGERR(
        "Error, scaling is not turned on for Widget with rsrcId: %" PRIu64
        ". .setScaleX() with param %lf, will not be performed",
        _drawParams.rsrcId, scaleX);
  } else if (FloatingPointUtils::areAlmostEqual(scaleX, MIN_SCALE_FACTOR)) {
    LOGERR(
        "Error, setScaleX() for Widget with rsrcId: "
        "%" PRIu64" with value %lf is forbidden.",
        _drawParams.rsrcId, MIN_SCALE_FACTOR);
  } else if (MIN_SCALE_FACTOR > scaleX || MAX_SCALE_FACTOR_INTERNAL < scaleX) {
    LOGERR(
        "Error, setScaleX() for Widget with rsrcId: %" PRIu64" with param "
        "%lf only takes values in range (%lf - %lf]",
        _drawParams.rsrcId, scaleX, MIN_SCALE_FACTOR, MAX_SCALE_FACTOR);
  } else {
    _scaleXFactor = scaleX;
    _drawParams.scaledHeight = static_cast<int32_t>(_origFrameRect.h * scaleX);
  }
}

void Widget::setScaleY(const double scaleY) {
  if (!_drawParams.hasScaling) {
    LOGERR(
        "Error, scaling is not turned on for Widget with rsrcId: %" PRIu64
        " . .setScaleY() with param %lf, will not be performed",
        _drawParams.rsrcId, scaleY);
  } else if (FloatingPointUtils::areAlmostEqual(scaleY, MIN_SCALE_FACTOR)) {
    LOGERR(
        "Error, setScaleY() for Widget with rsrcId: "
        "%" PRIu64" with value %lf is forbidden.",
        _drawParams.rsrcId, MIN_SCALE_FACTOR);
  } else if (MIN_SCALE_FACTOR > scaleY || MAX_SCALE_FACTOR_INTERNAL < scaleY) {
    LOGERR(
        "Error, setScaleY() for Widget with rsrcId: %" PRIu64" with param "
        "%lf only takes values in range (%lf - %lf]",
        _drawParams.rsrcId, scaleY, MIN_SCALE_FACTOR, MAX_SCALE_FACTOR);
  } else {
    _scaleYFactor = scaleY;
    _drawParams.scaledWidth = static_cast<int32_t>(_origFrameRect.w * scaleY);
  }
}

void Widget::setScale(const double scale) {
  setScaleX(scale);
  setScaleY(scale);
}

void Widget::setScaleCentered(const double scale, const Point &startPos) {
  if (!_drawParams.hasScaling) {
    LOGERR("Error! Scaling not enabled for widget with rsrcId: %" PRIu64,
        _drawParams.rsrcId);
    return;
  }

  const int32_t newScaledWidth = static_cast<int32_t>(_origFrameRect.w * scale);
  const int32_t newScaledHeight =
      static_cast<int32_t>(_origFrameRect.h * scale);

  const int32_t diffX = (_origFrameRect.w - newScaledWidth) >> 1;
  const int32_t diffY = (_origFrameRect.h - newScaledHeight) >> 1;

  setScale(scale);
  setPosition(startPos.x + diffX, startPos.y + diffY);
}

void Widget::setScaleXY(const double scaleX, const double scaleY) {
  setScaleX(scaleX);
  setScaleY(scaleY);
}

void Widget::setPosition(const int32_t x, const int32_t y) {
  _drawParams.pos.x = x;
  _drawParams.pos.y = y;

  if (_drawParams.hasCrop) {
    if (_drawParams.hasScaling) {
      applyScaledCrop();
    } else {
      applyCrop();
    }
  }
}

void Widget::setPosition(const Point &pos) {
  _drawParams.pos = pos;

  if (_drawParams.hasCrop) {
    if (_drawParams.hasScaling) {
      applyScaledCrop();
    } else {
      applyCrop();
    }
  }
}

void Widget::setX(const int32_t x) {
  _drawParams.pos.x = x;

  if (_drawParams.hasCrop) {
    if (_drawParams.hasScaling) {
      applyScaledCrop();
    } else {
      applyCrop();
    }
  }
}

void Widget::setY(const int32_t y) {
  _drawParams.pos.y = y;

  if (_drawParams.hasCrop) {
    if (_drawParams.hasScaling) {
      applyScaledCrop();
    } else {
      applyCrop();
    }
  }
}

void Widget::moveDown(const int32_t y) {
  _drawParams.pos.y += y;

  if (_drawParams.hasCrop) {
    if (_drawParams.hasScaling) {
      applyScaledCrop();
    } else {
      applyCrop();
    }
  }
}

void Widget::moveUp(const int32_t y) {
  _drawParams.pos.y -= y;

  if (_drawParams.hasCrop) {
    if (_drawParams.hasScaling) {
      applyScaledCrop();
    } else {
      applyCrop();
    }
  }
}

void Widget::moveLeft(const int32_t x) {
  _drawParams.pos.x -= x;

  if (_drawParams.hasCrop) {
    if (_drawParams.hasScaling) {
      applyScaledCrop();
    } else {
      applyCrop();
    }
  }
}

void Widget::moveRight(const int32_t x) {
  _drawParams.pos.x += x;

  if (_drawParams.hasCrop) {
    if (_drawParams.hasScaling) {
      applyScaledCrop();
    } else {
      applyCrop();
    }
  }
}

void Widget::setFrameRect(const Rectangle &rect) {
  _drawParams.frameRect = rect;
  _origFrameRect = rect;

#if OVERRIDE_SCALING_ON_NEW_FRAME_RECT
  if (_drawParams.hasScaling) {
    // check if maxScalingWidth is set
    if (_maxScalingWidth && _maxScalingWidth < rect.w) {
      _drawParams.scaledWidth = _maxScalingWidth;
    } else {
      _drawParams.scaledWidth = rect.w;
    }

    // check if maxScalingHeight is set
    if (_maxScalingHeight && _maxScalingHeight < rect.h) {
      _drawParams.scaledHeight = _maxScalingHeight;
    } else {
      _drawParams.scaledHeight = rect.h;
    }
  }
#endif //OVERRIDE_SCALING_ON_NEW_FRAME_RECT

  if (_drawParams.hasCrop) {
    if (_drawParams.hasScaling) {
      applyScaledCrop();
    } else {
      applyCrop();
    }
  }
}

void Widget::setPredefinedRotationCenter(
    const RotationCenterType rotCenterType) {
  switch (rotCenterType) {
  case RotationCenterType::TOP_LEFT:
    _drawParams.rotCenter = Points::ZERO;
    break;
  case RotationCenterType::ORIG_CENTER:
    _drawParams.rotCenter.x = _drawParams.frameRect.w / 2;
    _drawParams.rotCenter.y = _drawParams.frameRect.h / 2;
    break;
  case RotationCenterType::SCALED_CENTER:
    if (!_drawParams.hasScaling) {
      LOGERR("Error, RotationCenterType::SCALED_CENTER requested for widget "
          "with rsrcId: %" PRIu64", which does not have scaling enabled",
          _drawParams.rsrcId);
      return;
    }
    _drawParams.rotCenter.x = _drawParams.scaledWidth / 2;
    _drawParams.rotCenter.y = _drawParams.scaledHeight / 2;
    break;

  default:
    LOGERR("Error, received unsupported RotationCenterType: %hhu",
        getEnumValue(rotCenterType));
    break;
  }
}

void Widget::setOpacity(const int32_t opacity) {
  if (!_isAlphaModulationEnabled) {
    LOGERR(
        "Error, alphaModulation is not enabled for Widget with "
        "rsrcId: %" PRIu64". .setOpacity() will not be performed. Consider "
        "activating alphaModulation first with "
        ".activateAlphaModulation() method",
        _drawParams.rsrcId);

    return;
  }

  if (ZERO_OPACITY > opacity) {
    LOGERR(
        "Error, invalid opacity provided: %d for Widget with rsrcId: "
        "%" PRIu64". Opacity will be set to 0(ZERO_OPEACITY)",
        opacity, _drawParams.rsrcId);
    return;
  }
  if (FULL_OPACITY < opacity) {
    LOGERR(
        "Error, invalid opacity provided: %d for Widget with rsrcId: "
        "%" PRIu64". Opacity will be set to 255(FULL_OPACITY)",
        opacity, _drawParams.rsrcId);
    return;
  }

  _drawParams.opacity = opacity;

  /** Send RendererCmd for change in opacity only if widget is of type
   * WidgetType::TEXT or WidgetType::SPRITE_BUFFER.
   *
   * WidgetType::IMAGE actually change their alpha together with the
   * actual draw call.
   *
   * The reason for this is because texts and sprite buffers are unique
   * and their alpha could be changed immediately.
   * Images on the other hand are not unique. Several Image objects
   * could reuse the same graphical resource thus changing the alpha
   * for one Image object would result changing the alpha for
   * all of them.
   *
   * Images address this issue by simply storing their opacity as a
   * number and in the moment of actual draw call -> if the opacity is
   * different that FULL_OPACITY:
   *     - a local change in opacity is made;
   *     - the draw call is made;
   *     - restore to FULL_OPACITY is made;
   * */
  if (WidgetType::IMAGE == _drawParams.widgetType) {
    return;
  }

  //textId and and spriteBufferId are the same since they are in a union
  uint8_t data[sizeof(_drawParams.widgetType) + sizeof(opacity) +
               sizeof(_drawParams.textId)];
  uint64_t populatedBytes = 0;

  memcpy(data, &_drawParams.widgetType, sizeof(_drawParams.widgetType));
  populatedBytes += sizeof(_drawParams.widgetType);

  memcpy(data + populatedBytes, &opacity, sizeof(opacity));
  populatedBytes += sizeof(opacity);

  memcpy(data + populatedBytes, &_drawParams.textId,
         sizeof(_drawParams.textId));
  populatedBytes += sizeof(_drawParams.textId);

  gDrawMgr->addRendererCmd(RendererCmd::CHANGE_TEXTURE_OPACITY, data,
                           populatedBytes);
}

void Widget::rotate(const double angle) {
  _drawParams.angle += angle;

  if (FULL_ROTATION_ANGLE < _drawParams.angle) {
    while (FULL_ROTATION_ANGLE < _drawParams.angle) {
      _drawParams.angle -= FULL_ROTATION_ANGLE;
    }
  } else if (ZERO_ANGLE > _drawParams.angle) {
    while (ZERO_ANGLE > _drawParams.angle) {
      _drawParams.angle += FULL_ROTATION_ANGLE;
    }
  }
}

Point Widget::getPredefinedRotationCenter(
      const RotationCenterType rotCenterType) const {
  Point rotCenter;
  switch (rotCenterType) {
  case RotationCenterType::TOP_LEFT:
    rotCenter = Points::ZERO;
    break;
  case RotationCenterType::ORIG_CENTER:
    rotCenter.x = _drawParams.frameRect.w / 2;
    rotCenter.y = _drawParams.frameRect.h / 2;
    break;
  case RotationCenterType::SCALED_CENTER:
    if (_drawParams.hasScaling) {
      rotCenter.x = _drawParams.scaledWidth / 2;
      rotCenter.y = _drawParams.scaledHeight / 2;
    } else {
      LOGERR("Error, RotationCenterType::SCALED_CENTER requested for widget "
          "with rsrcId: %" PRIu64", which does not have scaling enabled",
          _drawParams.rsrcId);
    }
    break;

  default:
    LOGERR("Error, received unsupported RotationCenterType: %hhu",
        getEnumValue(rotCenterType));
    break;
  }
  return rotCenter;
}

int32_t Widget::getScaledWidth() const {
  if (_drawParams.hasScaling) {
    return _drawParams.scaledWidth;
  } else {
    LOGERR(
        "Error, scaling is not turned on for Widget with rsrcId: %" PRIu64
        ". getScaledWidth() will return 0",
        _drawParams.rsrcId);

    return 0;
  }
}

int32_t Widget::getScaledHeight() const {
  if (_drawParams.hasScaling) {
    return _drawParams.scaledHeight;
  } else {
    LOGERR(
        "Error, scaling is not turned on for Widget with rsrcId: %" PRIu64
        ". getScaledHeight() will return 0",
        _drawParams.rsrcId);

    return 0;
  }
}

int32_t Widget::getMaxScalingWidth() const {
  if (_drawParams.hasScaling) {
    return _maxScalingWidth;
  } else {
    LOGERR(
        "Error, scaling is not turned on for Widget with rsrcId: %" PRIu64
        ". getMaxScalingWidth() will return 0",
        _drawParams.rsrcId);

    return 0;
  }
}

int32_t Widget::getMaxScalingHeight() const {
  if (_drawParams.hasScaling) {
    return _maxScalingHeight;
  } else {
    LOGERR(
        "Error, scaling is not turned on for Widget with rsrcId: %" PRIu64
        ". getMaxScalingHeight() will return 0",
        _drawParams.rsrcId);

    return 0;
  }
}

void Widget::setCropRect(const Rectangle &cropRect) {
  if (_drawParams.hasCrop) {
    resetCrop();
  }

  _drawParams.hasCrop = true;

  // Remember the newly set crop rectangle boundaries
  _cropRectangle = cropRect;

  // Remember original frame dimensions, because later on crop
  // could be reseted
  _origFrameRect = _drawParams.frameRect;

  // give initial value to the widget's crop rectangle
  _drawParams.frameCropRect.x = _drawParams.pos.x;
  _drawParams.frameCropRect.y = _drawParams.pos.y;
  _drawParams.frameCropRect.w = _drawParams.frameRect.w;
  _drawParams.frameCropRect.h = _drawParams.frameRect.h;

  if (_drawParams.hasScaling) {
    applyScaledCrop();
  } else {
    applyCrop();
  }
}

void Widget::resetCrop() {
  _drawParams.hasCrop = false;

  // reset crop rectangle boundaries
  _cropRectangle = Rectangles::ZERO;

  // restore initial state of variables
  _drawParams.frameRect = _origFrameRect;

  // reset Widgets crop rectangle
  _drawParams.frameCropRect = Rectangles::ZERO;
}

void Widget::applyCrop() {
  const Rectangle frameRect {_drawParams.pos.x, _drawParams.pos.y,
                             _origFrameRect.w, _origFrameRect.h};
  Rectangle intersectRect;

  if (!GeometryUtils::findRectIntersection(frameRect,
                                           _cropRectangle,
                                           intersectRect)) {
    /** Rectangles don't have a common intersection ->
     *  set frameCropRect to Rectangle::ZERO so SDL rectangle boundary
     *  checking will catch that it's an empty rectangle and not draw
     *  anything.
     */
    _drawParams.frameCropRect = Rectangles::ZERO;

    return;
  } else  // rectangles have a common intersection
  {
    /** Apply intersect rectangle dimensions to
     *  frame crop rectangle dimensions
     * */
    _drawParams.frameCropRect.x = intersectRect.x;
    _drawParams.frameCropRect.y = intersectRect.y;
    _drawParams.frameCropRect.w = intersectRect.w;
    _drawParams.frameCropRect.h = intersectRect.h;

    //----------------------Start X Axis evaluation-------------------------

    const int32_t RIGHT_OF_CROP = (_drawParams.pos.x + _origFrameRect.w) -
                                  (_cropRectangle.x + _cropRectangle.w);

    /** Part of Widget is left of crop rectangle (Widget needs to be
     * cropped from left and frameRect.w needs to be adjusted)
     * */
    if (_drawParams.pos.x < _cropRectangle.x) {
      /** Widget has also part of it that is right of crop rectangle ->
       *  This means the crop "chops" the widget in the middle,
       *  leaving two rectangle (one left of _cropRectangle and one
       *  right of_cropRectangle) that should both be discarded
       *  */
      if (0 < RIGHT_OF_CROP) {
        _drawParams.frameRect.w = _cropRectangle.w;

        _drawParams.frameRect.x = _origFrameRect.x + _origFrameRect.w -
                                  (_cropRectangle.w + RIGHT_OF_CROP);
      }
      /** If remaining part of the widget (right of pos.x) is smaller than
       *  cropRectangle.w -> take only the intersection width
       * */
      else {
        _drawParams.frameRect.w = intersectRect.w;

        _drawParams.frameRect.x =
            _origFrameRect.x + (_origFrameRect.w - intersectRect.w);
      }
    }
    /** The Widget is right of the crop rectangle or
     *  their Y coordinate are the same
     * */
    else {
      /** Part of Widget is right of crop rectangle (Widget needs to be
       * cropped from right and frameRect.w and frameRect.x
       * needs to be adjusted)
       * */
      if (0 < RIGHT_OF_CROP) {
        _drawParams.frameRect.x = _origFrameRect.x;

        _drawParams.frameRect.w = intersectRect.w;
      }

      /** Widget is fully inside crop rectangle -> no need of crop.
       * Widget could already be cropped -> restore it's
       * original frameRect.w
       * */
      else {
        _drawParams.frameRect.x = _origFrameRect.x;

        _drawParams.frameRect.w = _origFrameRect.w;
      }
    }

    //----------------------End X Axis evaluation---------------------------

    //======================================================================

    //----------------------Start Y Axis evaluation-------------------------

    const int32_t BELOW_OF_CROP = (_drawParams.pos.y + _origFrameRect.h) -
                                  (_cropRectangle.y + _cropRectangle.h);

    /** Part of Widget is above the crop rectangle (Widget needs to be
     * cropped from bottom and frameRect.h and frameRect.y
     * needs to be adjusted)
     * */
    if (_drawParams.pos.y < _cropRectangle.y) {
      /** Widget has also part of it that is below of crop rectangle ->
       *  This means the crop "chops" the widget in the middle,
       *  leaving two rectangle (one above of _cropRectangle and one
       *  below of_cropRectangle) that should both be discarded
       *  */
      if (0 < BELOW_OF_CROP) {
        _drawParams.frameRect.h = intersectRect.h;

        _drawParams.frameRect.y = _origFrameRect.y + _origFrameRect.h -
                                  (_cropRectangle.h + BELOW_OF_CROP);
      }
      /** If remaining part of the widget (right of pos.y) is smaller than
       *  cropRectangle.w -> take only the intersection height
       * */
      else {
        _drawParams.frameRect.h = intersectRect.h;

        _drawParams.frameRect.y = _origFrameRect.h - intersectRect.h;
      }
    } else {
      /** Part of Widget is below the crop rectangle (Widget needs to be
       *  cropped from top and frameRect.h needs to be adjusted)
       * */
      if (0 < BELOW_OF_CROP) {
        _drawParams.frameRect.y = _origFrameRect.y;

        _drawParams.frameRect.h = intersectRect.h;
      }
      /** Widget is fully inside crop rectangle -> no need of crop.
       * Widget could already be cropped -> restore it's
       * original frameRect.h
       * */
      else {
        _drawParams.frameRect.y = _origFrameRect.y;

        _drawParams.frameRect.h = _origFrameRect.h;
      }
    }

    //----------------------End Y Axis evaluation---------------------------
  }
}

void Widget::applyScaledCrop() {
  const Rectangle frameRect {_drawParams.pos.x, _drawParams.pos.y,
                             _drawParams.scaledWidth, _drawParams.scaledHeight};
  Rectangle intersectRect;

  if (!GeometryUtils::findRectIntersection(frameRect,
                                           _cropRectangle,
                                           intersectRect)) {
    /** Rectangles don't have a common intersection ->
     *  set frameCropRect to Rectangle::ZERO so SDL rectangle boundary
     *  checking will catch that it's an empty rectangle and not draw
     *  anything.
     */
    _drawParams.frameCropRect = Rectangles::ZERO;

    return;
  } else { // rectangles have a common intersection
    /** Apply intersect rectangle dimensions to
     *  frame crop rectangle dimensions
     * */
    _drawParams.frameCropRect.x = intersectRect.x;
    _drawParams.frameCropRect.y = intersectRect.y;
    _drawParams.frameCropRect.w = intersectRect.w;
    _drawParams.frameCropRect.h = intersectRect.h;
  }
}
