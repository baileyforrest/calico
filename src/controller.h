#pragma once

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "src/action.h"
#include "src/base/macros.h"
#include "src/key_config.h"
#include "src/screen.h"
#include "src/window/command_window.h"

class Controller : public Screen::Observer {
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

    std::set<WindowInfo*> windows;  // Does not own.
    WindowInfo* active_window = nullptr;

   private:
    DISALLOW_COPY_AND_ASSIGN(TabInfo);
  };

  struct Command {
    const char* name;
    void (Controller::*handler)(const std::vector<std::string>& command);
  };
  static const Command kDefaultCommands[];

  Window* active_window() {
    return active_tab_->active_window->window.get();
  }

  // Screen::Observer implementation:
  void OnScreenSizeChanged() override;

  void Render();
  void RenderTabBar();
  void RenderStatus();

  void HandleCommand(const std::string& command);
  void HandleQuit(const std::vector<std::string>& command);

  bool should_quit_ = false;

  KeyConfig key_config_;
  Screen screen_;
  CommandWindow command_window_;
  std::string command_error_;

  Mode mode_ = Mode::NORMAL;
  Mode last_mode_ = Mode::NORMAL;

  std::list<TabInfo> tabs_;
  std::list<WindowInfo> windows_;
  std::list<TabInfo>::iterator active_tab_;

  std::map<std::string, std::function<void(const std::vector<std::string>&)>>
      command_to_action_;
};
