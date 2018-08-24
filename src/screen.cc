#include "screen.h"  // NOLINT(build/include)

#include <locale.h>
#include <curses.h>

#include <cassert>
#include <cstdlib>
#include <iostream>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

Screen::Screen() : render_cb_(std::bind(&Screen::SetChar, this, _1, _2, _3)) {
  setlocale(LC_ALL, "");
  initscr();
  start_color();
  getmaxyx(stdscr, rows_, cols_);
  refresh();
}

Screen::~Screen() {
  getch();
  endwin();
}

void Screen::SetChar(int row, int col, wchar_t val) {
  assert(row >= 0 && row < rows_);
  assert(col >= 0 && col < cols_);
  mvaddch(row, col, val);
}

void Screen::Refresh() {
  refresh();
}
