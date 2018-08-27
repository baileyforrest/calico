#pragma once

#include <list>
#include <memory>
#include <utility>

#include "action.h"  // NOLINT(build/include)
#include "base/macros.h"
#include "key_config.h"  // NOLINT(build/include)
#include "screen.h"      // NOLINT(build/include)
#include "window/command_window.h"

class Controller {
 public:
  Controller();
  ~Controller();

  void AddWindow(std::unique_ptr<Window> window);

  void Run();

 private:
  enum class Mode {
    NONE,
    NORMAL,
    COMMAND,
    INSERT,
  };

  struct WindowInfo {
    explicit WindowInfo(std::unique_ptr<Window> in_window)
        : window(std::move(in_window)) {}
    WindowInfo(WindowInfo&&) = default;

    std::unique_ptr<Window> window;
    int row_off = 0;
    int col_off = 0;
    int rows = 0;
    int cols = 0;

   private:
    DISALLOW_COPY_AND_ASSIGN(WindowInfo);
  };

  struct TabInfo {
    TabInfo() = default;
    TabInfo(TabInfo&&) = default;

    std::list<WindowInfo*> windows;  // Does not own.
    WindowInfo* active_window = nullptr;

   private:
    DISALLOW_COPY_AND_ASSIGN(TabInfo);
  };

  void Render();
  void RenderTabBar();
  void RenderStatus();

  KeyConfig key_config_;
  Screen screen_;
  CommandWindow command_window_;

  Mode mode_ = Mode::NORMAL;
  std::list<TabInfo> tabs_;
  std::list<WindowInfo> windows_;
  TabInfo* active_tab_ = nullptr;
};
