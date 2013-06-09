#ifndef _Token_h_
#define _Token_h_

/* AST node */

#include <string>

namespace eval {

struct Token {
  Token(const std::string& name, const std::string& value = "")
    : name(name), value(value) {}
  std::string name;
  std::string value;
};

};

#endif // _Token_h_
