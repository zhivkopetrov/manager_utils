// Corresponding header
#include "manager_utils/time/TimerClient.h"

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "manager_utils/managers/TimerMgr.h"
#include "utils/debug/FunctionTracer.hpp"
#include "utils/debug/StackTrace.hpp"
#include "utils/LimitValues.hpp"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

namespace {
// Hold step for increase of the limit for currently auto-managed timers
constexpr auto RESIZE_STEP = 5;
constexpr auto MAX_TIMERS = 30;
}

// default constructor
TimerClient::TimerClient()
    : _timerIdList(nullptr), _currTimerCount(0), _maxTimersCount(0) {}

// move constructor
TimerClient::TimerClient(TimerClient&& movedOther) {
  // scan linear and search for a started timers
  for (int32_t i = 0; i < movedOther._maxTimersCount; ++i) {
    if (INIT_INT32_VALUE != movedOther._timerIdList[i]) {
      // started timer found -> stop the search
      LOGERR("Warning, TimerClient instance is being moved while there are "
             "active timers attached to it. This is an illegal operation.");
      break;
    }
  }

  // take ownership of the resources
  _timerIdList = movedOther._timerIdList;
  _currTimerCount = movedOther._currTimerCount;
  _maxTimersCount = movedOther._maxTimersCount;

  // old entity should release the resources
  movedOther._timerIdList = nullptr;
  movedOther._currTimerCount = 0;
  movedOther._maxTimersCount = 0;
}

// move assignment operator
TimerClient& TimerClient::operator=(TimerClient&& movedOther) {
  if (this != &movedOther) {
    // scan linear and search for a started timers
    for (int32_t i = 0; i < movedOther._maxTimersCount; ++i) {
      if (INIT_INT32_VALUE != movedOther._timerIdList[i]) {
        // started timer found -> stop the search
        LOGERR("Warning, TimerClient instance is being moved while there are "
               "active timers attached to it. This is an illegal operation.");
        break;
      }
    }

    // take ownership of the resources
    _timerIdList = movedOther._timerIdList;
    _currTimerCount = movedOther._currTimerCount;
    _maxTimersCount = movedOther._maxTimersCount;

    // old entity should release the resources
    movedOther._timerIdList = nullptr;
    movedOther._currTimerCount = 0;
    movedOther._maxTimersCount = 0;
  }

  return *this;
}

TimerClient::~TimerClient() {
  TRACE_ENTRY_EXIT;

  // TimerClient is about to be destroyed -> stop all it's timers.
  for (int32_t i = 0; i < _maxTimersCount; ++i) {
    // search through non-empty slots
    if (INIT_INT32_VALUE != _timerIdList[i]) {
      // sanity check
      if (nullptr != gTimerMgr) {
        gTimerMgr->stopTimerAndDetachTimerClient(_timerIdList[i]);
      }
    }
  }

  // clean dynamically created resources
  if (_timerIdList)  // sanity check
  {
    delete[] _timerIdList;
    _timerIdList = nullptr;
  }
}

void TimerClient::startTimer(const int64_t interval, const int32_t timerId,
                             const TimerType timerType,
                             const TimerGroup timerGroup) {
  TRACE_ENTRY_EXIT;

  // if timer already exists -> do not start it
  if (gTimerMgr->isActiveTimerId(timerId)) {
    LOGERR(
        "Warning, timer with ID: %d already exist. "
        "Will not start new timer",
        timerId);

    LOG("Printing stack trace for better debug info");
    printStacktrace();

    return;
  }

  if (interval < 20) {
    LOGERR(
        "Warning, timer with timerId: %d requested startTimer() with "
        "interval %ld, while minimum interval is 20ms. Timer will not "
        "be started!",
        timerId, interval);

    return;
  }

  // check if maximum active timers count is reached
  if (_currTimerCount == _maxTimersCount) {
    if (MAX_TIMERS == _currTimerCount) {
      LOGERR(
          "Warning, Maximum possible active timers: %d for current "
          "TimerClient instance was reached. Timer with ID: %d could "
          "not be started!",
          MAX_TIMERS, timerId);

      return;
    }

    if (SUCCESS != resizeTimerList()) {
      LOGERR("Warning, timer with ID: %d could not be started", timerId);

      return;
    }
  }

  int32_t freeIndex = INIT_INT32_VALUE;

  // scan linear and search for a free slot
  for (int32_t i = 0; i < _maxTimersCount; ++i) {
    if (INIT_INT32_VALUE == _timerIdList[i]) {
      // free slot found -> stop the search
      freeIndex = i;

      break;
    }
  }

  _timerIdList[freeIndex] = timerId;
  ++_currTimerCount;

  gTimerMgr->startTimerClientTimer(this,         // TimerClient instance
                                       interval,     // interval
                                       timerId,      // remaining interval
                                       timerType,    // timer type
                                       timerGroup);  // timer group
}

void TimerClient::stopTimer(const int32_t timerId) {
  TRACE_ENTRY_EXIT;

  gTimerMgr->stopTimer(timerId);
}

bool TimerClient::isActiveTimerId(const int32_t timerId) const {
  return gTimerMgr->isActiveTimerId(timerId);
}

void TimerClient::restartTimerInterval(const int32_t timerId) {
  bool isTimerFound = false;

  for (int32_t i = 0; i < _maxTimersCount; ++i) {
    if (timerId == _timerIdList[i]) {
      isTimerFound = true;
      break;
    }
  }

  if (!isTimerFound) {
    LOGERR(
        "Warning, trying to restart a timer with ID: %d that this "
        "TimerClient instance is not owner of! "
        ".restartTimerInterval(timerId) can only be called for active "
        "timers currently owned(started from) this TimerClient instance",
        timerId);

    LOG("Printing stack trace for better debug info");
    printStacktrace();
    return;
  }

  gTimerMgr->restartTimerClientTimerInterval(timerId);
}

void TimerClient::addTimeToTimer(const int32_t timerId,
                                 const int64_t intervalToAdd) {
  bool isTimerFound = false;

  for (int32_t i = 0; i < _maxTimersCount; ++i) {
    if (timerId == _timerIdList[i]) {
      isTimerFound = true;
      break;
    }
  }

  if (!isTimerFound) {
    LOGERR(
        "Warning, trying to add time to timer with ID: %d that this "
        "TimerClient instance is not owner of! "
        ".restartTimerInterval(timerId) can only be called for active "
        "timers currently owned(started from) this TimerClient instance",
        timerId);

    LOG("Printing stack trace for better debug info");
    printStacktrace();
    return;
  }

  gTimerMgr->addTimeToTimerClientTimer(timerId, intervalToAdd);
}

void TimerClient::removeTimeFromTimer(const int32_t timerId,
                                      const int64_t intervalToRemove) {
  bool isTimerFound = false;

  for (int32_t i = 0; i < _maxTimersCount; ++i) {
    if (timerId == _timerIdList[i]) {
      isTimerFound = true;
      break;
    }
  }

  if (!isTimerFound) {
    LOGERR(
        "Warning, trying to remove time from timer with ID: %d that this"
        " TimerClient instance is not owner of! "
        ".restartTimerInterval(timerId) can only be called for active "
        "timers currently owned(started from) this TimerClient instance",
        timerId);

    LOG("Printing stack trace for better debug info");
    printStacktrace();
    return;
  }

  gTimerMgr->removeTimeFromTimerClientTimer(timerId, intervalToRemove);
}

int64_t TimerClient::getTimerRemainingInterval(const int32_t timerId) const {
  return gTimerMgr->getTimerRemainingInterval(timerId);
}

int32_t TimerClient::resizeTimerList() {
  TRACE_ENTRY_EXIT;

  // increase maximum timers
  _maxTimersCount += RESIZE_STEP;

  // allocate buffer for the new size
  int32_t* increasedList = new int32_t[_maxTimersCount];
  if (!increasedList) {
    LOGERR("Error, bad alloc for _timerIdList");

    _maxTimersCount -= RESIZE_STEP;
    return FAILURE;
  }

  // copy timer id's from the smaller buffer
  for (int32_t i = 0; i < _currTimerCount; ++i) {
    increasedList[i] = _timerIdList[i];
  }

  // set new timerID slots to its starting value
  for (int32_t i = _currTimerCount; i < _maxTimersCount; ++i) {
    increasedList[i] = INIT_INT32_VALUE;
  }

  // free resources from the smaller buffer
  delete[] _timerIdList;

  // point timerIdList to new bigger buffer
  _timerIdList = increasedList;

  increasedList = nullptr;

  return SUCCESS;
}

int32_t TimerClient::removeTimerIdFromList(const int32_t timerId) {
  for (int32_t i = 0; i < _maxTimersCount; ++i) {
    if (timerId == _timerIdList[i]) {
      --_currTimerCount;                   // lower total timers count
      _timerIdList[i] = INIT_INT32_VALUE;  // free the slot
      return SUCCESS;                      // and end the search
    }
  }

  return FAILURE;
}
