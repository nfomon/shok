// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _NewInit_h_
#define _NewInit_h_

/* NewInit */

#include "Expression.h"

#include "Scope.h"

#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/qi.hpp>
namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace ascii = spirit::ascii;
namespace qi = spirit::qi;

#include <memory>
#include <string>
#include <vector>

namespace compiler {

class NewInit {
public:
  NewInit();
  void init(Scope& scope);
  void attach_name(const std::string& name);
  void attach_type(const std::string& type);
  void attach_exp(const std::string& exp);
  void finalize();
  std::string bytecode() const;

private:
  Scope* m_scope;
  std::string m_name;
  boost::optional<Type> m_type;
  boost::optional<Expression> m_exp;
};

template <typename Iterator>
struct NewInitParser : qi::grammar<Iterator, std::string(Scope&), ascii::space_type> {
public:
  NewInitParser()
    : NewInitParser::base_type(newinit_, "newinit parser") {
    using phoenix::ref;
    using phoenix::val;
    using qi::_val;
    using qi::alnum;
    using qi::char_;
    using qi::lexeme;
    using qi::lit;
    using qi::omit;
    using qi::print;

    newinit_.name("newinit");

    identifier_ %= lit("ID:") > +(alnum | '_') > -(lit(":") > +(alnum | '_'));
    variable_ %= lit("(var") > identifier_ > lit(')');
    typespec_ %= lit("(type") > identifier_ > lit(')');
    newinit_ = (
      lit("(init")[phoenix::bind(&NewInit::init, &m_newinit, qi::_r1)]
      > identifier_[phoenix::bind(&NewInit::attach_name, &m_newinit, qi::_1)]
      > -typespec_[phoenix::bind(&NewInit::attach_type, &m_newinit, qi::_1)]
      > -exp_[phoenix::bind(&NewInit::attach_exp, &m_newinit, (phoenix::bind(&Expression::bytecode, qi::_1)))]
      > lit(")")
    )[_val = phoenix::bind(&NewInit::bytecode, &m_newinit)];
  }

private:
  NewInit m_newinit;

  ExpParser<Iterator> exp_;
  qi::rule<Iterator, std::string(), ascii::space_type> identifier_;
  qi::rule<Iterator, std::string(), ascii::space_type> variable_;
  qi::rule<Iterator, std::string(), ascii::space_type> typespec_;
  qi::rule<Iterator, std::string(Scope&), ascii::space_type> newinit_;
};

}

#endif // _NewInit_h_
