#ifndef MANAGER_UTILS_TIMERCLIENTSPEEDADJUSTABLE_H
#define MANAGER_UTILS_TIMERCLIENTSPEEDADJUSTABLE_H

// System headers

// Other libraries headers

// Own components headers
#include "manager_utils/time/TimerClient.h"

// Forward declarations

class TimerClientSpeedAdjustable : public TimerClient {
 public:
  virtual ~TimerClientSpeedAdjustable() noexcept;

  /** @brief starts timer with provided arguments
   *    this functions does not return error code for performance reasons
   *    WARNING: you need to manually stop your timer on game exit.
   *    (doing auto-cleanup with bring too much overhead to normal usage)
   *
   *  @param const int64_t    - time (in milliseconds) after which
   *                                     the timer Timeout will be called
   *  @param const int32_t    - unique timer ID
   *  @param const TimerType  - ONESHOT(single tick) or
   *                            PULSE(constant ticks)
   *  @param const TimerGroup - INTERRUPTIBLE   or NON_INTERRUPTIBLE -
   *                            (can be paused) or (not)
   * */
  void startTimer(const int64_t interval, const int32_t timerId,
                  const TimerType timerType,
                  const TimerGroup timerGroup = TimerGroup::NON_INTERRUPTIBLE);
};

#endif  // MANAGER_UTILS_TIMERCLIENTSPEEDADJUSTABLE_H
