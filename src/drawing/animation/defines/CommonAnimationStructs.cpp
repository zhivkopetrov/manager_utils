// Corresponding header
#include "manager_utils/drawing/animation/defines/CommonAnimationStructs.h"

// C system headers

// C++ system headers

// Other libraries headers
#include "utils/LimitValues.h"

// Own components headers

AnimBaseConfig::AnimBaseConfig()
 : externalImage(nullptr),
   startPos(Point::UNDEFINED),
   startAngle(0.0),
   rsrcId(0),
   timerInterval(0),
   timerId(INIT_INT32_VALUE),
   animDirection(AnimDir::UNKNOWN),
   animImageType(AnimImageType::INTERNAL),
   isTimerPauseble(false){
}

void AnimBaseConfig::reset() {
  externalImage = nullptr;
  startPos = Point::UNDEFINED;
  startAngle = 0.0;
  rsrcId = 0;
  timerInterval = 0;
  timerId = 0;
  animDirection = AnimDir::UNKNOWN;
  animImageType = AnimImageType::INTERNAL;
  isTimerPauseble = false;
}
