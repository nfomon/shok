// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "IStatik.h"

#include "ConnectorWindow.h"
#include "InputWindow.h"

#include "statik/Connector.h"

#include <curses.h>
#include <panel.h>
#include <signal.h>
#include <stdlib.h>

#include <boost/ptr_container/ptr_vector.hpp>
using boost::ptr_vector;

#include <iostream>
#include <string>
#include <vector>
using std::endl;
using std::string;
using std::vector;

using namespace istatik;

/* public */

IStatik::IStatik(const string& compiler_name, const string& graphdir)
  : m_compiler_name(compiler_name),
    m_graphdir(graphdir),
    m_compiler(exstatik::MakeCompiler(m_compiler_name)) {
  if (m_compiler->size() < 1) {
    throw ISError("Cannot use empty Compiler " + m_compiler_name);
  }
}

IStatik::~IStatik() {
  endwin();
}

void IStatik::run() {
  (void) signal(SIGINT, finish);

  InitScreen();

  int inrows, incols;
  (void) getmaxyx(m_windows.at(0), inrows, incols);
  InputWindow inputWindow(inrows, incols);

  typedef ptr_vector<ConnectorWindow> connectorWindow_vec;
  typedef connectorWindow_vec::iterator connectorWindow_mod_iter;
  ptr_vector<ConnectorWindow> connectorWindows;
  for (exstatik::Compiler_mod_iter i = m_compiler->begin();
       i != m_compiler->end(); ++i) {
    g_log.info() << "Adding connector window for " << i->Name();
    connectorWindows.push_back(new ConnectorWindow(*i, m_graphdir));
  }

  bool done = false;
  while (!done) {
    // Read input char at screen coords <y,x>
    int ch = wgetch(m_windows.at(0));
    int x, y;
    getyx(m_windows.at(0), y, x);

    g_log.info() << "IStatik: Received (" << y << "," << x << "):" << (char)ch;

    WindowResponse response = inputWindow.Input(y, x, ch);
    int window_index = 0;
    UpdateWindow(window_index, response.actions);
    int y0, x0;
    getyx(m_windows.at(0), y0, x0);

    for (connectorWindow_mod_iter i = connectorWindows.begin();
         i != connectorWindows.end(); ++i) {
      g_log.info() << " - update connector?";
      if (response.hotlist.IsEmpty()) { break; }
      response = i->Input(response.hotlist);
      ++window_index;
      UpdateWindow(window_index, response.actions);
    }
    wmove(m_windows.at(0), y0, x0);
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

  int numWindows = m_compiler->size() + 1;
  int h = (nrows - numWindows) / numWindows;
  g_log.debug() << numWindows << " windows with h=" << h;

  // Draw hlines
  for (int i = 1; i < numWindows; ++i) {
    g_log.debug() << "Drawing hline at " << (h+1)*i-1;
    mvwhline(stdscr, (h+1)*i-1, 0, '-', ncols);
  }
  refresh();

  for (int i = 0; i < numWindows; ++i) {
    g_log.debug() << "Window of height h=" << h << " starts at " << (h+1)*i;
    WINDOW* win = newwin(h, ncols, (h+1)*i, 0);
    m_windows.push_back(win);
    m_windowSizes.push_back(h);
    (void) keypad(win, true);
  }
  wmove(m_windows.at(0), 0, 0);
}

void IStatik::UpdateWindow(int window_index,
                           const WindowResponse::action_vec& actions) {
  WINDOW* window = m_windows.at(window_index);
  for (WindowResponse::action_iter i = actions.begin();
       i != actions.end(); ++i) {
    switch (i->action) {
    case WindowAction::MOVE:
      wmove(window, i->y, i->x);
      break;
    case WindowAction::INSERT:
      waddch(window, i->ch);
      break;
    case WindowAction::DELETE:
      wdelch(window);
      break;
    default:
      throw ISError("Unknown Action type");
    }
  }

  wrefresh(window);
}
