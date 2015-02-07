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

  InitScreen();

  int inrows, incols;
  (void) getmaxyx(m_windows.at(0), inrows, incols);
  InputWindow inputWindow(inrows, incols);

  typedef ptr_vector<ConnectorWindow> connectorWindow_vec;
  typedef connectorWindow_vec::iterator connectorWindow_mod_iter;
  ptr_vector<ConnectorWindow> connectorWindows;
  for (exstatik::Compiler::const_iterator i = m_compiler.begin();
       i != m_compiler.end(); ++i) {
    g_log.info() << "Adding connector window";
    connectorWindows.push_back(new ConnectorWindow(*i));
  }

  bool done = false;
  while (!done) {
    // Read input char at screen coords <y,x>
    int ch = wgetch(m_windows.at(0));
    int x, y;
    getyx(m_windows.at(0), y, x);

    g_log.info() << "(" << y << "," << x << "):" << (char)ch;

    WindowResponse response0 = inputWindow.Input(y, x, ch);
    int window_index = 0;
    UpdateWindow(window_index, response0.actions);

    const statik::Hotlist* prevHotlist = response0.hotlist;
    for (connectorWindow_mod_iter i = connectorWindows.begin();
         i != connectorWindows.end(); ++i) {
      if (!prevHotlist) { break; }
      g_log.info() << " - updating connector" << endl;
      WindowResponse response = i->Input(*prevHotlist);
      ++window_index;
      UpdateWindow(window_index, response.actions);
      prevHotlist = response.hotlist;
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
  for (WindowResponse::action_iter i = actions.begin();
       i != actions.end(); ++i) {
    const MoveAction* move = dynamic_cast<const MoveAction*>(&*i);
    const DeleteAction* del = dynamic_cast<const DeleteAction*>(&*i);
    const InsertAction* ins = dynamic_cast<const InsertAction*>(&*i);
    if (move) {
      wmove(window, move->y, move->x);
    } else if (del) {
      wdelch(window);
    } else if (ins) {
      waddch(window, ins->ch);
    } else {
      throw ISError("Unknown Action type");
    }
  }

  wrefresh(window);
}
