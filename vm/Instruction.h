// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Instruction_h_
#define _Instruction_h_

/* Executable instructions */

#include "Call.h"
#include "Context.h"
#include "Del.h"
#include "New.h"

#include "util/Log.h"

#include <boost/utility.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <string>
#include <vector>

namespace vm {

typedef boost::variant<
  New,
  Del,
  Call
> Instruction;

struct Exec_Instruction : public boost::static_visitor<> {
public:
  Exec_Instruction(Context& context)
    : m_context(context) {}

  void operator() (const Instruction& instruction, qi::unused_type, qi::unused_type) const {
    boost::apply_visitor(*this, instruction);
  }
  void operator() (const New& n) const;
  void operator() (const Del& del) const;
  void operator() (const Call& call) const;

private:
  Context& m_context;
};

template <typename Iterator>
struct InstructionParser : qi::grammar<Iterator, Instruction(), ascii::space_type> {
public:
  InstructionParser() : InstructionParser::base_type(instruction_, "Instruction") {

    instruction_ %=
      new_
      | del_
      | call_
    ;

    //BOOST_SPIRIT_DEBUG_NODE(instruction_);
  }

private:
  NewParser<Iterator> new_;
  DelParser<Iterator> del_;
  CallParser<Iterator> call_;
  qi::rule<Iterator, Instruction(), ascii::space_type> instruction_;
};

}

#endif // _Instruction_h_
