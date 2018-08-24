#pragma once

#include <memory>
#include <set>

#include "base/macros.h"
#include "screen.h"  // NOLINT(build/include)
#include "window/file_window.h"

class Controller {
 public:
  Controller();
  ~Controller();

  void AddWindow(std::unique_ptr<Window> window);

  void Run();

 private:
  Screen screen_;
  std::set<std::unique_ptr<Window>> windows_;
  Window* active_window_ = nullptr;
};
