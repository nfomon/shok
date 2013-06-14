#ifndef _Token_h_
#define _Token_h_

/* AST node */

#include <string>
#include <vector>

namespace eval {

struct Token {
  Token(const std::string& name, const std::string& value = "")
    : name(name), value(value) {}
  std::string print() const;
  std::string name;
  std::string value;
};

namespace Tokenizer {
  typedef std::vector<Token> token_vec;
  typedef token_vec::const_iterator token_iter;

  token_vec tokenize(const std::string& ast);
};

};

#endif // _Token_h_
