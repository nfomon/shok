// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Expression_h_
#define _Expression_h_

/* An Expression can be executed to retrieve an Object */

#include "Context.h"
#include "Instruction.h"
#include "Object.h"
#include "Symbol.h"
#include "VMError.h"

#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/variant.hpp>
namespace fusion = boost::fusion;
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <string>
#include <vector>

namespace vm {

struct MethodCall;
struct ObjectLiteral;
struct FunctionLiteral;

typedef std::string Variable;
struct IntLiteral {
  std::string num;
};
struct StringLiteral {
  std::string str;
};

typedef boost::variant<
    Variable,
    IntLiteral,
    StringLiteral,
    boost::recursive_wrapper<MethodCall>,
    boost::recursive_wrapper<ObjectLiteral>,
    boost::recursive_wrapper<FunctionLiteral>
  > Expression;

typedef std::vector<Expression> argexps_vec;
typedef argexps_vec::const_iterator argexps_iter;

struct MethodCall {
  Expression source;
  std::string method;
  argexps_vec argexps;
};

typedef std::pair<std::string,Expression> member_pair;

struct ObjectLiteral {
  typedef std::pair<std::string,Expression> member_pair;
  typedef std::vector<member_pair> member_vec;
  typedef member_vec::const_iterator member_iter;

  member_vec members;
};

struct FunctionLiteral {
  std::vector<Instruction> body;
};

class Exec_Exp : public boost::static_visitor<std::auto_ptr<Object> > {
public:
  Exec_Exp(Context& context);

  std::auto_ptr<Object> operator() (const Variable& var);
  std::auto_ptr<Object> operator() (const IntLiteral& lit);
  std::auto_ptr<Object> operator() (const StringLiteral& lit);
  std::auto_ptr<Object> operator() (const MethodCall& methodCall);
  std::auto_ptr<Object> operator() (const ObjectLiteral& object);
  std::auto_ptr<Object> operator() (const FunctionLiteral& function);

private:
  Context& m_context;
  const Object* const m_rootObject;
  const Object* const m_rootFunction;
};

}

#endif // _Expression_h_
