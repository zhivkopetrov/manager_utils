// Corresponding header
#include "manager_utils/time/UserTimerClient.h"

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "manager_utils/managers_base/TimerMgrBase.h"

#include "utils/Log.h"

void UserTimerClient::startTimer(const int64_t interval, const int32_t timerId,
                                 const cbFunc func, const cbFunc freeFunc,
                                 void* funcData, const TimerType timerType,
                                 const TimerGroup timerGroup) {
  if (interval >= 20) {
    gTimerMgrBase->startUserTimer(interval, timerId, func, freeFunc, funcData,
                                  timerType, timerGroup);
  } else {
    LOGERR(
        "Warning, timer with timerId: %d requested startTimer() with "
        "interval %ld, while minimum interval is 20ms. Timer will not "
        "be started!",
        timerId, interval);
  }
}

void UserTimerClient::stopTimer(const int32_t timerId) {
  gTimerMgrBase->stopTimer(timerId);
}

bool UserTimerClient::isActiveTimerId(const int32_t timerId) {
  return gTimerMgrBase->isActiveTimerId(timerId);
}

void UserTimerClient::restartTimerInterval(const int32_t timerId) {
  gTimerMgrBase->restartUserTimerInterval(timerId);
}

void UserTimerClient::addTimeToTimer(const int32_t timerId,
                                     const int64_t intervalToAdd) {
  gTimerMgrBase->addTimeToUserTimer(timerId, intervalToAdd);
}

void UserTimerClient::removeTimeFromTimer(const int32_t timerId,
                                          const int64_t intervalToRemove) {
  gTimerMgrBase->removeTimeFromUserTimer(timerId, intervalToRemove);
}

int64_t getTimerRemainingInterval(const int32_t timerId) {
  return gTimerMgrBase->getTimerRemainingInterval(timerId);
}
