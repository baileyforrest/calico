#pragma once

#include <functional>
#include <string>
#include <utility>

#include "absl/types/optional.h"
#include "absl/types/span.h"
#include "src/action.h"
#include "src/command-status.h"

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

  // Returns command status if we handled the command.
  virtual absl::optional<CommandStatus> NotifyCommand(
      absl::Span<std::string> command) {
    return absl::nullopt;
  }
};
