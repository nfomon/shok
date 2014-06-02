// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Compiler.h"

#include "Cmd.h"
#include "Code.h"
#include "CompileError.h"
#include "ExpParser.h"
#include "Scope.h"
#include "StdLib.h"

#include <boost/bind.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <istream>
#include <ostream>
#include <string>
using std::endl;
using std::istream;
using std::ostream;
using std::string;

using namespace compiler;

Compiler::Compiler(istream& input, ostream& output)
  : m_input(input),
    m_output(output) {
}

bool Compiler::execute() {
  using phoenix::ref;
  using qi::lit;

  // iterate over stream input
  typedef std::istreambuf_iterator<char> base_iterator_type;
  base_iterator_type in_begin(m_input);

  // convert input iterator to forward iterator, usable by spirit parser
  typedef spirit::multi_pass<base_iterator_type> forward_iterator_type;
  forward_iterator_type fwd_begin = spirit::make_default_multi_pass(in_begin);
  forward_iterator_type fwd_end;

  // Initialize global scope
  Scope globalScope;
  StdLib::Init(globalScope);

  // Parsers
  ExpParser<forward_iterator_type> exp_;
  CmdParser<forward_iterator_type> cmd_(exp_, globalScope);
  CodeParser<forward_iterator_type> code_(exp_);

  typedef qi::rule<forward_iterator_type, std::string(), ascii::space_type> StringRule;
  typedef qi::rule<forward_iterator_type, ascii::space_type> VoidRule;
  StringRule cmdline_ =
    lit('[')
    > (cmd_ | code_(ref(globalScope)))
    > lit(']')
  ;
  VoidRule program_ = +(
    cmdline_[ref(m_output) << qi::_1]
    > -qi::no_skip[ lit("\n")[ref(m_output) << endl] ]
  );

  //BOOST_SPIRIT_DEBUG_NODE(cmdline_);
  //BOOST_SPIRIT_DEBUG_NODE(program_);

  bool r = qi::phrase_parse(
    fwd_begin, fwd_end,
    program_,
    ascii::space
  );

  if (!r || fwd_begin != fwd_end) {
    return false;
  }

  // Cleanup the global scope :)
  m_output << globalScope.bytecode() << endl;

  return r;
}
