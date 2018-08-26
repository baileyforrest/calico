#pragma once

#include <string>

#include "window/buffer_window.h"

class FileWindow : public BufferWindow {
 public:
  explicit FileWindow(const std::string& filename);
  ~FileWindow();

  // Window implementation:
  std::string Name() override;

 private:
  std::string filename_;
};
