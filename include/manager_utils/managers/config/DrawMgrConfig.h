#ifndef MANAGER_UTILS_DRAWMGRBASECONFIG_H_
#define MANAGER_UTILS_DRAWMGRBASECONFIG_H_

//C system headers

//C++ system headers

//Other libraries headers
#include "sdl_utils/drawing/defines/MonitorDefines.h"
#include "sdl_utils/drawing/config/RendererConfig.h"

//Own components headers

//Forward declarations

struct DrawMgrConfig {
  RendererConfig rendererConfig;
  int32_t monitorWidth = 0;
  int32_t monitorHeight = 0;
  WindowDisplayMode windowDisplayMode = WindowDisplayMode::UNKNOWN;
  WindowBorderMode windowBorderMode = WindowBorderMode::UNKNOWN;
};

#endif /* MANAGER_UTILS_DRAWMGRBASECONFIG_H_ */
