#include <fstream>
#include <iostream>

#include "tiny_lexer_st"

int main(int argc, char* argv[]) {
  using namespace std;

  if (argc != 1) {
    cout << "usage: " << argv[0] << endl;
    return 1;
  }

  quex::Token token;
  // Zero pointer to constructor --> use raw memory
  quex::tiny_lexer_st  qlex((QUEX_TYPE_CHARACTER*)0x0, 0);

  int line_number = 0;
  int number_of_tokens = 0;
  (void)qlex.token_p_switch(&token);
  while(cin) {
    qlex.buffer_fill_region_prepare();

    // Read a line from standard input
    cin.getline((char*)qlex.buffer_fill_region_begin(),
                qlex.buffer_fill_region_size());
    if(cin.gcount() == 0) {
      return 0;
    }
    ++line_number;

    // Inform about number of read characters. Note, that getline
    // writes a terminating zero, which has not to be part of the
    // buffer content.
    qlex.buffer_fill_region_finish(cin.gcount() - 1);

    // Loop until the 'termination' token arrives
    bool first = true;
    (void)qlex.receive();
    while (token.type_id() != QUEX_TKN_TERMINATION && token.type_id() != QUEX_TKN_EXIT) {
      // serialize token
      if (first) {
        first = false;
        cout << line_number;
      }
      cout << " " << token.column_number() << ":" << token.type_id_name();
      if (token.get_text().length() > 0) {
        cout << ":'" << token.get_text().c_str() << "'";
      }

      ++number_of_tokens;
      (void)qlex.receive();
    }
    if (!first) {
      cout << endl;
    }

    if(token.type_id() == QUEX_TKN_EXIT) break;
  }

  cout << "Processed " << number_of_tokens << " tokens." << endl;
  return 0;
}
