// Corresponding .h file
#include "manager_utils/time/TimerClientSpeedAdjustable.h"

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "manager_utils/managers/TimerMgr.h"

TimerClientSpeedAdjustable::TimerClientSpeedAdjustable() {}

TimerClientSpeedAdjustable::~TimerClientSpeedAdjustable() {}

void TimerClientSpeedAdjustable::startTimer(const int64_t interval,
                                            const int32_t timerId,
                                            const TimerType timerType,
                                            const TimerGroup timerGroup) {
  const int32_t timerSpeed = gTimerMgr->getTimerSpeed();

  int64_t adjustedInterval = (interval * timerSpeed) / 100;
  adjustedInterval = (adjustedInterval < 20) ? 20 : adjustedInterval;

  TimerClient::startTimer(adjustedInterval, timerId, timerType, timerGroup);
}

