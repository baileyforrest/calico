#pragma once

#include <string>

#include "base/buffer.h"
#include "window.h"  // NOLINT(build/include)

class FileWindow : public Window {
 public:
  explicit FileWindow(const std::string& filename);
  ~FileWindow() override;

  // Window implementation:
  void NotifySize(int rows, int cols) override;
  bool NotifyKey(int key) override;
  void Render(
      const std::function<void(int row, int col, wchar_t val)>& cb) override;

 private:
  const std::string filename_;

  Buffer buf_;
  int rows_ = 0;
  int cols_ = 0;

  Buffer::iterator line_start_;

  DISALLOW_MOVE_COPY_AND_ASSIGN(FileWindow);
};
