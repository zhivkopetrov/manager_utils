#ifndef MANAGER_UTILS_DRAWMGRBASECONFIG_HPP_
#define MANAGER_UTILS_DRAWMGRBASECONFIG_HPP_

//C system headers

//C++ system headers

//Other libraries headers

//Own components headers
#include "sdl_utils/containers/config/SDLContainersConfig.hpp"

//Forward declarations
class Renderer;
class MonitorWindow;

/*
 *  @param Renderer *      - the actual renderer used for drawing
 *  @param MonitorWindow * - the program window
 *  @param const int32_t   - the width of a single monitor (in px)
 *  @param const int32_t   - the height of a single monitor (in px)
 * */
struct DrawMgrBaseConfig {
  Renderer* renderer = nullptr;
  MonitorWindow* window = nullptr;
  int32_t monitorWidth = 0;
  int32_t monitorHeight = 0;
};

#endif /* MANAGER_UTILS_DRAWMGRBASECONFIG_HPP_ */
