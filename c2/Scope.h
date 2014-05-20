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
  enum Locality {
    GLOBAL,
    LOCAL
  };
  typedef int Depth;

  static std::string LocalityPrefix(Locality locality) {
    switch (locality) {
    case GLOBAL: return "";
    case LOCAL: return "L:";
    default: throw CompileError("Unknown locality " + boost::lexical_cast<std::string>(locality));
    }
  }

  Scope(const Scope* parent = NULL);
  virtual ~Scope() {}

  void reParent(const Scope& parent);

  Depth depth() const { return m_depth; }
  Locality locality() const { return m_locality; }
  const Scope& root() const { return *m_root; }
  // Variable name appropriate for bytecode (includes locality and depth)
  std::string bytename(const std::string& varname) const {
    return varnamePrefix() + varname + varnameSuffix();
  }

  void insert(const std::string& name, std::auto_ptr<Type> type);
  virtual const Type* find(const std::string& name) const;
  virtual const Type* findLocal(const std::string& name) const;
  const Type* findRoot(const std::string& name) const;
  std::string bytecode() const;

protected:
  Scope(const Scope&);
  std::string varnamePrefix() const;
  std::string varnameSuffix() const;

  const Scope* m_parent;
  const Scope* m_root;
  Depth m_depth;
  Locality m_locality;
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
