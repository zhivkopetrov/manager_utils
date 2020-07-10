// Corresponding header
#include "manager_utils/drawing/animation/defines/CommonAnimationStructs.h"

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "utils/LimitValues.hpp"

AnimBaseConfig::AnimBaseConfig()
 : rsrcId(0),
   startPos(Point::UNDEFINED),
   timerInterval(0),
   timerId(INIT_INT32_VALUE),
   animDirection(AnimDir::UNKNOWN),
   isTimerPauseble(false){
}

void AnimBaseConfig::reset() {
  rsrcId = 0;
  startPos = Point::UNDEFINED;
  timerInterval = 0;
  timerId = 0;
  animDirection = AnimDir::UNKNOWN;
  isTimerPauseble = false;
}
