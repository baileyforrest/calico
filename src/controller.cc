#include "controller.h"  // NOLINT(build/include)

#include <curses.h>

#include <cassert>
#include <utility>

Controller::Controller() = default;
Controller::~Controller() = default;

void Controller::AddWindow(std::unique_ptr<Window> window) {
  windows_.insert(std::move(window));
}

void Controller::Run() {
  // We only support 1 window right now.
  assert(windows_.size() == 1);

  for (const auto& window : windows_) {
    active_window_ = window.get();
    window->NotifySize(screen_.rows(), screen_.cols());
    window->Render(screen_.render_cb());
  }
  screen_.Refresh();

  while (true) {
    // TODO(bcf): Refactor input to another class, handle non-blocking.
    int ch = getch();
    if (ch == KEY_RESIZE) {
      // TODO(bcf): Handle window resize.
      continue;
    }

    if (active_window_->NotifyKey(ch)) {
      active_window_->Render(screen_.render_cb());
      screen_.Refresh();
    }

    // TODO(bcf): Handle better exit case.
    if (ch == 'z') {
      break;
    }
  }
}
