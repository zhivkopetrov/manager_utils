#ifndef MANAGER_UTILS_DRAWMGR_H_
#define MANAGER_UTILS_DRAWMGR_H_

// System headers

// Other libraries headers
#include "sdl_utils/drawing/defines/RendererDefines.h"
#include "sdl_utils/drawing/defines/DrawConstants.h"
#include "sdl_utils/drawing/MonitorWindow.h"

// Own components headers
#include "manager_utils/managers/MgrBase.h"
#include "manager_utils/managers/config/DrawMgrConfig.h"

// Forward declarations
class SDLContainers;
class InputEvent;
class Renderer;
class MonitorWindow;
struct DrawParams;
struct DrawMgrConfig;

class DrawMgr final : public MgrBase {
public:
  DrawMgr() = delete;

  explicit DrawMgr(const DrawMgrConfig &cfg);

  virtual ~DrawMgr() noexcept;

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
  void handleEvent(const InputEvent &e) override;

  /** @brief returns the name of the current manager
   *
   *  @return const char * - current manager name
   * */
  const char* getName() override;

  //================== END MgrBase related functions =====================

  /** @brief used to enter the main(thread) rendering loop
   * */
  void startRenderingLoop();

  /** @brief used to sending the main(thread) a message to exit it's
   *         render loop
   * */
  void shutdownRenderer();

  /** @brief used to provide the renderer with an API to communicate with
   *         containers that are responsible for holding
   *                              the loaded SDL_Surface's/SDL_Texture's
   *
   *  @param SDLContainers * - address of actual containers
   *
   *         NOTE: this step can NOT be done as part of the ::init()
   *               method, because when ::init() method is being invoked
   *               there is no way the SDLCointainers object is already
   *               constructed
   * */
  void setSDLContainers(SDLContainers *containers);

  /** @brief Every frame should start with this function call
   * */
  void clearScreen();

  /** All the drawing functions calls should be encapsulated
   *   between clearScreen() and finishFrame() functions
   * */

  /** @brief Every frame should end with this function call
   *
   *  @param const bool - developer option to override the the check
   *                      for locked/unlocked status of the renderer.
   *
   *         NOTE: this can be used for example when the developer is
   *               sure he want's to "steal" the draw call from default
   *               renderer target to other provided one
   *               (usually a SpriteBuffer).
   *
   *         WARNING: set the flag to true only if you are certain what
   *                  you are doing
   * */
  void finishFrame(const bool overrideRendererLockCheck = false);

  /** @brief transfer draw specific data from Widgets to renderer
   *
   *  @param const drawParams& - draw specific data for a single Widget
   * */
  void addDrawCmd(const DrawParams &drawParams) const;

  /* @brief used to store draw specific rendering commands populated by
   *                                              the main(update) thread
   *
   * @param const RendererCmd - render specific command
   * @param const uint8_t *   - inData buffer
   * @param const uint64_t    - bytes count to write
   * */
  void addRendererCmd(const RendererCmd rendererCmd,
                      const uint8_t *data = nullptr,
                      const uint64_t bytes = 0);

  /* @brief used to store draw specific data populated by
   *                                              the main(update) thread
   *
   * @param const uint8_t *   - inData buffer
   * @param const uint64_t    - bytes count to write
   *
   *       NOTE: this method is intended to be use separately of
   *             ::addRendererCmd() only for performance reasons.
   *
   *       Example: SpriteBuffer what has 30 stored widgets;
   *                const uint32_t SIZE = 30;
   *                ::addRendererData(
   *                             reinterpret_cast<const uint8_t *(&SIZE),
   *                             sizeof(SIZE));
   *
   *                After which ::addRendererCmd(
   *                             RendererCmd::UPDATE_RENDERER_TARGET,
   *                             reinterpret_cast<const uint8_t *(
   *                                              _storedWidgets.data()),
   *                             sizeof(DrawParams) * SIZE)
   * */
  void addRendererData(const uint8_t *data, const uint64_t bytes);

  /** @brief used to swap the command back buffers
   *                       (swap the update and rendering thread targets)
   *
   *         WARNING: do NOT invoke this method if you don't know what
   *                  you are doing
   * */
  void swapBackBuffers();

  /** @brief takes a snapshot of current renderer pixels
   *
   *  @param const char*               - file path
   *  @param const ScreenshotContainer - type of container [PNG, JPG, ...]
   *  @param const int32_t             - quality (applied only for JPG)
   *                                     range: [0, 100], worst(0) - best(100)
   *
   *  WARNING: the renderer operation that would be performed from this call
   *           is quite slow and should not be used in performance
   *           critical parts of the code
   * */
  void takeScreenshot(const char *file, const ScreenshotContainer container,
                      const int32_t quality);

  /** @brief used to monitor the number of widgets
   *                              currently being drawn by the renderer
   *
   *  @returns uint32_t - total widget count by the renderer
   * */
  uint32_t getTotalWidgetCount() const;

  /** @brief used to unlock renderer (for more information check
   *                ::unlockRenderer() in thirdparty/sdlutils/Renderer.h)
   *
   *  @return ErrorCode - error code
   * */
  ErrorCode unlockRenderer();

  /** @brief used to lock renderer (for more information check
   *                ::lockRenderer() in thirdparty/sdlutils/Renderer.h)
   *
   *  @return ErrorCode - error code
   * */
  ErrorCode lockRenderer();

  /** @brief used to limit the frame rate to a specific value.
   *         In order not to over burden the CPU, when the desired FPS
   *         is reached, the thread that executes the drawing is put
   *                                                             to sleep
   *
   *  @param const uint32_t - max frame cap
   * */
  void setMaxFrameRate(const uint32_t maxFrames) {
    _maxFrames = maxFrames;
  }

  /** @brief used to acquire the _maxFrames rate, which was set
   *
   *  @return uint32_t - max frames
   * */
  uint32_t getMaxFrameRate() const {
    return _maxFrames;
  }

  /** @brief used to acquire screen width
   *
   *  @return int32_t - screen width
   * */
  int32_t getMonitorWidth() const {
    return _config.monitorWindowConfig.width;
  }

  /** @brief used to acquire screen height
   *
   *  @return int32_t - screen height
   * */
  int32_t getMonitorHeight() const {
    return _config.monitorWindowConfig.height;
  }

  void moveGlobalX(const int32_t x);

  void moveGlobalY(const int32_t y);

  void resetAbsoluteGlobalMovement();

  Renderer* getRenderer() {
    return _renderer;
  }

private:
  // Hide renderer implementation under user defined renderer class.
  // On later stages renderer internal implementation could be switched
  // to OPEN_GL one
  Renderer *_renderer;

  // The window we'll be rendering to
  MonitorWindow _window;

  // Hold maximum frame rate cap
  uint32_t _maxFrames;

  DrawMgrConfig _config;
};

extern DrawMgr *gDrawMgr;

#endif /* MANAGER_UTILS_DRAWMGR_H_ */
