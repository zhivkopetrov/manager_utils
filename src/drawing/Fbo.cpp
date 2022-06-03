// Corresponding header
#include "manager_utils/drawing/Fbo.h"

// C system headers

// C++ system headers
#include <cstring>

// Other libraries headers
#include "utils/ErrorCode.h"
#include "utils/Log.h"

// Own components headers
#include "manager_utils/managers/DrawMgr.h"
#include "manager_utils/managers/RsrcMgr.h"

Fbo::Fbo()
    : _clearColor(Colors::BLACK),
      _itemsOffsetX(0),
      _itemsOffsetY(0),
      _isLocked(true),
      _isCustomClearTargetSet(false),
      _isDestroyed(false) {
  _drawParams.widgetType = WidgetType::SPRITE_BUFFER;
}

Fbo::~Fbo() noexcept {
  // attempt to destroy the Fbo only
  // if it was first created and not destroyed
  if (true == _isCreated && false == _isDestroyed) {
    destroy();
  }
}

Fbo::Fbo(Fbo &&movedOther)
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
Fbo &Fbo::operator=(Fbo &&movedOther) {
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

void Fbo::create(const int32_t coordinateX,
                          const int32_t coordinateY,
                          const int32_t spriteBufferWidth,
                          const int32_t spriteBufferHeight,
                          const double rotationAngle,
                          const Point &rotationCenter) {
  if (_isCreated) {
    LOGERR("Warning, trying to create a Fbo with ID: %d, "
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

  if (Points::UNDEFINED != rotationCenter) {
    _drawParams.rotCenter = rotationCenter;
  }

  /** Explicitly call setFrameRect() method in order to invoke any
   * crop modification (if such is enabled).
   * */
  setFrameRect(Rectangle(0, 0, spriteBufferWidth, spriteBufferHeight));

  gRsrcMgr->createFbo(spriteBufferWidth, spriteBufferHeight,
                               _drawParams.spriteBufferId);
}

void Fbo::create(const Rectangle& dimensions,
                          const double rotationAngle,
                          const Point& rotationCenter) {
  Fbo::create(dimensions.x, dimensions.y, dimensions.w, dimensions.h,
                       rotationAngle, rotationCenter);
}

void Fbo::destroy() {
  if (_isDestroyed) {
    LOGERR("Warning, trying to destroy a Fbo that was already destroyed!");
    return;
  }

  if (!_isCreated) {
    LOGERR("Warning, trying to destroy a not-created sprite buffer");
    return;
  }

  // sanity check, because manager could already been destroyed
  if (nullptr != gRsrcMgr) {
    gRsrcMgr->destroyFbo(_drawParams.spriteBufferId);
  }

  Widget::reset();
  resetInternals();

  _isDestroyed = true;
}

void Fbo::unlock() {
  if (!_isCreated) {
    LOGERR("Error, Fbo::unlock() failed, because Fbo is "
           "not yet created. Consider using ::create() method first");
    return;
  }

  if (!_isLocked) {
    LOGERR("Error, trying to unlock a Fbo with ID: %d"
           "that is already unlocked", _drawParams.spriteBufferId);
    return;
  }

  _isLocked = false;

  if (ErrorCode::SUCCESS != gDrawMgr->unlockRenderer()) {
    LOGERR("Error, Fbo with ID: %d can not be unlocked, because some "
           "other entity is currently in possession of the main renderer lock. "
           "Usually this is another Fbo. Be sure to lock your "
           "Fbos when you are done with your work on them.",
           _drawParams.spriteBufferId);
    return;
  }

  gDrawMgr->addRendererCmd(
      RendererCmd::CHANGE_RENDERER_TARGET,
      reinterpret_cast<const uint8_t *>(&_drawParams.spriteBufferId),
      sizeof(_drawParams.spriteBufferId));
}

void Fbo::lock() {
  if (!_isCreated) {
    LOGERR("Error, Fbo::lock() failed, because Fbo is "
           "not yet created. Consider using ::create() method first");
    return;
  }

  if (_isLocked) {
    LOGERR("Error, trying to lock a Fbo with ID: %d "
           "that is already locked", _drawParams.spriteBufferId);
    return;
  }

  _isLocked = true;

  if (ErrorCode::SUCCESS != gDrawMgr->lockRenderer()) {
    LOGERR("gDrawMgr->lockRenderer() failed");
    return;
  }
}

void Fbo::reset() {
  if (!_isCreated) {
    LOGERR("Error, Fbo::reset() failed, because Fbo is "
           "not yet created. Consider using ::create() method first");
    return;
  }

  if (_isLocked) {
    LOGERR("Error, Fbo with ID: %d ::reset() failed, because "
           "Fbo is still locked. Consider using the sequence "
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

void Fbo::addWidget(const Widget &widget) {
  if (!widget.isCreated()) {
    LOGERR("Widget is not created, therefore -> it could not be added to Fbo");
    return;
  }

  if (widget.isVisible()) {
    _storedItems.emplace_back(widget.getDrawParams());
  }
}

void Fbo::update() {
  if (!_isCreated) {
    LOGERR("Error, SpriteBuffe::update() failed, because Fbo is not yet "
           "created. Consider using ::create() method first");
    return;
  }

  if (_isLocked) {
    LOGERR("Error, Fbo with ID: %d ::update() failed, because "
          "Fbo is still locked. Consider using the sequence "
          "::unlock(), ::update(), ::lock()", _drawParams.spriteBufferId);

    return;
  }

  const uint32_t SIZE = static_cast<uint32_t>(_storedItems.size());

  // transform relative sprite buffer coordinates to
  // relative ones for the monitor, on which the Fbo is attached to
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

void Fbo::updateRanged(const int32_t fromIndex,
                                const int32_t toIndex) {
  if (!_isCreated) {
    LOGERR("Error, Fbo::updateRanged() failed, because "
           "Fbo is not yet created. Consider using ::create() method first");
    return;
  }

  if (_isLocked) {
    LOGERR("Error, Fbo with ID: %d ::updateRanged() failed, because "
           "Fbo is still locked. Consider using the sequence "
           "::unlock(), ::update(), ::lock()", _drawParams.spriteBufferId);
    return;
  }

  const int32_t SIZE = static_cast<int32_t>(_storedItems.size());

  if ((0 > fromIndex) || (fromIndex > toIndex) || (toIndex >= SIZE)) {
    LOGERR("Error, Illegal ranges provided. fromIndex: %d, toIndex: %d, "
           "storedItems.size(): %u for Fbo with ID: %d",
        fromIndex, toIndex, SIZE, _drawParams.spriteBufferId);
    return;
  }

  // transform relative sprite buffer coordinates to
  // relative ones for the monitor, on which the Fbo is attached to
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

void Fbo::addCustomClearTarget(const Widget &widget) {
  if (!widget.isCreated()) {
    LOGERR("Widget is not created, therefore -> "
        "it could not be added to Fbo");
    return;
  }

  _isCustomClearTargetSet = true;
  _customClearTarget = widget.getDrawParams();
}

void Fbo::setResetColor(const Color &clearColor) {
  _clearColor = clearColor;

  if (!_isAlphaModulationEnabled && _clearColor == Colors::FULL_TRANSPARENT) {
    LOGERR("Warning, Fbo::setFboResetColor() invoked "
           "with ID: %d with Colors::FULL_TRANSPARENT while alpha modulation "
           "is not enabled. This will result in not proper reset color when "
           "Fbo::reset() is invoked.", _drawParams.spriteBufferId);
  }
}

void Fbo::transformToMonitorRelativeCoordinates(
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

void Fbo::transformToMonitorRelativeCoordinatesRanged(
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

void Fbo::resetInternals() {
  _storedItems.clear();
  _customClearTarget.reset();

  _clearColor = Colors::BLACK;
  _itemsOffsetX = 0;
  _itemsOffsetY = 0;
  _isLocked = true;
  _isCustomClearTargetSet = false;
  _isDestroyed = false;
}

