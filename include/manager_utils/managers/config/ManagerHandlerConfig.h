#ifndef MANAGER_UTILS_MANAGERHANDLERCFG_H_
#define MANAGER_UTILS_MANAGERHANDLERCFG_H_

//System headers

//Other libraries headers
#include "sdl_utils/containers/config/SDLContainersConfig.h"

//Own components headers
#include "manager_utils/managers/config/DrawMgrConfig.h"

//Forward declarations

struct ManagerHandlerConfig {
  SDLContainersConfig sdlContainersCfg;
  DrawMgrConfig drawMgrCfg;
};

#endif /* MANAGER_UTILS_MANAGERHANDLERCFG_H_ */
