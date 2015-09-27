// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "IStatik.h"

#include "ParserWindow.h"
#include "InputWindow.h"

#include "statik/Rule.h"

#include <climits>
#include <curses.h>
#include <panel.h>
#include <signal.h>
#include <stdlib.h>

#include <boost/ptr_container/ptr_vector.hpp>
using boost::ptr_vector;

#include <iostream>
#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::endl;
using std::string;
using std::vector;

using namespace istatik;

/* public */

IStatik::IStatik(const string& compiler_name, const string& graphdir)
  : m_compiler_name(compiler_name),
    m_graphdir(graphdir) {
}

IStatik::~IStatik() {
  endwin();
}

void IStatik::run() {
  (void) signal(SIGINT, finish);

  auto_ptr<exstatik::Compiler> compiler = exstatik::MakeCompiler(m_compiler_name);
  if (!compiler.get() || compiler->empty()) {
    throw ISError("Cannot use empty Compiler " + m_compiler_name);
  }

  InitScreen(compiler->size());

  int inrows, incols;
  (void) getmaxyx(m_windows.at(0), inrows, incols);
  InputWindow inputWindow(inrows, incols);

  typedef ptr_vector<ParserWindow> parserWindow_vec;
  typedef parserWindow_vec::iterator parserWindow_mod_iter;
  ptr_vector<ParserWindow> parserWindows;

  size_t num_compilers = compiler->size();
  for (size_t i = 0; i < num_compilers; ++i) {
    exstatik::Compiler::auto_type c = compiler->release(compiler->begin());
    string name = c->Name();
    g_log.info() << "Adding parser window for " << name;
    parserWindows.push_back(new ParserWindow(auto_ptr<statik::Rule>(c.release()), name, m_graphdir));
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

    for (parserWindow_mod_iter i = parserWindows.begin();
         i != parserWindows.end(); ++i) {
      g_log.info() << " - update parser?";
      if (response.batch.IsEmpty()) { break; }
      response = i->Input(response.batch);
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

void IStatik::InitScreen(size_t numParsers) {
  (void) initscr();
  (void) cbreak();
  (void) noecho();

  int nrows, ncols;
  (void) getmaxyx(stdscr, nrows, ncols);

  if (numParsers >= INT_MAX) {
    throw ISError("Too many parsers");
  }
  int numWindows = (int)numParsers + 1;
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
