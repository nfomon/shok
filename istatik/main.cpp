// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* Interactive client for statik compilers */

#include "ISError.h"
#include "ISLog.h"

#include "InputReader.h"
//#include "OutputAction.h"

#include <curses.h>
#include <panel.h>
#include <signal.h>
#include <stdlib.h>

#include <boost/program_options.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
using boost::ptr_vector;
namespace po = boost::program_options;

#include <iostream>
#include <string>
#include <vector>
using std::cout;
using std::endl;
using std::string;
using std::vector;

using namespace istatik;

namespace {
  const string PROGRAM_NAME = "istatik";
}

static void finish(int sig);

void run() {
  (void) signal(SIGINT, finish);
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

  //int h2 = nrows-h1;
  //WINDOW* out = newwin(h2, ncols, h2+1, 0);

  //InputReader reader;

  while (true) {
    int ch = wgetch(in);
    int x, y;
    getyx(in, y, x);

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
    wrefresh(in);

    // Feed to InputReader
    //reader.Insert(y, x, ch);

    // Get back list<OutputActions>
    // Execute the OutputActions
    // Get back list<Display

  }

  // But what of the next?  We have list of "in" tokens...
  // But that's not really the same.  That's just another "out" window!! :D

  delwin(in);
  //delwin(out);
  finish(0);
}

int main(int argc, char *argv[]) {
  try {
    // Retrieve program options
    po::options_description desc(PROGRAM_NAME + " usage");
    desc.add_options()
      ("help,h", "show help message")
      ("logfile,f", po::value<string>(), "output log file")
      ("loglevel,L", po::value<string>(), "log level: debug, info, warning, error")
    ;
    po::positional_options_description p;
    po::variables_map vm;
    string logfile;
    string loglevel;

    try {
      po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
      po::notify(vm);

      if (vm.count("help")) {
        cout << desc;
        return 0;
      }
      if (vm.count("logfile")) {
        logfile = vm["logfile"].as<string>();
      }
      if (vm.count("loglevel")) {
        loglevel = vm["loglevel"].as<string>();
      }
    } catch (po::error& e) {
      cout << desc;
      return 1;
    }

    // Initialize logging
    if (!logfile.empty()) {
      if (!loglevel.empty()) {
        g_log.setLevel(loglevel);
      }
      g_log.Init(logfile);
    }

    run();
  } catch (const ISError& e) {
    endwin();
    g_log.error() << "IStatik error: " << e.what();
    return 1;
  } catch (const std::exception& e) {
    g_log.error() << "Unknown error: " << e.what();
    endwin();
    return 1;
  } catch (...) {
    g_log.error() << "Unknown error";
    return 1;
  }

  return 0;
}

static void finish(int sig) {
  endwin();
  exit(0);
}
