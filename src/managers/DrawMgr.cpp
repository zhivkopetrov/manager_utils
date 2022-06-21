// Corresponding header
#include "manager_utils/managers/DrawMgr.h"

// System headers

// Other libraries headers
#include "sdl_utils/input/InputEvent.h"
#include "sdl_utils/drawing/Renderer.h"
#include "utils/debug/FunctionTracer.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

// Own components headers

DrawMgr *gDrawMgr = nullptr;

DrawMgr::DrawMgr(const DrawMgrConfig &cfg)
    : _renderer(nullptr), _maxFrames(0), _config(cfg) {
}

DrawMgr::~DrawMgr() noexcept {
  TRACE_ENTRY_EXIT;
}

ErrorCode DrawMgr::init() {
  TRACE_ENTRY_EXIT;

  _renderer = new Renderer;
  if (nullptr == _renderer) {
    LOGERR("Error, bad alloc for _renderer");
    return ErrorCode::FAILURE;
  }

  if (ErrorCode::SUCCESS != _window.init(_config.monitorWindowConfig)) {
    LOGERR("_window.init() failed");
    return ErrorCode::FAILURE;
  }
  _config.rendererConfig.window = _window.getNativeWindow();

  if (ErrorCode::SUCCESS != _renderer->init(_config.rendererConfig)) {
    LOGERR("_renderer.init() failed");
    return ErrorCode::FAILURE;
  }

  return ErrorCode::SUCCESS;
}

ErrorCode DrawMgr::recover() {
  return ErrorCode::SUCCESS;
}

void DrawMgr::deinit() {
  TRACE_ENTRY_EXIT;

  if (nullptr != _renderer) {
    delete _renderer;
    _renderer = nullptr;
  }

  _window.deinit();
}

const char* DrawMgr::getName() {
  return "DrawMgr";
}

void DrawMgr::shutdownRenderer() {
  _renderer->shutdownRenderer_UT();
}

void DrawMgr::startRenderingLoop() {
  _renderer->executeRenderCommands_RT();
}

void DrawMgr::process() {
}

void DrawMgr::handleEvent([[maybe_unused]]const InputEvent &e) {
}

void DrawMgr::setSDLContainers(SDLContainers *containers) {
  _renderer->setSDLContainers(containers);
}

void DrawMgr::clearScreen() {
  _renderer->clearScreen_UT();
}

void DrawMgr::finishFrame(const bool overrideRendererLockCheck) {
  _renderer->finishFrame_UT(overrideRendererLockCheck);
}

void DrawMgr::addDrawCmd(const DrawParams &drawParams) const {
  _renderer->addDrawCmd_UT(drawParams);
}

void DrawMgr::addRendererCmd(const RendererCmd rendererCmd, const uint8_t *data,
                             const uint64_t bytes) {
  _renderer->addRendererCmd_UT(rendererCmd, data, bytes);
}

void DrawMgr::addRendererData(const uint8_t *data, const uint64_t bytes) {
  _renderer->addRendererData_UT(data, bytes);
}

void DrawMgr::swapBackBuffers() {
  _renderer->swapBackBuffers_UT();
}

void DrawMgr::takeScreenshot(const char *file,
                             const ScreenshotContainer container,
                             const int32_t quality) {
  _renderer->takeScreenshot_UT(file, container, quality);
}

uint32_t DrawMgr::getTotalWidgetCount() const {
  return _renderer->getTotalWidgetCount_UT();
}

ErrorCode DrawMgr::unlockRenderer() {
  return _renderer->unlockRenderer_UT();
}

ErrorCode DrawMgr::lockRenderer() {
  return _renderer->lockRenderer_UT();
}

void DrawMgr::moveGlobalX(const int32_t x) {
  _renderer->moveGlobalX_UT(x);
}

void DrawMgr::moveGlobalY(const int32_t y) {
  _renderer->moveGlobalY_UT(y);
}

void DrawMgr::resetAbsoluteGlobalMovement() {
  _renderer->resetAbsoluteGlobalMovement_UT();
}

