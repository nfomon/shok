// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Scope_h_
#define _Scope_h_

/* Scope
 *
 * A Scope holds a local namespace of instantiated objects.  Block and RootNode
 * both have of Scope.  The depth tells you its nesting level: depth 0 is the
 * global scope (held by RootNode).  A scope depth of 1 is fake, for silly
 * implementation reasons relating to how a '{' flips you from command-mode
 * into the global scope.  Thus scope depth 1 just defers down to the global
 * scope, and depth 2 really represents the first nested scope level.
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

#include "Log.h"
#include "Object.h"
#include "ObjectStore.h"
#include "Type.h"

#include <map>
#include <memory>
#include <string>
#include <utility>

namespace eval {

class Scope {
public:
  Scope(Log& log)
    : m_log(log),
      m_objectStore(log),
      m_parentScope(NULL),
      m_depth(0) {}
  ~Scope();

  void init(Scope* parentScope);
  void reset();
  void commit(const std::string& varname);
  void commitAll();
  void revert(const std::string& varname);
  void revertAll();

  // Lookup an object, deferring up the tree if it's not found locally.
  // Returns NULL if it does not exist anywhere.
  Object* getObject(const std::string& varname) const;
  // Insert a new object, as "pending" until it's either commit or revert
  Object& newObject(const std::string& varname, std::auto_ptr<Type> type);
  void delObject(const std::string& varname);

private:
  Log& m_log;
  ObjectStore m_objectStore;
  Scope* m_parentScope;   // NULL for the root scope (held by RootNode)
  int m_depth;            // 0 at root (global).  1 is special: defers to root
};

};

#endif // _Scope_h_
