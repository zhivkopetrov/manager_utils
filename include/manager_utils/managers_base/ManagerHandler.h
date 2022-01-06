#ifndef MANAGER_UTILS_MANAGERHANDLER_H_
#define MANAGER_UTILS_MANAGERHANDLER_H_

//C system headers

//C++ system headers
#include <cstdint>
#include <string>

//Other libraries headers

//Own components headers

//Forward declarations
class MgrBase;
struct ManagerHandlerConfig;

namespace Managers {
enum Indexes {
  DRAW_MGR_IDX, RSRC_MGR_IDX, TIMER_MGR_IDX,

  TOTAL_MGRS_COUNT
};
}

class ManagerHandler {
public:
  int32_t init(const ManagerHandlerConfig &cfg);

  /** @brief used to deinitialize all engine managers.
   * */
  void deinit();

  /** @brief used to process tall the engine managers (poll them on every
   *         engine cycle so the managers can do any internal updates, if
   *                                                     such are needed).
   * */
  void process();

  //================== END engine interface functions ====================

private:
  int32_t allocateManagers(const ManagerHandlerConfig &cfg);

  /** @brief used to reset the global manager pointer to nullptr at deinit
   *         so sanity checks could catch possible failures on system
   *         deinit
   *
   *  const int32_t - selected manager id
   * */
  void nullifyGlobalManager(const int32_t managerId);

  /** A container for all managers. We use polymorphical approach
   * so we can iterate it more easily
   * */
  MgrBase *_managers[Managers::TOTAL_MGRS_COUNT];
};

#endif /* MANAGER_UTILS_MANAGERHANDLER_H_ */

