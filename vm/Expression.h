// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Expression_h_
#define _Expression_h_

/* A Expression instruction: defines a symbol (name, type, value) */

#include "Object.h"
#include "Object.h"
#include "VMError.h"

#include "util/Log.h"

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
//struct FunctionLiteral;

typedef boost::variant<
    std::string,
    boost::recursive_wrapper<MethodCall>,
    boost::recursive_wrapper<ObjectLiteral>
    //boost::recursive_wrapper<FunctionLiteral>
  > Expression;

struct MethodCall {
  typedef std::vector<Expression> args_vec;
  typedef args_vec::const_iterator args_iter;

  Expression source;
  std::string method;
  args_vec args;
};

typedef std::pair<std::string,Expression> member_pair;

struct ObjectLiteral {
  typedef std::pair<std::string,Expression> member_pair;
  typedef std::vector<member_pair> member_vec;
  typedef member_vec::const_iterator member_iter;

  member_vec members;
};

/*
struct FunctionLiteral {
};
*/

class Exec_Exp : public boost::static_visitor<std::auto_ptr<Object> > {
public:
  Exec_Exp(const symbol_map& symbols)
    : m_symbols(symbols) {}

  std::auto_ptr<Object> operator() (const std::string& var) const;
  std::auto_ptr<Object> operator() (const MethodCall& methodCall) const;
  std::auto_ptr<Object> operator() (const ObjectLiteral& object) const;
  //std::auto_ptr<Object> operator() (const FunctionLiteral& function) const;

private:
  const symbol_map& m_symbols;
};

template <typename Iterator>
struct ExpParser : qi::grammar<Iterator, Expression(), ascii::space_type> {
public:
  ExpParser() : ExpParser::base_type(exp_, "expression parser") {
    using qi::char_;
    using qi::lexeme;
    using qi::lit;
    using qi::graph;

    identifier_ %= lexeme[ char_("A-Za-z_") > *char_("0-9A-Za-z_:") ];
    methodcall_ = lit("(call") > exp_ > identifier_ > *exp_ > lit(')');
    member_ = lit("(member") > identifier_ > identifier_ > lit(')');
    object_ = lit("(object") > *member_ > lit(')');
    //function_ = lit("(function") > +statement_ > lit(')');  // TODO
    exp_ %= (
      identifier_
      | methodcall_
      | object_
      //| function_
    );
  }

private:
  qi::rule<Iterator, std::string(), ascii::space_type> identifier_;
  qi::rule<Iterator, MethodCall(), ascii::space_type> methodcall_;
  qi::rule<Iterator, ObjectLiteral::member_pair(), ascii::space_type> member_;
  qi::rule<Iterator, ObjectLiteral(), ascii::space_type> object_;
  qi::rule<Iterator, Expression(), ascii::space_type> exp_;
};

}

#endif // _Expression_h_
