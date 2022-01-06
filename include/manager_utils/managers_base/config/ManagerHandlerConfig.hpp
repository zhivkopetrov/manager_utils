#ifndef MANAGER_UTILS_MANAGERHANDLERCFG_HPP_
#define MANAGER_UTILS_MANAGERHANDLERCFG_HPP_

//C system headers

//C++ system headers

//Other libraries headers

//Own components headers
#include "sdl_utils/containers/config/SDLContainersConfig.hpp"
#include "manager_utils/managers_base/config/DrawMgrBaseConfig.hpp"

//Forward declarations

struct ManagerHandlerConfig {
  SDLContainersConfig sdlContainersCfg;
  DrawMgrBaseConfig drawMgrBaseCfg;
};

#endif /* MANAGER_UTILS_MANAGERHANDLERCFG_HPP_ */
