#ifndef MANAGER_UTILS_TIMERMGR_H_
#define MANAGER_UTILS_TIMERMGR_H_

// System headers
#include <cstdint>
#include <map>
#include <set>

// Other libraries headers
#include "utils/time/Time.h"

// Own components headers
#include "manager_utils/managers/MgrBase.h"
#include "manager_utils/time/defines/TimerClientDefines.h"

// Forward declarations
class InputEvent;
class TimerClient;

class TimerMgr final : public MgrBase {
 public:
  TimerMgr();
  virtual ~TimerMgr() noexcept;

  //================= START MgrBase related functions ====================

  /** @brief used to initialize the current manager.
   *         NOTE: this is the first function that will be called.
   *
   *  @return ErrorCode - error code
   * */
  ErrorCode init() override;

  /** @brief used to recover the current manager.
   *         NOTE: this function that will be called if init() passed
   *                                                       successfully.
   *         NOTE2: recover() function will be called after the
   *                successful pass of init() function, even if system
   *                was shutdown correctly.
   *
   *  @return ErrorCode - error code
   * */
  ErrorCode recover() override;

  /** @brief used to deinitialize the current manager.
   * */
  void deinit() override;

  /** @brief used to process the current manager (poll him on every
   *         engine cycle so the managers can do any internal updates, if
   *                                                     such are needed).
   * */
  void process() override;

  /** @brief captures user inputs (if any)
   *
   *  @param const InputEvent & - user input event
   * */
  void handleEvent(const InputEvent& e) override;

  /** @brief returns the name of the current manager
   *
   *  @return const char * - current manager name
   * */
  const char* getName() override;

  //================== END MgrBase related functions =====================

  /**
   * @brief an interface function to change the speed of the timers
   */
  virtual void changeSpeed();

  /** @brief used to force an external update on the timer measurement.
   *
   *  This is needed in some special cases for example right before
   *  opening a game the timers should be forcefully updated.
   *
   *  If this is not done what would happen is:
   *      > Game MagicalEgypt is selected.
   *      > The dynamic resource loading will take ~2000ms;
   *      > The game ::init() method passes and some timers are started
   *          (lets say with interval 50ms);
   *      > On next engine cycle the TimerMgr is up processed and
   *        the passes time is 2000ms resulting in a ticked timer
   *        with remaining interval of 50 - 2000 = - 1950ms.
   *        This will mean that for 19+ contiguous engine cycle those same
   *        timers onTimeout() will be wrongly invoked.
   * */
  void forceUpdate() { TimerMgr::process(); }

  /** @brief starts timer with provided arguments
   *    this functions does not return error code for performance reasons
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
  void startUserTimer(
      const int64_t interval, const int32_t timerId, const cbFunc func,
      const cbFunc freeFunc, void* funcData, const TimerType timerType,
      const TimerGroup timerGroup = TimerGroup::NON_INTERRUPTIBLE);

  /** @brief starts timer with provided arguments
   *    this functions does not return error code for performance reasons
   *
   *  @param TimerClient *    - instance of TimerClient, which
   *                                  onTimeout() function will be called
   *  @param const int64_t    - time (in milliseconds) after which
   *                                     the timer Timeout will be called
   *  @param const int32_t    - unique timer ID
   *  @param const TimerType  - ONESHOT(single tick) or
   *                            PULSE(constant ticks)
   *  @param const TimerGroup - INTERRUPTIBLE   or NON_INTERRUPTIBLE -
   *                            (can be paused) or (not)
   * */
  void startTimerClientTimer(
      TimerClient* tcIstance, const int64_t interval, const int32_t timerId,
      const TimerType timerType,
      const TimerGroup timerGroup = TimerGroup::NON_INTERRUPTIBLE);

  /** @brief stops timer with specified timerId (if such timer exits)
   *
   *  @param const int32_t - unique timerID
   * */
  void stopTimer(const int32_t timerId);

  /** @brief stops timer with specified timerId (if such timer exits)
   *         and detach TimerClient instance, because it is about to be
   *                                                           destroyed.
   *
   *  @param const int32_t - unique timerID
   * */
  void stopTimerAndDetachTimerClient(const int32_t timerId);

  /** @brief used to restart timer remaining interval to
   *         it's original interval
   *
   *  @param const int32_t - unique timerID
   *
   *         NOTE: this method could only be called on a timer that is
   *               owned by it's TimerClient instance and that is
   *                                                      already active.
   * */
  void restartTimerClientTimerInterval(const int32_t timerId);

  /** @brief used to restart timer remaining interval to
   *         it's original interval
   *
   *  @param const int32_t - unique timerID
   *
   *         NOTE: this method could only be called on a timer that is
   *               not owned by a TimerClient instance and that is
   *                                                      already active.
   * */
  void restartUserTimerInterval(const int32_t timerId);

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
  void addTimeToTimerClientTimer(const int32_t timerId,
                                 const int64_t intervalToAdd);

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
  void addTimeToUserTimer(const int32_t timerId, const int64_t intervalToAdd);

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
  void removeTimeFromTimerClientTimer(const int32_t timerId,
                                      const int64_t intervalToRemove);

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
  void removeTimeFromUserTimer(const int32_t timerId,
                               const int64_t intervalToRemove);

  /** @brief used to acquire the remaining timer interval for the
   *                                                      selected timer
   *
   *  @param const int32_t - unique timerID
   *
   *  @return int64_t - active timers count
   * */
  int64_t getTimerRemainingInterval(const int32_t timerId) const;

  /** @brief checks if timer with specific Id is activated
   *
   *  @param const int32_t - unique timerID
   *
   *  @returns bool - is active timer or not
   * */
  bool isActiveTimerId(const int32_t timerId) const;

  /** @brief used to acquire the size of the actual active timers
   *                                                      in the system
   *
   *  @return uint64_t - active timers count
   * */
  uint64_t getActiveTimersCount() const { return _timerMap.size(); }

  /** @brief paused all timers(which has TimerGroup::INTERRUPTIBLE)
   *
   * */
  void pauseAllTimers();

  /** @brief resume all paused timers(which has TimerGroup::INTERRUPTIBLE)
   *
   * */
  void resumeAllTimers();

  /** @brief used to acquire the interval from the timer that will tick
   *         first from all the started timers
   *
   *  @return int64_t - time interval in milliseconds
   * */
  int64_t getClosestNonZeroTimerInterval() const;

  /**
   * @brief expose the timer speed so that outside parties can
   *      benefit from it
   * */
  int32_t getTimerSpeed() const { return _timerSpeed; }

  /** @brief must be called on project init() method end in order to
   *         set proper internal TimerMgr calculations */
  void onInitEnd();

 protected:

  /** Holds ajdustableSpeed percentage */
  int32_t _timerSpeed;

 private:
  /**
   * @brief the current values of the enum means how many percents of the
   *      passed timer interval will be actually set when the timer starts
   *
   * @example 100 -> 100%, then the timer will not be changed
   * @example 75 -> 75%, which means that the timer interval will be modified,
   *      and it will only be reduced with 25%. (75% of the interval
   *      will be kept)
   */
  enum TimerSpeed { NORMAL = 100, FAST = 75, VERY_FAST = 60 };

  /** @brief calls timer onTimeout callback function and
   *                                resets the timer (if TimerType::PULSE)
   *
   *  @param const int32_t - unique timerID
   *  @param TimerData &   - structure that holds timer specific data
   * */
  void onTimerTimeout(const int32_t timerId, TimerData& timerData);

  /** @brief used to remove timers from the _timerMap that are contained
   *                                                  in _removeTimerSet.
   * */
  void removeTimersInternal();

  /** @brief used to search whether the specific timer is present in the
   *         timerMap
   *
   *  @param const int32_t unique timerID
   *
   *  @return bool - is timer present in the timerMap or not
   * */
  bool isTimerLocatedInTheTimerMap(const int32_t timerId) const;

  /** Used to measure elapsed time and update _timerMap
   *                                               on every engine cycle
   * */
  Time _timeInternal;

  /** @brief a map that holds all active timers
   *          std::map is used since we'll be constantly iterating over it
   *          and we need to keep it sorted in order to give priority
   *          to system timers /they have lower unique ID's and therefore
   *                                          they will be iterated first/
   *
   *  @param int32_t   - unique timerID
   *  @param TimerData - structure that holds timer specific data
   * */
  std::map<int32_t, TimerData> _timerMap;

  /** @brief a set that holds all timers that requested external closing
   *                                           /with .stopTimer(timerId)/
   * */
  std::set<int32_t> _removeTimerSet;

  /** A flag to indicate whether the whole TimerMgr is in paused state
   * */
  bool _isTimerMgrPaused;
};

extern TimerMgr* gTimerMgr;

#endif /* MANAGER_UTILS_TIMERMGR_H_ */
