// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Token.h"

#include "EvalError.h"

#include <ctype.h>
#include <string>
using std::string;

using namespace eval;

string Token::print() const {
  if ("" == value) {
    return name;
  }
  return name + ":" + value;
}

// Note: we don't support command-line redirection etc. yet
// Note: strings (and their escapes) are not supported either!
Tokenizer::token_vec Tokenizer::tokenize(Log& log, const string& ast) {
  Token current;    // current chunk; becomes the next token
  Tokenizer::token_vec v;
  bool escape = false;    // only \ escape is supported
  bool inToken = false;
  bool inValue = false;
  for (int i=0; i < ast.length(); ++i) {
    char c = ast[i];
    //log.debug(" - tokenizing char '" + string(1, c) + "'");
    switch (mode) {
    case MODE_NONE:
      if (inToken || inValue) {
        throw EvalError("Unexpectedly inToken or inValue during NONE state");
      }
      if ('[' == c) {
        v.push_back(Token("["));
        mode = MODE_CMD;
      } else {
        throw EvalError("Bad character in AST input: '" + string(1, c) + "'");
      }
      break;
    case MODE_CMD:
      if ('[' == c) {
        if (inToken) {
          throw EvalError("Unexpected '[' within token of CMD mode");
        }
        v.push_back(Token("["));
      } else if (']' == c) {
        if (inToken) {
          v.push_back(current);
          current = Token();
          inToken = false;
          inValue = false;
        }
        v.push_back(Token("]"));
        mode = MODE_NONE;
      } else if ('{' == c) {
        if (inToken) {
          v.push_back(current);
          current = Token();
          inToken = false;
          inValue = false;
        }
        v.push_back(Token("{"));
        mode = MODE_CODE;
        ++codeDepth;
      } else if ('}' == c) {
        throw EvalError("Unexpected '}' within CMD mode");
      } else {
        if (!inToken) {
          inToken = true;
          current.name = "cmd";
        }
        inValue = true;
        current.value += c;
      }
      break;
    case MODE_CODE:
      if (escape && inValue) {
        current.value += c;
        escape = false;
      } else if ('\\' == c && inValue) {
        escape = true;
      } else if ('}' == c) {
        if (inToken) {
          v.push_back(current);
          current = Token();
          inToken = false;
          inValue = false;
        }
        v.push_back(Token("}"));
        --codeDepth;
        if (0 == codeDepth) {
          mode = MODE_CMD;
        } else if (codeDepth < 0) {
          throw EvalError("CODE mode observed codeDepth < 0");
        }
      } else if ('{' == c) {
        if (inToken) {
          v.push_back(current);
          current = Token();
          inToken = false;
          inValue = false;
        }
        v.push_back(Token("{"));
        ++codeDepth;
      } else if (':' == c) {
        if (!inToken) {
          throw EvalError("Found unexpected ':' while in CODE non-Token");
        } else if (inValue) {
          throw EvalError("Found unexpected ':' while in CODE Value");
        }
        inValue = true;
      } else if (' ' == c || ';' == c) {
        if (inToken) {
          v.push_back(current);
          current = Token();
          inToken = false;
          inValue = false;
        }
      // A Value must be between single-quotes after the : separator
      } else if ('\'' == c && inValue) {
        if (current.value != "") {
          v.push_back(current);
          current = Token();
          inToken = false;
          inValue = false;
        } else if (i >= 1 && ':' == ast[i-1]) {
          // start of actual value; skip
        } else if (i >= 2 && '\'' == ast[i-1] && ':' == ast[i-2]) {
          // empty value.  strange but ok.
          log.info("Found token with :'' empty value");
          v.push_back(current);
          current = Token();
          inToken = false;
          inValue = false;
        } else {
          throw EvalError("Unexpected single-quote in CODE Value");
        }
      } else if (!isalpha(c)) {
        if (inToken) {
          v.push_back(current);
          current = Token();
          inToken = false;
          inValue = false;
        }
        v.push_back(Token(string(1, c)));
      } else if (inValue) {
        current.value += c;
      } else {
        inToken = true;
        current.name += c;
      }
      break;
    default:
      throw EvalError("Internal error: unknown MODE detected");
    }
  }
  if (inToken) {
    throw EvalError("Line ended with incomplete token");
  }
  return v;
}
