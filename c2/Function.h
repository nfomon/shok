// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Function_h_
#define _Function_h_

/* Functions */

#include "Common.h"
#include "Code.h"
//#include "Expression.h"
#include "Scope.h"
#include "Type.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace ascii = spirit::ascii;
namespace qi = spirit::qi;

#include <memory>
#include <string>
#include <vector>

namespace compiler {

class Expression;
class FunctionScope;
class Scope;

typedef Expression Arg;
typedef Expression Returns;

class Function {
public:
  void init(const Scope& parentScope);
  void attach_arg(const Arg& arg);
  void attach_returns(const Returns& returns);
  Scope& scope() const;
  const Type& type() const { return *m_type; }
  std::string bytecode() const;

private:
  boost::shared_ptr<FunctionScope> m_scope;
  boost::shared_ptr<Type> m_type;
  std::string m_bytecode;
};

template <typename Iterator>
struct NewInitParser;

template <typename Iterator>
struct FunctionParser
  : qi::grammar<Iterator, Function(Scope&), ascii::space_type> {
public:
  FunctionParser(ExpParser<Iterator>& exp_)
    : FunctionParser::base_type(function_, "function parser"),
      exp_(exp_),
      body_(exp_) {
    using phoenix::ref;
    using qi::_val;
    using qi::_1;
    using qi::_a;
    using qi::_r1;
    using qi::alnum;
    using qi::lit;

    exp_.name("expression");
    body_.name("body");
    arg_.name("arg");
    returns_.name("returns");
    function_.name("function");

/*
    // Note: provide arg_ the Function's scope for maximal awesome
    arg_ %= (
      lit("(arg")
      > exp_(_r1, true)
      > lit(')')
    );
*/

    function_ = (
      lit("(func")[phoenix::bind(&Function::init, _val, _r1)]
/*
      > -(
        lit("(args")
        > +arg_(_r1, _val)[phoenix::bind(&Function::attach_arg, _val, _1)]
        > lit(')')
      )
      > -(
        lit("(returns")
        > +returns_(_r1, _val)[phoenix::bind(&Function::attach_returns, _val, _1)]
        > lit(')')
      )
      //> body_(phoenix::bind(&Function::scope, _val))
*/
      > lit(')')
    );
  }

private:
  ExpParser<Iterator>& exp_;

  CodeParser<Iterator> body_;
  qi::rule<Iterator, void(Scope&), ascii::space_type> arg_;
  qi::rule<Iterator, void(Scope&), ascii::space_type> returns_;
  qi::rule<Iterator, Function(Scope&), ascii::space_type> function_;
};

}

#endif // _Function_h_
