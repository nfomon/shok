// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Compiler.h"

//#include "AST.h"
#include "CmdLine.h"
#include "CompileError.h"

#include "util/Util.h"

#include <boost/bind.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <istream>
#include <string>
#include <utility>
#include <vector>
using std::istream;
using std::string;
using std::vector;

// debug
#include <iostream>
using std::cout;
using std::endl;

using namespace compiler;

Compiler::Compiler(Log& log, istream& input)
  : m_log(log),
    m_input(input),
    on_cmdline(boost::bind(&Compiler::exec_cmdline, this, _1)) {
}

void Compiler::exec_cmdline(const std::string& cmdline) {
  cout << "RUN CMD:" << cmdline << endl;
}

bool Compiler::execute() {
  // iterate over stream input
  typedef std::istreambuf_iterator<char> base_iterator_type;
  base_iterator_type in_begin(m_input);

  // convert input iterator to forward iterator, usable by spirit parser
  typedef spirit::multi_pass<base_iterator_type> forward_iterator_type;
  forward_iterator_type fwd_begin = spirit::make_default_multi_pass(in_begin);
  forward_iterator_type fwd_end;

  CmdLineParser<forward_iterator_type> cmdline_(m_log);
  typedef qi::rule<forward_iterator_type, ascii::space_type> Rule;

  Rule program_ = +(cmdline_[on_cmdline]);

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
