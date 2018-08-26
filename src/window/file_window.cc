#include "window/file_window.h"

#include <ncurses.h>

#include <algorithm>
#include <cassert>

FileWindow::FileWindow(const std::string& filename)
    : filename_(filename),
      buf_(Buffer::FromFile(filename)),
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
      ++cursor_col_;
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
      --cursor_col_;
      return;
    }
    case KEY_UP:
    case KEY_DOWN: {
      size_t diff;
      if (key == KEY_UP) {
        cursor_pos_ =
            cursor_pos_.LastLineStart(true /* ignore_current_pos */, &diff);
        if (diff > 0 && cursor_row_ >= 0) {
          --cursor_row_;
        }
      } else {
        cursor_pos_.NextLineStart(&diff);
        if (diff > 0 && cursor_row_ < rows_) {
          ++cursor_row_;
        }
      }
      for (int i = 0; i < cursor_col_; ++i) {
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
      return;
    }
  }

  cursor_pos_ = buf_.insert(cursor_pos_, key);
  ++cursor_pos_;

  // Reposition cursor if current position won't fit on screen.
  size_t line_start_diff;
  cursor_pos_.LastLineStart(false /* ignore_current_pos */, &line_start_diff);

  size_t line_end_diff;
  cursor_pos_.NextLineStart(&line_end_diff);

  size_t line_length = line_start_diff + line_end_diff;
  int rows_used = (line_length + cols_ - 1) / cols_;
  if (cursor_row_ + rows_used >= rows_) {
    cursor_row_ = std::max(0, rows_ - 1 - rows_used);
  }
}

void FileWindow::Render(
    const std::function<void(int row, int col, wchar_t val)>& cb) {
  auto cur_line_start =
      cursor_pos_.LastLineStart(false /* ignore_current_pos */);

  // Render cursor line and after.
  int row = cursor_row_;
  int col = 0;
  for (auto it = cur_line_start; it != buf_.end() && row < rows_; ++it) {
    if (*it == '\n') {
      col = 0;
      ++row;
      continue;
    }

    cb(row, col, *it);
    ++col;
    if (col == cols_) {
      col = 0;
      ++row;
    }
  }

  // Render lines before cursor.
  row = cursor_row_;
  size_t line_length;
  for (auto line_start = cur_line_start.LastLineStart(
           true /* ignore_current_pos*/, &line_length);
       row >= 0; line_start = line_start.LastLineStart(
                     true /* ignore_current_pos*/, &line_length)) {
    // Done if there are no more lines left.
    if (line_length == 0) {
      break;
    }

    int rows_used = (line_length + cols_ - 1) / cols_;
    int line_row = row - rows_used;

    for (auto it = line_start;; ++it) {
      assert(it != buf_.end());
      // Done with the row.
      if (*it == '\n') {
        break;
      }

      if (line_row >= 0) {
        cb(line_row, col, *it);
      }
      ++col;
      if (col == cols_) {
        col = 0;
        ++line_row;
      }
    }

    row -= rows_used;
  }
}

std::pair<int, int> FileWindow::GetCursorPos() {
  size_t diff;
  cursor_pos_.LastLineStart(false /* ignore_current_pos */, &diff);
  int cursor_col = diff % cols_;
  int cursor_row = cursor_row_ + diff / cols_;
  assert(cursor_row < rows_);
  return {cursor_row, cursor_col};
}
