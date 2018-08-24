#include "window/file_window.h"

FileWindow::FileWindow(const std::string& filename)
    : filename_(filename),
      buf_(Buffer::FromFile(filename)),
      line_start_(buf_.begin()) {}

FileWindow::~FileWindow() = default;

void FileWindow::NotifySize(int rows, int cols) {
  rows_ = rows;
  cols_ = cols;
}

bool FileWindow::NotifyKey(int key) {
  buf_.insert(buf_.end(), key);
  return true;
}

void FileWindow::Render(
    const std::function<void(int row, int col, wchar_t val)>& cb) {
  int row = 0;
  int col = 0;

  for (Buffer::iterator pos = line_start_; pos != buf_.end() && row < rows_;
       ++pos) {
    if (*pos == '\n') {
      col = 0;
      ++row;
      continue;
    }

    cb(row, col, *pos);
    ++col;
    if (col == cols_) {
      col = 0;
      ++row;
    }
  }
}
