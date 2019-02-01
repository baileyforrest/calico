#include "controller.h"  // NOLINT(build/include)

#include <libgen.h>

#include <cassert>
#include <string>
#include <utility>

#include "base/string_util.h"
#include "key.h"  // NOLINT(build/include)
#include "window/window.h"

using std::placeholders::_1;

// clang-format off
// static
const Controller::Command Controller::kDefaultCommands[] = {
  {"q", &Controller::HandleQuit},
};
// clang-format on

Controller::Controller() {
  tabs_.push_back(TabInfo());
  active_tab_ = tabs_.begin();
  screen_.AddObserver(this);

  for (const auto& command : kDefaultCommands) {
    command_to_action_.emplace(command.name,
                               std::bind(command.handler, this, _1));
  }
}

Controller::~Controller() {
  screen_.RemoveObserver(this);
}

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
  active_tab_->windows.insert(&windows_.back());
}

void Controller::Run() {
  while (!should_quit_) {
    if (mode_ != last_mode_ && mode_ != Mode::NORMAL) {
      command_error_.clear();
    }
    last_mode_ = mode_;

    Render();

    Screen::KeyState key = screen_.ReadKey();

    if (mode_ == Mode::INSERT && !key.is_key_code) {
      active_tab_->active_window->window->NotifyChar(key.code);
      continue;
    }

    if (mode_ == Mode::COMMAND && !key.is_key_code) {
      command_window_.NotifyChar(key.code);
      if (key.code == '\n') {
        if (command_window_.HasNewCommand()) {
          HandleCommand(command_window_.LastCommand());
        }
        mode_ = Mode::NORMAL;
      }
      continue;
    }

    Action action = key_config_.GetAction(key.code);
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
      default: {
        if (mode_ == Mode::COMMAND) {
          command_window_.NotifyAction(action);
        } else {
          active_tab_->active_window->window->NotifyAction(action);
        }
      }
    }
  }
}

void Controller::OnScreenSizeChanged() {
  for (TabInfo& tab : tabs_) {
    for (WindowInfo* window : tab.windows) {
      window->rows = screen_.rows() - 2;
      window->cols = screen_.cols();
    }
  }
  Render();
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

    bool is_primary = &*active_tab_ == &tab;
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
      if (!command_error_.empty()) {
        screen_.EnableReverse();
        screen_.SetChars(screen_.rows() - 1, 0, command_error_);
        screen_.DisableReverse();
      }
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

void Controller::HandleCommand(const std::string& command) {
  std::vector<std::string> split = StringSplit(command);
  if (split.empty()) {
    return;
  }

  auto it = command_to_action_.find(split[0]);
  if (it == command_to_action_.end()) {
    command_error_ = "Not an editor command: " + command;
    return;
  }

  it->second(split);
}

void Controller::HandleQuit(const std::vector<std::string>& command) {
  active_tab_->windows.erase(active_tab_->active_window);

  // Delete the tab if empty, then exit the program if no tabs are left.
  if (active_tab_->windows.empty()) {
    active_tab_ = tabs_.erase(active_tab_);
    if (active_tab_ == tabs_.end()) {
      should_quit_ = true;
      return;
    }
  }

  active_tab_->active_window = *active_tab_->windows.begin();
}
