// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Variable_h_
#define _Variable_h_

/* Variable */

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

class Variable {
public:
  Variable();
  void init(Scope& scope);
  void attach_name(const std::string& name);
  void attach_member(const std::string& member);

  std::string fullname() const;
  const Type& type() const;

private:
  typedef std::vector<std::string> member_vec;
  typedef member_vec::const_iterator member_iter;

  Scope* m_scope;
  std::string m_name;
  member_vec m_members;
  // This could probably be a bare const Type*
  boost::shared_ptr<Type> m_type;
};

template <typename Iterator>
struct VariableParser : qi::grammar<Iterator, Variable(Scope&), ascii::space_type> {
public:
  VariableParser()
    : VariableParser::base_type(variable_, "variable parser") {
    using phoenix::ref;
    using phoenix::val;
    using qi::_val;
    using qi::alnum;
    using qi::lit;

    variable_.name("variable");

    identifier_ %= lit("ID:'") > +(alnum | '_') > lit('\'');
    variable_ = (
      lit("(var")[phoenix::bind(&Variable::init, &m_var, qi::_r1)]
      > identifier_[phoenix::bind(&Variable::attach_name, &m_var, qi::_1)]
      > *(identifier_[phoenix::bind(&Variable::attach_member, &m_var, qi::_1)])
      > lit(')')
    )[_val = ref(m_var)];
  }

private:
  Variable m_var;

  qi::rule<Iterator, std::string(), ascii::space_type> identifier_;
  qi::rule<Iterator, Variable(Scope&), ascii::space_type> variable_;
};

}

#endif // _Variable_h_
