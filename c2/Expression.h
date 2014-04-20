// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Expression_h_
#define _Expression_h_

/* Expression */

#include "util/Log.h"

#include <boost/bind.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/qi.hpp>
namespace spirit = boost::spirit;
namespace phoenix = boost::phoenix;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <string>
#include <vector>

// debug
#include <iostream>
using std::cout;
using std::endl;

namespace compiler {

struct Expression {
public:
  Expression(Log& log)
    : m_log(log) {}

  void init();
  void attach_atom(const std::string&);
  void attach_preop(const std::string&);
  void attach_binop(const std::string&);
  void end();

private:
  Log& m_log;
};

template <typename Iterator>
struct ExpParser : qi::grammar<Iterator, std::string(), ascii::space_type> {
private:
  Log& m_log;
  Expression m_exp;
  boost::function<void (boost::spirit::qi::unused_type, boost::spirit::qi::unused_type, boost::spirit::qi::unused_type)> init;
  boost::function<void (const std::string&, boost::spirit::qi::unused_type, boost::spirit::qi::unused_type)> attach_atom;
  boost::function<void (const std::string&, boost::spirit::qi::unused_type, boost::spirit::qi::unused_type)> attach_preop;
  boost::function<void (const std::string&, boost::spirit::qi::unused_type, boost::spirit::qi::unused_type)> attach_binop;
  boost::function<void (boost::spirit::qi::unused_type, boost::spirit::qi::unused_type, boost::spirit::qi::unused_type)> end;

public:
  ExpParser(Log& log)
    : ExpParser::base_type(exp_, "expression"),
      m_log(log),
      m_exp(log),
      init(boost::bind(&Expression::init, &m_exp)),
      attach_atom(boost::bind(&Expression::attach_atom, &m_exp, _1)),
      attach_preop(boost::bind(&Expression::attach_preop, &m_exp, _1)),
      attach_binop(boost::bind(&Expression::attach_binop, &m_exp, _1)),
      end(boost::bind(&Expression::end, &m_exp)) {
    using qi::alnum;
    using qi::char_;
    using qi::lexeme;
    using qi::lit;
    using qi::print;

    identifier_.name("identifier");
    variable_.name("variable");
    atom_.name("atom");
    preop_.name("prefix operator");
    binop_.name("binary operator");
    exp_.name("expression");

    identifier_ %= lit("ID:") > +(alnum | '_') > -(lit(":") > +(alnum | '_'));
    variable_ %= lit("(var ") >> identifier_ >> ")";
    atom_ %= variable_;
    preop_ %= lit("PLUS") | lit("MINUS");
    binop_ %= lit("PLUS") | lit("MINUS") | lit("STAR") | lit("DIV");

    exp_ %=
      lit("(exp ")[init]
      >> -preop_[attach_preop]
      >> atom_[attach_atom]
      >> *(binop_[attach_binop] > -preop_[attach_preop] > atom_[attach_atom])
      >> lit(")")[end];

/*
    qi::on_error<qi::fail>(
      exp_,
      cout << "Failed to compile expression" << endl);
*/
  }

  qi::rule<Iterator, std::string(), ascii::space_type> identifier_;
  qi::rule<Iterator, std::string(), ascii::space_type> variable_;
  qi::rule<Iterator, std::string(), ascii::space_type> atom_;
  qi::rule<Iterator, std::string(), ascii::space_type> preop_;
  qi::rule<Iterator, std::string(), ascii::space_type> binop_;
  qi::rule<Iterator, std::string(), ascii::space_type> exp_;
};

}

#endif // _Expression_h_
