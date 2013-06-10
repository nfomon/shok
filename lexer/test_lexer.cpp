#include <iostream>
#include <string>
using namespace std;

namespace {
  const string PROGRAM_NAME = "test_lexer";
};

int main(int argc, char* argv[]) {
  if (argc != 1) {
    cout << "usage: " << PROGRAM_NAME << endl;
    return 1;
  }

  return 0;
}
