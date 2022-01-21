#ifndef MANAGER_UTILS_NUMBERCOUNTER_H_
#define MANAGER_UTILS_NUMBERCOUNTER_H_

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "manager_utils/time/TimerClientSpeedAdjustable.h"
#include "manager_utils/drawing/Image.h"
#include "manager_utils/drawing/Text.h"

// Forward declarations

//values represent timer period
enum class NumberCounterSpeed {
  NORMAL = 125,
  FAST = 20,
  INSTANT = 0
};

class NumberCounter: public TimerClientSpeedAdjustable {
public:
  NumberCounter();

  /** @brief used to initialize the credit rotation entity
   *
   *   @param uint64_t    startValue - initial value for number
   *   @param uint64_t    fontId     - unique font resource ID
   *   @param int32_t     incTimerId - timer ID for increase in credit
   *   @param int32_t     decTimerId - timer ID for decrease in credit
   *   @param Rectangle & rect       - the number counter bounding rect
   *   @param uint64_t    backgroundRsrcId - unique resource ID for
   *                             background resources (if such is provided)
   *
   *   @return int32_t             - error code
   */
  int32_t init(const uint64_t startValue, const uint64_t fontId,
               const int32_t incTimerId, const int32_t decTimerId,
               const Rectangle &rect, const uint64_t backgroundRsrcId = 0);

  void draw();

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
    return _area;
  }

  uint64_t getValue() const {
    return _currentValue;
  }

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
  virtual void onTimeout(const int32_t timerId) override;

  // true - draw image for background
  bool _useBackground;

  // rectangle for number area
  Rectangle _area;

  // background for number area
  Image _balanceBackground;
};

#endif  // MANAGER_UTILS_NUMBERCOUNTER_H_
