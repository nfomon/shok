// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Atom_h_
#define _Atom_h_

/* Atom */

#include "Type.h"

#include <boost/variant.hpp>

#include <memory>
#include <string>
#include <vector>

namespace compiler {

class Object;
class Variable;

typedef boost::variant<
  Variable,
  boost::recursive_wrapper<Object> > Atom;

class Atom_bytecode : public boost::static_visitor<std::string> {
public:
  std::string operator() (const Variable& var) const;
  std::string operator() (const Object& object) const;
};

class Atom_type : public boost::static_visitor<const Type&> {
public:
  const Type& operator() (const Variable& var) const;
  const Type& operator() (const Object& object) const;
};

}

#endif // _Atom_h_
