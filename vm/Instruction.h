// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Instruction_h_
#define _Instruction_h_

/* Executable instructions */

#include "Context.h"

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

struct Call;
struct Cmd;
struct Del;
struct New;

typedef boost::variant<
  boost::recursive_wrapper<Cmd>,
  boost::recursive_wrapper<New>,
  boost::recursive_wrapper<Del>,
  boost::recursive_wrapper<Call>
> Instruction;

struct Exec_Instruction : public boost::static_visitor<> {
public:
  Exec_Instruction(Context& context)
    : m_context(context) {}

  void operator() (const Instruction& instruction, qi::unused_type, qi::unused_type) const {
    boost::apply_visitor(*this, instruction);
  }
  void operator() (const Cmd& cmd) const;
  void operator() (const New& n) const;
  void operator() (const Del& del) const;
  void operator() (const Call& call) const;

private:
  Context& m_context;
};

}

#endif // _Instruction_h_
