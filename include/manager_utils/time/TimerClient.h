#ifndef MANAGER_UTILS_TIME_TIMERCLIENT_H_
#define MANAGER_UTILS_TIME_TIMERCLIENT_H_

// System headers
#include <cstdint>

// Other libraries headers
#include "utils/class/NonCopyable.h"
#include "utils/ErrorCode.h"

// Own components headers
#include "manager_utils/time/defines/TimerClientDefines.h"

class TimerClient : public NonCopyable {
 public:
  TimerClient();
  virtual ~TimerClient() noexcept;

  TimerClient(TimerClient&& movedOther);
  TimerClient& operator=(TimerClient&& movedOther);

  /** @brief Callback on Timer timeout
   *
   *  @param const int32_t - timerId, which made the callback
   * */
  virtual void onTimeout(const int32_t timerId) = 0;

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

  /** @brief stops timer with specified timerId (if such timer exits)
   *
   *  @param const int32_t - unique timerID
   * */
  void stopTimer(const int32_t timerId);

  /** @brief checks if timer with specific Id is activated
   *
   *  @param const int32_t - unique timerID
   *
   *  @returns bool - is active timer or not
   * */
  bool isActiveTimerId(const int32_t timerId) const;

  /** @brief used to restart timer remaining interval to
   *                                              it's original interval
   *
   *  @param const int32_t - unique timerID
   *
   *         NOTE: this method could only be called on a timer that is
   *               owned by it's TimerClient instance and that is
   *                                                      already active.
   * */
  void restartTimerInterval(const int32_t timerId);

  /** @brief used to add additional time to the remaining interval of
   *                                 a TimerClient instance started timer
   *
   *  @param const int32_t - unique timerID
   *  @param const int64_t - interval to add
   *
   *         NOTE: this method could only be called on a timer that is
   *               owned by a TimerClient instance and that is
   *                                                      already active.
   * */
  void addTimeToTimer(const int32_t timerId, const int64_t intervalToAdd);

  /** @brief used to remove additional time from the remaining interval
   *                                   of a UserTimerClient started timer
   *
   *  @param const int32_t - unique timerID
   *  @param const int64_t - interval to add
   *
   *         NOTE: this method could only be called on a timer that is
   *               owned by a TimerClient instance and that is
   *                                                      already active.
   *
   *         NOTE2: you can only remove a interval, which is lower than
   *                the current remaining interval for the timer.
   * */
  void removeTimeFromTimer(const int32_t timerId,
                           const int64_t intervalToRemove);

  /** @brief used to acquire the remaining timer interval for the
   *                                                      selected timer
   *
   *  @param const int32_t - unique timerID
   *
   *  @return int64_t - active timers count
   * */
  int64_t getTimerRemainingInterval(const int32_t timerId) const;

  /** @brief used to remove timerId from list of managed timers
   *         NOTE: this function should be used only by TimerMgr itself.
   *         Do not call this function. Instead use .stopTimer(timerId).
   *
   *  @param const int32_t unique timerID
   *
   *  @returns ErrorCode - error code
   * */
  ErrorCode removeTimerIdFromList(const int32_t timerId);

 private:
  /** Since TimerClient header will be included a lot -> try not to
   *  include heavy includes such as std::unordered_set.
   *  This will heavily influence the compile time for every file
   *  that includes the TimerClient header.
   *  */

  /** @brief used to resize the managed timer list for this specific
   *                                              TimerClient instance
   *
   *  @returns ErrorCode - error code
   * */
  ErrorCode resizeTimerList();

  /** Timer ID list for auto-clean up of timers
   *  (to automatically stop all it's timers when TimerClient instance
   *  gets destroyed)
   *  */
  int32_t* _timerIdList;

  // Holds counter for current active timers
  int32_t _currTimerCount;

  // Hold limit for currently auto-managed timers
  int32_t _maxTimersCount;
};

#endif /* MANAGER_UTILS_TIME_TIMERCLIENT_H_ */
