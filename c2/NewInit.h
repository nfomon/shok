// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _NewInit_h_
#define _NewInit_h_

/* NewInit */

#include "Expression.h"

#include "util/Log.h"

#include <boost/bind.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/qi.hpp>
namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace ascii = spirit::ascii;
namespace qi = spirit::qi;

#include <string>
#include <vector>

namespace compiler {

class NewInit {
public:
  NewInit(Log& log);
  void attach_name(const std::string& name);
  void attach_type(const std::string& type);
  void attach_exp(const std::string& exp);
  std::string bytecode() const;

private:
  Log& m_log;
  std::string m_name;
  bool m_hasType;
  bool m_hasExp;
};

template <typename Iterator>
struct NewInitParser : qi::grammar<Iterator, std::string(), ascii::space_type> {
public:
  NewInitParser(Log& log)
    : NewInitParser::base_type(newinit_, "newinit parser"),
      m_log(log),
      m_newinit(log),
      exp_(log) {
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
      lit("(init")
      > identifier_[phoenix::bind(&NewInit::attach_name, &m_newinit, qi::_1)]
      > -typespec_[phoenix::bind(&NewInit::attach_type, &m_newinit, qi::_1)]
      > -exp_[phoenix::bind(&NewInit::attach_exp, &m_newinit, (phoenix::bind(&Expression::bytecode, qi::_1)))]
      > lit(")")
    )[_val = phoenix::bind(&NewInit::bytecode, &m_newinit)];
  }

private:
  Log& m_log;
  NewInit m_newinit;

  ExpParser<Iterator> exp_;
  qi::rule<Iterator, std::string(), ascii::space_type> identifier_;
  qi::rule<Iterator, std::string(), ascii::space_type> variable_;
  qi::rule<Iterator, std::string(), ascii::space_type> typespec_;
  qi::rule<Iterator, std::string(), ascii::space_type> newinit_;
};

}

#endif // _NewInit_h_
