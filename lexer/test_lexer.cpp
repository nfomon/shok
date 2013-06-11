#include "Proc.h"

#include <iostream>
#include <string>
using namespace std;

namespace {
  const string PROGRAM_NAME = "test_lexer";
};

class Evaluator : public Proc {
public:
  Evaluator() : Proc("evaluator") {}
protected:
  virtual void f() {
    execlp("./lush_eval", "lush_eval", (char*)NULL);
  }
};

int main(int argc, char* argv[]) {
  if (argc != 1) {
    cout << "usage: " << PROGRAM_NAME << endl;
    return 1;
  }

  Lexer lexer;
  lexer.run();

  lexer.close();

  if (-1 == wait(NULL)) {
    perror("waiting for child lexer");
    _exit(-1);
  }
  return 0;
}
