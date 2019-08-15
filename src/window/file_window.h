#pragma once

#include <string>

#include "src/window/buffer_window.h"

class FileWindow : public BufferWindow {
 public:
  explicit FileWindow(const std::string& filename);
  ~FileWindow();

  // Window implementation:
  std::string Name() override;
  bool NotifyCommand(absl::Span<std::string> command,
      std::string* error_string) override;

 private:
  std::string filename_;
};
