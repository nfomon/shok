#include "AST.h"
#include "EvalError.h"
#include "Log.h"
#include "Token.h"

#include <iostream>
#include <string>
using namespace std;
using namespace eval;

namespace {
  const string PROGRAM_NAME = "lush_eval";
};

int main(int argc, char *argv[]) {
  if (argc < 1 || argc > 2) {
    cout << "usage: " << PROGRAM_NAME << " [log level]" << endl;
    return 1;
  }

  Log log;
  try {
    if (2 == argc) {
      log.setLevel(argv[1]);
    }

    AST ast(log);
    log.info("Initialized AST");

    string line;
    while (getline(cin, line)) {
      log.debug("Received input line: '" + line + "'");
      try {
        Tokenizer::token_vec tokens = Tokenizer::tokenize(line);
        for (Tokenizer::token_iter i = tokens.begin();
             i != tokens.end(); ++i) {
          log.debug("Inserting token: '" + i->name + ":" + i->value + "'");
          ast.insert(*i);
        }
        log.info("Evaluating: '" + ast.print() + "'");
        ast.evaluate();
        cout << "Evaluated. " << ast.print() << endl;
      } catch (EvalError& e) {
        log.error(string("Error evaluating parse tree: ") + e.what());
        ast.reset();
      }
    }
  } catch (exception& e) {
    log.error(string("Unknown error: ") + e.what());
  } catch (...) {
    log.error("Unknown error");
  }

  return 0;
}
