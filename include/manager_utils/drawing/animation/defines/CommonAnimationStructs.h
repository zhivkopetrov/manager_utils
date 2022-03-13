#ifndef MANAGER_UTILS_COMMONANIMATIONSTRUCTS_H_
#define MANAGER_UTILS_COMMONANIMATIONSTRUCTS_H_

// System headers
#include <cstdint>

// Other libraries headers
#include "utils/drawing/Point.h"

// Own components headers
#include "manager_utils/drawing/animation/defines/AnimationDefines.h"

// Forward declarations
class Image;

struct AnimBaseConfig {
  AnimBaseConfig();

  void reset();

  Image *externalImage;
  Point startPos;
  double startAngle;
  uint64_t rsrcId;
  int64_t timerInterval;
  int32_t timerId;
  AnimDir animDirection;
  AnimImageType animImageType;
  bool isTimerPauseble;
};

#endif /* MANAGER_UTILS_COMMONANIMATIONSTRUCTS_H_ */
