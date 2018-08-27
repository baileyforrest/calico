#pragma once

#include <string>

#include "window/buffer_window.h"

class CommandWindow : public BufferWindow {
 public:
  CommandWindow();
  ~CommandWindow();

  // Window implementation:
  std::string Name() override;
};
