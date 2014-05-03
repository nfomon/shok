// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Expression_h_
#define _Expression_h_

/* Expression */

#include "Operator.h"
#include "Scope.h"
#include "Type.h"
#include "Variable.h"

#include <boost/fusion/include/std_pair.hpp>
#include <boost/optional.hpp>
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

#include <string>

namespace compiler {

class Expression {
public:
  Expression();
  void init(Scope& scope);
  void attach_atom(const Variable& atom);   // TODO variant of other atoms
  void attach_preop(const std::string& preop);
  void attach_binop(const std::string& binop);
  void finalize();
  std::string bytecode() const;
  const Type& type() const;

private:
  typedef boost::ptr_vector<OperatorNode> stack_vec;
  typedef stack_vec::const_iterator stack_iter;

  Scope* m_scope;
  stack_vec m_stack;
  boost::shared_ptr<Type> m_type;
  std::string m_bytecode;
};

template <typename Iterator>
struct ExpParser : qi::grammar<Iterator, Expression(Scope&), ascii::space_type> {
public:
  ExpParser(bool isTypeSpec = false)
    : ExpParser::base_type(exp_, std::string(isTypeSpec ? "typespec" : "expression") + " parser") {
    using phoenix::ref;
    using phoenix::val;
    using qi::_val;
    using qi::alnum;
    using qi::char_;
    using qi::lexeme;
    using qi::lit;
    using qi::omit;
    using qi::print;

    atom_.name("atom");
    preops_.name("prefix operators");
    preop_.name("prefix operator");
    binops_.name("binary operators");
    binop_.name("binary operator");
    exp_.name("expression");

    atom_ = variable_(qi::_r1)[phoenix::bind(&Expression::attach_atom, &m_exp, qi::_1)];
    preops_ %= lit("PLUS") | lit("MINUS");
    preop_ %= preops_[phoenix::bind(&Expression::attach_preop, &m_exp, qi::_1)];
    binops_ %= lit("PLUS") | lit("MINUS") | lit("STAR") | lit("DIV");
    binop_ %= binops_[phoenix::bind(&Expression::attach_binop, &m_exp, qi::_1)];

    exp_ = (
      lit("(" + std::string(isTypeSpec ? "type" : "exp"))[phoenix::bind(&Expression::init, &m_exp, qi::_r1)]
      >> -preop_
      >> atom_(qi::_r1)
      >> *(binop_ > -preop_ > atom_(qi::_r1))
      >> lit(")")[phoenix::bind(&Expression::finalize, &m_exp)]
    )[_val = ref(m_exp)];
  }

private:
  Expression m_exp;

  VariableParser<Iterator> variable_;
  qi::rule<Iterator, spirit::unused_type(Scope&), ascii::space_type> atom_;
  qi::rule<Iterator, std::string(), ascii::space_type> preops_;
  qi::rule<Iterator, std::string(), ascii::space_type> preop_;
  qi::rule<Iterator, std::string(), ascii::space_type> binops_;
  qi::rule<Iterator, std::string(), ascii::space_type> binop_;
  qi::rule<Iterator, Expression(Scope&), ascii::space_type> exp_;
};

}

#endif // _Expression_h_
