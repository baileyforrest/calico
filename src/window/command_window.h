#pragma once

#include <string>

#include "window/buffer_window.h"

class CommandWindow : public BufferWindow {
 public:
  CommandWindow();
  ~CommandWindow();

  // Window implementation:
  std::string Name() override;
  void NotifyChar(wchar_t key) override;

  // Returns true if a new command was received since the last time this
  // function was called. Should be checked after every call to |NotifyChar|.
  bool HasNewCommand();
  std::string LastCommand();

 private:
  bool has_command_ = false;
};
