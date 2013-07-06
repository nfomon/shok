// Copyright (C) 2013 Michael Biggs.  See the COPYRIGHT file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Type_h_
#define _Type_h_

/* Variable type */

#include "Log.h"
//#include "Variable.h"

#include <string>

namespace eval {

class Variable;

class Type {
};

class BasicType : public Type {
public:
  BasicType(Variable* v)
    : m_var(v) {}
private:
  Variable* m_var;
};

/*
class AndType : public Type {
public:
  AndType(Operator*) {
  }
private:
  std::vector<Type*> m_vars;
};

class OrType : public Type {
public:
  OrType(Operator*) {
  }
private:
  std::vector<Type*> m_vars;
};
*/

};

#endif // _Type_h_
