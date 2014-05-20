// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _InstructionParser_h_
#define _InstructionParser_h_

/* Instruction parser */

#include "Call.h"
#include "Context.h"
#include "Del.h"
#include "Instruction.h"
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

template <typename Iterator> struct ExpParser;

template <typename Iterator>
struct InstructionParser
  : qi::grammar<Iterator, Instruction(), ascii::space_type> {
public:
  InstructionParser(ExpParser<Iterator>& exp_)
    : InstructionParser::base_type(instruction_, "Instruction"),
      exp_(exp_),
      new_(exp_),
      call_(exp_) {

    instruction_ %=
      new_
      | del_
      | call_
    ;

    //BOOST_SPIRIT_DEBUG_NODE(instruction_);
  }

private:
  ExpParser<Iterator>& exp_;

  NewParser<Iterator> new_;
  DelParser<Iterator> del_;
  CallParser<Iterator> call_;
  qi::rule<Iterator, Instruction(), ascii::space_type> instruction_;
};

}

#endif // _InstructionParser_h_
