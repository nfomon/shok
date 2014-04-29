// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _NewInit_h_
#define _NewInit_h_

/* NewInit */

#include "Expression.h"

#include "Scope.h"
#include "Type.h"

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
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
  void attach_type(const Type& type);
  void attach_exp(const Expression& exp);
  void finalize();
  std::string bytecode() const;

private:
  Scope* m_scope;
  std::string m_name;
  boost::shared_ptr<Type> m_type;
  boost::shared_ptr<Expression> m_exp;
  std::string m_bytecode;
};

template <typename Iterator>
struct NewInitParser : qi::grammar<Iterator, std::string(Scope&), ascii::space_type> {
public:
  NewInitParser()
    : NewInitParser::base_type(newinit_, "newinit parser"),
      typespec_(true) {
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

    identifier_ %= lit("ID:'") > +(alnum | '_') > lit('\'');
    newinit_ = (
      lit("(init")[phoenix::bind(&NewInit::init, &m_newinit, qi::_r1)]
      > identifier_[phoenix::bind(&NewInit::attach_name, &m_newinit, qi::_1)]
      > -typespec_(qi::_r1)[phoenix::bind(&NewInit::attach_type, &m_newinit, phoenix::bind(&Expression::type, qi::_1))]
      > -exp_(qi::_r1)[phoenix::bind(&NewInit::attach_exp, &m_newinit, qi::_1)]
      > lit(")")[phoenix::bind(&NewInit::finalize, &m_newinit)]
    )[_val = phoenix::bind(&NewInit::bytecode, &m_newinit)];
  }

private:
  NewInit m_newinit;

  ExpParser<Iterator> typespec_;
  ExpParser<Iterator> exp_;
  qi::rule<Iterator, std::string(), ascii::space_type> identifier_;
  qi::rule<Iterator, std::string(Scope&), ascii::space_type> newinit_;
};

}

#endif // _NewInit_h_
