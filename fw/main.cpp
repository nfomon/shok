// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* shok compiler framework example runner */

#include "Char.h"
#include "Connector.h"
#include "DS.h"
#include "Keyword.h"
#include "Or.h"
#include "Rule.h"

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

class Lexer {
public:
  Lexer(Log& log, const std::string& name)
    : m_log(log),
      m_name(name),
      m_machine(log, name) {
    //RegexpRule identifier_("[A-Za-z_][0-9A-Za-z_]");
    std::auto_ptr<Rule> new_(new KeywordRule(log, "new"));
    m_machine.AddChild(new_);
    std::auto_ptr<Rule> x_(new KeywordRule(log, "x"));
    m_machine.AddChild(x_);
  }

  Rule& Machine() { return m_machine; }

private:
  Log& m_log;
  string m_name;
  OrRule m_machine;
};

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

    // Lexer
    Lexer lexer(log, "lexer");
    log.info("Lexer: " + lexer.Machine().print());
    Connector<ListDS> lexerConnector(log, lexer.Machine());

    ListDS* start = NULL;
    ListDS* prev = NULL;
    string line;
    while (getline(cin, line)) {
      line += "\n";
      for (int i=0; i < line.size(); ++i) {
        ListDS* c = new ListDS(std::auto_ptr<State>(new CharState(line.at(i))));
        if (!start) { start = c; }
        if (prev) {
          prev->right = c;
          c->left = prev;
        }
        log.info("");
        log.info("Lexer input: " + c->print());
        lexerConnector.Insert(*c);
        prev = c;
      }
    }
    log.info("Clearing input");
    ListDS* i = start;
    while (i) {
      ListDS* j = i->right;
      delete i;
      i = j;
    }

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
