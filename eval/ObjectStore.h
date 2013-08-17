// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _ObjectStore_h_
#define _ObjectStore_h_

/* ObjectStore
 *
 * What do a block-scope and an Object have in common?  They both own a set of
 * Object members!  The ObjectStore just has a list of Objects that it owns,
 * and has commit/revert logic on new additions.  Scope and Object each have an
 * ObjectStore internally backing their members.
 */

#include "Log.h"
#include "Object.h"
#include "Type.h"

#include <map>
#include <memory>
#include <string>
#include <utility>

namespace eval {

class Object;
class Type;

class ObjectStore {
public:
  ObjectStore(Log& log)
    : m_log(log) {}
  ~ObjectStore();

  void reset();
  void commit(const std::string& varname);
  void commitAll();
  void revert(const std::string& varname);
  void revertAll();

  // Lookup an object, deferring up the tree if it's not found locally.
  // Returns NULL if it does not exist anywhere.
  const Object* getObject(const std::string& varname) const;
  // Insert a new object, as "pending" until it's either commit or revert
  const Object& newObject(const std::string& varname, std::auto_ptr<Type> type);
  void delObject(const std::string& varname);

  size_t size() const { return m_objects.size(); }
  size_t pendingSize() const { return m_pending.size(); }

private:
  typedef std::map<std::string,Object*> object_map;
  typedef std::pair<std::string,Object*> object_pair;
  typedef object_map::const_iterator object_iter;
  typedef object_map::iterator object_mod_iter;
 
  Log& m_log;
  object_map m_objects;
  // Objects that are part of a not-yet-applied (not-yet-evaluated) changeset
  object_map m_pending;
};

};

#endif // _ObjectStore_h_
