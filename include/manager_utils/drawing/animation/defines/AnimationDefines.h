#ifndef MANAGER_UTILS_ANIMATIONDEFINES_H_
#define MANAGER_UTILS_ANIMATIONDEFINES_H_

// System headers
#include <cstdint>

// Other libraries headers

// Own components headers

// Forward declarations

enum class AnimImageType  : uint8_t {
  INTERNAL = 0,
  EXTERNAL
};

enum class AnimType : uint8_t {
  FINITE = 0,
  INFINITE = 1,

  UNKNOWN = 255
};

enum class AnimDir : uint8_t {
  FORWARD = 0,
  BACKWARD = 1,

  UNKNOWN = 255
};

enum class PosAnimType : uint8_t {
  ONE_DIRECTIONAL = 0,
  BI_DIRECTIONAL = 1,

  UNKNOWN = 255
};

#endif /* MANAGER_UTILS_ANIMATIONDEFINES_H_ */
