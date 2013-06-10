#include "AST.h"
#include "Log.h"
#include "Token.h"

#include <ctype.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
using namespace eval;

namespace {
  const string PROGRAM_NAME = "lush_eval";
};

/* Horrible buggy incomplete insecure overly-restrictive locale-specific
 * tokenizer.  This is perhaps the worst code ever.  Don't even ask. */
vector<Token> tokenize(const string& line) {
  vector<Token> v;

  int i = 0;
  bool gotokvalue = false;
  bool intokvalue = false;
  bool escape = false;
  Token confirmed("");
  Token current("");
  while (i < line.length()) {
    char c = line[i];
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
        throw ASTError("Bad token value for '" + current.name + "'");
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
      throw ASTError("Bad character in AST input: '" + string(1, c) + "'");
    }
  }
  if (current.name.length() > 0) {
    v.push_back(current);
    current.name = "";
  }
  if (gotokvalue || intokvalue || escape) {
    throw ASTError("Incomplete token");
  }

  return v;
}

typedef vector<Token>::const_iterator c_vec_iter;

int main(int argc, char *argv[]) {
  if (argc < 1 || argc > 2) {
    cout << "usage: " << PROGRAM_NAME << " [log level]" << endl;
    return 1;
  }

  Log log;
  try {
    if (2 == argc) {
      log.setLevel(argv[1]);
    }

    AST ast(log);
    log.info("Initialized AST");

    string line;
    while (getline(cin, line)) {
      log.debug("Received input line: '" + line + "'");
      try {
        vector<Token> tokens = tokenize(line);
        for (c_vec_iter i = tokens.begin(); i != tokens.end(); ++i) {
          log.debug("Inserting token: '" + i->name + ":" + i->value + "'");
          ast.insert(*i);
        }
        log.info("Evaluating: '" + ast.print() + "'");
        ast.evaluate();
        cout << "Evaluated. " << ast.print() << endl;
      } catch (ASTError& e) {
        log.error(string("Error evaluating parse tree: ") + e.what());
        ast.reset();
      }
    }
  } catch (exception& e) {
    log.error(string("Unknown error: ") + e.what());
  } catch (...) {
    log.error("Unknown error");
  }

  return 0;
}
