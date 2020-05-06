#ifndef MANAGER_UTILS_MGRBASE_H_
#define MANAGER_UTILS_MGRBASE_H_

// C system headers

// C++ system headers
#include <cstdint>

// Other libraries headers

// Own components headers

// Forward declarations
class InputEvent;

class MgrBase {
 public:
  MgrBase() = default;
  virtual ~MgrBase() = default;

  // forbid the copy and move constructors
  MgrBase(const MgrBase& other) = delete;
  MgrBase(MgrBase&& other) = delete;

  // forbid the copy and move assignment operators
  MgrBase& operator=(const MgrBase& other) = delete;
  MgrBase& operator=(MgrBase&& other) = delete;

  /** @brief used to initialize the current manager.
   *         NOTE: this is the first function that will be called.
   *
   *  @return int32_t - error code
   * */
  virtual int32_t init() = 0;

  /** @brief used to recover the current manager.
   *         NOTE: this function that will be called if init() passed
   *                                                       successfully.
   *         NOTE2: recover() function will be called after every
   *                successful pass of init() function, even if system
   *                was shutdown correctly.
   *
   *  @return int32_t - error code
   * */
  virtual int32_t recover() = 0;

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
