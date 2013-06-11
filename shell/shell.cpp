#include "Proc.h"

#include <iostream>
using namespace std;

namespace {
  const string PROGRAM_NAME = "lush";
  const string PROMPT = "lush: ";
};

class Lexer : public Proc {
public:
  Lexer() : Proc("lexer") {}
protected:
  virtual void f() {
    execlp("./lush_lexer", "lush_lexer", (char*)NULL);
  }
};

class Parser : public Proc {
public:
  Parser() : Proc("parser") {}
protected:
  virtual void f() {
    execlp("./lush_parser", "lush_parser", (char*)NULL);
  }
};

class Evaluator : public Proc {
public:
  Evaluator() : Proc("evaluator") {}
protected:
  virtual void f() {
    execlp("./lush_eval", "lush_eval", (char*)NULL);
  }
};

int main(int argc, char *argv[]) {
  if (1 != argc) {
    cout << "usage: " << PROGRAM_NAME << endl;
    return 1;
  }

  Lexer lexer;
  lexer.run();

  Parser parser;
  parser.run();

  Evaluator eval;
  eval.run();

  cout << PROMPT;
  string line;
  while (std::getline(cin, line)) {
    // cout << "received input line: '" << line << "'" << endl;

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

    // get results
    string evalout;
    std::getline(eval.in, evalout);
    if ("" != evalout) {
      cout << "[shell] eval: '" << evalout << "'" << endl;
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
