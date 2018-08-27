#pragma once

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <set>
#include <string>

#include "action.h"  // NOLINT(build/include)
#include "base/macros.h"
#include "key_config.h"  // NOLINT(build/include)
#include "screen.h"      // NOLINT(build/include)
#include "window/command_window.h"

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
