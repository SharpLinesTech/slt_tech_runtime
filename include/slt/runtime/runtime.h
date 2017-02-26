#ifndef SLT_RUNTIME_H
#define SLT_RUNTIME_H

#include <stdexcept>

#include "slt/input/input_manager.h"
#include "slt/render/render_target.h"
#include "slt/render/resource_manager.h"
#include "slt/runtime/details/backend.h"
#include "slt/settings/settings.h"
#include "slt/string/string_view.h"

namespace slt {

// As a matter of convention, all load operations performed by the SLT
// engine will be relative to this directory.
extern slt::Setting<std::string> assets_dir;

struct RuntimeInitError : public std::runtime_error {
  RuntimeInitError(std::string error);
};

class Runtime : public runtime_::Implementation {
 public:
  Runtime(render::MainRenderTargetConfig const& main_rdt_config,
          StringView window_name);
  ~Runtime();

  bool keepRunning() const;

  // Syncs with the OS and runs various housekeeping tasks.
  void update(float dt);
  void finishFrame();

  render::ResourceManager resources;
  input::InputManager inputs;
  static Runtime* instance;
};
}
#endif