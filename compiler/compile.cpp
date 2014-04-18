// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* shok abstract syntax tree code-generator
 *
 * Reads lines of specially-formatted AST text input from stdin, performs
 * static analysis to ensure it specifies a valid program, and emits "bytecode"
 * instructions to the vm for execution.  Some built-in language constructs
 * will execute immediately during analysis.
 *
 * In the future, this may be broken into several parts; such as operator
 * re-ordering, type-checking and static analysis, optimization, etc.
 */

#include "AST.h"
#include "CompileError.h"
#include "util/Log.h"
#include "Token.h"

#include <iostream>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::string;

using namespace compiler;

namespace {
  const string PROGRAM_NAME = "shok_compiler";
  const string LOGFILE = "compiler.log";
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
        log.info("Compiling: '" + ast.print() + "'");
        ast.codegen();
        cout << endl;
      } catch (RecoveredError& e) {
        log.error(string("Compilation error: ") + e.what());
        cout << endl;
        // TODO: synchronize state with the parser
      } catch (CompileError& e) {
        log.error(string("Compilation error: ") + e.what());
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
