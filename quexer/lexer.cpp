// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "tiny_lexer_st"

#include <fstream>
#include <iostream>
#include <string>
using namespace std;

namespace {
  const string PROGRAM_NAME = "shok_lexer";
}

int main(int argc, char* argv[]) {
  if (argc != 1) {
    cout << "usage: " << PROGRAM_NAME << endl;
    return 1;
  }

  quex::Token token;
  quex::tiny_lexer_st qlex((QUEX_TYPE_CHARACTER*)0x0, 0);

  int line_number = 0;
  int num_tokens = 0;
  qlex.token_p_switch(&token);
  while (cin) {
    qlex.buffer_fill_region_prepare();

    cin.getline((char*)qlex.buffer_fill_region_begin(),
                qlex.buffer_fill_region_size());
    if(cin.gcount() == 0) {
      return 0;
    }
    ++line_number;

    qlex.buffer_fill_region_finish(cin.gcount()-1);

    qlex.receive();
    cout << line_number;
    while (token.type_id() != QUEX_TKN_TERMINATION && token.type_id() != QUEX_TKN_EXIT) {
      // serialize token
      cout << " " << token.column_number() << ":" << token.type_id_name();
      if (token.get_text().length() > 0) {
        // escape '\' and '\'' in the value string with '\'
        //string tvalue = token.get_text();
        //size_t i = tvalue.find('\\'
        cout << ":'" << token.get_text().c_str() << "'";
      }

      ++num_tokens;
      qlex.receive();
    }
    cout << endl;

    if (QUEX_TKN_EXIT == token.type_id()) break;
  }

  //cout << "Processed " << num_tokens << " tokens." << endl;
  return 0;
}
