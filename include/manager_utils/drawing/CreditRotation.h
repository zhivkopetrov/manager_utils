#ifndef MANAGER_UTILS_CREDITROTATION_H_
#define MANAGER_UTILS_CREDITROTATION_H_

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "manager_utils/time/TimerClientSpeedAdjustable.h"
#include "manager_utils/drawing/Image.h"
#include "manager_utils/drawing/Text.h"

// Forward declarations

namespace Speed {
enum Possibilities { INSTANTLY = 0, FAST = 20, NORMAL = 125 };
};

class CreditRotation : public TimerClientSpeedAdjustable {
 public:
  CreditRotation();

  virtual ~CreditRotation() = default;

  /** @brief used to initialize the credit rotation entity
   *
   *   @param uint64_t    startValue - initial value for credit rotation
   *   @param uint64_t    fontId     - unique font resource ID
   *   @param int32_t     incTimerId - timer ID for increase in credit
   *   @param int32_t     decTimerId - timer ID for decrease in credit
   *   @param Rectangle & rect       - the credit rotation bounding rect
   *   @param uint64_t    backgroundRsrcId - unique resource ID for
   *                             background resources (if such is provided)
   *
   *   @return int32_t             - error code
   */
  int32_t init(const uint64_t startValue, const uint64_t fontId,
               const int32_t incTimerId, const int32_t decTimerId,
               const Rectangle& rect, const uint64_t backgroundRsrcId = 0);

  int32_t recover(const uint64_t amount);

  void draw();

  /** @brief Update data in balance/win field
   *
   *   @param uint64_t finalValue - value that field must to show
   *   @param enum speed - speed of rotation
   */
  virtual void update(const uint64_t finalValue,
                      enum Speed::Possibilities speed = Speed::NORMAL);

  /** @brief Increase current value with specific amount
   *
   *   @param uint64_t amount - amount to add
   *   @param enum speed - speed of rotation
   */
  virtual void increaseWith(const uint64_t amount,
                            enum Speed::Possibilities speed = Speed::NORMAL);

  /** @brief Decrease current value with specific amount
   *
   *   @param uint64_t amount - amount to subtract
   *   @param enum speed - speed of rotation
   */
  virtual void decreaseWith(const uint64_t amount,
                            enum Speed::Possibilities speed = Speed::NORMAL);

  /** @brief Stop updating process - show final value immediately
   *
   */
  virtual void fastStop();

  bool isRotating() const;

  /**
   * @brief set position of the credit rotation
   *      elements (background and balance)
   */
  void setPosition(const Point& pos);

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
   * @brief an interface function to get the credit rotation rectangle
   */
  Rectangle getRect() const { return _area; }

  uint64_t getCredit() const { return _currentValue; }

 protected:
  // period for timers
  uint8_t _timerPeriod;

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

 private:
  virtual void onTimeout(const int32_t timerId) override;

  // true - draw image for background
  bool _useBackground;

  // rectangle for balance/win area
  Rectangle _area;

  // background for balance/win area
  Image _balanceBackground;
};

#endif  // MANAGER_UTILS_CREDITROTATION_H_
