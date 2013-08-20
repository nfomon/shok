// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

/* lush command shell
 *
 * WARNING: This is insecure software.  There are security
 * vulnerabilities, and this software may easily produce undesired
 * behaviour, using full permissions of the caller.  Do NOT use this on
 * a system where integrity matters, and do NOT run this program from
 * an account that has permission to do anything at all dangerous.
 *
 * This is a work in (the early stages of) progress.
 */

#include "Proc.h"
#include "Util.h"

#include <boost/tokenizer.hpp>

#include <iostream>
#include <string>
#include <utility>
using namespace std;

namespace {
  const string PROGRAM_NAME = "lush";
  const string PROMPT = "lush: ";
};

struct CmdResult {
  CmdResult(int returnCode = -1)
    : returnCode(returnCode) {}
  int returnCode;
  string print() {
    return boost::lexical_cast<string>(returnCode);
  }
};

CmdResult runCommand(string cmd) {
  // Parse the cmd into something exec-able
  // TODO: allow escaped spaces in the program name
  // First trim whitespace from left and right ends of cmd
  cmd = Util::ltrim_space(cmd);
  cmd = Util::rtrim_space(cmd);
  typedef boost::escaped_list_separator<char> els_t;
  typedef boost::tokenizer<els_t> tok_t;
  els_t els("\\", " ", "\'\"");
  tok_t tok(cmd, els);
  if (tok.begin() == tok.end()) {
    perror(("Cannot run empty command \"" + cmd + "\"").c_str());
    _exit(1);
  }
  Proc cmdProc("cmd");
  cmdProc.cmd = "";
  cmdProc.pipechat = false;
  for (tok_t::const_iterator i = tok.begin(); i != tok.end(); ++i) {
    if ("" == cmdProc.cmd) {
      cmdProc.cmd = *i;
    } else {
      cmdProc.args.push_back(*i);
    }
  }
  if ("" == cmdProc.cmd) {
    perror(("Cannot run curiously empty command \"" + cmd + "\"").c_str());
    _exit(1);
  }
  cmdProc.run();
  int status;
  if (-1 == waitpid(cmdProc.pid, &status, 0)) {
    perror(("Error waiting for child cmd " + cmdProc.cmd).c_str());
    _exit(1);
  }
  if (!WIFEXITED(status)) {
    perror(("Something strange happened to child cmd " + cmdProc.cmd).c_str());
    _exit(1);
  }
  return CmdResult(WEXITSTATUS(status));
}

int main(int argc, char *argv[]) {
  if (1 != argc) {
    cout << "usage: " << PROGRAM_NAME << endl;
    return 1;
  }

  Proc lexer("./lush_lexer");
  lexer.run();

  Proc parser("./lush_parser");
  parser.run();

  Proc eval("./lush_eval");
  eval.run();

  cout << PROMPT;
  string line;
  while (std::getline(cin, line)) {
    // send line to lexer
    lexer.out << line << endl;

    // get tokens
    string tokens;
    std::getline(lexer.in, tokens);

    // send tokens to parser
    parser.out << tokens << endl;

    // get AST
    string ast;
    std::getline(parser.in, ast);
    if ("::Parse error:" == ast.substr(0, 14)) {
      cout << "[shell] parser: " << ast.substr(15) << endl;
      ast = "";   // skip eval
    }

    // send AST to eval
    eval.out << ast << endl;

    // get commands or result
    string eval_result;
    std::getline(eval.in, eval_result);
    while ("CMD:" == eval_result.substr(0, 4)) {
      string cmd = eval_result.substr(4);
      CmdResult cmd_result = runCommand(cmd);
      eval.out << cmd_result.print() << endl;
      std::getline(eval.in, eval_result);
    }
    while ("" != eval_result) {
      cout << "[shell] eval: '" << eval_result << "'" << endl;
      std::getline(eval.in, eval_result);
      // TODO: if error, signal the Parser to restart parsing
    }

    // redisplay prompt
    cout << PROMPT;
  }
  cout << endl;

  lexer.finish();
  parser.finish();
  eval.finish();

  if (-1 == wait(NULL)) {
    perror("waiting for child");
    _exit(1);
  }
  return 0;
}
