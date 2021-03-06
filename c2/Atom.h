// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Atom_h_
#define _Atom_h_

/* Atom */

#include "Literal.h"
#include "Type.h"

#include <boost/variant.hpp>

#include <memory>
#include <string>
#include <vector>

namespace compiler {

class Function;
class Object;
class Variable;

typedef boost::variant<
  Variable,
  Literal,
  boost::recursive_wrapper<Object>,
  boost::recursive_wrapper<Function>
> Atom;

class Atom_bytecode : public boost::static_visitor<std::string> {
public:
  std::string operator() (const Variable& var) const;
  std::string operator() (const Literal& var) const;
  std::string operator() (const Object& object) const;
  std::string operator() (const Function& function) const;
};

class Atom_type : public boost::static_visitor<const Type&> {
public:
  const Type& operator() (const Variable& var) const;
  const Type& operator() (const Literal& var) const;
  const Type& operator() (const Object& object) const;
  const Type& operator() (const Function& function) const;
};

}

#endif // _Atom_h_
