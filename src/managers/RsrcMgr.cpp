// Corresponding header
#include "manager_utils/managers/RsrcMgr.h"

// System headers

// Other libraries headers
#include "utils/input/InputEvent.h"
#include "utils/debug/FunctionTracer.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

// Own components headers

RsrcMgr* gRsrcMgr = nullptr;

RsrcMgr::RsrcMgr(const SDLContainersConfig &cfg) : SDLContainers(cfg) {

}

RsrcMgr::~RsrcMgr() noexcept {
  TRACE_ENTRY_EXIT;
}

ErrorCode RsrcMgr::init() {
  TRACE_ENTRY_EXIT;

  if (ErrorCode::SUCCESS != SDLContainers::init()) {
    LOGERR("Error in SDLContainers::init() -> Terminating ...");
    return ErrorCode::FAILURE;
  }

  return ErrorCode::SUCCESS;
}

ErrorCode RsrcMgr::recover() {
  return ErrorCode::SUCCESS;
}

void RsrcMgr::deinit() {
  SDLContainers::deinit();
}

const char* RsrcMgr::getName() {
  return "RsrcMgr";
}

void RsrcMgr::process() {

}

void RsrcMgr::handleEvent([[maybe_unused]]const InputEvent& e) {
}

void RsrcMgr::onLoadTextureMultipleCompleted(
    [[maybe_unused]]const int32_t batchId) {
}

uint64_t RsrcMgr::getGPUMemoryUsage() const {
  return ResourceContainer::getGPUMemoryUsage() +
         TextContainer::getGPUMemoryUsage() +
         FboContainer::getGPUMemoryUsage();
}

