#pragma once

#include <functional>
#include <string>
#include <utility>

#include "src/action.h"

class Window {
 public:
  virtual ~Window() = default;

  virtual std::string Name() = 0;
  virtual void NotifySize(int rows, int cols) = 0;
  virtual void NotifyAction(Action action) = 0;
  virtual void NotifyChar(wchar_t key) = 0;
  virtual void Render(
      const std::function<void(int row, int col, wchar_t val)>& cb) = 0;
  virtual std::pair<int, int> GetCursorPos() = 0;
};
