#pragma once

#include <functional>

class Screen {
 public:
  Screen();
  ~Screen();

  void Clear();
  void SetChar(int row, int col, wchar_t val);
  void SetCursorPos(int row, int col);
  void Refresh();

  int rows() const { return rows_; }
  int cols() const { return cols_; }
  const std::function<void(int row, int col, wchar_t val)>& render_cb() {
    return render_cb_;
  }

 private:
  const std::function<void(int row, int col, wchar_t val)> render_cb_;
  int rows_ = 0;
  int cols_ = 0;
};
