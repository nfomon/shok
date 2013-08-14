// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Scope_h_
#define _Scope_h_

/* Scope
 *
 * A Scope is a local namespace of instantiated objects.  Block and RootNode
 * both have of Scope.  The depth tells you its nesting level: depth 0 is the
 * global scope (held by RootNode).  A scope depth of 1 is fake, for silly
 * implementation reasons relating to how a '{' flips you from command-mode
 * into the global scope.  Thus scope depth 1 just defers down to the global
 * scope, and depth 2 really represents the first nested scope level.
 *
 * The Scope takes ownership of Object*s that are inserted into it.
 *
 * newObject() inserts a new Object to the scope, but it is marked as "pending"
 * until it is finally either commit() or revert().  This allows any
 * setup/analysis stages to track the Object and its Type but lets us abort in
 * case of error.
 */

#include "Log.h"
#include "Object.h"
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
  void newObject(const std::string& varname, Object* object);
  void delObject(const std::string& varname);

private:
  typedef std::map<std::string,Object*> object_map;
  typedef std::pair<std::string,Object*> object_pair;
  typedef object_map::const_iterator object_iter;
  typedef object_map::iterator object_mod_iter;
 
  Log& m_log;
  object_map m_objects;
  // Objects that are part of a not-yet-applied (not-yet-evaluated) changeset
  object_map m_pending;
  Scope* m_parentScope;   // NULL for the root scope (held by RootNode)
  int m_depth;            // 0 at root (global).  1 is special: defers to root
};

};

#endif // _Scope_h_
