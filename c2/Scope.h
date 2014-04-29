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
#include <string>
#include <vector>

namespace compiler {

class Scope : public boost::noncopyable {
public:
  typedef int Depth;

  Scope(Scope* parent = NULL)
    : m_parent(parent),
      m_root(parent ? parent->root() : *this),
      m_depth(parent ? (parent->depth() + 1) : 0) {
  }
  virtual ~Scope() {}

  Depth depth() const { return m_depth; }
  Scope& root() const { return m_root; }

  void insert(const std::string& name, std::auto_ptr<Type> type);
  const Type* find(const std::string& name) const;
  const Type* findLocal(const std::string& name) const;
  const Type* findRoot(const std::string& name) const;

private:
  Scope(const Scope&);

  Scope* m_parent;
  Scope& m_root;
  Depth m_depth;
  SymbolTable m_symbolTable;
};

class ObjectScope : public Scope {
public:
private:
  //Object* m_object;
};

class FunctionScope : public Scope {
public:
private:
  //Function* m_function;
};

}

#endif // _Scope_h_
