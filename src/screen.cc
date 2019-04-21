#include "src/screen.h"

#include <locale.h>
#include <ncurses.h>

#include <cassert>
#include <cstdlib>
#include <iostream>

#include "src/key.h"

namespace {

constexpr int kEscapeDelayMs = 25;

}  // namespace

Screen::Screen() {
  setlocale(LC_ALL, "");
  initscr();
  raw();
  noecho();
  keypad(stdscr, true);
  start_color();
  curs_set(2);
  set_escdelay(kEscapeDelayMs);

  RefreshSize();
  Refresh();
}

Screen::~Screen() { endwin(); }

void Screen::AddObserver(Observer* o) { observers_.insert(o); }

void Screen::RemoveObserver(Observer* o) { observers_.erase(o); }

void Screen::Clear() { clear(); }

void Screen::SetChar(int row, int col, wchar_t val) {
  assert(row >= 0 && row < rows_);
  assert(col >= 0 && col < cols_);
  mvaddch(row, col, val);
}

void Screen::SetChars(int row, int col, const std::string& str) {
  for (char c : str) {
    SetChar(row, col++, c);
  }
}

void Screen::SetCursorPos(int row, int col) {
  assert(row >= 0 && row < rows_);
  assert(col >= 0 && col < cols_);
  move(row, col);  // NOLINT(build/include_what_you_use)
}

void Screen::Refresh() { refresh(); }

void Screen::EnableReverse() { attron(A_REVERSE); }

void Screen::DisableReverse() { attroff(A_REVERSE); }

void Screen::EnableBold() { attron(A_BOLD); }

void Screen::DisableBold() { attroff(A_BOLD); }

Screen::KeyState Screen::ReadKey() {
  while (true) {
    KeyState result;

    int res = get_wch(&result.code);
    switch (res) {
      case KEY_CODE_YES:
        result.is_key_code = true;
        break;
      case OK:
        break;
      default:
        continue;
    }

    if (result.is_key_code && result.code == KEY_RESIZE) {
      RefreshSize();
      for (auto* obs : observers_) {
        obs->OnScreenSizeChanged();
      }
      continue;
    }

    if (result.code == static_cast<wchar_t>(Key::ESCAPE)) {
      result.is_key_code = true;

      nodelay(stdscr, true);
      wint_t wch;
      res = get_wch(&wch);
      nodelay(stdscr, false);

      // If another key is available, then assume it is alt + key
      if (res != ERR) {
        result.code = wch;
        result.alt = true;
      }
    }

    return result;
  }
}

void Screen::RefreshSize() { getmaxyx(stdscr, rows_, cols_); }
