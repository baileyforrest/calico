#include "src/window/buffer_window.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <limits>

BufferWindow::BufferWindow() : cursor_pos_(buf_.begin()) {}

BufferWindow::~BufferWindow() = default;

void BufferWindow::NotifySize(int rows, int cols) {
  rows_ = rows;
  cols_ = cols;
}

void BufferWindow::NotifyAction(Action action) {
  switch (action) {
    case Action::LEFT: {
      if (cursor_pos_ == buf_.begin()) {
        return;
      }
      --cursor_pos_;
      // Can't go left if we are at beginning of line.
      if (*cursor_pos_ == '\n') {
        ++cursor_pos_;
        return;
      }
      size_t diff;
      cursor_pos_.LastLineStart(false /* ignore_current_pos */, &diff);
      cursor_col_ = diff;
      return;
    }
    case Action::RIGHT: {
      if (cursor_pos_ == buf_.end() || *cursor_pos_ == '\n') {
        return;
      }
      ++cursor_pos_;
      // Can't go right if we're at end.
      if (cursor_pos_ != buf_.end() && *cursor_pos_ == '\n') {
        return;
      }
      ++cursor_col_;
      return;
    }
    case Action::UP:
    case Action::DOWN: {
      if (action == Action::UP) {
        auto cur_start =
            cursor_pos_.LastLineStart(false /* ignore_current_pos */);
        size_t diff;
        cursor_pos_ =
            cur_start.LastLineStart(true /* ignore_current_pos */, &diff);
        int rows_used = (cols_ - 1 + diff) / cols_;

        cursor_row_ = std::max(0, cursor_row_ - rows_used);
      } else {  // Action::DOWN
        size_t start_diff;
        cursor_pos_.LastLineStart(false /* ignore_current_pos */, &start_diff);

        size_t end_diff;
        cursor_pos_ = cursor_pos_.NextLineStart(&end_diff);

        int rows_used = (cols_ - 1 + start_diff + end_diff) / cols_;
        cursor_row_ = std::min(cursor_row_ + rows_used, rows_ - 1);
      }

      // Move cursor to the desired column
      for (int i = 0; i < cursor_col_; ++i) {
        if (cursor_pos_ == buf_.end() || *cursor_pos_ == '\n') {
          break;
        }
        ++cursor_pos_;
      }
      return;
    }
    case Action::BACKSPACE: {
      if (cursor_pos_ == buf_.begin()) {
        return;
      }
      --cursor_pos_;
      cursor_pos_ = buf_.erase(cursor_pos_);
      return;
    }

    case Action::HOME: {
      cursor_col_ = 0;
      cursor_pos_ = cursor_pos_.LastLineStart(false /* ignore_current_pos */);
      return;
    }
    case Action::END: {
      cursor_col_ = std::numeric_limits<decltype(cursor_col_)>::max();
      size_t diff;
      cursor_pos_ = cursor_pos_.NextLineStart(&diff);
      if (diff > 0) {
        --cursor_pos_;
      }
      return;
    }
    default:
      break;
  }
}

void BufferWindow::NotifyChar(wchar_t key) {
  // Default case, just insert the key.
  cursor_pos_ = buf_.insert(cursor_pos_, key);
  ++cursor_pos_;

  if (key == '\n' && cursor_row_ < rows_ - 1) {
    ++cursor_row_;
  }

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

  cursor_col_ = line_start_diff;
}

void BufferWindow::Render(
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
  row = cursor_row_ - 1;
  size_t diff;
  for (auto line_start =
           cur_line_start.LastLineStart(true /* ignore_current_pos*/, &diff);
       row >= 0; line_start = line_start.LastLineStart(
                     true /* ignore_current_pos*/, &diff)) {
    // Done if there are no more lines left.
    if (diff == 0) {
      break;
    }
    int line_length = diff;

    int rows_used = std::max(1, (line_length + cols_ - 1) / cols_);
    int line_row = row - (rows_used - 1);
    int col = 0;

    for (auto it = line_start;; ++it) {
      assert(it != buf_.end());
      // Done with the row.
      if (*it == '\n') {
        break;
      }

      if (line_row >= 0) {
        assert(line_row <= row);
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

std::pair<int, int> BufferWindow::GetCursorPos() {
  size_t diff;
  cursor_pos_.LastLineStart(false /* ignore_current_pos */, &diff);
  int cursor_col = diff % cols_;
  int cursor_row = cursor_row_ + diff / cols_;
  assert(cursor_row < rows_);
  return {cursor_row, cursor_col};
}
