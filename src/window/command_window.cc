#include "window/command_window.h"

CommandWindow::CommandWindow() {}

CommandWindow::~CommandWindow() = default;

std::string CommandWindow::Name() {
  return "[Command Window]";
}

void CommandWindow::NotifyChar(wchar_t key) {
  if (key != '\n') {
    return BufferWindow::NotifyChar(key);
  }

  // If we receive a newline in command window, while not on the last line,
  // copy it to the bottom of the command buffer and set the cursor to the end
  // of the buffer.
  if (cursor_pos() != buf().end()) {
    std::string cmd;
    for (auto it = cursor_pos().LastLineStart(false /* ignore_current_pos */);
         it != buf().end() && *it != '\n'; ++it) {
      cmd.push_back(*it);
    }

    for (auto c : cmd) {
      buf().insert(buf().end(), c);
    }

    cursor_pos() = buf().end();
  } else {
    // Don't send empty lines.
    size_t diff;
    cursor_pos().LastLineStart(false /* ignore_current_pos */, &diff);
    if (diff == 0) {
      return;
    }
  }

  has_command_ = true;
  BufferWindow::NotifyChar(key);
}

bool CommandWindow::HasNewCommand() {
  bool ret = has_command_;
  has_command_ = false;
  return ret;
}

std::string CommandWindow::LastCommand() {
  std::string ret;
  for (auto it = cursor_pos().LastLineStart(true /* ignore_current_pos */);
       *it != '\n' && it != buf().end(); ++it) {
    ret.push_back(*it);
  }

  return ret;
}
