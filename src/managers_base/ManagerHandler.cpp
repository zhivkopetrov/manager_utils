//Corresponding header
#include "manager_utils/managers_base/ManagerHandler.h"

//C system headers

//C++ system headers

//Other libraries headers

//Own components headers
#include "manager_utils/managers_base/config/ManagerHandlerConfig.hpp"
#include "manager_utils/managers_base/DrawMgrBase.h"
#include "manager_utils/managers_base/RsrcMgrBase.h"
#include "manager_utils/managers_base/TimerMgrBase.h"
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
  gRsrcMgrBase->setRenderer(gDrawMgrBase->getRenderer());

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
  gDrawMgrBase = new DrawMgrBase(cfg.drawMgrBaseCfg);
  if (!gDrawMgrBase) {
    LOGERR("Error! Bad alloc for DrawMgrBase class -> Terminating...");
    return FAILURE;
  }

  gRsrcMgrBase = new RsrcMgrBase(cfg.sdlContainersCfg);
  if (!gRsrcMgrBase) {
    LOGERR("Error! Bad alloc for RsrcMgrBase class -> Terminating...");
    return FAILURE;
  }

  gTimerMgrBase = new TimerMgrBase;
  if (!gTimerMgrBase) {
    LOGERR("Error! Bad alloc for TimerMgrBase class -> Terminating...");
    return FAILURE;
  }

  //put global managers into container so they can be easily iterated
  //and used polymorphically
  _managers[Managers::DRAW_MGR_IDX] = gDrawMgrBase;
  _managers[Managers::RSRC_MGR_IDX] = gRsrcMgrBase;
  _managers[Managers::TIMER_MGR_IDX] = gTimerMgrBase;

  return SUCCESS;
}

void ManagerHandler::nullifyGlobalManager(const int32_t managerId) {
  /** Explicitly set the singleton pointer to nullptr, because someone might
   * still try to use them -> if so, the sanity checks should catch them
   * */
  switch (managerId) {
  case Managers::DRAW_MGR_IDX:
    gDrawMgrBase = nullptr;
    gDrawMgrBase = nullptr;
    break;

  case Managers::RSRC_MGR_IDX:
    gRsrcMgrBase = nullptr;
    gRsrcMgrBase = nullptr;
    break;

  case Managers::TIMER_MGR_IDX:
    gTimerMgrBase = nullptr;
    gTimerMgrBase = nullptr;
    break;

  default:
    LOGERR("Unknown managerId: %d provided", managerId);
    break;
  }
}

