#ifndef MANAGER_UTILS_DRAWMGRBASECONFIG_HPP_
#define MANAGER_UTILS_DRAWMGRBASECONFIG_HPP_

//C system headers

//C++ system headers

//Other libraries headers

//Own components headers
#include "sdl_utils/containers/config/SDLContainersConfig.hpp"
#include "sdl_utils/drawing/defines/MonitorDefines.h"

//Forward declarations

struct DrawMgrBaseConfig {
  int32_t monitorWidth = 0;
  int32_t monitorHeight = 0;
  WindowDisplayMode windowDisplayMode = WindowDisplayMode::UNKNOWN;
  WindowBorderMode windowBorderMode = WindowBorderMode::UNKNOWN;
};

#endif /* MANAGER_UTILS_DRAWMGRBASECONFIG_HPP_ */
