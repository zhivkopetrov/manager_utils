#ifndef MANAGER_UTILS_MANAGERHANDLERCFG_HPP_
#define MANAGER_UTILS_MANAGERHANDLERCFG_HPP_

//C system headers

//C++ system headers

//Other libraries headers

//Own components headers
#include "sdl_utils/containers/config/SDLContainersConfig.hpp"
#include "manager_utils/managers/config/DrawMgrConfig.hpp"

//Forward declarations

struct ManagerHandlerConfig {
  SDLContainersConfig sdlContainersCfg;
  DrawMgrConfig drawMgrBaseCfg;
};

#endif /* MANAGER_UTILS_MANAGERHANDLERCFG_HPP_ */
