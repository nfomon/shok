// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Token_h_
#define _Token_h_

/* Token of AST input read from the parser */

#include "Log.h"

#include <string>
#include <vector>

namespace eval {

struct Token {
  Token() {}
  Token(const std::string& name, const std::string& value = "")
    : name(name), value(value) {}
  std::string print() const;
  std::string name;
  std::string value;
};

class Tokenizer {
public:
  Tokenizer()
    : mode(MODE_NONE),
      codeDepth(0) {}
  typedef std::vector<Token> token_vec;
  typedef token_vec::const_iterator token_iter;

  token_vec tokenize(Log& log, const std::string& ast);

private:
  enum MODE {
    MODE_NONE,
    MODE_CMD,
    MODE_CODE,
  };

  MODE mode;
  int codeDepth;
};

};

#endif // _Token_h_
