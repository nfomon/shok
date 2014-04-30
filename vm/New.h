// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _New_h_
#define _New_h_

/* A New instruction: defines a symbol (name, type, value) */

#include "Expression.h"
#include "SymbolTable.h"

#include "util/Log.h"

#include <boost/utility.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <string>
#include <vector>

// debug
#include <iostream>
using std::cout;
using std::endl;

namespace vm {

struct New {
  std::string name;
  Expression exp;
};

struct Exec_New {
public:
  Exec_New(symbol_map& symbols)
    : m_symbols(symbols) {}

  void operator() (const New& n, qi::unused_type, qi::unused_type) const {
    cout << "New: name=" << n.name << endl;
    Exec_Exp exec_Exp(m_symbols);
    std::auto_ptr<Object> value = boost::apply_visitor(exec_Exp, n.exp);
    if (m_symbols.find(n.name) != m_symbols.end()) {
      throw VMError("Cannot insert symbol " + n.name + "; already exists");
    }
  }

private:
  symbol_map& m_symbols;
};

template <typename Iterator>
struct NewParser : qi::grammar<Iterator, New(), ascii::space_type> {
public:
  NewParser() : NewParser::base_type(new_, "new parser") {
    using qi::int_;
    using qi::lit;
    using qi::graph;

    identifier_ %= qi::lexeme[ (qi::alpha | '_') >> *(qi::alnum | '_' | ':') ];
    new_ %=
      lit("(new")
      > identifier_
      > exp_
      > lit(")")
    ;
  }

private:
  ExpParser<Iterator> exp_;
  qi::rule<Iterator, New(), ascii::space_type> new_;
  qi::rule<Iterator, std::string(), ascii::space_type> identifier_;
};

}

#endif // _New_h_
