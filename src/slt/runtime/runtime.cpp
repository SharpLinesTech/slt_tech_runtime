#include "slt/runtime/runtime.h"
#include "slt/log/log.h"

namespace slt {
// As a matter of convention, all load operations performed by the SLT
// engine will be relative to this directory.
slt::Setting<std::string> assets_dir("", "assets_dir",
                                     "Location of built assets");

RuntimeInitError::RuntimeInitError(std::string error)
    : std::runtime_error(error) {}

Runtime* Runtime::instance = nullptr;
}
