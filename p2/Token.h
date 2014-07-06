// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Token_h_
#define _Token_h_

/* Parser of Lexer tokens */

#include <boost/fusion/tuple.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;

#include <istream>
#include <ostream>

namespace parser {

template <typename Iterator>
struct TokenParser : qi::grammar<Iterator> {
public:
  TokenParser(const std::string& name)
    : TokenParser::base_type(token_, "Token") {
    using qi::int_;
    using qi::lit;

    col_ %= int_;
    name_ %= lit(name);

    token_ %=
      col_
      >> lit(':')
      >> name_
    ;

    //BOOST_SPIRIT_DEBUG_NODE(token_);
  }

private:
  qi::rule<Iterator> col_;
  qi::rule<Iterator> name_;
  qi::rule<Iterator> token_;
};

// Token that emits its name verbatim if matched
template <typename Iterator>
struct NTokenParser : qi::grammar<Iterator, std::string()> {
public:
  NTokenParser(const std::string& name)
    : NTokenParser::base_type(ntoken_, "Token") {
    using qi::_val;
    using qi::int_;
    using qi::lit;

    col_ %= int_;
    name_ %= lit(name);

    ntoken_ = (
      col_
      >> lit(':')
      >> name_
    )[_val = name];

    //BOOST_SPIRIT_DEBUG_NODE(ntoken_);
  }

private:
  qi::rule<Iterator> col_;
  qi::rule<Iterator> name_;
  qi::rule<Iterator, std::string()> ntoken_;
};

template <typename Iterator>
struct VTokenParser : qi::grammar<Iterator, std::string()> {
public:
  VTokenParser(const std::string& name)
    : VTokenParser::base_type(vtoken_, "VToken") {
  using qi::char_;
  using qi::int_;
  using qi::lit;

    col_ %= int_;
    name_ %= lit(name);

    sq_inner_ %= +(~char_("\\\'") | (char_('\\') > char_));
    dq_inner_ %= +(~char_("\\\"") | (char_('\\') > char_));
    sq_ %= (char_('\'') >> sq_inner_ >> char_('\''));
    dq_ %= (char_('\"') >> dq_inner_ >> char_('\"'));
    value_ %= sq_ | dq_;

    vtoken_ %=
      col_
      >> lit(':')
      >> name_
      >> lit(':')
      >> value_
    ;

    //BOOST_SPIRIT_DEBUG_NODE(vtoken_);
  }

private:
  qi::rule<Iterator> col_;
  qi::rule<Iterator> name_;
  qi::rule<Iterator, std::string()> sq_inner_;
  qi::rule<Iterator, std::string()> dq_inner_;
  qi::rule<Iterator, std::string()> sq_;
  qi::rule<Iterator, std::string()> dq_;
  qi::rule<Iterator, std::string()> value_;
  qi::rule<Iterator, std::string()> vtoken_;
};

}

#endif // _Token_h_
