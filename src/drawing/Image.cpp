// Corresponding header
#include "manager_utils/drawing/Image.h"

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "manager_utils/managers/RsrcMgr.h"
#include "manager_utils/drawing/Sprite.h"
#include "utils/data_type/EnumClassUtils.hpp"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

// default constructor
Image::Image() : _sprites(nullptr), _isDestroyed(false) {
  _drawParams.widgetType = WidgetType::IMAGE;
}

// move constructor
Image::Image(Image&& movedOther) : Widget(std::move(movedOther)) {
  _drawParams.widgetType = WidgetType::IMAGE;

  // take ownership of resources
  _sprites = movedOther._sprites;
  _isDestroyed = movedOther._isDestroyed;

  // ownership of resource should be taken from moved instance
  movedOther._sprites = nullptr;
  movedOther._isDestroyed = false;
}

// move assignment operator
Image& Image::operator=(Image&& movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    _drawParams.widgetType = WidgetType::IMAGE;

    // take ownership of resources
    _sprites = movedOther._sprites;
    _isDestroyed = movedOther._isDestroyed;

    // explicitly invoke Widget's move assignment operator
    Widget::operator=(std::move(movedOther));

    // ownership of resource should be taken from moved instance
    movedOther._sprites = nullptr;
    movedOther._isDestroyed = false;
  }

  return *this;
}

Image::~Image() {
  // attempt to destroy Image only if it's was first created and not destroyed
  if (true == _isCreated && false == _isDestroyed) {
    Image::destroy();
  }
}

void Image::create(const uint64_t rsrcId) {
  if (_isCreated) {
    LOGERR(
        "Error, Image with rsrcId: %#16lX already created,"
        " will not create twice",
        rsrcId);
    return;
  }

  const ResourceData* rsrcData = nullptr;
  if (SUCCESS != gRsrcMgr->getRsrcData(rsrcId, rsrcData)) {
    LOGERR(
        "Error, getRsrcData failed for rsrcId: %#16lX, "
        "will not create Image",
        rsrcId);

    return;
  }

  _sprites = new Sprite();
  if (nullptr == _sprites) {
    LOGERR("Warning bad alloc for sprites for rsrcId: %#16lX", rsrcId);
    return;
  }

  _isCreated = true;
  _isDestroyed = false;

  _drawParams.rsrcId = rsrcId;

  _drawParams.pos.x = rsrcData->imageRect.x;
  _drawParams.pos.y = rsrcData->imageRect.y;

  _imageWidth = rsrcData->imageRect.w;
  _imageHeight = rsrcData->imageRect.h;

  _sprites->init(rsrcId, rsrcData->spriteData);

  /** Explicitly call setFrameRect() method in order to invoke any
   * crop modification (if such is enabled)
   *     NOTE: Images act as a sprite list.
   *           Even if we do not have sprite list (single Image)
   *           we simulate one as masking the whole
   *           surface/texture behind a sprite list with
   *           single sprite in it with
   *           frameRect.x = 0 and frameRect.y = 0.
   * */
  setFrameRect(_sprites->getFrameRect());
}

void Image::destroy() {
  if (_isDestroyed) {
    LOGERR(
        "Warning, trying to destroy already destroyed Image with rsrcId: "
        "%#16lX",
        _drawParams.rsrcId);
    return;
  }

  _isDestroyed = true;

  Widget::reset();

  if (_sprites)  // Sanity check
  {
    _sprites->deinit();

    delete _sprites;
    _sprites = nullptr;
  }
}

void Image::setTexture(const uint64_t rsrcId) {
  if (!_isCreated) {
    LOGERR(
        "Error, setTexture() method failed with param rsrcId: %#16lX. "
        " Reason: Image was not initially created.",
        rsrcId);
    return;
  }

  if (_drawParams.rsrcId == rsrcId) {
    LOGERR(
        "Error, setTexture() called with same rsrcId as the "
        "original rsrcId: %#16lX held by the Image.",
        rsrcId);

    return;
  }

  const ResourceData* rsrcData = nullptr;
  if (SUCCESS != gRsrcMgr->getRsrcData(rsrcId, rsrcData)) {
    LOGERR(
        "Error, getRsrcData failed for rsrcId: %#16lX, "
        "will not setTexture for Image",
        rsrcId);

    return;
  }

  // sanity check
  if (nullptr == _sprites) {
    LOGERR(
        "Logic error, sprites were not initialised for Image "
        "with rsrcId: %#16lX",
        _drawParams.rsrcId);

    return;
  }

  // remember the current frame before deinit
  const int32_t CURR_FRAME = _sprites->getFrame();

  _sprites->deinit();

  // set Texture passes successfully
  _drawParams.rsrcId = rsrcId;

  // remember new resource dimensions
  _imageWidth = rsrcData->imageRect.w;
  _imageHeight = rsrcData->imageRect.h;

  // initialise new resource sprites
  _sprites->init(rsrcId, rsrcData->spriteData);

  // set the old frame
  _sprites->setFrame(CURR_FRAME);

  /** Explicitly call setFrameRect() method in order to invoke any
   * crop modification (if such is enabled)
   *     NOTE: Images act as a sprite list.
   *           Even if we do not have sprite list (single Image)
   *           we simulate one as masking the whole
   *           surface/texture behind a sprite list with
   *           single sprite in it with
   *           frameRect.x = 0 and frameRect.y = 0.
   * */
  setFrameRect(_sprites->getFrameRect());
}

void Image::setFrame(const int32_t frameIndex) {
  _sprites->setFrame(frameIndex);

  /** Explicitly call setFrameRect() method in order to invoke any
   * crop modification (if such is enabled)
   * */
  setFrameRect(_sprites->getFrameRect());
}

void Image::setNextFrame() {
  _sprites->setNextFrame();

  /** Explicitly call setFrameRect() method in order to invoke any
   * crop modification (if such is enabled)
   * */
  setFrameRect(_sprites->getFrameRect());
}

void Image::setPrevFrame() {
  _sprites->setPrevFrame();

  /** Explicitly call setFrameRect() method in order to invoke any
   * crop modification (if such is enabled)
   * */
  setFrameRect(_sprites->getFrameRect());
}

void Image::addFrame(const Rectangle& rectFrame) {
  _sprites->addFrame(rectFrame);
}

void Image::setManualFrames(const Rectangle* frameRects,
                            const uint32_t rectanglesCount) {
  if (!_isCreated) {
    LOGERR(
        "Error, Image with rsrcId: %#16lX already created. "
        "::setManualFrames() will take no effect",
        _sprites->getFramesRsrcId());
    return;
  }

  /** choosing to pay the price of the overhead to recreate the vector
   * here, because:
   *  1) it will be awful to include <vector> header into the header
   *     of the Image class (used all across the code base)
   *  2) this method should be almost to none called
   * */
  std::vector<Rectangle> rects;
  rects.reserve(rectanglesCount);

  for (uint32_t i = 0; i < rectanglesCount; ++i) {
    rects.emplace_back(frameRects[i]);
  }

  const uint64_t currRsrcId = _sprites->getFramesRsrcId();

  _sprites->deinit();
  _sprites->init(currRsrcId, rects);
}

int32_t Image::getFrame() const { return _sprites->getFrame(); }

int32_t Image::getFrameCount() const { return _sprites->getFrameCount(); }
