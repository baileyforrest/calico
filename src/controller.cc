#include "controller.h"  // NOLINT(build/include)

#include <curses.h>
#include <libgen.h>

#include <cassert>
#include <string>
#include <utility>

#include "window/window.h"

Controller::Controller() {
  tabs_.push_back(TabInfo());
  active_tab_ = &tabs_.front();
}

Controller::~Controller() = default;

void Controller::AddWindow(std::unique_ptr<Window> window) {
  // We only support one window per tab right now.
  assert(active_tab_->windows.empty());

  WindowInfo info(std::move(window));
  info.row_off = 1;  // One row for tab bar
  info.col_off = 0;

  // One row for tab bar, another for command propmt
  info.rows = screen_.rows() - 2;
  info.cols = screen_.cols();

  windows_.push_back(std::move(info));
  active_tab_->active_window = &windows_.back();
  active_tab_->windows.push_back(&windows_.back());
}

void Controller::Run() {
  Render();
  while (true) {
    // TODO(bcf): Refactor input to another class, handle non-blocking.
    int ch = getch();
    // TODO(bcf): Handle better exit case.
    if (ch == 'z') {
      break;
    }

    if (ch == KEY_RESIZE) {
      screen_.RefreshSize();
      for (TabInfo& tab : tabs_) {
        for (WindowInfo* window : tab.windows) {
          window->rows = screen_.rows() - 2;
          window->cols = screen_.cols();
        }
      }
    } else {
      active_tab_->active_window->window->NotifyKey(ch);
    }

    Render();
  }
}

void Controller::Render() {
  screen_.Clear();

  // Render each window in the current tab.
  for (WindowInfo* window : active_tab_->windows) {
    window->window->NotifySize(window->rows, window->cols);
    auto render_cb = [this, window](int row, int col, wchar_t value) {
      assert(row >= 0 && row < window->rows);
      assert(col >= 0 && col < window->cols);
      screen_.SetChar(row + window->row_off, col + window->col_off, value);
    };
    window->window->Render(render_cb);
  }

  RenderTabBar();
  RenderStatus();

  // Render the cursor.
  WindowInfo* active_window = active_tab_->active_window;
  std::pair<int, int> cursor_pos = active_window->window->GetCursorPos();
  assert(cursor_pos.first >= 0 && cursor_pos.first <= active_window->rows);
  assert(cursor_pos.second >= 0 && cursor_pos.second <= active_window->cols);
  screen_.SetCursorPos(cursor_pos.first + active_window->row_off,
                       cursor_pos.second + active_window->col_off);

  screen_.Refresh();
}

void Controller::RenderTabBar() {
  int offset = 0;
  for (TabInfo& tab : tabs_) {
    size_t num_windows = tab.windows.size();
    assert(num_windows > 0);

    std::string active_name = tab.active_window->window->Name();
    std::string tab_name =
        std::to_string(num_windows) + " " + basename(&active_name[0]);

    // We're done if tab won't fit on screen.
    if (offset + static_cast<int>(tab_name.size()) + 2 >= screen_.rows()) {
      break;
    }

    bool is_primary = active_tab_ == &tab;
    if (!is_primary) {
      screen_.EnableReverse();
    }

    screen_.SetChar(0 /* row */, offset++, ' ');
    for (char c : tab_name) {
      screen_.SetChar(0 /* row */, offset++, c);
    }
    screen_.SetChar(0 /* row */, offset++, ' ');
    if (!is_primary) {
      screen_.DisableReverse();
    }
  }

  // Fill rest of bar with reversed blanks.
  screen_.EnableReverse();
  while (offset < screen_.cols()) {
    screen_.SetChar(0 /* row */, offset++, ' ');
  }
  screen_.DisableReverse();
}

void Controller::RenderStatus() {}
