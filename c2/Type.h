// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Type_h_
#define _Type_h_

/* Type */

#include "CompileError.h"

#include <string>
#include <vector>

namespace compiler {

class Type {
public:
  Type() {}
  virtual ~Type() {}
/*
  virtual void addMemberType(const std::string& name, std::auto_ptr<Type> type) = 0;
  virtual std::auto_ptr<Type> getMemberType(const std::string& name) const = 0;
  virtual std::string defaultO() const = 0;
  virtual bool isParentOf(const Type& child) const = 0;
  virtual bool takesArgs(const paramtype_vec& paramtypes) const = 0;
  virtual std::string getName() const = 0;
*/

protected:
};

}

#endif // _Type_h_
