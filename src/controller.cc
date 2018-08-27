#include "controller.h"  // NOLINT(build/include)

#include <curses.h>
#include <libgen.h>

#include <cassert>
#include <string>
#include <utility>

#include "key.h"  // NOLINT(build/include)
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
  while (true) {
    Render();
    // TODO(bcf): Refactor input to another class, handle non-blocking.
    wint_t wch = 0;
    int res = get_wch(&wch);
    bool is_key_code = false;

    switch (res) {
      case KEY_CODE_YES:
        is_key_code = true;
        break;
      case OK:
        break;
      default:
        continue;
    }

    // TODO(bcf): Handle ctrl + alt key combos which are considered escape.
    if (wch == static_cast<wchar_t>(Key::ESCAPE)) {
      is_key_code = true;
    }

    if (is_key_code && wch == KEY_RESIZE) {
      screen_.RefreshSize();
      for (TabInfo& tab : tabs_) {
        for (WindowInfo* window : tab.windows) {
          window->rows = screen_.rows() - 2;
          window->cols = screen_.cols();
        }
      }
      continue;
    }

    if (mode_ == Mode::INSERT && !is_key_code) {
      active_tab_->active_window->window->NotifyChar(wch);
      continue;
    }

    if (mode_ == Mode::COMMAND && !is_key_code) {
      command_window_.NotifyChar(wch);
      if (wch == '\n') {
        std::string last_command = command_window_.LastCommnd();
        // TODO(bcf): Refactor to command handler when this gets big enough
        if (last_command == "q") {
          return;
        }

        mode_ = Mode::NORMAL;
      }
      continue;
    }

    Action action = key_config_.GetAction(wch);
    if (action == Action::NONE) {
      continue;
    }

    switch (action) {
      case Action::START_COMMAND_MODE: {
        mode_ = Mode::COMMAND;
        continue;
      }
      case Action::START_INSERT_MODE: {
        mode_ = Mode::INSERT;
        continue;
      }
      case Action::ESCAPE: {
        switch (mode_) {
          case Mode::COMMAND:
          case Mode::INSERT:
            mode_ = Mode::NORMAL;
          default:
            break;
        }
        continue;
      }
      default:
        if (mode_ == Mode::COMMAND) {
          command_window_.NotifyAction(action);
        } else {
          active_tab_->active_window->window->NotifyAction(action);
        }
    }
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
  if (mode_ == Mode::COMMAND) {
    std::pair<int, int> cursor_pos = command_window_.GetCursorPos();
    assert(cursor_pos.first == 0);
    screen_.SetCursorPos(screen_.rows() - 1, cursor_pos.second + 1);
  } else {
    WindowInfo* active_window = active_tab_->active_window;
    std::pair<int, int> cursor_pos = active_window->window->GetCursorPos();
    assert(cursor_pos.first >= 0 && cursor_pos.first <= active_window->rows);
    assert(cursor_pos.second >= 0 && cursor_pos.second <= active_window->cols);
    screen_.SetCursorPos(cursor_pos.first + active_window->row_off,
                         cursor_pos.second + active_window->col_off);
  }

  screen_.Refresh();
}

void Controller::RenderTabBar() {
  int offset = 0;
  for (TabInfo& tab : tabs_) {
    size_t num_windows = tab.windows.size();
    assert(num_windows > 0);

    std::string active_name = tab.active_window->window->Name();
    std::string tab_name = " " + std::to_string(num_windows) + " " +
                           basename(&active_name[0]) + " ";

    // We're done if tab won't fit on screen.
    if (offset + static_cast<int>(tab_name.size()) >= screen_.rows()) {
      break;
    }

    bool is_primary = active_tab_ == &tab;
    if (!is_primary) {
      screen_.EnableReverse();
    }
    screen_.SetChars(0 /* row */, offset, tab_name);
    if (!is_primary) {
      screen_.DisableReverse();
    }
    offset += tab_name.size();
  }

  // Fill rest of bar with reversed blanks.
  screen_.EnableReverse();
  while (offset < screen_.cols()) {
    screen_.SetChar(0 /* row */, offset++, ' ');
  }
  screen_.DisableReverse();
}

void Controller::RenderStatus() {
  switch (mode_) {
    case Mode::NONE: {
      assert(false);
      return;
    }
    case Mode::NORMAL: {
      return;
    }
    case Mode::COMMAND: {
      screen_.SetChars(screen_.rows() - 1, 0, ":");
      command_window_.NotifySize(1 /* rows */, screen_.cols() - 1);
      auto render_cb = [this](int row, int col, wchar_t value) {
        assert(row == 0);
        assert(col >= 0 && col < screen_.cols());
        screen_.SetChar(screen_.rows() - 1, col + 1, value);
      };
      command_window_.Render(render_cb);
      return;
    }
    case Mode::INSERT: {
      screen_.EnableBold();
      screen_.SetChars(screen_.rows() - 1, 0, "-- INSERT --");
      screen_.DisableBold();
      return;
    }
  }
}
