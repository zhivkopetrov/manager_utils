// Corresponding header
#include "manager_utils/managers_base/RsrcMgrBase.h"

// C system headers

// C++ system headers

// Other libraries headers

// Own components headers
#include "sdl_utils/input/InputEvent.h"
#include "utils/debug/FunctionTracer.hpp"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

RsrcMgrBase* gRsrcMgrBase = nullptr;

RsrcMgrBase::RsrcMgrBase(const SDLContainersConfig &cfg) : SDLContainers(cfg) {}

RsrcMgrBase::~RsrcMgrBase() { TRACE_ENTRY_EXIT; }

int32_t RsrcMgrBase::init() {
  TRACE_ENTRY_EXIT;

  if (SUCCESS != SDLContainers::init()) {
    LOGERR("Error in SDLContainers::init() -> Terminating ...");
    return FAILURE;
  }

  return SUCCESS;
}

int32_t RsrcMgrBase::recover() { return SUCCESS; }

void RsrcMgrBase::deinit() { SDLContainers::deinit(); }

const char* RsrcMgrBase::getName() { return "RsrcMgrBase"; }

void RsrcMgrBase::process() {}

void RsrcMgrBase::handleEvent([[maybe_unused]]const InputEvent& e) {
}

void RsrcMgrBase::onLoadTextureMultipleCompleted(
    [[maybe_unused]]const int32_t batchId) {
}

uint64_t RsrcMgrBase::getGPUMemoryUsage() const {
  return ResourceContainer::getGPUMemoryUsage() +
         TextContainer::getGPUMemoryUsage() +
         SpriteBufferContainer::getGPUMemoryUsage();
}

