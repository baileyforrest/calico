#pragma once

#include <string>

class Screen {
 public:
  Screen();
  ~Screen();

  void Clear();
  void RefreshSize();
  void SetChar(int row, int col, wchar_t val);
  void SetChars(int row, int col, const std::string& str);
  void SetCursorPos(int row, int col);
  void Refresh();
  void EnableReverse();
  void DisableReverse();
  void EnableBold();
  void DisableBold();

  int rows() const { return rows_; }
  int cols() const { return cols_; }

 private:
  int rows_ = 0;
  int cols_ = 0;
};
