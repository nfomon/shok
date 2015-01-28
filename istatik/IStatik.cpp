// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "IStatik.h"

//#include "ConnectorWindow.h"
#include "InputWindow.h"

#include "statik/Connector.h"

#include <curses.h>
#include <panel.h>
#include <signal.h>
#include <stdlib.h>

#include <boost/ptr_container/ptr_vector.hpp>
using boost::ptr_vector;

#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace istatik;

/* public */

IStatik::IStatik(const string& compiler_name)
  : m_compiler_name(compiler_name) {
  m_compiler = exstatik::MakeCompiler(m_compiler_name);
  if (m_compiler.size() < 1) {
    throw ISError("Cannot use empty Compiler " + m_compiler_name);
  }
}

IStatik::~IStatik() {
  endwin();
}

void IStatik::run() {
  (void) signal(SIGINT, finish);

  //int leny = 1;
  //vector<int> lenx;
  //lenx.push_back(0);

  InputWindow inputWindow;

/*
  typedef ptr_vector<ConnectorWindow> connectorWindow_vec;
  typedef connectorWindow_vec::const_iterator connectorWindow_iter;
  ptr_vector<ConnectorWindow> connectorWindows;
  // First "Connector" simply represents the input window
  // Actually, we need Connector-adapters here
  // No, the first thing is different!!
  for (exstatik::Compiler::const_iterator i = m_compiler.begin();
       i != m_compiler.end(); ++i) {
    connectorWindows.push_back(new ConnectorWindow(*i));
  }
*/

  InitScreen();
  bool done = false;
  while (!done) {
    // Read input char at screen coords <y,x>
    int ch = wgetch(m_windows.at(0));
    int x, y;
    getyx(m_windows.at(0), y, x);

    WindowResponse response0 = inputWindow.Input(y, x, ch);
    int window = 0;
    UpdateWindow(window, response0.actions);

/*
    const statik::Hotlist* prevHotlist = response0.hotlist;
    for (connectorWindow_iter i = connectorWindows.begin();
         i != connectorWindows.end(); ++i) {
      WindowResponse response = i->Input(prevHotlist);
      ++window;
      UpdateWindow(window, response.actions);
      prevHotlist = response.hotlist;
    }
*/

    for (size_t i = 0; i < m_windows.size(); ++i) {
    }
  }

  for (size_t i = 0; i < m_windows.size(); ++i) {
    delwin(m_windows.at(i));
  }
  endwin();
}

/* private static */

void IStatik::finish(int sig) {
  endwin();
  exit(0);
}

/* private */

void IStatik::InitScreen() {
  (void) initscr();
  (void) cbreak();
  (void) noecho();

  int nrows, ncols;
  (void) getmaxyx(stdscr, nrows, ncols);

  int numWindows = m_compiler.size() + 1;
  int h = nrows/numWindows - 1;

  // Draw hlines
  for (int i = 0; i < numWindows; ++i) {
    mvwhline(stdscr, h*(i+1), 0, '-', ncols);
  }
  refresh();

  for (int i = 0; i < numWindows; ++i) {
    WINDOW* win = newwin(h, ncols, h*(i+1)+1, 0);
    m_windows.push_back(win);
    m_windowSizes.push_back(h);
    (void) keypad(win, true);
  }
  wmove(m_windows.at(0), 0, 0);
}

void IStatik::UpdateWindow(int window_index,
                           const WindowResponse::action_vec& actions) {
  WINDOW* window = m_windows.at(window_index);
/*
  for (WindowResponse::action_iter i = actions.begin();
       i != actions.end(); ++i) {
    switch (i->op) {
    case INSERT:
      break;
    case DELETE:
      break;
    default:
      throw ISError("Unknown Action");
    }
  }
*/

/*
    switch (ch) {
    case KEY_DC:
      if (x < lenx[y]) {
        wdelch(window);
        --lenx[y];
      }
      break;
    case KEY_BACKSPACE:
      if (x > 0) {
        --x;
        wmove(window, y, x);
        wdelch(window);
        --lenx[y];
        if (lenx[y] < 0) {
          throw ISError("X underflow");
        }
      }
      break;
    case KEY_LEFT:
      if (x > 0) {
        --x;
        wmove(window, y, x);
      }
      break;
    case KEY_RIGHT:
      if (x < lenx[y]) {
        ++x;
        wmove(m_windows.at(0), y, x);
      }
      break;
    case KEY_UP:
      if (y > 0) {
        --y;
        if (x > lenx[y]) {
          x = lenx[y];
        }
        wmove(m_windows.at(0), y, x);
      }
      break;
    case KEY_DOWN:
      if (y < leny-1) {
        ++y;
        if (x > lenx[y]) {
          x = lenx[y];
        }
        wmove(m_windows.at(0), y, x);
      }
      break;
    case KEY_ENTER:
    case '\n':
      if (y < m_windowSizes.at(0)-1) {
        ++y;
        if (y == leny) {
          ++leny;
          lenx.push_back(0);
        }
        x = 0;
        wmove(m_windows.at(0), y, x);
      }
      break;
    default:
      waddch(m_windows.at(0), ch);
      if (x == lenx[y]) {
        ++lenx[y];
      }
      break;
    }
*/

  wrefresh(window);
}
