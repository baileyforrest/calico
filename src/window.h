#pragma once

#include <functional>

class Window {
 public:
  virtual void NotifySize(int rows, int cols) = 0;

  // Returns true if rerender is needed.
  virtual bool NotifyKey(int key) = 0;
  virtual void Render(
      const std::function<void(int row, int col, wchar_t val)>& cb) = 0;

  virtual ~Window() = default;
};
