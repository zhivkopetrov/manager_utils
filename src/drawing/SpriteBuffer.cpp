// Corresponding header
#include "manager_utils/drawing/SpriteBuffer.h"

// C system headers

// C++ system headers
#include <cstring>

// Other libraries headers

// Own components headers
#include "manager_utils/managers/DrawMgr.h"
#include "manager_utils/managers/RsrcMgr.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

// default constructor
SpriteBuffer::SpriteBuffer()
    : _clearColor(Colors::BLACK),
      _itemsOffsetX(0),
      _itemsOffsetY(0),
      _isLocked(true),
      _isCustomClearTargetSet(false),
      _isDestroyed(false) {
  _drawParams.widgetType = WidgetType::SPRITE_BUFFER;
}

// default destructor
SpriteBuffer::~SpriteBuffer() {
  // attempt to destroy the SpriteBuffer only
  // if it was first created and not destroyed
  if (true == _isCreated && false == _isDestroyed) {
    destroy();
  }
}

// move constructor
SpriteBuffer::SpriteBuffer(SpriteBuffer &&movedOther)
    : Widget(std::move(movedOther)), _clearColor(Colors::BLACK) {
  _drawParams.widgetType = WidgetType::SPRITE_BUFFER;

  // take ownership of resources
  _storedItems = std::move(movedOther._storedItems);
  _customClearTarget = std::move(movedOther._customClearTarget);
  _clearColor = movedOther._clearColor;
  _itemsOffsetX = movedOther._itemsOffsetX;
  _itemsOffsetY = movedOther._itemsOffsetY;
  _isLocked = movedOther._isLocked;
  _isCustomClearTargetSet = movedOther._isCustomClearTargetSet;
  _isDestroyed = movedOther._isDestroyed;

  // ownership of resource should be taken from moved instance
  movedOther.resetInternals();
}

// move assignment operator
SpriteBuffer &SpriteBuffer::operator=(SpriteBuffer &&movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    _drawParams.widgetType = WidgetType::SPRITE_BUFFER;

    // take ownership of resources
    _storedItems = std::move(movedOther._storedItems);
    _customClearTarget = std::move(movedOther._customClearTarget);
    _clearColor = movedOther._clearColor;
    _itemsOffsetX = movedOther._itemsOffsetX;
    _itemsOffsetY = movedOther._itemsOffsetY;
    _isLocked = movedOther._isLocked;
    _isCustomClearTargetSet = movedOther._isCustomClearTargetSet;
    _isDestroyed = movedOther._isDestroyed;

    // explicitly invoke Widget's move assignment operator
    Widget::operator=(std::move(movedOther));

    // ownership of resource should be taken from moved instance
    movedOther.resetInternals();
  }

  return *this;
}

void SpriteBuffer::create(const int32_t coordinateX,
                          const int32_t coordinateY,
                          const int32_t spriteBufferWidth,
                          const int32_t spriteBufferHeight,
                          const double rotationAngle,
                          const Point &rotationCenter) {
  if (_isCreated) {
    LOGERR("Warning, trying to create a SpriteBuffer with ID: %d, "
        "that was already created!", _drawParams.spriteBufferId);
    return;
  }

  // populate Widget's DrawParams
  _isCreated = true;
  _isDestroyed = false;

  _drawParams.pos.x = coordinateX;
  _drawParams.pos.y = coordinateY;
  setImageWidth(spriteBufferWidth);
  setImageHeight(spriteBufferHeight);

  _drawParams.angle = rotationAngle;

  if (Point::UNDEFINED != rotationCenter) {
    _drawParams.centerPos = rotationCenter;
  }

  /** Explicitly call setFrameRect() method in order to invoke any
   * crop modification (if such is enabled).
   * */
  setFrameRect(Rectangle(0, 0, spriteBufferWidth, spriteBufferHeight));

  gRsrcMgr->createSpriteBuffer(spriteBufferWidth, spriteBufferHeight,
                               _drawParams.spriteBufferId);
}

void SpriteBuffer::create(const Rectangle& dimensions,
                          const double rotationAngle,
                          const Point& rotationCenter) {
  SpriteBuffer::create(dimensions.x, dimensions.y, dimensions.w, dimensions.h,
                       rotationAngle, rotationCenter);
}

void SpriteBuffer::destroy() {
  if (_isDestroyed) {
    LOGERR("Warning, trying to destroy a SpriteBuffer "
        "that was already destroyed!");
    return;
  }

  if (!_isCreated) {
    LOGERR("Warning, trying to destroy a not-created sprite buffer");
    return;
  }

  // sanity check, because manager could already been destroyed
  if (nullptr != gRsrcMgr) {
    gRsrcMgr->destroySpriteBuffer(_drawParams.spriteBufferId);
  }

  Widget::reset();
  resetInternals();

  _isDestroyed = true;
}

void SpriteBuffer::unlock() {
  if (!_isCreated) {
    LOGERR(
        "Error, SpriteBuffer::unlock() failed, because SpriteBuffer is "
        "not yet created. Consider using ::create() method first");

    return;
  }

  if (!_isLocked) {
    LOGERR(
        "Error, trying to unlock a SpriteBuffer with ID: %d"
        "that is already unlocked", _drawParams.spriteBufferId);
    return;
  }

  _isLocked = false;

  if (SUCCESS != gDrawMgr->unlockRenderer()) {
    LOGERR(
        "Error, SpriteBuffer with ID: %d can not be unlocked, because some "
        "other entity is currently in possession of the main renderer lock. "
        "Usually this is another SpriteBuffer. Be sure to lock your "
        "SpriteBuffers when you are done with your work on them.",
        _drawParams.spriteBufferId);
    return;
  }

  gDrawMgr->addRendererCmd(
      RendererCmd::CHANGE_RENDERER_TARGET,
      reinterpret_cast<const uint8_t *>(&_drawParams.spriteBufferId),
      sizeof(_drawParams.spriteBufferId));
}

void SpriteBuffer::lock() {
  if (!_isCreated) {
    LOGERR(
        "Error, SpriteBuffer::lock() failed, because SpriteBuffer is "
        "not yet created. Consider using ::create() method first");
    return;
  }

  if (_isLocked) {
    LOGERR("Error, trying to lock a SpriteBuffer with ID: %d "
        "that is already locked", _drawParams.spriteBufferId);
    return;
  }

  _isLocked = true;

  if (SUCCESS != gDrawMgr->lockRenderer()) {
    LOGERR("gDrawMgr->lockRenderer() failed");

    return;
  }
}

void SpriteBuffer::reset() {
  if (!_isCreated) {
    LOGERR(
        "Error, SpriteBuffer::reset() failed, because SpriteBuffer is "
        "not yet created. Consider using ::create() method first");
    return;
  }

  if (_isLocked) {
    LOGERR(
        "Error, SpriteBuffer with ID: %d ::reset() failed, because "
        "SpriteBuffer is still locked. Consider using the sequence "
        "::unlock(), ::reset(), ::lock()", _drawParams.spriteBufferId);
    return;
  }

  _storedItems.clear();

  if (!_isCustomClearTargetSet) {
    gDrawMgr->addRendererCmd(
        RendererCmd::CLEAR_RENDERER_TARGET,
        reinterpret_cast<const uint8_t *>(&_clearColor), sizeof(_clearColor));
  } else  //_isCustomClearTargetSet is set
  {
    _storedItems.emplace_back(_customClearTarget);

    update();

    _storedItems.clear();
  }
}

void SpriteBuffer::addWidget(const Widget &widget) {
  if (!widget.isCreated()) {
    LOGERR("Widget is not created, therefore -> "
        "it could not be added to SpriteBuffer");
    return;
  }

  _storedItems.emplace_back(widget.getDrawParams());
}

void SpriteBuffer::update() {
  if (!_isCreated) {
    LOGERR(
        "Error, SpriteBuffe::update() failed, because SpriteBuffer is not yet "
        "created. Consider using ::create() method first");
    return;
  }

  if (_isLocked) {
    LOGERR(
        "Error, SpriteBuffer with ID: %d ::update() failed, because "
        "SpriteBuffer is still locked. Consider using the sequence "
        "::unlock(), ::update(), ::lock()", _drawParams.spriteBufferId);

    return;
  }

  const uint32_t SIZE = static_cast<uint32_t>(_storedItems.size());

  // transform relative sprite buffer coordinates to
  // relative ones for the monitor, on which the SpriteBuffer is attached to
  transformToMonitorRelativeCoordinates(SIZE);

  // add size of used widgets
  gDrawMgr->addRendererData(reinterpret_cast<const uint8_t *>(&SIZE),
                                sizeof(SIZE));

  // add the actual command and the data for the stored DrawParams
  gDrawMgr->addRendererCmd(
      RendererCmd::UPDATE_RENDERER_TARGET,
      reinterpret_cast<const uint8_t *>(_storedItems.data()),
      sizeof(DrawParams) * SIZE);
}

void SpriteBuffer::updateRanged(const int32_t fromIndex,
                                const int32_t toIndex) {
  if (!_isCreated) {
    LOGERR(
        "Error, SpriteBuffer::updateRanged() failed, because "
        "SpriteBuffer is not yet created. Consider using ::create() "
        "method first");
    return;
  }

  if (_isLocked) {
    LOGERR(
        "Error, SpriteBuffer with ID: %d ::updateRanged() failed, because "
        "SpriteBuffer is still locked. Consider using the sequence "
        "::unlock(), ::update(), ::lock()", _drawParams.spriteBufferId);
    return;
  }

  const int32_t SIZE = static_cast<int32_t>(_storedItems.size());

  if ((0 > fromIndex) || (fromIndex > toIndex) || (toIndex >= SIZE)) {
    LOGERR(
        "Error, Illegal ranges provided. fromIndex: %d, toIndex: %d, "
        "storedItems.size(): %u for SpriteBuffer with ID: %d",
        fromIndex, toIndex, SIZE, _drawParams.spriteBufferId);
    return;
  }

  // transform relative sprite buffer coordinates to
  // relative ones for the monitor, on which the SpriteBuffer is attached to
  transformToMonitorRelativeCoordinatesRanged(fromIndex, fromIndex);

  const uint32_t NEW_ELEMENTS = toIndex - fromIndex + 1;

  // add size of used widgets
  gDrawMgr->addRendererData(
      reinterpret_cast<const uint8_t *>(&NEW_ELEMENTS), sizeof(NEW_ELEMENTS));

  // add the actual command and the data for the stored DrawParams
  gDrawMgr->addRendererCmd(
      RendererCmd::UPDATE_RENDERER_TARGET,
      reinterpret_cast<const uint8_t *>(&_storedItems[fromIndex]),
      sizeof(DrawParams) * NEW_ELEMENTS);
}

void SpriteBuffer::addCustomClearTarget(const Widget &widget) {
  if (!widget.isCreated()) {
    LOGERR("Widget is not created, therefore -> "
        "it could not be added to SpriteBuffer");
    return;
  }

  _isCustomClearTargetSet = true;
  _customClearTarget = widget.getDrawParams();
}

void SpriteBuffer::setSpriteBufferResetColor(const Color &clearColor) {
  _clearColor = clearColor;

  if (!_isAlphaModulationEnabled && _clearColor == Colors::FULL_TRANSPARENT) {
    LOGERR(
        "Warning, SpriteBuffer::setSpriteBufferResetColor() invoked "
        "with ID: %d with Colors::FULL_TRANSPARENT while alpha modulation "
        "is not enabled. This will result in not proper reset color when "
        "SpriteBuffer::reset() is invoked.", _drawParams.spriteBufferId);
  }
}

void SpriteBuffer::transformToMonitorRelativeCoordinates(
    const uint32_t storedItemsSize) {
  const int32_t SPRITE_BUFFER_POS_X = _drawParams.pos.x - _itemsOffsetX;
  const int32_t SPRITE_BUFFER_POS_Y = _drawParams.pos.y - _itemsOffsetY;

  for (uint32_t i = 0; i < storedItemsSize; ++i) {
    if (_storedItems[i].hasCrop) {
      _storedItems[i].frameCropRect.x -= SPRITE_BUFFER_POS_X;
      _storedItems[i].frameCropRect.y -= SPRITE_BUFFER_POS_Y;
    } else {
      _storedItems[i].pos.x -= SPRITE_BUFFER_POS_X;
      _storedItems[i].pos.y -= SPRITE_BUFFER_POS_Y;
    }
  }
}

void SpriteBuffer::transformToMonitorRelativeCoordinatesRanged(
    const int32_t fromIndex, const int32_t toIndex) {
  const int32_t SPRITE_BUFFER_POS_X = _drawParams.pos.x - _itemsOffsetX;
  const int32_t SPRITE_BUFFER_POS_Y = _drawParams.pos.y - _itemsOffsetY;

  for (int32_t i = fromIndex; i <= toIndex; ++i) {
    if (_storedItems[i].hasCrop) {
      _storedItems[i].frameCropRect.x -= SPRITE_BUFFER_POS_X;
      _storedItems[i].frameCropRect.y -= SPRITE_BUFFER_POS_Y;
    } else {
      _storedItems[i].pos.x -= SPRITE_BUFFER_POS_X;
      _storedItems[i].pos.y -= SPRITE_BUFFER_POS_Y;
    }
  }
}

void SpriteBuffer::resetInternals() {
  _storedItems.clear();
  _customClearTarget.reset();

  _clearColor = Colors::BLACK;
  _itemsOffsetX = 0;
  _itemsOffsetY = 0;
  _isLocked = true;
  _isCustomClearTargetSet = false;
  _isDestroyed = false;
}

