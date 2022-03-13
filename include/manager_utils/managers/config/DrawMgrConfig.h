#ifndef MANAGER_UTILS_DRAWMGRBASECONFIG_H_
#define MANAGER_UTILS_DRAWMGRBASECONFIG_H_

//System headers

//Other libraries headers
#include "sdl_utils/drawing/defines/MonitorDefines.h"
#include "sdl_utils/drawing/config/RendererConfig.h"
#include "sdl_utils/drawing/config/MonitorWindowConfig.h"

//Own components headers

//Forward declarations

struct DrawMgrConfig {
  RendererConfig rendererConfig;
  MonitorWindowConfig monitorWindowConfig;
};

#endif /* MANAGER_UTILS_DRAWMGRBASECONFIG_H_ */
