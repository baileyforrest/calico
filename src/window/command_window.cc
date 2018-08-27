#include "window/command_window.h"

CommandWindow::CommandWindow() {
  buf().insert(buf().end(), '\n');
  cursor_pos() = buf().begin();
}

CommandWindow::~CommandWindow() = default;

std::string CommandWindow::Name() {
  return "[Command Window]";
}

std::string CommandWindow::LastCommnd() {
  std::string ret;
  for (auto it = cursor_pos().LastLineStart(true /* ignore_current_pos */);
       *it != '\n' && it != buf().end(); ++it) {
    ret.push_back(*it);
  }

  return ret;
}
