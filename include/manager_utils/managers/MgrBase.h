#ifndef MANAGER_UTILS_MGRBASE_H_
#define MANAGER_UTILS_MGRBASE_H_

// System headers
#include <cstdint>

// Other libraries headers
#include "utils/class/NonCopyable.h"
#include "utils/class/NonMoveable.h"
#include "utils/ErrorCode.h"

// Own components headers

// Forward declarations
class InputEvent;

class MgrBase : public NonCopyable, public NonMoveable {
 public:
  MgrBase() = default;
  virtual ~MgrBase() noexcept = default;

  /** @brief used to initialize the current manager.
   *         NOTE: this is the first function that will be called.
   *
   *  @return ErrorCode - error code
   * */
  virtual ErrorCode init() = 0;

  /** @brief used to recover the current manager.
   *         NOTE: this function that will be called if init() passed
   *                                                       successfully.
   *         NOTE2: recover() function will be called after every
   *                successful pass of init() function, even if system
   *                was shutdown correctly.
   *
   *  @return ErrorCode - error code
   * */
  virtual ErrorCode recover() = 0;

  /** @brief used to deinitialize the current manager.
   * */
  virtual void deinit() = 0;

  /** @brief used to process the current manager (poll him on every
   *         engine cycle so the managers can do any internal updates, if
   *                                                     such are needed).
   * */
  virtual void process() = 0;

  /** @brief captures user inputs (if any)
   *
   *  @param const InputEvent & - user input event
   * */
  virtual void handleEvent(const InputEvent& e) = 0;

  /** @brief returns the name of the current manager
   *
   *  @return const char * - current manager name
   * */
  virtual const char* getName() = 0;
};

#endif /* MANAGER_UTILS_MGRBASE_H_ */
