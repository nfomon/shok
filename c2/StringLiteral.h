// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _StringLiteral_h_
#define _StringLiteral_h_

/* String literals */

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

// debug
#include <ostream>

namespace compiler {

class StringLiteral {
public:
  StringLiteral();
  void init(const Scope& scope);
  void attach_text(const std::string& text);

  const Type& type() const;
  std::string text() const { return m_text; }
  std::string bytecode() const;

private:
  const Scope* m_scope;
  std::string m_text;
  boost::shared_ptr<Type> m_type;
};

inline std::ostream& operator<<(std::ostream& o, const StringLiteral& lit) {
  o << "(:[STR:" << lit.text() << "]:)";
  return o;
}

template <typename Iterator>
struct StringLiteralParser
  : qi::grammar<Iterator, StringLiteral(const Scope&)> {
public:
  StringLiteralParser()
    : StringLiteralParser::base_type(str_, "StringLiteral") {
    using phoenix::ref;
    using qi::_1;
    using qi::_r1;
    using qi::_val;
    using qi::char_;
    using qi::lexeme;
    using qi::lit;

    sq_inner_ %= +(~char_("\\\'") | (char_('\\') > char_));
    dq_inner_ %= +(~char_("\\\"") | (char_('\\') > char_));
    sq_ %= (char_('\'') > sq_inner_ > char_('\''));
    dq_ %= (char_('\"') > dq_inner_ > char_('\"'));
    text_ %= sq_ | dq_;

    str_ =
      lit("STR:")[phoenix::bind(&StringLiteral::init, _val, _r1)]
      > text_[phoenix::bind(&StringLiteral::attach_text, _val, _1)];

    //BOOST_SPIRIT_DEBUG_NODE(sq_);
    //BOOST_SPIRIT_DEBUG_NODE(dq_);
    //BOOST_SPIRIT_DEBUG_NODE(text_);
    //BOOST_SPIRIT_DEBUG_NODE(str_);
  }

private:
  qi::rule<Iterator, std::string()> sq_inner_;
  qi::rule<Iterator, std::string()> dq_inner_;
  qi::rule<Iterator, std::string()> sq_;
  qi::rule<Iterator, std::string()> dq_;
  qi::rule<Iterator, std::string()> text_;
  qi::rule<Iterator, StringLiteral(const Scope&)> str_;
};

}

#endif // _StringLiteral_h_
