#ifndef MANAGER_UTILS_RSRCMGR_H_
#define MANAGER_UTILS_RSRCMGR_H_

// System headers
#include <cstdint>

// Other libraries headers
#include "sdl_utils/containers/SDLContainers.h"

// Own components headers
#include "manager_utils/managers/MgrBase.h"

// Forward declarations
class InputEvent;

class RsrcMgr final : public MgrBase, public SDLContainers {
 public:
  RsrcMgr() = delete;

  explicit RsrcMgr(const SDLContainersConfig &cfg);

  virtual ~RsrcMgr() noexcept;

  //================= START MgrBase related functions ====================

  /** @brief used to initialize the current manager.
   *         NOTE: this is the first function that will be called.
   *
   *  @return ErrorCode - error code
   * */
  ErrorCode init() override;

  /** @brief used to recover the current manager.
   *         NOTE: this function that will be called if init() passed
   *                                                       successfully.
   *         NOTE2: recover() function will be called after the
   *                successful pass of init() function, even if system
   *                was shutdown correctly.
   *
   *  @return ErrorCode - error code
   * */
  ErrorCode recover() override;

  /** @brief used to deinitialize the current manager.
   * */
  void deinit() override;

  /** @brief used to process the current manager (poll him on every
   *         engine cycle so the managers can do any internal updates, if
   *                                                     such are needed).
   * */
  void process() override;

  /** @brief captures user inputs (if any)
   *
   *  @param const InputEvent & - user input event
   * */
  void handleEvent(const InputEvent& e) override;

  /** @brief returns the name of the current manager
   *
   *  @return const char * - current manager name
   * */
  const char* getName() override;

  //================== END MgrBase related functions =====================

  /** @brief a callback fired when a successful
   *         RendererCmd::LOAD_TEXTURE_MULTIPLE batch has been completed
   *         by the renderer.
   *
   *  @param const int32_t - unique ID of the batch that was loaded
   *
   *         NOTE: every project individual RsrcMgr should override
   *               this method if wants to attach a callback and be
   *               informed when multiple texture batch upload to the GPU
   *               is finished.
   *
   *         Example: when game dynamic resources are finished loading.
   *
   *         WARNING: this method will be called from the main(drawing)
   *                  thread. Do not forget to lock your data!
   **/
  void onLoadTextureMultipleCompleted(const int32_t batchId) override;

  /** @brief used to acquire the occupied GPU VRAM from the RsrcMgr
   *
   *  @return uint64_t - occupied VRAM in bytes
   * */
  uint64_t getGPUMemoryUsage() const;
};

extern RsrcMgr* gRsrcMgr;

#endif /* MANAGER_UTILS_RSRCMGR_H_ */
