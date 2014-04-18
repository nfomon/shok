// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Variable_h_
#define _Variable_h_

/* Variable
 *
 * A Variable simply refers to an object, or nested object-members, by name.
 * Its children are Identifiers, and there must be at least one.  The first
 * must refer to an object that exists.  The second and subsequent are the
 * names of a member, a member on that member, etc.  These must all exist in
 * the parent scope at setup()-time, so the Variable can be a TypedNode.
 */

#include "Object.h"
#include "RootNode.h"
#include "Token.h"
#include "TypedNode.h"

#include "util/Log.h"

#include <string>

namespace compiler {

class Variable : public TypedNode {
public:
  Variable(Log& log, RootNode*const root, const Token& token)
    : TypedNode(log, root, token),
      m_symbol(NULL) {}
  virtual void setup();

  Object& getObject() const;

private:
  typedef std::vector<std::string> select_vec;
  typedef select_vec::const_iterator select_iter;
  // from TypedNode
  virtual void computeType();
  std::string m_varname;
  std::string m_fullname;
  Symbol* m_symbol;
  select_vec m_select;
};

}

#endif // _Variable_h_
