// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* shok compiler framework example runner */

#include "Char.h"
#include "Connector.h"
#include "DS.h"
#include "Or.h"
#include "Literal.h"

#include "util/Log.h"

#include <iostream>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::string;

using namespace fw;

namespace {
  const string PROGRAM_NAME = "shok_fw";
  const string LOGFILE = "fw.log";
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

    // Lexer machine
    OrNode lexer(log, "lexer");
    //RegexpNode identifier_("[A-Za-z_][0-9A-Za-z_]");
    std::auto_ptr<MachineNode> new_(new LiteralNode(log, "new"));
    lexer.AddChild(new_);
    std::auto_ptr<MachineNode> x_(new LiteralNode(log, "x"));
    lexer.AddChild(x_);

    // Lexer connector
    Connector<ListDS> lexerConnector(log, lexer);

    // Insert a character
    ListDS c1(std::auto_ptr<State>(new CharState('{')));
    lexerConnector.Insert(c1);
    ListDS c2(std::auto_ptr<State>(new CharState('e')));
    c1.right = &c2;
    c2.left = &c1;
    lexerConnector.Insert(c2);

  } catch (const FWError& e) {
    log.error(string("Compilation framework error: ") + e.what());
    return 1;
  } catch (const std::exception& e) {
    log.error(string("Unknown error: ") + e.what());
    return 1;
  } catch (...) {
    log.error("Unknown error");
    return 1;
  }

  return 0;
}
