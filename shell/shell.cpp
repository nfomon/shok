#include "Proc.h"

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

CmdResult runCommand(const string& cmd) {
  cout << "RUN: " << cmd << endl;
  // Parse the cmd into something exec-able
  // TODO: allow escaped spaces in the program name
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

  Proc lexer("lush_lexer");
  lexer.run();

  Proc parser("lush_parser");
  parser.run();

  Proc eval("lush_eval");
  eval.run();

  cout << PROMPT;
  string line;
  while (std::getline(cin, line)) {
    //cout << "received input line: '" << line << "'" << endl;

    // send line to lexer
    lexer.out << line << endl;
    //cout << "sent '" << line << "' to lexer" << endl;
    if (!lexer.isRunning()) {   // bad race
      cout << "[shell] Lexer error; aborting" << endl;
      break;
    }

    // get tokens
    string tokens;
    std::getline(lexer.in, tokens);
    //cout << "[shell] tokens: '" << tokens << "'" << endl;

    // send tokens to parser
    parser.out << tokens << endl;
    //cout << "sent '" << tokens << "' to parser" << endl;
    if (!parser.isRunning()) {  // bad race
      cout << "[shell] Parser error; aborting" << endl;
      break;
    }

    // get AST
    string ast;
    std::getline(parser.in, ast);
    //cout << "[shell] ast: '" << ast << "'" << endl;

    // send AST to eval
    eval.out << ast << endl;
    //cout << "sent '" << ast << "' to evaluator" << endl;
    if (!eval.isRunning()) {    // bad race
      cout << "[shell] Evaluator error; aborting" << endl;
      break;
    }

    // get commands or result
    string eval_result;
    std::getline(eval.in, eval_result);
    while ("CMD:" == eval_result.substr(0, 4)) {
      cout << "[shell] run cmd: " << eval_result << endl;
      string cmd = eval_result.substr(4);
      CmdResult cmd_result = runCommand(cmd);
      eval.out << cmd_result.print() << endl;
      std::getline(eval.in, eval_result);
    }
    if ("" != eval_result) {
      cout << "[shell] eval: '" << eval_result << "'" << endl;
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
