// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Method_h_
#define _Method_h_

/* Method: arguments, return type, and code body.  Owned by Object.  */

#include "Common.h"
#include "Log.h"
#include "Type.h"

#include <memory>
#include <string>
#include <vector>

namespace eval {

class Block;

class Method {
public:
  Method(const arg_vec* args,
         std::auto_ptr<Type> returnType,
         std::auto_ptr<Block> body);
  ~Method();

  const argspec_vec& args() const {
    return m_args;
  }

  const Type* returnType() const {
    return m_returnType.get();
  }

  //bool isEquivalentTo(const Method& rhs) const;
  //bool areArgsCompatible(const type_list& rhs_args) const;

private:
  argspec_vec m_args;
  std::auto_ptr<Type> m_returnType;
  std::auto_ptr<Block> m_body;
};

}

#endif // _Method_h_
