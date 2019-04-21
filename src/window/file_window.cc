#include "src/window/file_window.h"

FileWindow::FileWindow(const std::string& filename) : filename_(filename) {
  buf() = Buffer::FromFile(filename);
  cursor_pos() = buf().begin();
}

FileWindow::~FileWindow() = default;

std::string FileWindow::Name() {
  return filename_;
}
