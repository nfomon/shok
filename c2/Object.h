// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Object_h_
#define _Object_h_

/* Object literals */

#include "Common.h"
#include "NewInit.h"
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

class NewInit;
class ObjectScope;
class Scope;

class Object {
public:
  void init(const Scope& parentScope);
  void attach_new(const NewInit& newInit);
  ObjectScope& scope() const;
  const Type& type() const;
  std::string bytecode() const;

private:
  boost::shared_ptr<ObjectScope> m_scope;
  boost::shared_ptr<Type> m_type;
  std::string m_bytecode;
};

template <typename Iterator>
struct ObjectParser
  : qi::grammar<Iterator, Object(const Scope&), ascii::space_type> {
public:
  ObjectParser(ExpParser<Iterator>& exp_)
    : ObjectParser::base_type(object_, "object parser"),
      newinit_(exp_) {
    using phoenix::ref;
    using qi::_val;
    using qi::_1;
    using qi::_a;
    using qi::_r1;
    using qi::alnum;
    using qi::lit;

    newinit_.name("newinit");
    object_.name("object");

    object_ = (
      lit("(object")[phoenix::bind(&Object::init, _val, _r1)]
      > -(
        lit('{')
        > +(
          newinit_(phoenix::bind(&Object::scope, _val))[phoenix::bind(&Object::attach_new, _val, _1)]
          > -lit(';')
        )
        > lit('}')
      )
      > lit(')')
    );
  }

private:
  NewInitParser<Iterator> newinit_;
  qi::rule<Iterator, Object(const Scope&), ascii::space_type> object_;
};

}

#endif // _Object_h_
