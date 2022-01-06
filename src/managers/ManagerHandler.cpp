//Corresponding header
#include "manager_utils/managers/ManagerHandler.h"

//C system headers

//C++ system headers

//Other libraries headers

//Own components headers
#include "manager_utils/managers/config/ManagerHandlerConfig.hpp"
#include "manager_utils/managers/DrawMgr.h"
#include "manager_utils/managers/RsrcMgr.h"
#include "manager_utils/managers/TimerMgr.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

int32_t ManagerHandler::init(const ManagerHandlerConfig &cfg) {
  if (SUCCESS != allocateManagers(cfg)) {
    LOGERR("allocateManagers() failed -> Terminating...");
    return FAILURE;
  }

  Time initTime;
  //gDrawMgr should be initialized first, because it contains the renderer
  //Other managers (such as the RsrcMgr) needs it's renderer
  //to load graphical resources
  if (SUCCESS != _managers[Managers::DRAW_MGR_IDX]->init()) {
    LOGERR("Error in %s init() -> Terminating...",
        _managers[Managers::DRAW_MGR_IDX]->getName());
    return FAILURE;
  }

  //IMPORTANT: set renderer for SDLContainers
  gRsrcMgr->setRenderer(gDrawMgr->getRenderer());

  for (int32_t i = 1; i < Managers::TOTAL_MGRS_COUNT; ++i) {
    if (SUCCESS != _managers[i]->init()) {
      LOGERR("Error in %s init() -> Terminating...", _managers[i]->getName());
      return FAILURE;
    }

    LOGG("%s init() passed successfully for %ld ms", _managers[i]->getName(),
        initTime.getElapsed().toMilliseconds());
  }

  return SUCCESS;
}

void ManagerHandler::deinit() {
  /** Following the logic that DrawMgr should be initialized first ->
   * it should be deinitialized last
   * / a.k.a. last one to shut the door :) /
   * */
  for (int32_t i = Managers::TOTAL_MGRS_COUNT - 1; i >= 0; --i) {
    if (_managers[i]) //Sanity check
    {
      _managers[i]->deinit();

      delete _managers[i];
      _managers[i] = nullptr;

      nullifyGlobalManager(i);
    }
  }
}

void ManagerHandler::process() {
  for (int32_t i = 0; i < Managers::TOTAL_MGRS_COUNT; ++i) {
    _managers[i]->process();
  }
}

int32_t ManagerHandler::allocateManagers(const ManagerHandlerConfig &cfg) {
  //gDrawMgr should be initialized first, because it contains the renderer
  //Other managers may want to load graphical resources
  gDrawMgr = new DrawMgr(cfg.drawMgrBaseCfg);
  if (!gDrawMgr) {
    LOGERR("Error! Bad alloc for DrawMgr class -> Terminating...");
    return FAILURE;
  }

  gRsrcMgr = new RsrcMgr(cfg.sdlContainersCfg);
  if (!gRsrcMgr) {
    LOGERR("Error! Bad alloc for RsrcMgr class -> Terminating...");
    return FAILURE;
  }

  gTimerMgr = new TimerMgr;
  if (!gTimerMgr) {
    LOGERR("Error! Bad alloc for TimerMgr class -> Terminating...");
    return FAILURE;
  }

  //put global managers into container so they can be easily iterated
  //and used polymorphically
  _managers[Managers::DRAW_MGR_IDX] = gDrawMgr;
  _managers[Managers::RSRC_MGR_IDX] = gRsrcMgr;
  _managers[Managers::TIMER_MGR_IDX] = gTimerMgr;

  return SUCCESS;
}

void ManagerHandler::nullifyGlobalManager(const int32_t managerId) {
  /** Explicitly set the singleton pointer to nullptr, because someone might
   * still try to use them -> if so, the sanity checks should catch them
   * */
  switch (managerId) {
  case Managers::DRAW_MGR_IDX:
    gDrawMgr = nullptr;
    break;

  case Managers::RSRC_MGR_IDX:
    gRsrcMgr = nullptr;
    break;

  case Managers::TIMER_MGR_IDX:
    gTimerMgr = nullptr;
    break;

  default:
    LOGERR("Unknown managerId: %d provided", managerId);
    break;
  }
}

