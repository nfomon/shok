// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* shok abstract syntax tree evaluator
 *
 * Reads lines of specially-formatted AST text input from stdin, performs
 * static analysis to ensure it specifies a valid program, and emits text on
 * stdout possibly instructing the shell to run programs on our behalf.
 *
 * In the future, this may be broken into several parts; such as operator
 * re-ordering, type-checking and static analysis, optimization, code/bytecode
 * generation, and execution.
 */

#include "AST.h"
#include "EvalError.h"
#include "util/Log.h"
#include "Token.h"

#include <iostream>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::string;

using namespace eval;

namespace {
  const string PROGRAM_NAME = "shok_eval";
  const string LOGFILE = "eval.log";
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

    AST ast(log);
    log.info("Initialized AST");

    Tokenizer tokenizer;
    string line;
    while (getline(cin, line)) {
      log.debug("Received input line: '" + line + "'");
      try {
        Tokenizer::token_vec tokens = tokenizer.tokenize(log, line);
        for (Tokenizer::token_iter i = tokens.begin();
             i != tokens.end(); ++i) {
          log.debug("Inserting token: '" + i->name + ":" + i->value + "'");
          ast.insert(*i);
        }
        log.info("Evaluating: '" + ast.print() + "'");
        ast.evaluate();
        cout << endl;
      } catch (RecoveredError& e) {
        log.error(string("Error evaluating parse tree: ") + e.what());
        cout << endl;
        // TODO: synchronize state with the parser
      } catch (EvalError& e) {
        log.error(string("Error evaluating parse tree: ") + e.what());
        cout << endl;
        ast.reset();
      }
    }
  } catch (std::exception& e) {
    log.error(string("Unknown error: ") + e.what());
  } catch (...) {
    log.error("Unknown error");
  }

  return 0;
}
