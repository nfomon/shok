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
#include "SymbolTable.h"
#include "Type.h"

#include <boost/utility.hpp>

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace compiler {

class Function;
class Object;

class Scope : public boost::noncopyable {
public:
  typedef int Depth;

  Scope(const Scope* parent = NULL);
  virtual ~Scope() {}

  void reParent(const Scope& parent);

  Depth depth() const { return m_depth; }
  const Scope& root() const { return *m_root; }

  void insert(const std::string& name, std::auto_ptr<Type> type);
  virtual const Type* find(const std::string& name) const;
  virtual const Type* findLocal(const std::string& name) const;
  const Type* findRoot(const std::string& name) const;
  std::string bytecode() const;

protected:
  Scope(const Scope&);

  const Scope* m_parent;
  const Scope* m_root;
  Depth m_depth;
  symbol_map m_locals;
};

/* FunctionScope: member lookups check the function's scope before deferring to
 * the parent scope. */
class FunctionScope : public Scope {
public:
  FunctionScope(const Scope& parent, const Function& function);

  virtual const Type* find(const std::string& name) const;
  virtual const Type* findLocal(const std::string& name) const;

protected:
  const Function& m_function;
};

/* ObjectScope: member lookups check the Object's type before deferring to the
 * parent scope. */
class ObjectScope : public Scope {
public:
  ObjectScope(const Scope& parent, const Object& object);

  virtual const Type* find(const std::string& name) const;
  virtual const Type* findLocal(const std::string& name) const;

protected:
  const Object& m_object;
};

inline std::ostream& operator<<(std::ostream& out, const Scope& scope) {
  out << "(scope)";
  return out;
}

}

#endif // _Scope_h_
