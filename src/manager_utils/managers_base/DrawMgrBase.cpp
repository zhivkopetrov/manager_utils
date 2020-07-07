// Corresponding header
#include "manager_utils/managers_base/DrawMgrBase.h"

// C system headers

// C++ system headers
#include <cstdlib>

// Other libraries headers

// Own components headers
#include "manager_utils/managers_base/config/DrawMgrBaseConfig.hpp"
#include "sdl_utils/input/InputEvent.h"
#include "sdl_utils/drawing/MonitorWindow.h"
#include "sdl_utils/drawing/Renderer.h"
#include "utils/debug/FunctionTracer.hpp"
#include "utils/Log.h"

DrawMgrBase* gDrawMgrBase = nullptr;

// SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS
#define FULL_SCREEN_MODE 1 | 16

DrawMgrBase::DrawMgrBase(const DrawMgrBaseConfig &cfg)
    : _renderer(cfg.renderer),
      _window(cfg.window),
      _displayMode(cfg.displayMode),
      _maxFrames(0),
      _SCREEN_WIDTH(cfg.monitorWidth),
      _SCREEN_HEIGHT(cfg.monitorHeight) {
}

DrawMgrBase::~DrawMgrBase() { TRACE_ENTRY_EXIT; }

int32_t DrawMgrBase::init() {
  TRACE_ENTRY_EXIT;

  return EXIT_SUCCESS;
}

int32_t DrawMgrBase::recover() { return EXIT_SUCCESS; }

void DrawMgrBase::deinit() { TRACE_ENTRY_EXIT; }

const char* DrawMgrBase::getName() { return "DrawMgrBase"; }

void DrawMgrBase::process() {}

void DrawMgrBase::handleEvent([[maybe_unused]]const InputEvent& e) {
}

void DrawMgrBase::setSDLContainers(SDLContainers* containers) {
  _renderer->setSDLContainers(containers);
}

void DrawMgrBase::clearScreen() { _renderer->clearScreen_UT(); }

void DrawMgrBase::finishFrame(const bool overrideRendererLockCheck) {
  _renderer->finishFrame_UT(overrideRendererLockCheck);
}

void DrawMgrBase::addDrawCmd(DrawParams* drawParams) {
  _renderer->addDrawCmd_UT(drawParams);
}

void DrawMgrBase::addRendererCmd(const RendererCmd rendererCmd,
                                 const uint8_t* data, const uint64_t bytes) {
  _renderer->addRendererCmd_UT(rendererCmd, data, bytes);
}

void DrawMgrBase::addRendererData(const uint8_t* data, const uint64_t bytes) {
  _renderer->addRendererData_UT(data, bytes);
}

void DrawMgrBase::shutdownRenderer() { _renderer->shutdownRenderer_UT(); }

void DrawMgrBase::swapBackBuffers() { _renderer->swapBackBuffers_UT(); }

bool DrawMgrBase::isFullScreenModeActive() const {
  return _displayMode == (FULL_SCREEN_MODE);
}

uint64_t DrawMgrBase::getTotalWidgetCount() const {
  return _renderer->getTotalWidgetCount_UT();
}

int32_t DrawMgrBase::unlockRenderer() { return _renderer->unlockRenderer_UT(); }

int32_t DrawMgrBase::lockRenderer() { return _renderer->lockRenderer_UT(); }

void DrawMgrBase::moveGlobalX(const int32_t x) { _renderer->moveGlobalX_UT(x); }

void DrawMgrBase::moveGlobalY(const int32_t y) { _renderer->moveGlobalY_UT(y); }

void DrawMgrBase::resetAbsoluteGlobalMovement() {
  _renderer->resetAbsoluteGlobalMovement_UT();
}

