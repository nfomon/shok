// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* shok lexer */

#include "Lexer.h"
#include "LexError.h"

#include "util/Log.h"

#include <iostream>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::string;

namespace {
  const string PROGRAM_NAME = "shok_lexer";
  const string LOGFILE = "lexer.log";
}

using namespace lexer;

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
    Lexer lexer(log, cin, cout);
    if (!lexer.lex()) {
      cout << "failed to lex program!" << endl;
    }
  } catch (const LexError& e) {
    log.error(string("Error lexing program: ") + e.what());
    cout << endl;
  } catch (const std::exception& e) {
    log.error(string("Unknown error: ") + e.what());
  } catch (...) {
    log.error("Unknown error");
  }

  return 0;
}
