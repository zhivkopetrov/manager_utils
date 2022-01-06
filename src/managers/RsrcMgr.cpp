// Corresponding header
#include "manager_utils/managers/RsrcMgr.h"

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "sdl_utils/input/InputEvent.h"
#include "utils/debug/FunctionTracer.hpp"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

RsrcMgr* gRsrcMgr = nullptr;

RsrcMgr::RsrcMgr(const SDLContainersConfig &cfg) : SDLContainers(cfg) {}

RsrcMgr::~RsrcMgr() { TRACE_ENTRY_EXIT; }

int32_t RsrcMgr::init() {
  TRACE_ENTRY_EXIT;

  if (SUCCESS != SDLContainers::init()) {
    LOGERR("Error in SDLContainers::init() -> Terminating ...");
    return FAILURE;
  }

  return SUCCESS;
}

int32_t RsrcMgr::recover() { return SUCCESS; }

void RsrcMgr::deinit() { SDLContainers::deinit(); }

const char* RsrcMgr::getName() { return "RsrcMgr"; }

void RsrcMgr::process() {}

void RsrcMgr::handleEvent([[maybe_unused]]const InputEvent& e) {
}

void RsrcMgr::onLoadTextureMultipleCompleted(
    [[maybe_unused]]const int32_t batchId) {
}

uint64_t RsrcMgr::getGPUMemoryUsage() const {
  return ResourceContainer::getGPUMemoryUsage() +
         TextContainer::getGPUMemoryUsage() +
         SpriteBufferContainer::getGPUMemoryUsage();
}

