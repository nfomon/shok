// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Scope_h_
#define _Scope_h_

/* Scope
 *
 * A Scope is a local namespace of instantiated objects.  Block and RootNode
 * are both subclasses of Scope.
 */

#include "Log.h"
#include "Object.h"

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
  void cleanup();

  bool hasObject(const std::string& varname) const;
  Object* newObject(const std::string& varname);
  void delObject(const std::string& varname);

private:
  typedef std::map<std::string,Object*> object_map;
  typedef std::pair<std::string,Object*> object_pair;
  typedef std::map<std::string,Object*>::const_iterator object_iter;
  typedef std::map<std::string,Object*>::iterator object_mod_iter;
 
  Log& m_log;
  object_map m_objects;
  // Objects that are part of a not-yet-applied (not-yet-evaluated) changeset
  object_map m_pending;
  Scope* m_parentScope;   // NULL for the root scope (held by RootNode)
  int m_depth;            // 0 at root (global).  1 is special: defers to root
};

};

#endif // _Scope_h_
