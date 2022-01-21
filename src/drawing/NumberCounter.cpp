// Corresponding .h file
#include "manager_utils/drawing/NumberCounter.h"

// C system headers

// C++ system headers
#include <string>

// Other libraries headers
#include "utils/data_type/EnumClassUtils.h"
#include "utils/common/Position.h"
#include "utils/LimitValues.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

// Own components headers

NumberCounter::NumberCounter()
    : _timerPeriod(INIT_UINT8_VALUE),
      _currentValue(INIT_UINT64_VALUE),
      _increaseTimerId(0),
      _decreaseTimerId(0),
      _firstGear(INIT_UINT64_VALUE),
      _step(INIT_UINT64_VALUE),
      _final(INIT_UINT64_VALUE),
      _useBackground(false) {}

int32_t NumberCounter::init(const uint64_t startValue, const uint64_t fontId,
                             const int32_t incTimerId, const int32_t decTimerId,
                             const Rectangle& rect,
                             const uint64_t backgroundRsrcId) {
  // set timer IDs
  _increaseTimerId = incTimerId;
  _decreaseTimerId = decTimerId;

  // set Rectangle data
  _area = rect;

  // set start value
  _currentValue = startValue;

  // create balance field with text 0
  _balanceText.create(fontId, "00", Colors::WHITE);

  setAmountText();

  _useBackground = backgroundRsrcId;

  if (_useBackground) {
    _balanceBackground.create(backgroundRsrcId);
  }

  return SUCCESS;
}

void NumberCounter::setAmountText() {
  // calculate position for balance field
  _balanceText.setText(std::to_string(_currentValue).c_str());
  setTextPosition();
}

void NumberCounter::draw() {
  // if balance field use own background draw it
  if (_useBackground) {
    _balanceBackground.draw();
  }
  _balanceText.draw();
}

void NumberCounter::update(const uint64_t newValue, NumberCounterSpeed speed) {
  // new value is equal to old
  if (newValue == _currentValue) {
    return;
  }

  // set speed for update
  _timerPeriod = getEnumValue(speed);

  _final = newValue;

  // if speed equal to 0 - make change immediately
  if (!_timerPeriod) {
    fastStop();
  }
  // else calculate value of step
  else {
    calculateStep();
  }
}

void NumberCounter::increaseWith(const uint64_t amount,
                                 NumberCounterSpeed speed) {
  // set speed for update
  _timerPeriod = getEnumValue(speed);

  _final = _currentValue + amount;

  // if speed equal to 0 - make change immediately
  if (!_timerPeriod) {
    fastStop();
  }
  // else calculate value of step
  else {
    calculateStep();
  }
}

void NumberCounter::decreaseWith(const uint64_t amount,
                                 NumberCounterSpeed speed) {
  // set speed for update
  _timerPeriod = getEnumValue(speed);

  // don't decrease with amount larger then current amount
  if (amount > _currentValue) {
    _final = 0;
  } else {
    _final = _currentValue - amount;
  }

  // if speed equal to 0 - make change immediately
  if (!_timerPeriod) {
    fastStop();
  }
  // else calculate value of step
  else {
    calculateStep();
  }
}

void NumberCounter::calculateStep() {
  // difference between current and final value
  const int64_t diff = _currentValue - _final;

  // step is 4% from difference
  _step = static_cast<uint64_t>(diff / 25);

  // min step is 1
  _step = _step ? _step : 1;

  // if difference is less then 0 -> new value is bigger then old
  if (diff < 0) {
    // calculate value to change speed
    _firstGear = (_step * 24) + _currentValue;

    // start increase process
    increase();
  }
  // if difference is bigger then 0 -> new value is less then old
  else if (diff > 0) {
    // calculate value to change speed
    _firstGear = _currentValue - (_step * 24);

    // start decrease process
    decrease();
  }
}

void NumberCounter::setTextPosition() {
  int32_t currentWidth = 0;
  int32_t currentHeight = 0;

  if (_balanceText.isScalingActive()) {
    currentWidth = _balanceText.getScaledWidth();
    currentHeight = _balanceText.getScaledHeight();
  } else {
    currentWidth = _balanceText.getImageWidth();
    currentHeight = _balanceText.getImageHeight();
  }

  // set coordinate for text
  _balanceText.setPosition(
      Position::getCentral(_area, currentWidth, currentHeight));
}

void NumberCounter::increase() {
  // while current value is less then _firstGear don't change step value
  // after then slow down it twice for every iteration
  // min value for step is 1
  if (!(_currentValue < _firstGear) && !(_step /= 2)) {
    _step = 1;
  }

  _currentValue += _step;

  // start timer if it is not started
  if (!isActiveTimerId(_increaseTimerId)) {
    startTimer(_timerPeriod, _increaseTimerId, TimerType::PULSE,
               TimerGroup::INTERRUPTIBLE);
  }

  // update text panel
  setAmountText();
}

void NumberCounter::decrease() {
  // while current value is bigger then _firstGear don't change step value
  // after then slow down it twice for every iteration
  // min value for step is 1
  if (!(_currentValue > _firstGear) && !(_step /= 2)) {
    _step = 1;
  }

  _currentValue -= _step;

  // start timer if it is not started
  if (!isActiveTimerId(_decreaseTimerId)) {
    startTimer(_timerPeriod, _decreaseTimerId, TimerType::PULSE,
               TimerGroup::INTERRUPTIBLE);
  }

  // update text panel
  setAmountText();
}

void NumberCounter::fastStop() {
  if (isActiveTimerId(_decreaseTimerId)) {
    stopTimer(_decreaseTimerId);
  }
  if (isActiveTimerId(_increaseTimerId)) {
    stopTimer(_increaseTimerId);
  }

  // Don't update current value if fastStop() is called before update()
  if (INIT_UINT64_VALUE != _final) {
    _currentValue = _final;

    // update text panel
    setAmountText();
  }
}

bool NumberCounter::isRotating() const {
  return isActiveTimerId(_increaseTimerId) || isActiveTimerId(_decreaseTimerId);
}

void NumberCounter::setPosition(const Point& pos) {
  _area.x = pos.x;
  _area.y = pos.y;
  setTextPosition();
}

uint64_t NumberCounter::getEndValue() const {
  if (_currentValue != _final) {
    LOGERR("This NumberCounter is still in progress. The value will be "
           "able to be fetched, once the rotating finishes!");
    return 0;
  }

  return _final;
}

void NumberCounter::activateTextScaling() {
  _balanceText.activateScaling();
}

void NumberCounter::setTextMaxScalingWidth(const int32_t maxWidth) {
  _balanceText.setMaxScalingWidth(maxWidth);
}

void NumberCounter::onTimeout(const int32_t timerId) {
  if (timerId == _increaseTimerId) {
    // if visible amount is equal to real amount stop change it
    if (_currentValue == _final) {
      stopTimer(_increaseTimerId);
    }
    // else increase it again
    else {
      increase();
    }
  }
  else if (timerId == _decreaseTimerId) {
    // if visible amount is equal to real amount stop change it
    if (_currentValue == _final) {
      stopTimer(_decreaseTimerId);
    }
    // else decrease it again
    else {
      decrease();
    }
  } else {
    LOGERR("Error, received unsupported timerId: %d", timerId);
  }
}
