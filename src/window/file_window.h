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
  void NotifyKey(int key) override;
  void Render(
      const std::function<void(int row, int col, wchar_t val)>& cb) override;
  std::pair<int, int> GetCursorPos() override;

 private:
  const std::string filename_;

  Buffer buf_;
  int rows_ = 0;
  int cols_ = 0;

  // Line where the screen starts.
  Buffer::iterator screen_start_;

  // Current line we are on.
  Buffer::iterator cursor_pos_;
  int current_col_;

  // Real cursor coordinates based on cursor_pos_
  int cursor_row_ = 0;
  int cursor_col_ = 0;

  DISALLOW_MOVE_COPY_AND_ASSIGN(FileWindow);
};
