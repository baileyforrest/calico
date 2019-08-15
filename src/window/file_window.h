#pragma once

#include <string>

#include "bcf/err.h"
#include "src/window/buffer_window.h"

class FileWindow : public BufferWindow {
 public:
  explicit FileWindow(const std::string& filename);
  ~FileWindow();

  // Window implementation:
  std::string Name() override;
  absl::optional<CommandStatus> NotifyCommand(
      absl::Span<std::string> command) override;

 private:
  CommandStatus HandleWrite(absl::Span<std::string> args);

  std::string filename_;
};
