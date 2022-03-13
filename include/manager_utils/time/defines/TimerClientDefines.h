#ifndef MANAGER_UTILS_TIMERCLIENTDEFINES_H_
#define MANAGER_UTILS_TIMERCLIENTDEFINES_H_

// System headers
#include <cstdint>

// Other libraries headers

// Own components headers

// Forward declarations
class TimerClient;

enum class TimerType : uint8_t { UNKNOWN = 0, ONESHOT = 1, PULSE = 2 };

enum class TimerGroup : uint8_t {
  UNKNOWN = 0,
  INTERRUPTIBLE = 1,
  NON_INTERRUPTIBLE = 2
};

enum class TimerStructure : uint8_t {
  UNKNOWN = 0,
  TIMER_CLIENT = 1,
  USER_DEFINED = 2
};

// function pointer type
typedef void (*cbFunc)(void* params);

/* Common timer structure that holds internal data for the timer.
 * The structure is used for 2 kind of timers:
 *      > TimerClient instances:
 *      > Timers with user provided callbacks;
 * */
struct TimerData {
  TimerData() {
    interval = 0;
    remaining = 0;
    func = nullptr;
    freeFunc = nullptr;
    funcData = nullptr;
    timerType = TimerType::UNKNOWN;
    timerGroup = TimerGroup::UNKNOWN;
    timerStructure = TimerStructure::UNKNOWN;
    tcInstance = nullptr;
    isPaused = false;
  }

  explicit TimerData(const int64_t inputInterval, const int64_t inputRemaining,
                     const cbFunc inputFunc, const cbFunc inputFreeFunc,
                     void* inputFuncData, const TimerType inputTimerType,
                     const TimerGroup inputTimerGroup,
                     const TimerStructure inputTimerStructure,
                     TimerClient* inputTcInstance, const bool inputIsPaused)
      : interval(inputInterval),
        remaining(inputRemaining),
        func(inputFunc),
        freeFunc(inputFreeFunc),
        funcData(inputFuncData),
        timerType(inputTimerType),
        timerGroup(inputTimerGroup),
        timerStructure(inputTimerStructure),
        tcInstance(inputTcInstance),
        isPaused(inputIsPaused) {}

  int64_t interval;               // original interval
  int64_t remaining;              // remaining interval
  cbFunc func;                    // user provided callback
  cbFunc freeFunc;                // user provided clean up callback
  void* funcData;                 // user provided data for the callback
  TimerType timerType;            // ONESHOT or PULSE
  TimerGroup timerGroup;          // INTERRUPTIBLE or NON_INTERRUPTIBLE
  TimerStructure timerStructure;  // TIMER_CLIENT or USER_DEFINED timer
  TimerClient* tcInstance;        // TimerClient instance
  bool isPaused;                  // isTimerPaused
};

#endif /* MANAGER_UTILS_TIMERCLIENTDEFINES_H_ */
