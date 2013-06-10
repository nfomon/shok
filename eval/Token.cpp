#include "Token.h"

#include "EvalError.h"

#include <ctype.h>
#include <string>
using namespace std;

namespace eval {

/* Horrible buggy incomplete insecure overly-restrictive locale-specific
 * tokenizer.  This is perhaps the worst code ever.  Don't even ask. */
Tokenizer::token_vec Tokenizer::tokenize(const string& ast) {
  Tokenizer::token_vec v;

  int i = 0;
  bool gotokvalue = false;
  bool intokvalue = false;
  bool escape = false;
  Token confirmed("");
  Token current("");
  while (i < ast.length()) {
    char c = ast[i];
    bool intok = current.name.length() > 0;
    if (intok) {
      if (':' == c) {
        gotokvalue = true;
        confirmed.name = current.name;
        current.name = "";
        ++i;
        continue;
      } else if (isalpha((int)c)) {
        current.name += c;
        ++i;
        continue;
      } else {
        // no token value; fall through
        confirmed.name = current.name;
        v.push_back(confirmed);
        confirmed.name = "";
        current.name = "";
      }
    } else if (gotokvalue) {
      if (c == '\'') {
        gotokvalue = false;
        intokvalue = true;
        ++i;
        continue;
      } else {
        throw EvalError("Bad token value for '" + current.name + "'");
      }
    } else if (intokvalue) {
      if (escape) {
        escape = false;
        current.value += c;
      } else if ('\\' == c) {
        escape = true;
      } else if ('\'' == c) {
        intokvalue = false;
        confirmed.value = current.value;
        current.name = "";
        current.value = "";   // paranoid
        v.push_back(confirmed);
        confirmed.name = "";
        confirmed.value = "";
      } else {
        current.value += c;
      }
      ++i;
      continue;
    }

    if (' ' == c ||
        ';' == c) {   // it turns out these are useless. lol
      ++i;
    } else if ('{' == c ||
               '}' == c ||
               '(' == c ||
               ')' == c ||
               '=' == c ||
               ',' == c) {
      v.push_back(Token(string(1, c)));
      ++i;
    } else if (isalpha((int)c)) {
      current.name += c;
      ++i;
    } else {
      throw EvalError("Bad character in AST input: '" + string(1, c) + "'");
    }
  }
  if (current.name.length() > 0) {
    v.push_back(current);
    current.name = "";
  }
  if (gotokvalue || intokvalue || escape) {
    throw EvalError("Incomplete token");
  }

  return v;
}

};
