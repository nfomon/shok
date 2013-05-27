#include "Proc.h"

#include <iostream>
using namespace std;

const string PROGRAM_NAME = "lush_eval";

int main(int argc, char *argv[]) {
  if (1 != argc) {
    cout << "usage: " << PROGRAM_NAME << endl;
    return 1;
  }

  return 0;
}
