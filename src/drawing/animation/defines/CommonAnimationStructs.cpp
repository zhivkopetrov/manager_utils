// Corresponding header
#include "manager_utils/drawing/animation/defines/CommonAnimationStructs.h"

// System headers

// Other libraries headers
#include "utils/LimitValues.h"

// Own components headers

AnimBaseConfig::AnimBaseConfig()
 : externalImage(nullptr),
   startPos(Points::UNDEFINED),
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
  startPos = Points::UNDEFINED;
  startAngle = 0.0;
  rsrcId = 0;
  timerInterval = 0;
  timerId = 0;
  animDirection = AnimDir::UNKNOWN;
  animImageType = AnimImageType::INTERNAL;
  isTimerPauseble = false;
}
