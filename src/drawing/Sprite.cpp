// Corresponding header
#include "manager_utils/drawing/Sprite.h"

// System headers

// Other libraries headers
#include "utils/log/Log.h"

// Own components headers

Sprite::Sprite()
    : _currFrame(0), _maxFrames(0), _rsrcId(0) {
}

Sprite::~Sprite() noexcept {
  deinit();
}

void Sprite::init(const uint64_t rsrcId,
                  const std::vector<Rectangle> &spriteData) {
  _maxFrames = static_cast<int32_t>(spriteData.size());
  _spriteData = spriteData;
  _rsrcId = rsrcId;
}

void Sprite::deinit() {
  _currFrame = 0;
  _rsrcId = 0;
  _spriteData.clear();
}

void Sprite::setFrame(const int32_t frameIndex) {
  if (frameIndex < _maxFrames) {
    _currFrame = frameIndex;
  } else {
    LOGERR("Invalid frame: %d for rsrcId: %" PRIu64, frameIndex, _rsrcId);
  }
}

void Sprite::setNextFrame() {
  ++_currFrame;
  if (_currFrame >= _maxFrames) {
    _currFrame = 0;
  }
}

void Sprite::setPrevFrame() {
  --_currFrame;
  if (0 > _currFrame) {
    _currFrame = _maxFrames - 1;
  }
}

void Sprite::addFrame(const Rectangle &frameRect) {
  ++_maxFrames;
  _spriteData.emplace_back(frameRect);
}

