// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Executor.h"

#include "Call.h"
#include "Cmd.h"
#include "Del.h"
#include "Expression.h"
#include "Instruction.h"
#include "New.h"
#include "StdLib.h"
#include "VMError.h"

#include "util/Util.h"

#include <boost/bind.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/variant.hpp>

namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace ascii = spirit::ascii;
namespace qi = spirit::qi;

#include <istream>
#include <string>
#include <utility>
#include <vector>
using std::istream;
using std::string;
using std::vector;

using namespace vm;

Executor::Executor(Log& log, istream& input)
  : m_log(log),
    m_input(input) {
  StdLib::Initialize(m_context.globals());
}

BOOST_FUSION_ADAPT_STRUCT(
  Cmd,
  (std::vector<Expression>, exps)
  (std::string, cmd)
)

BOOST_FUSION_ADAPT_STRUCT(
  New,
  (std::string, name)
  (Expression, exp)
)

BOOST_FUSION_ADAPT_STRUCT(
  Del,
  (std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
  Call,
  (Expression, function)
  (std::vector<Expression>, args)
)

BOOST_FUSION_ADAPT_STRUCT(
  MethodCall,
  (Expression, source)
  (std::string, method)
  (std::vector<Expression>, args)
)

BOOST_FUSION_ADAPT_STRUCT(
  ObjectLiteral,
  (ObjectLiteral::member_vec, members)
)

bool Executor::execute() {
  // iterate over stream input
  typedef std::istreambuf_iterator<char> base_iterator_type;
  base_iterator_type in_begin(m_input);

  // convert input iterator to forward iterator, usable by spirit parser
  typedef spirit::multi_pass<base_iterator_type> forward_iterator_type;
  forward_iterator_type fwd_begin = spirit::make_default_multi_pass(in_begin);
  forward_iterator_type fwd_end;

  typedef qi::rule<forward_iterator_type, ascii::space_type> Rule;
  CmdParser<forward_iterator_type> cmd_;
  InstructionParser<forward_iterator_type> instruction_;
  Exec_Cmd exec_Cmd(m_context);
  Exec_Instruction exec_Instruction(m_context);

  Rule bytecode_ = *(
    cmd_[exec_Cmd]
    | instruction_[exec_Instruction]
  );

  //BOOST_SPIRIT_DEBUG_NODE(bytecode_);

  bool r = qi::phrase_parse(
    fwd_begin, fwd_end,
    bytecode_,
    ascii::space
  );

  if (!r || fwd_begin != fwd_end) {
    return false;
  }
  return r;
}
