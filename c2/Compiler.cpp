// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Compiler.h"

#include "Cmd.h"
#include "Code.h"
#include "CompileError.h"
#include "Scope.h"

#include "util/Util.h"

#include <boost/bind.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <iostream>
#include <istream>
#include <string>
#include <utility>
#include <vector>
using std::cout;
using std::endl;
using std::istream;
using std::string;
using std::vector;

using namespace compiler;

Compiler::Compiler(istream& input)
  : m_input(input),
    on_cmdline(boost::bind(&Compiler::emit, this, _1)) {
}

void Compiler::emit(const std::string& bytecode) {
  cout << "bytecode:'" << bytecode << "'" << endl;
}

bool Compiler::execute() {
  using qi::lit;
  using qi::omit;

  // iterate over stream input
  typedef std::istreambuf_iterator<char> base_iterator_type;
  base_iterator_type in_begin(m_input);

  // convert input iterator to forward iterator, usable by spirit parser
  typedef spirit::multi_pass<base_iterator_type> forward_iterator_type;
  forward_iterator_type fwd_begin = spirit::make_default_multi_pass(in_begin);
  forward_iterator_type fwd_end;

  Scope globalScope;

  CmdParser<forward_iterator_type> cmd_;
  CodeParser<forward_iterator_type> code_(globalScope);

  typedef qi::rule<forward_iterator_type, std::string(), ascii::space_type> StringRule;
  typedef qi::rule<forward_iterator_type, ascii::space_type> VoidRule;

  StringRule cmdline_ = lit('[') > (cmd_ | code_) > lit(']');
  VoidRule program_ = +(cmdline_[on_cmdline]);

  bool r = qi::phrase_parse(
    fwd_begin, fwd_end,
    program_,
    ascii::space
  );

  if (!r || fwd_begin != fwd_end) {
    return false;
  }
  return r;
}
