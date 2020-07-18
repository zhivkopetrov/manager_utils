#ifndef MANAGER_UTILS_COMMONANIMATIONSTRUCTS_H_
#define MANAGER_UTILS_COMMONANIMATIONSTRUCTS_H_

// C system headers

// C++ system headers
#include <cstdint>

// Other libraries headers

// Own components headers
#include "manager_utils/drawing/animation/defines/AnimationDefines.h"
#include "utils/drawing/Point.h"

// Forward declarations
class Image;

struct AnimBaseConfig {
  AnimBaseConfig();

  void reset();

  Image *externalImage;
  uint64_t rsrcId;
  Point startPos;
  int64_t timerInterval;
  int32_t timerId;
  AnimDir animDirection;
  AnimImageType animImageType;
  bool isTimerPauseble;
};

#endif /* MANAGER_UTILS_COMMONANIMATIONSTRUCTS_H_ */
