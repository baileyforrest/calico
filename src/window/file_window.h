#pragma once

#include <string>
#include <utility>

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

  // Current line we are on.
  Buffer::iterator cursor_pos_;

  int cursor_row_ = 0;

  // Note: Might not be real row of cursor if line is not long enough.
  int cursor_col_ = 0;

  DISALLOW_MOVE_COPY_AND_ASSIGN(FileWindow);
};
