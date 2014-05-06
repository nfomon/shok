// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Object_h_
#define _Object_h_

/* Object literals */

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

class Object {
public:
  Object();
  void init(Scope& scope);
  const Type& type() const;

private:
  Scope* m_scope;
  boost::shared_ptr<Type> m_type;
};

template <typename Iterator>
struct ObjectParser
  : qi::grammar<Iterator, Object(Scope&), qi::locals<Object>, ascii::space_type> {
public:
  ObjectParser()
    : ObjectParser::base_type(object_, "object parser") {
    using phoenix::ref;
    using phoenix::val;
    using qi::_val;
    using qi::_1;
    using qi::_a;
    using qi::_r1;
    using qi::alnum;
    using qi::lit;

    object_.name("object");

    object_ = (
      lit('{')[phoenix::bind(&Object::init, _a, _r1)]
      > lit('}')
    )[_val = _a];
  }

private:
  qi::rule<Iterator, Object(Scope&), qi::locals<Object>, ascii::space_type> object_;
};

}

#endif // _Object_h_
