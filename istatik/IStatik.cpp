// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "IStatik.h"

#include <curses.h>
#include <panel.h>
#include <signal.h>
#include <stdlib.h>

#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace istatik;

IStatik::IStatik(const string& compiler_name)
  : m_compiler_name(compiler_name) {
  //m_compiler = Compiler::MakeCompiler(m_compiler_name);   // TODO
}

IStatik::~IStatik() {
  clear_screen(0);
}

void IStatik::run() {
  (void) signal(SIGINT, clear_screen);
  (void) initscr();
  (void) cbreak();
  (void) noecho();

  int nrows, ncols;
  (void) getmaxyx(stdscr, nrows, ncols);

  int leny = 1;
  vector<int> lenx;
  lenx.push_back(0);

/*
  ptr_vector<Connector> connectors;
  int numWindows = connectors.size() + 1;
  int h1 = nrows/numWindows;
*/
  int h1 = nrows/2;

  mvwhline(stdscr, h1, 0, '-', ncols);
  refresh();

  WINDOW* in = newwin(h1, ncols, 0, 0);
  (void) keypad(in, true);
  wmove(in, 0, 0);

  int h2 = nrows-h1;
  WINDOW* out = newwin(h2, ncols, h2+1, 0);

  while (true) {
    int ch = wgetch(in);
    int x, y;
    getyx(in, y, x);

    //Action action = actuator.Insert(y, x, ch);

/*
    for (Action::iaction_iter i = action.iactions.begin();
         i != action.iactions.end(); ++i) {
      switch (i->op) {
      case IAction::INSERT:
        break;
      case IAction::DELETE:
        break;
      default:
        throw ISError("Unknown IAction");
      }
    }
*/

    switch (ch) {
    case KEY_DC:
      if (x < lenx[y]) {
        wdelch(in);
        --lenx[y];
      }
      break;
    case KEY_BACKSPACE:
      if (x > 0) {
        --x;
        wmove(in, y, x);
        wdelch(in);
        --lenx[y];
        if (lenx[y] < 0) {
          throw ISError("X underflow");
        }
      }
      break;
    case KEY_LEFT:
      if (x > 0) {
        --x;
        wmove(in, y, x);
      }
      break;
    case KEY_RIGHT:
      if (x < lenx[y]) {
        ++x;
        wmove(in, y, x);
      }
      break;
    case KEY_UP:
      if (y > 0) {
        --y;
        if (x > lenx[y]) {
          x = lenx[y];
        }
        wmove(in, y, x);
      }
      break;
    case KEY_DOWN:
      if (y < leny-1) {
        ++y;
        if (x > lenx[y]) {
          x = lenx[y];
        }
        wmove(in, y, x);
      }
      break;
    case KEY_ENTER:
    case '\n':
      if (y < h1-1) {
        ++y;
        if (y == leny) {
          ++leny;
          lenx.push_back(0);
        }
        x = 0;
        wmove(in, y, x);
      }
      break;
    default:
      waddch(in, ch);
      if (x == lenx[y]) {
        ++lenx[y];
      }
      break;
    }

    // Feed to Actuator
    //actuator.Insert(y, x, ch);

    // Get back list<OutputActions>
    // Execute the OutputActions
    // Get back list<Display
    wrefresh(out);
    wrefresh(in);
  }

  // But what of the next?  We have list of "in" tokens...
  // But that's not really the same.  That's just another "out" window!! :D

  delwin(out);
  delwin(in);
  endwin();
}

void IStatik::clear_screen(int sig) {
  endwin();
}
