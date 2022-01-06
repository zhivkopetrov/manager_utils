// Corresponding header
#include "manager_utils/sound/SoundWidget.h"

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "manager_utils/managers/RsrcMgr.h"
#include "manager_utils/sound/SoundWidgetEndCb.hpp"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

// default constructor
SoundWidget::SoundWidget()
    : _endCb(nullptr),
      _rsrcId(0),
      _soundType(SoundType::UNKNOWN),
      _soundLevel(SoundLevel::UNKNOWN),
      _isCreated(false),
      _isDestroyed(false) {}

// move constructor
SoundWidget::SoundWidget(SoundWidget&& movedOther) {
  // take ownership of resources
  _endCb = movedOther._endCb;
  _rsrcId = movedOther._rsrcId;
  _soundType = movedOther._soundType;
  _soundLevel = movedOther._soundLevel;
  _isCreated = movedOther._isCreated;
  _isDestroyed = movedOther._isDestroyed;

  // ownership of resource should be taken from moved instance
  movedOther.reset();
}

// move assignment operator implementation
SoundWidget& SoundWidget::operator=(SoundWidget&& movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    // take ownership of resources
    _endCb = movedOther._endCb;
    _rsrcId = movedOther._rsrcId;
    _soundType = movedOther._soundType;
    _soundLevel = movedOther._soundLevel;
    _isCreated = movedOther._isCreated;
    _isDestroyed = movedOther._isDestroyed;

    // ownership of resource should be taken from moved instance
    movedOther.reset();
  }

  return *this;
}

void SoundWidget::create(const uint64_t rsrcId, SoundWidgetEndCb* endCb) {
  if (_isCreated) {
    LOGERR("Error, SoundWidget with _rsrcId: %#16lX already created,"
           " will not create twice", _rsrcId);
    return;
  }

  _endCb = endCb;
  _isCreated = true;
  _isDestroyed = false;

  const SoundData* soundData = nullptr;
  if (SUCCESS != gRsrcMgr->getSoundData(rsrcId, soundData)) {
    LOGERR(
        "Error, getSoundData() failed for rsrcId: %#16lX, "
        "will not create SoundWidget",
        rsrcId);

    return;
  }

  _rsrcId = soundData->header.hashValue;

  _soundType = soundData->soundType;
  _soundLevel = soundData->soundLevel;
}

void SoundWidget::destroy() {
  _isDestroyed = true;
  _isCreated = false;

  _endCb = nullptr;
  _rsrcId = 0;
  _soundType = SoundType::UNKNOWN;
  _soundLevel = SoundLevel::UNKNOWN;
}

void SoundWidget::reset() {
  _endCb = nullptr;
  _rsrcId = 0;
  _soundType = SoundType::UNKNOWN;
  _soundLevel = SoundLevel::UNKNOWN;
  _isCreated = false;
  _isDestroyed = false;
}
