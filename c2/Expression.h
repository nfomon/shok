// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Expression_h_
#define _Expression_h_

/* Expression */

#include "Operator.h"
#include "Type.h"

#include "util/Log.h"

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
#include <vector>

// debug
#include <iostream>
using std::cout;
using std::endl;

namespace compiler {

class Expression {
public:
  Expression();
  void attach_atom(const std::string& atom);
  void attach_preop(const std::string& preop);
  void attach_binop(const std::string& binop);
  void finalize();
  std::string bytecode() const;

private:
  typedef std::vector<Operator*> stack_vec;
  typedef stack_vec::const_iterator stack_iter;

  bool m_infixing;
  stack_vec m_stack;
};

template <typename Iterator>
struct ExpParser : qi::grammar<Iterator, Expression(), ascii::space_type> {
public:
  ExpParser(Log& log)
    : ExpParser::base_type(exp_, "expression parser"),
      m_log(log) {
    using phoenix::ref;
    using phoenix::val;
    using qi::_val;
    using qi::alnum;
    using qi::char_;
    using qi::lexeme;
    using qi::lit;
    using qi::print;

    identifier_.name("identifier");
    variable_.name("variable");
    atoms_.name("atoms");
    atom_.name("atom");
    preops_.name("prefix operators");
    preop_.name("prefix operator");
    binops_.name("binary operators");
    binop_.name("binary operator");
    exp_.name("expression");

    identifier_ %= lit("ID:") > +(alnum | '_') > -(lit(":") > +(alnum | '_'));
    variable_ %= lit("(var") >> identifier_ >> ")";
    atoms_ %= variable_;
    atom_ %= atoms_[phoenix::bind(&Expression::attach_atom, &m_exp, qi::_1)];
    preops_ %= lit("PLUS") | lit("MINUS");
    preop_ %= preops_[phoenix::bind(&Expression::attach_preop, &m_exp, qi::_1)];
    binops_ %= lit("PLUS") | lit("MINUS") | lit("STAR") | lit("DIV");
    binop_ %= binops_[phoenix::bind(&Expression::attach_binop, &m_exp, qi::_1)];

    exp_ = (
      lit("(exp ")
      >> -preop_
      >> atom_
      >> *(binop_ > -preop_ > atom_)
      >> lit(")")[phoenix::bind(&Expression::finalize, &m_exp)]
    )[_val = ref(m_exp)];
  }

private:
  Log& m_log;
  Expression m_exp;

  qi::rule<Iterator, std::string(), ascii::space_type> identifier_;
  qi::rule<Iterator, std::string(), ascii::space_type> variable_;
  qi::rule<Iterator, std::string(), ascii::space_type> atoms_;
  qi::rule<Iterator, std::string(), ascii::space_type> atom_;
  qi::rule<Iterator, std::string(), ascii::space_type> preops_;
  qi::rule<Iterator, std::string(), ascii::space_type> preop_;
  qi::rule<Iterator, std::string(), ascii::space_type> binops_;
  qi::rule<Iterator, std::string(), ascii::space_type> binop_;
  qi::rule<Iterator, Expression(), ascii::space_type> exp_;
};

}

#endif // _Expression_h_
