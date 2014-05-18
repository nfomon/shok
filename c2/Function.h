// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Function_h_
#define _Function_h_

/* Functions */

#include "Common.h"
#include "Code.h"
#include "Scope.h"
#include "Type.h"

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
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

namespace compiler {

class Expression;
class FunctionScope;
class Scope;

class Function {
public:
  void init(const Scope& parentScope);
  void init_args();
  void attach_arg(const std::string& name, const Expression& exp);
  void attach_returns(const Expression& returns);
  void attach_body(const std::string& code);
  FunctionScope& scope() const;
  const Type& type() const;
  std::string bytecode() const;

private:
  typedef boost::ptr_vector<Type> arg_vec;
  typedef arg_vec::const_iterator arg_iter;

  boost::shared_ptr<FunctionScope> m_scope;
  const Type* m_froot;
  boost::shared_ptr<Type> m_type;
  std::string m_bytecode;
};

template <typename Iterator>
struct FunctionParser
  : qi::grammar<Iterator, Function(const Scope&), ascii::space_type> {
public:
  FunctionParser(ExpParser<Iterator>& exp_)
    : FunctionParser::base_type(function_, "Function"),
      exp_(exp_),
      body_(exp_) {
    using phoenix::ref;
    using qi::_val;
    using qi::_1;
    using qi::_a;
    using qi::_r1;
    using qi::char_;
    using qi::lit;

    identifier_ %= lit("ID:'") > +char_("0-9A-Za-z_") > lit('\'');

    arg_ %= (
      lit("(arg")
      > -identifier_[_a = _1]
      > exp_(phoenix::bind(&Function::scope, _r1), std::string("type"))[phoenix::bind(&Function::attach_arg, _r1, _a, _1)]
      > lit(')')
    );

    args_ %= (
      lit("(args")[phoenix::bind(&Function::init_args, _r1)]
      > *arg_(_r1)
      > lit(')')
    );

    returns_ %= (
      lit("(returns")
      > exp_(phoenix::bind(&Function::scope, _r1), std::string("type"))[phoenix::bind(&Function::attach_returns, _r1, _1)]
      > lit(')')
    );

    function_ = (
      lit("(func")[phoenix::bind(&Function::init, _val, _r1)]
      > -args_(_val)
      > -returns_(_val)
      > body_(phoenix::bind(&Function::scope, _val))[phoenix::bind(&Function::attach_body, _val, _1)]
      > lit(')')
    );

    //BOOST_SPIRIT_DEBUG_NODE(arg_);
    //BOOST_SPIRIT_DEBUG_NODE(args_);
    //BOOST_SPIRIT_DEBUG_NODE(returns_);
    //BOOST_SPIRIT_DEBUG_NODE(function_);
  }

private:
  ExpParser<Iterator>& exp_;

  CodeParser<Iterator> body_;
  qi::rule<Iterator, std::string(), ascii::space_type> identifier_;
  qi::rule<Iterator, void(Function&), qi::locals<std::string>, ascii::space_type> arg_;
  qi::rule<Iterator, void(Function&), ascii::space_type> args_;
  qi::rule<Iterator, void(Function&), ascii::space_type> returns_;
  qi::rule<Iterator, Function(const Scope&), ascii::space_type> function_;
};

}

#endif // _Function_h_
