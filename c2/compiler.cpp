// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* shok static analyzer, type checker, and code generator.
 *
 * Reads lines of AST (except with unordered expressions) from the parser,
 * performs many checks including type checks, and emits bytecode appropriate
 * for the shok vm.
 */

#include "CompileError.h"
#include "Compiler.h"

#include "util/Log.h"

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
    Compiler compiler(cin);
    if (compiler.execute()) {
      cout << "compiled!" << endl;
    } else {
      cout << "failed to compile" << endl;
    }
  } catch (const CompileError& e) {
    log.error(string("Compilation error: ") + e.what());
    cout << endl;
  } catch (const std::exception& e) {
    log.error(string("Unknown error: ") + e.what());
  } catch (...) {
    log.error("Unknown error");
  }

  return 0;
}
