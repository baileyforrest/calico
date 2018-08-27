#include "screen.h"  // NOLINT(build/include)

#include <locale.h>
#include <curses.h>

#include <cassert>
#include <cstdlib>
#include <iostream>

Screen::Screen() {
  setlocale(LC_ALL, "");
  initscr();
  raw();
  noecho();
  keypad(stdscr, true);
  start_color();
  curs_set(2);

  RefreshSize();
  Refresh();
}

Screen::~Screen() {
  endwin();
}

void Screen::Clear() {
  clear();
}

void Screen::RefreshSize() {
  getmaxyx(stdscr, rows_, cols_);
}

void Screen::SetChar(int row, int col, wchar_t val) {
  assert(row >= 0 && row < rows_);
  assert(col >= 0 && col < cols_);
  mvaddch(row, col, val);
}

void Screen::SetCursorPos(int row, int col) {
  assert(row >= 0 && row < rows_);
  assert(col >= 0 && col < cols_);
  move(row, col);  // NOLINT(build/include_what_you_use)
}

void Screen::Refresh() {
  refresh();
}

void Screen::EnableReverse() {
  attron(A_REVERSE);
}

void Screen::DisableReverse() {
  attroff(A_REVERSE);
}
