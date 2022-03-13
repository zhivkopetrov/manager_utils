#ifndef MANAGER_UTILS_USERTIMERCLIENT_H_
#define MANAGER_UTILS_USERTIMERCLIENT_H_

// System headers
#include <cstdint>

// Other libraries headers

// Own components headers
#include "manager_utils/time/defines/TimerClientDefines.h"

/* A class used to start timers with user defines callbacks.
 * The motivation behind it:
 *      > No need to bind your timer to a object (Inherit TimerClient);
 *      > Not to include any unnecessary includes from TimerMgr.h;
 * */
class UserTimerClient {
 public:
  // forbid the default constructor and destructor
  UserTimerClient() = delete;

  /** @brief starts timer with provided arguments
   *    this functions does not return error code for performance reasons
   *    WARNING: you need to manually stop your timer on game exit.
   *    (doing auto-cleanup with bring too much overhead to normal usage)
   *
   *  @param const int64_t    - time (in milliseconds) after which
   *                                     the timer Timeout will be called
   *  @param const int32_t    - unique timer ID
   *  @param const cbFunc     - callback function, which will be called
   *                                        after remaining interval is 0
   *  @param const cbFunc     - callback function that will called to
   *                              clean the dynamically created resources
   *  @param void *           - dynamically created
   *                                               function data resources
   *  @param const TimerType  - ONESHOT(single tick) or
   *                            PULSE(constant ticks)
   *  @param const TimerGroup - INTERRUPTIBLE   or NON_INTERRUPTIBLE -
   *                            (can be paused) or (not)
   * */
  static void startTimer(
      const int64_t interval, const int32_t timerId, const cbFunc func,
      const cbFunc freeFunc, void* funcData, const TimerType timerType,
      const TimerGroup timerGroup = TimerGroup::NON_INTERRUPTIBLE);

  /** @brief stops timer with specified timerId (if such timer exits)
   *
   *  @param const int32_t - unique timerID
   * */
  static void stopTimer(const int32_t timerId);

  /** @brief checks if timer with specific Id is activated
   *
   *  @param const int32_t - unique timerID
   *
   *  @returns bool - is active timer or not
   * */
  static bool isActiveTimerId(const int32_t timerId);

  /** @brief used to restart timer remaining interval to
   *                                              it's original interval
   *
   *  @param const int32_t - unique timerID
   *
   *         NOTE: this method could only be called on a timer that is
   *               not owned by a TimerClient instance and that is
   *                                                      already active.
   * */
  static void restartTimerInterval(const int32_t timerId);

  /** @brief used to add additional time to the remaining interval of
   *                                      a UserTimerClient started timer
   *
   *  @param const int32_t - unique timerID
   *  @param const int64_t - interval to add
   *
   *         NOTE: this method could only be called on a timer that is
   *               not owned by a TimerClient instance and that is
   *                                                      already active.
   * */
  static void addTimeToTimer(const int32_t timerId,
                             const int64_t intervalToAdd);

  /** @brief used to remove additional time from the remaining interval
   *                                   of a UserTimerClient started timer
   *
   *  @param const int32_t - unique timerID
   *  @param const int64_t - interval to add
   *
   *         NOTE: this method could only be called on a timer that is
   *               not owned by a TimerClient instance and that is
   *                                                      already active.
   *
   *         NOTE2: you can only remove a interval, which is lower than
   *                the current remaining interval for the timer.
   * */
  static void removeTimeFromTimer(const int32_t timerId,
                                  const int64_t intervalToRemove);

  /** @brief used to acquire the remaining timer interval for the
   *                                                      selected timer
   *
   *  @param const int32_t - unique timerID
   *
   *  @return int64_t - active timers count
   * */
  static int64_t getTimerRemainingInterval(const int32_t timerId);
};

#endif /* MANAGER_UTILS_USERTIMERCLIENT_H_ */
