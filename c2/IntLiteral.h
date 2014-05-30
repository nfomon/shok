// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _IntLiteral_h_
#define _IntLiteral_h_

/* Integer literals */

#include "Scope.h"
#include "Type.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>
namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace ascii = spirit::ascii;
namespace qi = spirit::qi;

#include <memory>
#include <string>
#include <vector>

namespace compiler {

class IntLiteral {
public:
  IntLiteral();
  void init(const Scope& scope);
  void attach_text(const std::string& text);

  const Type& type() const;
  std::string bytecode() const;

private:
  const Scope* m_scope;
  std::string m_text;
  boost::shared_ptr<Type> m_type;
};

template <typename Iterator>
struct IntLiteralParser
  : qi::grammar<Iterator, IntLiteral(const Scope&), ascii::space_type> {
public:
  IntLiteralParser()
    : IntLiteralParser::base_type(int_, "IntLiteral") {
    using phoenix::ref;
    using qi::_1;
    using qi::_r1;
    using qi::_val;
    using qi::char_;
    using qi::lit;

    nums_ %= +char_("0-9");
    int_ =
      lit("INT:'")[phoenix::bind(&IntLiteral::init, _val, _r1)]
      > nums_[phoenix::bind(&IntLiteral::attach_text, _val, _1)]
      > lit('\'');

    //BOOST_SPIRIT_DEBUG_NODE(int_);
  }

private:
  qi::rule<Iterator, std::string()> nums_;
  qi::rule<Iterator, IntLiteral(const Scope&), ascii::space_type> int_;
};

}

#endif // _IntLiteral_h_
