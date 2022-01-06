// Corresponding header
#include "manager_utils/managers/TimerMgr.h"

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "manager_utils/time/TimerClient.h"
#include "sdl_utils/input/InputEvent.h"
#include "utils/debug/FunctionTracer.hpp"
#include "utils/debug/StackTrace.hpp"
#include "utils/LimitValues.hpp"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

TimerMgr* gTimerMgr = nullptr;

typedef std::map<int32_t, TimerData>::iterator _timerMapIt;
typedef std::map<int32_t, TimerData>::const_iterator _timerMapConstIt;

typedef std::set<int32_t>::iterator _removeTimerSetIt;

TimerMgr::TimerMgr()
    : _timerSpeed(TimerSpeed::NORMAL), _isTimerMgrPaused(false) {

}

TimerMgr::~TimerMgr() {
  TRACE_ENTRY_EXIT;
}

int32_t TimerMgr::init() {
  TRACE_ENTRY_EXIT;

  return SUCCESS;
}

int32_t TimerMgr::recover() { return SUCCESS; }

void TimerMgr::deinit() {
  // free dynamically allocated timer data resources
  // no need to erase the elemenets -> the map destructor will do it
  for (_timerMapIt it = _timerMap.begin(); it != _timerMap.end(); ++it) {
    if (TimerStructure::USER_DEFINED == it->second.timerStructure) {
      // free dynamically allocated resources if free func is set
      if (nullptr != it->second.freeFunc) {
        it->second.freeFunc(it->second.funcData);
        it->second.funcData = nullptr;
      }
    }
  }
}

const char* TimerMgr::getName() { return "TimerMgr"; }

void TimerMgr::process() {
  const int64_t millisecondsElapsed =
      _timeInternal.getElapsed().toMilliseconds();

  // do the loop update by hand so we can safely remove elements
  for (_timerMapIt it = _timerMap.begin(); it != _timerMap.end();) {
    if (it->second.isPaused)  // timer paused, do not update it
    {
      ++it;
      continue;
    }

    it->second.remaining -= millisecondsElapsed;
    if (0 > it->second.remaining) {
      onTimerTimeout(it->first, it->second);
    }

    ++it;  // normal loop update
  }

  // check for timers that requested external closing
  removeTimersInternal();
}

void TimerMgr::handleEvent([[maybe_unused]]const InputEvent& e) {
}

void TimerMgr::changeSpeed() {
  switch (_timerSpeed) {
    case TimerSpeed::NORMAL:
      _timerSpeed = TimerSpeed::FAST;
      break;

    case TimerSpeed::FAST:
      _timerSpeed = TimerSpeed::VERY_FAST;
      break;

    case TimerSpeed::VERY_FAST:
      _timerSpeed = TimerSpeed::NORMAL;
      break;

    default:
      LOGERR("Unknown value of _timerSpeed: %d", _timerSpeed);
      break;
  }
}

void TimerMgr::startUserTimer(const int64_t interval, const int32_t timerId,
                                  const cbFunc func, const cbFunc freeFunc,
                                  void* funcData, const TimerType timerType,
                                  const TimerGroup timerGroup) {
  TRACE_ENTRY_EXIT;

  if (isActiveTimerId(timerId)) {
    LOGERR(
        "Warning, timer with ID: %d already exist. "
        "Will not start new timer",
        timerId);
    return;
  }

  bool isPaused = false;
  if (TimerGroup::INTERRUPTIBLE == timerGroup) {
    // TimerMgr is paused -> add the new timer, but add it in paused state
    if (_isTimerMgrPaused) {
      isPaused = true;
    }
  }

  // at start the remaining interval is equal to whole interval
  const TimerData timerData(
      interval,                      // original interval
      interval,                      // remaining interval
      func,                          // function callback
      freeFunc,                      // free function callback
      funcData,                      // callback data
      timerType,                     // ONESHOT or PULSE
      timerGroup,                    // INTERRUPTIBLE or NON_INTERRUPTIBLE,
      TimerStructure::USER_DEFINED,  // TIMER_CLIENT or USER_DEFINED timer
      nullptr,                       // TimerClient instance
      isPaused);                     // isPaused flag

  _timerMap.emplace(timerId, timerData);
}

void TimerMgr::startTimerClientTimer(TimerClient* tcIstance,
                                         const int64_t interval,
                                         const int32_t timerId,
                                         const TimerType timerType,
                                         const TimerGroup timerGroup) {
  // The check for isActiveTimerId is invoked from TimerClient class

  bool isPaused = false;
  if (TimerGroup::INTERRUPTIBLE == timerGroup) {
    // TimerMgr is paused -> add the new timer, but add it in paused state
    if (_isTimerMgrPaused) {
      isPaused = true;
    }
  }

  // at start the remaining interval is equal to whole interval
  const TimerData timerData(
      interval,                      // original interval
      interval,                      // remaining interval
      nullptr,                       // function callback
      nullptr,                       // free function callback
      nullptr,                       // callback data
      timerType,                     // ONESHOT or PULSE
      timerGroup,                    // INTERRUPTIBLE or NON_INTERRUPTIBLE,
      TimerStructure::TIMER_CLIENT,  // TIMER_CLIENT or USER_DEFINED timer
      tcIstance,                     // TimerClient instance
      isPaused);                     // isPaused flag

  _timerMap.emplace(timerId, timerData);
}

void TimerMgr::stopTimer(const int32_t timerId) {
  TRACE_ENTRY_EXIT;

  if (isActiveTimerId(timerId)) {
    _removeTimerSet.insert(timerId);
  } else {
    LOGERR(
        "Warning, trying to remove a non-existing timer with ID: %d."
        " Be sure to check your timerId with .isActiveTimerId(timerId) "
        "before calling .stopTimer(timerId)",
        timerId);

    LOG("Printing stack trace for better debug info");
    printStacktrace();
  }
}

void TimerMgr::stopTimerAndDetachTimerClient(const int32_t timerId) {
  TRACE_ENTRY_EXIT;

  /** NOTE: timer could have already been stopped in some ::deinit() func
   * which will lead for it to be present in the _removeTimerSet so
   * invoking of ::isActiveTimer() here will result in false -> leading to
   * no detaching of the TimerClient instance.
   * Instead just search whether the timer is present in the timerMap
   * */
  if (isTimerLocatedInTheTimerMap(timerId)) {
    // but be sure to add it to the _removeTimerSet though :)
    _removeTimerSet.insert(timerId);

    /** isActiveTimerId() already assured that such key exists.
     *  Now, detach TimerClient instance, because it is about to be
     *                              destroyed by TimerClient desctructor.
     * */
    _timerMap[timerId].tcInstance = nullptr;
  } else {
    LOGERR(
        "Warning, trying to remove a non-existing timer with ID: %d."
        " Be sure to check your timerId with .isActiveTimerId(timerId) "
        "before calling .stopTimer(timerId)",
        timerId);

    LOG("Printing stack trace for better debug info");
    printStacktrace();
  }
}

void TimerMgr::restartTimerClientTimerInterval(const int32_t timerId) {
  TRACE_ENTRY_EXIT;

  if (isActiveTimerId(timerId)) {
    _timerMapIt it = _timerMap.find(timerId);

    // NOTE: it is guaranteed by the TimerClient class
    //      (the called of this method) that the owner of the timer would
    //      indeed be a TimerClient instance

    // restart the remaining interval
    it->second.remaining = it->second.interval;
  } else {
    LOGERR(
        "Warning, trying to restart a non-existing timer with ID: %d."
        "Only timers that are already active can be restarted",
        timerId);

    LOG("Printing stack trace for better debug info");
    printStacktrace();
  }
}

void TimerMgr::restartUserTimerInterval(const int32_t timerId) {
  TRACE_ENTRY_EXIT;

  if (isActiveTimerId(timerId)) {
    _timerMapIt it = _timerMap.find(timerId);

    if (nullptr == it->second.tcInstance) {
      // restart the remaining interval
      it->second.remaining = it->second.interval;
    } else {
      LOGERR(
          "Warning, trying to restart a timer with ID: %d from a "
          "UserTimerClient method, while the timer is owned by some "
          "other TimerClient instance!",
          timerId);

      LOG("Printing stack trace for better debug info");
      printStacktrace();
    }
  } else {
    LOGERR(
        "Warning, trying to restart a non-existing timer with ID: %d."
        "Only timers that are already active can be restarted",
        timerId);

    LOG("Printing stack trace for better debug info");
    printStacktrace();
  }
}

void TimerMgr::addTimeToTimerClientTimer(const int32_t timerId,
                                             const int64_t intervalToAdd) {
  if (isActiveTimerId(timerId)) {
    _timerMapIt it = _timerMap.find(timerId);

    // NOTE: it is guaranteed by the TimerClient class
    //      (the called of this method) that the owner of the timer would
    //      indeed be a TimerClient instance

    // increase the remaining interval
    it->second.remaining += intervalToAdd;
  } else {
    LOGERR(
        "Warning, trying to add time to a non-existing timer with ID: %d"
        ". Only timers that are already active can be manipulated",
        timerId);

    LOG("Printing stack trace for better debug info");
    printStacktrace();
  }
}

void TimerMgr::addTimeToUserTimer(const int32_t timerId,
                                      const int64_t intervalToAdd) {
  TRACE_ENTRY_EXIT;

  if (isActiveTimerId(timerId)) {
    _timerMapIt it = _timerMap.find(timerId);

    if (nullptr == it->second.tcInstance) {
      // increase the remaining interval
      it->second.remaining += intervalToAdd;
    } else {
      LOGERR(
          "Warning, trying to add time to timer with ID: %d from a "
          "UserTimerClient method, while the timer is owned by some "
          "other TimerClient instance!",
          timerId);

      LOG("Printing stack trace for better debug info");
      printStacktrace();
    }
  } else {
    LOGERR(
        "Warning, trying to add time to a non-existing timer with ID: %d"
        ". Only timers that are already active can be manipulated",
        timerId);

    LOG("Printing stack trace for better debug info");
    printStacktrace();
  }
}

void TimerMgr::removeTimeFromTimerClientTimer(
    const int32_t timerId, const int64_t intervalToRemove) {
  TRACE_ENTRY_EXIT;

  if (isActiveTimerId(timerId)) {
    _timerMapIt it = _timerMap.find(timerId);

    // NOTE: it is guaranteed by the TimerClient class
    //      (the called of this method) that the owner of the timer would
    //      indeed be a TimerClient instance

    if (it->second.remaining > intervalToRemove) {
      // lower the remaining interval
      it->second.remaining -= intervalToRemove;
    } else {
      LOGERR(
          "Warning, trying to remove time interval: %ld from timer"
          " with ID: %d while the timer only has: %ld ms "
          "remaining. Method will take no effect!,",
          intervalToRemove, timerId, it->second.remaining);

      LOG("Printing stack trace for better debug info");
      printStacktrace();
    }
  } else {
    LOGERR(
        "Warning, trying to remove time from a non-existing timer with "
        "ID: %d. Only timers that are already active can be manipulated",
        timerId);

    LOG("Printing stack trace for better debug info");
    printStacktrace();
  }
}

void TimerMgr::removeTimeFromUserTimer(const int32_t timerId,
                                           const int64_t intervalToRemove) {
  TRACE_ENTRY_EXIT;

  if (isActiveTimerId(timerId)) {
    _timerMapIt it = _timerMap.find(timerId);

    if (nullptr == it->second.tcInstance) {
      if (it->second.remaining > intervalToRemove) {
        // lower the remaining interval
        it->second.remaining -= intervalToRemove;
      } else {
        LOGERR(
            "Warning, trying to remove time interval: %ld from timer"
            " with ID: %d while the timer only has: %ld ms "
            "remaining. Method will take no effect!,",
            intervalToRemove, timerId, it->second.remaining);

        LOG("Printing stack trace for better debug info");
        printStacktrace();
      }
    } else {
      LOGERR(
          "Warning, trying to add time to timer with ID: %d from a "
          "UserTimerClient method, while the timer is owned by some "
          "other TimerClient instance!",
          timerId);

      LOG("Printing stack trace for better debug info");
      printStacktrace();
    }
  } else {
    LOGERR(
        "Warning, trying to remove time from a non-existing timer with "
        "ID: %d. Only timers that are already active can be manipulated",
        timerId);

    LOG("Printing stack trace for better debug info");
    printStacktrace();
  }
}

int64_t TimerMgr::getTimerRemainingInterval(const int32_t timerId) const {
  int64_t remainingTime = 0;

  if (isActiveTimerId(timerId)) {
    _timerMapConstIt it = _timerMap.find(timerId);
    remainingTime = it->second.remaining;
  } else {
    LOGERR(
        "Warning, invoking of .getTimerRemainingInterval() for "
        "non-existing timer with ID: %d. Be sure to check your timerId "
        "with .isActiveTimerId(timerId) before calling "
        ".getTimerRemainingInterval(timerId)",
        timerId);

    LOG("Printing stack trace for better debug info");
    printStacktrace();
  }

  return remainingTime;
}

bool TimerMgr::isActiveTimerId(const int32_t timerId) const {
  return (_removeTimerSet.end() == _removeTimerSet.find(timerId)) &&
         (_timerMap.end() != _timerMap.find(timerId));
}

void TimerMgr::onTimerTimeout(const int32_t timerId, TimerData& timerData) {
  TRACE_ENTRY_EXIT;

  if (_removeTimerSet.end() != _removeTimerSet.find(timerId)) {
    /** Someone could have requested external closure of the timer
     * in this case do not attempt to execute callback, because it could be
     * invalid.
     * Example: TimerClient() destructor asked for stopTimer() and it's
     * instance is already destroyed
     * */
    return;
  }

  // execute function callback with provided data
  if (TimerStructure::USER_DEFINED == timerData.timerStructure) {
    timerData.func(timerData.funcData);
  } else  // it is timer client instance
  {
    timerData.tcInstance->onTimeout(timerId);
  }

  if (timerData.timerType == TimerType::ONESHOT) {
    // If timer was on TimerType::ONESHOT it should close on it's own
    _removeTimerSet.insert(timerId);

    return;
  }

  // at this point remaining will be zero or negative value
  // we need to restart the remaining time while taking into account
  // that the timer probably postponed the original entered user interval
  // Example: timerData.interval = 5000ms;
  // process() is called every 2000ms.
  // after first process()  -> remaining interval is +3000ms;
  // after second process() -> remaining interval is +1000ms;
  // after third process()  -> remaining interval is -1000ms;
  // at this point execute function callback and if TimerTime is set to PULSE
  // restart timer's remaining interval to original interval (5000ms)
  // minus the postponed period (-1000ms) ->
  // new remaining interval value is 5000ms - 1000ms = 4000ms
  timerData.remaining += timerData.interval;
}

void TimerMgr::removeTimersInternal() {
  // set is empty -> no timers requested external closing
  if (_removeTimerSet.empty()) {
    return;
  }

  _removeTimerSetIt setIt = _removeTimerSet.begin();
  for (; setIt != _removeTimerSet.end(); ++setIt) {
    _timerMapIt mapIt = _timerMap.find(*setIt);

    // key not found
    if (mapIt == _timerMap.end()) {
      LOGERR(
          "Warning, trying to remove a non-existing timer with ID: %d."
          " Be sure to check your timerId with .isActiveTimerId(timerId)"
          " before calling .stopTimer(timerId)",
          *setIt);
    } else  // key found
    {
      // only non-timerClient timers contain dynamically created resources
      if (TimerStructure::USER_DEFINED == mapIt->second.timerStructure) {
        // free dynamically allocated resources if free func is set
        if (nullptr != mapIt->second.freeFunc) {
          mapIt->second.freeFunc(mapIt->second.funcData);
          mapIt->second.funcData = nullptr;
        }
      } else {
        // check if TimerClient instance is still active
        if (nullptr != mapIt->second.tcInstance) {
          // send signal to TimerClient instance to remove timerId
          // from it's list of managed timers

          if (SUCCESS !=
              mapIt->second.tcInstance->removeTimerIdFromList(mapIt->first)) {
            LOGERR(
                "Warning, internal error in "
                "removeTimerIdFromList() with timerId: %d",
                mapIt->first);
          }
        }
      }

      // erase the timer from the _timerMap
      _timerMap.erase(mapIt);
    }
  }

  // clear the removeTimerSet
  _removeTimerSet.clear();
}

void TimerMgr::pauseAllTimers() {
  TRACE_ENTRY_EXIT;

  if (_isTimerMgrPaused) {
    LOGERR(
        "TimerMgr is already paused, ::pauseAllTimers() "
        "will not be executed twice");

    return;
  }

  _isTimerMgrPaused = true;

  for (_timerMapIt it = _timerMap.begin(); it != _timerMap.end(); ++it) {
    if (TimerGroup::INTERRUPTIBLE == it->second.timerGroup) {
      it->second.isPaused = true;
    }
  }
}

void TimerMgr::resumeAllTimers() {
  TRACE_ENTRY_EXIT;

  if (!_isTimerMgrPaused) {
    LOGERR(
        "TimerMgr was not paused in the first place, "
        "::resumeAllTimers() will not be executed");

    return;
  }

  _isTimerMgrPaused = false;

  for (_timerMapIt it = _timerMap.begin(); it != _timerMap.end(); ++it) {
    if (TimerGroup::INTERRUPTIBLE == it->second.timerGroup) {
      it->second.isPaused = false;
    }
  }
}

int64_t TimerMgr::getClosestNonZeroTimerInterval() const {
  int64_t interval = INIT_INT64_VALUE;

  for (_timerMapConstIt it = _timerMap.begin(); it != _timerMap.end(); ++it) {
    if (interval > it->second.remaining) {
      /** If remaining interval is 0 or less -> this means the timer
       * is just about to tick. It is important here to take
       * the original interval for calculations.
       *
       * Example:
       *          We have 3 timers with original PULSE intervals of
       *                                                   50, 200, 500.
       *          The first call to the getClosestNonZeroTimerInterval()
       *          will result in 50.
       *          On the second call to the method however their
       *          remaining intervals are 0, 150, 500 but the result
       *          150 is not the correct answer.
       *          When a timer remaining interval is 0 -> it's original
       *          timer interval should be taken into account.
       *          Since the first timer had a original interval of 50 ->
       *          the result of the second invocation of the method
       *          should yield again the result 50.
       * */
      if (0 >= it->second.remaining)  // update with original interval
      {
        if (interval > it->second.interval) {
          interval = it->second.interval;
        }
      } else  // normal update with the remaining interval
      {
        interval = it->second.remaining;
      }
    }
  }

  return interval;
}

void TimerMgr::onInitEnd() {
  // reset the timer so it can clear the "stored" time since the creation
  // of the TimerMgr instance and this function call
  _timeInternal.getElapsed();
}

bool TimerMgr::isTimerLocatedInTheTimerMap(const int32_t timerId) const {
  return _timerMap.end() != _timerMap.find(timerId);
}
