// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Expression_h_
#define _Expression_h_

/* Expression */

#include "Atom.h"
#include "Operator.h"
#include "Scope.h"
#include "Type.h"

#include <string>

namespace compiler {

class Expression {
public:
  Expression();
  void init(const Scope& scope);
  void attach_atom(const Atom& atom);
  void attach_preop(const std::string& preop);
  void attach_binop(const std::string& binop);
  void finalize();
  std::string bytecode() const;
  const Type& type() const;

private:
  typedef boost::ptr_vector<OperatorNode> stack_vec;
  typedef stack_vec::const_iterator stack_iter;

  const Scope* m_scope;
  stack_vec m_stack;
  boost::shared_ptr<Type> m_type;
  std::string m_bytecode;
};

}

#endif // _Expression_h_
