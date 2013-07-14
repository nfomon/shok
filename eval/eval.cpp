// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

/* Lush abstract syntax tree evaluator
 *
 * Reads lines of specially-formatted AST text input from stdin, performs
 * static analysis to ensure it specifies a valid program, and executes the
 * program.  This may involve forking off child processes to run shell
 * commands.
 *
 * In the future, this may be broken into several parts; such as operator
 * re-ordering, type-checking and static analysis, optimization, code/bytecode
 * generation, and execution.
 *
 * Note that in the multiple stages of the evaluator, we perform each step
 * through to completion before advancing to the next phase.  I'd like to undo
 * this.  Certainly evaluation (i.e. execution) should not happen until all
 * possible validation / analysis of all possible nodes has happened.  But for
 * the other stages, I think it might be best to provide as deep error-checking
 * as possible on a line of code before reporting any errors regarding nodes
 * that follow it.  Unsure about that though.  For now we perform each full
 * stage of analysis (completion, reordering operators, static analysis,
 * evaluation) in sequence.
 */

#include "AST.h"
#include "EvalError.h"
#include "Log.h"
#include "Token.h"

#include <iostream>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::string;

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
        cout << "" << endl;
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
