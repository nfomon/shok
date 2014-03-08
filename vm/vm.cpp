// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* shok virtual machine execution engine
 *
 * Reads lines of stack-based "shok vm bytecode", and executes them.
 */

#include "Executor.h"
#include "VMError.h"

#include "util/Log.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::string;

using namespace vm;

namespace {
  const string PROGRAM_NAME = "shok_vm";
  const string LOGFILE = "vm.log";
}

int main(int argc, char *argv[]) {
  if (argc < 1 || argc > 2) {
    cout << "usage: " << PROGRAM_NAME << " [log level]" << endl;
    return 1;
  }

  Log log(LOGFILE);
  try {
    if (2 == argc) {
      log.setLevel(argv[1]);
    }

    Executor executor(log);
    log.info("Initialized executor");

    string line;
    int line_num = 0;
    while (std::getline(cin, line)) {
      ++line_num;
      log.debug("Received input line [" + boost::lexical_cast<string>(line_num) + "]: '" + line + "'");
      try {
        executor.exec(line);
      } catch (VMError& e) {
        log.error(string("Error executing bytecode at line " + boost::lexical_cast<string>(line_num) + ": ") + e.what());
        cout << endl;
        return -1;    // abort!
      }
    }
  } catch (std::exception& e) {
    log.error(string("Unknown error: ") + e.what());
  } catch (...) {
    log.error("Unknown error");
  }

  return 0;
}
