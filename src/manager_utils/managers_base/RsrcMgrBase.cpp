// Corresponding header
#include "manager_utils/managers_base/RsrcMgrBase.h"

// C system headers

// C++ system headers
#include <cstdlib>

// Other libraries headers

// Own components headers
#include "sdl_utils/input/InputEvent.h"
#include "utils/debug/FunctionTracer.hpp"
#include "utils/Log.h"

RsrcMgrBase* gRsrcMgrBase = nullptr;

RsrcMgrBase::RsrcMgrBase(Renderer* renderer, const std::string& projectName,
                         const bool isMultithreadResAllowed)
    :

      SDLContainers(renderer, projectName, isMultithreadResAllowed) {}

RsrcMgrBase::~RsrcMgrBase() { TRACE_ENTRY_EXIT; }

int32_t RsrcMgrBase::init() {
  TRACE_ENTRY_EXIT;

  if (EXIT_SUCCESS != SDLContainers::init()) {
    LOGERR("Error in SDLContainers::init() -> Terminating ...");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int32_t RsrcMgrBase::recover() { return EXIT_SUCCESS; }

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

