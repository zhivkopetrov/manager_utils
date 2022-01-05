// Corresponding header
#include "manager_utils/drawing/animation/defines/CommonAnimationStructs.h"

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "utils/LimitValues.hpp"

AnimBaseConfig::AnimBaseConfig()
 : externalImage(nullptr),
   rsrcId(0),
   startPos(Point::UNDEFINED),
   timerInterval(0),
   timerId(INIT_INT32_VALUE),
   animDirection(AnimDir::UNKNOWN),
   animImageType(AnimImageType::INTERNAL),
   isTimerPauseble(false){
}

void AnimBaseConfig::reset() {
  externalImage = nullptr;
  rsrcId = 0;
  startPos = Point::UNDEFINED;
  timerInterval = 0;
  timerId = 0;
  animDirection = AnimDir::UNKNOWN;
  animImageType = AnimImageType::INTERNAL;
  isTimerPauseble = false;
}
