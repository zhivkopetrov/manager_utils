// Corresponding header
#include "manager_utils/managers/DrawMgr.h"

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "sdl_utils/input/InputEvent.h"
#include "sdl_utils/drawing/MonitorWindow.h"
#include "sdl_utils/drawing/Renderer.h"
#include "utils/debug/FunctionTracer.hpp"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

DrawMgr *gDrawMgr = nullptr;

DrawMgr::DrawMgr(const DrawMgrConfig &cfg)
    : _renderer(nullptr), _window(nullptr), _maxFrames(0), _config(cfg) {
}

DrawMgr::~DrawMgr() {
  TRACE_ENTRY_EXIT;
}

int32_t DrawMgr::init() {
  TRACE_ENTRY_EXIT;

  _window = new MonitorWindow(_config.monitorWidth, _config.monitorHeight);
  if (nullptr == _window) {
    LOGERR("Error, bad alloc for _window");
    return FAILURE;
  }

  _renderer = new Renderer;
  if (nullptr == _renderer) {
    LOGERR("Error, bad alloc for _renderer");
    return FAILURE;
  }

  if (SUCCESS !=
      _window->init(_config.windowDisplayMode, _config.windowBorderMode)) {
    LOGERR("_window.init() failed");
    return FAILURE;
  }
  _config.rendererConfig.window = _window->getWindow();

  if (SUCCESS != _renderer->init(_config.rendererConfig)) {
    LOGERR("_renderer.init() failed");
    return FAILURE;
  }

  return SUCCESS;
}

int32_t DrawMgr::recover() {
  return SUCCESS;
}

void DrawMgr::deinit() {
  TRACE_ENTRY_EXIT;

  if (nullptr != _renderer) {
    delete _renderer;
    _renderer = nullptr;
  }

  if (nullptr != _window) {
    delete _window;
    _window = nullptr;
  }
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

void DrawMgr::addDrawCmd(DrawParams *drawParams) {
  _renderer->addDrawCmd_UT(drawParams);
}

void DrawMgr::addRendererCmd(const RendererCmd rendererCmd,
                                 const uint8_t *data, const uint64_t bytes) {
  _renderer->addRendererCmd_UT(rendererCmd, data, bytes);
}

void DrawMgr::addRendererData(const uint8_t *data, const uint64_t bytes) {
  _renderer->addRendererData_UT(data, bytes);
}

void DrawMgr::swapBackBuffers() {
  _renderer->swapBackBuffers_UT();
}

uint32_t DrawMgr::getTotalWidgetCount() const {
  return _renderer->getTotalWidgetCount_UT();
}

int32_t DrawMgr::unlockRenderer() {
  return _renderer->unlockRenderer_UT();
}

int32_t DrawMgr::lockRenderer() {
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

