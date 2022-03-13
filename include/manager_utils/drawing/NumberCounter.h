#ifndef MANAGER_UTILS_NUMBERCOUNTER_H_
#define MANAGER_UTILS_NUMBERCOUNTER_H_

// System headers
#include <functional>

// Other libraries headers
#include "utils/ErrorCode.h"

// Own components headers
#include "manager_utils/time/TimerClientSpeedAdjustable.h"
#include "manager_utils/drawing/Image.h"
#include "manager_utils/drawing/Text.h"

// Forward declarations

//values represent timer period
enum class NumberCounterSpeed {
  SLOW = 200,
  NORMAL = 125,
  FAST = 20,
  INSTANT = 0
};

/** @brief used to a trigger event
 *
 *  @param std::function<void(uint64_t) - callback on trigger value reached
 *  @param value                        - trigger value
 *  @param isIncreasingTrigger          - should trigger be called on increasing
 *                                        or decreasing step
 * */
struct NumberCounterTriggerConfig {
  std::function<void(uint64_t)> triggerCb;
  uint64_t value = 0;
  bool isIncreasingTrigger = true;
};

/** @brief used to initialize the credit rotation entity
 *
 *   @param Rectangle   rect           - the number counter bounding rect
 *   @param uint64_t  backgroundRsrcId - unique resource ID for background
 *                                       resources (if such is provided)
 *   @param Point backgroundRsrcPos    - start pos for background (if any)
 *                   if Point::UNDEFINED is specified the param will be ignored
 *   @param uint64_t    fontId         - unique font resource ID
 *   @param uint64_t    startValue     - initial value for number
 *   @param int32_t     incTimerId     - timer ID for increase in credit
 *   @param int32_t     decTimerId     - timer ID for decrease in credit
 *   @param NumberCounterTriggerConfig - trigger config
 */
struct NumberCounterConfig {
  Rectangle boundaryRect;
  uint64_t backgroundRsrcId = 0;
  Point backgroundRsrcPos = Points::UNDEFINED;
  uint64_t fontId = 0;
  Color fontColor = Colors::WHITE;
  uint64_t startValue = 0;
  int32_t incrTimerId = 0;
  int32_t decrTimerId = 0;
  NumberCounterTriggerConfig triggerCfg;
};

class NumberCounter: public TimerClientSpeedAdjustable {
public:
  NumberCounter();

  ErrorCode init(const NumberCounterConfig& cfg);

  void draw() const;

  /** @brief Update data in balance/win field
   *
   *   @param uint64_t           - value that field must to show
   *   @param NumberCounterSpeed - speed of rotation
   */
  virtual void update(const uint64_t finalValue,
                      NumberCounterSpeed speed = NumberCounterSpeed::NORMAL);

  /** @brief Increase current value with specific amount
   *
   *   @param uint64_t amount    - amount to add
   *   @param NumberCounterSpeed - speed of rotation
   */
  virtual void increaseWith(const uint64_t amount,
                            NumberCounterSpeed speed =
                                NumberCounterSpeed::NORMAL);

  /** @brief Decrease current value with specific amount
   *
   *   @param uint64_t amount    - amount to subtract
   *   @param NumberCounterSpeed - speed of rotation
   */
  virtual void decreaseWith(const uint64_t amount,
                            NumberCounterSpeed speed =
                                NumberCounterSpeed::NORMAL);

  /** @brief Stop updating process - show final value immediately
   *
   */
  virtual void fastStop();

  bool isRotating() const;

  /**
   * @brief set position of the number counter
   *      elements (background and balance)
   */
  void setPosition(const Point &pos);

  /**
   * @brief returns the target value, to which the value is rotating
   *
   * @return the targeted end value (also always will be the current
   *      visible value)
   */
  uint64_t getEndValue() const;

  /**
   * @brief activate the text scaling
   */
  void activateTextScaling();

  /**
   * @brief set the text max scaling width
   *
   * @param maxWidth - max scaling width
   */
  void setTextMaxScalingWidth(const int32_t maxWidth);

  /**
   * @brief an interface function to get the number counter rectangle
   */
  Rectangle getBoundaryRect() const {
    return _boundaryRect;
  }

  uint64_t getValue() const {
    return _currentValue;
  }

  void reactivateTrigger();

protected:
  /** @brief Update decreasing value
   *
   */
  void decrease();

  /** @brief Update increasing value
   *
   */
  void increase();

  /** @brief Set current credit to text
   *
   */
  virtual void setAmountText();

  /** @brief Set text in central position in balance/win area
   *
   */
  void setTextPosition();

  /** @brief Calculate size of step for every update iteration
   *
   */
  virtual void calculateStep();

  // period for timers
  int64_t _timerPeriod;

  // current value for showing
  uint64_t _currentValue;

  // ID for increasing timer
  int32_t _increaseTimerId;

  // ID for decreasing timer
  int32_t _decreaseTimerId;

  // use 4% step for 96% then reduction it progressively
  uint64_t _firstGear;

  // step for every iteration
  uint64_t _step;

  // value that must to show after update is end
  uint64_t _final;

  // text that show current balance
  Text _balanceText;

private:
  void onTimeout(const int32_t timerId) override;

  // rectangle for number area
  Rectangle _boundaryRect;

  // background for number area
  Image _balanceBackground;

  NumberCounterTriggerConfig _triggerCfg;
  bool _wasTriggerCalled = false;
};

#endif  // MANAGER_UTILS_NUMBERCOUNTER_H_
