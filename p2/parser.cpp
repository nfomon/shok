// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* shok parser runner */

#include "ParseError.h"
#include "Parser.h"

#include "util/Log.h"

#include <iostream>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::string;

using namespace parser;

namespace {
  const string PROGRAM_NAME = "shok_parser";
  const string LOGFILE = "parser.log";
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
    Parser parser(cin, cout);
    if (!parser.parse()) {
      cout << "failed to parse" << endl;
    }
  } catch (const ParseError& e) {
    log.error(string("Parse error: ") + e.what());
    cout << endl;
  } catch (const std::exception& e) {
    log.error(string("Unknown error: ") + e.what());
  } catch (...) {
    log.error("Unknown error");
  }

  return 0;
}
