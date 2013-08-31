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
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace {
  const string PROGRAM_NAME = "lush";
  const string PROMPT = "lush: ";
};

string runBuiltin_cd(const vector<string>& args) {
  std::string dir;
  if (0 == args.size()) {
    dir = getenv("HOME");
  } else if (1 == args.size()) {
    dir = args.at(0);
  } else {
    return "Invalid arguments to cd.  Usage:  cd [dir]";
  }
  if (-1 == chdir(dir.c_str())) {
    string msg;
    if (ENOENT == errno) {
      msg = "The directory does not exist";
    } else if (EACCES == errno) {
      msg = "Search permission denied for one of the components of PATH";
    } else if (EFAULT == errno) {
      msg = "PATH points outside of user-accessible address space";
    } else if (EIO == errno) {
      msg = "I/O error";
    } else if (ELOOP == errno) {
      msg = "Too many symbolic links were encountered while resolving PATH";
    } else if (ENAMETOOLONG == errno) {
      msg = "PATH is too long";
    } else if (ENOMEM == errno) {
      msg = "Insufficient kernel memory";
    } else if (ENOTDIR == errno) {
      msg = "A component of PATH is not a directory";
    }
    return "Failed to cd to " + dir + ": " + msg;
  }
  return "";
}

struct CmdResult {
  CmdResult(int returnCode = -1)
    : returnCode(returnCode) {}
  int returnCode;
  string print() {
    return boost::lexical_cast<string>(returnCode);
  }
};

CmdResult runCommand(string cmd) {
  // Parse the cmd into something exec-able.
  // Check if the program name is a shell built-in before we try to exec it.
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
  // Would prefer to dispatch builtins somewhere else.  Oh well.
  } else if ("cd" == cmdProc.cmd) {
    string result = runBuiltin_cd(cmdProc.args);
    if (result != "") {
      cout << result << endl;
      return CmdResult(0);    // builtins don't have nonzero error status
    }
    return CmdResult(0);
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
    bool error = false;
    while (true) {
      std::getline(eval.in, eval_result);
      if ("" == eval_result) {
        break;
      } else if ("CMD:" == eval_result.substr(0, 4)) {
        string cmd = eval_result.substr(4);
        CmdResult cmd_result = runCommand(cmd);
        // send back the command return-code to the evaluator
        eval.out << cmd_result.print() << endl;
      } else if ("PRINT:" == eval_result.substr(0, 6)) {
        cout << "[shell]: " << eval_result.substr(6) << endl;
      } else {
        // Unexpected communication from the evaluator, probably but not
        // necessarily starting with 'ERROR:'.  Grab up to a max of 20 lines
        // from the evaluator until we hit "" or give up.  This makes sure we
        // eat any CMDs that could have otherwise happened after the error.
        const int MAX_ERROR_COUNT = 20;
        int error_count = 1;
        while ("" != eval_result && error_count <= MAX_ERROR_COUNT) {
          cout << "[shell] eval: '" << eval_result << "'" << endl;
          std::getline(eval.in, eval_result);
          ++error_count;
        }
        if (MAX_ERROR_COUNT == error_count) {
          cout << "[shell] eval: found 20 errors; aborting" << endl;
        }
        error = true;
        break;
      }
    }

    if (error) {
      // TODO: signal the Parser to restart parsing
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
