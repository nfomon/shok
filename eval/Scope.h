// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Scope_h_
#define _Scope_h_

/* Scope
 *
 * A Scope holds a local namespace of instantiated objects.  Block and RootNode
 * both have a Scope.  The depth tells you its nesting level: depth 0 is the
 * global scope (held by RootNode), or if the scope is owned by a function or
 * object literal.  A scope depth of 1 is fake, for silly implementation
 * reasons relating to how a '{' flips you from command-mode into the global
 * scope.  Thus scope depth 1 just defers down to the global scope, and depth 2
 * really represents the first nested scope level.
 *
 * A Scope is backed by an ObjectStore, and merely implements the parent-scope
 * and scope-depth logic on top of it.  An ObjectStore only knows what's in its
 * store; a Scope knows that if an object can't be found in its store then it
 * should defer up a parentScope chain until it finds it or doesn't.
 *
 * newObject() creates a new Object in the scope, but it is marked as "pending"
 * until it is finally either commit() or revert().  This allows any
 * setup/analysis stages to track the Object and its Type but lets us abort in
 * case of error.  All of this is handled by the underlying ObjectStore.
 */

#include "Common.h"
#include "Log.h"
#include "Object.h"
#include "ObjectStore.h"

#include <map>
#include <memory>
#include <string>
#include <utility>

namespace eval {

class Function;

class Scope {
public:
  Scope(Log& log)
    : m_log(log),
      m_isInit(false),
      m_objectStore(log),
      m_parentScope(NULL),
      m_function(NULL),
      m_depth(0) {}
  ~Scope();

  void init(Scope* parentScope);
  void init(Scope* parentScope, Function* parentFunction);
  //void init(Scope* parentScope, ObjectLiteral* parentObject);
  void reset();
  void commitFirst();
  void commitAll();
  void revertLast();
  void revertAll();

  // Lookup an object, deferring up the tree if it's not found locally.
  // Returns NULL if it does not exist anywhere.
  Object* getObject(const std::string& varname) const;
  // Insert a new object, as "pending" until it's either commit or revert
  void newObject(const std::string& varname, std::auto_ptr<Type> type);
  void delObject(const std::string& varname);
  void initObject(const std::string& varname, std::auto_ptr<Object> newObject);
  void replaceObject(const std::string& varname, std::auto_ptr<Object> newObject);

private:
  Log& m_log;
  bool m_isInit;
  ObjectStore m_objectStore;
  Scope* m_parentScope;     // NULL for the root scope (held by RootNode)
  Function* m_function;     // set if this is a function's block-scope
  //ObjectLiteral* m_object;  // set if this is an object literal's block-scope
  int m_depth;              // 0 at root (global). 1 is special: defers to root
};

}

#endif // _Scope_h_
