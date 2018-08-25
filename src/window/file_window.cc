#include "window/file_window.h"

#include <ncurses.h>

FileWindow::FileWindow(const std::string& filename)
    : filename_(filename),
      buf_(Buffer::FromFile(filename)),
      screen_start_(buf_.begin()),
      cursor_pos_(buf_.begin()) {}

FileWindow::~FileWindow() = default;

void FileWindow::NotifySize(int rows, int cols) {
  rows_ = rows;
  cols_ = cols;
}

void FileWindow::NotifyKey(int key) {
  switch (key) {
    case KEY_LEFT: {
      if (cursor_pos_ == buf_.begin()) {
        return;
      }
      --cursor_pos_;
      // Can't go left if we are at beginning of line.
      if (*cursor_pos_ == '\n') {
        ++cursor_pos_;
        return;
      }
      ++current_col_;
      return;
    }
    case KEY_RIGHT: {
      if (cursor_pos_ == buf_.end()) {
        return;
      }
      ++cursor_pos_;
      // Can't go right if we're at end.
      if (*cursor_pos_ == '\n') {
        --cursor_pos_;
        return;
      }
      --current_col_;
      return;
    }
    case KEY_UP:
    case KEY_DOWN: {
      cursor_pos_ = key == KEY_UP ? cursor_pos_.LastLineStart(true /* ignore_current_pos */) : cursor_pos_.NextLineStart();
      for (int i = 0; i < current_col_; ++i) {
        ++cursor_pos_;
        if (*cursor_pos_ == '\n') {
          --cursor_pos_;
          break;
        }
      }
      return;
    }
    case KEY_BACKSPACE: {
      if (cursor_pos_ == buf_.begin()) {
        return;
      }
      --cursor_pos_;
      cursor_pos_ = buf_.erase(cursor_pos_);
      ++cursor_pos_;
    }
  }

  bool is_first_line = screen_start_ == cursor_pos_.LastLineStart(false /* ignore_current_pos */);

  cursor_pos_ = buf_.insert(cursor_pos_, key);
  ++cursor_pos_;

  if (is_first_line) {
    screen_start_ = cursor_pos_.LastLineStart(true /* ignore_current_pos */);
  }
}

void FileWindow::Render(
    const std::function<void(int row, int col, wchar_t val)>& cb) {
  int row = 0;
  int col = 0;

  for (Buffer::iterator pos = screen_start_; pos != buf_.end() && row < rows_;
       ++pos) {
    if (pos == cursor_pos_) {
      cursor_row_ = row;
      cursor_col_ = col;
    }
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

std::pair<int, int> FileWindow::GetCursorPos() {
  return {cursor_row_, cursor_col_};
}
