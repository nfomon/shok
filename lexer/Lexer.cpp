// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Lexer.h"

#include "Tokenizer.h"

#include <boost/bind.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include <algorithm>
#include <istream>
#include <ostream>
#include <string>
#include <utility>

namespace spirit = boost::spirit;
namespace lex = spirit::lex;

using std::istream;
using std::ostream;
using std::string;
using std::endl;

using namespace lexer;

Lexer::Lexer(Log& log, istream& input, ostream& output)
  : m_log(log),
    m_input(input),
    m_output(output),
    m_linenum(1),
    m_charnum(1),
    m_emitter(output, m_linenum, m_charnum) {
}

bool Lexer::lex() {
  m_input >> std::noskipws;
  string sbuf;

  typedef lex::lexertl::token<const char*> token_type;
  typedef lex::lexertl::lexer<token_type> lexer_type;
  Tokenizer<lexer_type> tokenizer;

  while (m_input) {
    string line;
    getline(m_input, line);
    sbuf += line + "\n";

    const char* sbuf_first = sbuf.c_str();
    const char* sbuf_last = &sbuf_first[sbuf.size()];

    (void) lex::tokenize(sbuf_first, sbuf_last, tokenizer, boost::bind(m_emitter, _1));
    if (sbuf_first == sbuf_last) {
      sbuf.clear();
    } else {
      std::string rest(sbuf_first, sbuf_last);
      std::copy(rest.begin(), rest.end(), sbuf.begin());
    }
  }

  return sbuf.empty();
}
