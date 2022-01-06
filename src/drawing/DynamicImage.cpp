// Corresponding header
#include "manager_utils/drawing/DynamicImage.h"

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
DynamicImage::DynamicImage() {}

// move constructor
DynamicImage::DynamicImage(DynamicImage&& movedOther)
    : Image(std::move(movedOther)) {}

// move assignment operator
DynamicImage& DynamicImage::operator=(DynamicImage&& movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    // explicitly invoke Image's move assignment operator
    Image::operator=(std::move(movedOther));
  }

  return *this;
}

DynamicImage::~DynamicImage() {
  // attempt to destroy DynamicImage only if it's was first created
  // and not destroyed
  if (true == _isCreated && false == _isDestroyed) {
    DynamicImage::destroy();
  }
}

void DynamicImage::create(const uint64_t rsrcId) {
  if (_isCreated) {
    LOGERR(
        "Error, Image with rsrcId: %#16lX already created,"
        " will not create twice",
        rsrcId);
    return;
  }

  gRsrcMgr->loadResourceOnDemandSingle(rsrcId);
  const ResourceData* rsrcData = nullptr;
  if (SUCCESS != gRsrcMgr->getRsrcData(rsrcId, rsrcData)) {
    LOGERR(
        "Error, getRsrcData failed for rsrcId: %#16lX, "
        "will not create Image",
        rsrcId);
    gRsrcMgr->unloadResourceOnDemandSingle(rsrcId);
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

void DynamicImage::destroy() {
  if (_isDestroyed) {
    LOGERR(
        "Warning, trying to destroy already destroyed Image with rsrcId: "
        "%#16lX",
        _drawParams.rsrcId);
    return;
  }

  // make a sanity check, because gRsrcMgrBase might already be destroyed in
  // case of application shutdown and chain of destructor calls
  if (gRsrcMgr) {
    gRsrcMgr->unloadResourceOnDemandSingle(_drawParams.rsrcId);
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
