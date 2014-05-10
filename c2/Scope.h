// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Scope_h_
#define _Scope_h_

/* Scope
 *
 * Scopes form a tree of symbol tables.  Each has a local symbol table, and
 * knows how to defer non-local lookups up the tree.
 *
 * Function and Object scopes can also look up members in their enclosing
 * object or function.
 */

#include "CompileError.h"
#include "Object.h"
#include "SymbolTable.h"
#include "Type.h"

#include <boost/utility.hpp>

#include <memory>
#include <string>
#include <vector>

namespace compiler {

class Object;

class Scope : public boost::noncopyable {
public:
  typedef int Depth;

  Scope(const Scope* parent = NULL);
  virtual ~Scope() {}

  Depth depth() const { return m_depth; }
  const Scope& root() const { return m_root; }

  void insert(const std::string& name, std::auto_ptr<Type> type);
  virtual const Type* find(const std::string& name) const;
  virtual const Type* findLocal(const std::string& name) const;
  const Type* findRoot(const std::string& name) const;

protected:
  Scope(const Scope&);

  const Scope* m_parent;
  const Scope& m_root;
  Depth m_depth;
  symbol_map m_locals;
};

/* ObjectScope: member lookups check the Object's type before deferring to the
 * parent scope. */
class ObjectScope : public Scope {
public:
  ObjectScope(const Object& object, const Scope& parent);

  virtual const Type* find(const std::string& name) const;
  virtual const Type* findLocal(const std::string& name) const;

protected:
  const Object& m_object;
};

class FunctionScope : public Scope {
public:
protected:
  //Function* m_function;
};

}

#endif // _Scope_h_
