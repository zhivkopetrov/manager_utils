// Corresponding header
#include "manager_utils/drawing/Sprite.h"

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "utils/Log.h"

Sprite::Sprite() : _currFrame(0), _maxFrames(0), _rsrcId(0) {}

Sprite::~Sprite() { deinit(); }

void Sprite::init(const uint64_t rsrcId,
                  const std::vector<Rectangle>& spriteData) {
  _maxFrames = static_cast<int32_t>(spriteData.size());

  // the default copy constructor should do the job
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
    LOGERR("Invalid frame: %d for rsrcId: %#16lX", frameIndex, _rsrcId);
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

void Sprite::addFrame(const Rectangle& frameRect) {
  ++_maxFrames;
  _spriteData.emplace_back(frameRect);
}


