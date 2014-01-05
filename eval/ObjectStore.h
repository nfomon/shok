// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ObjectStore_h_
#define _ObjectStore_h_

/* ObjectStore
 *
 * What do a block-scope and an Object have in common?  They both own a set of
 * Object members!  The ObjectStore just has a list of Objects that it owns,
 * and has commit/revert logic on new additions.  Scope and Object each have an
 * ObjectStore internally backing their members.
 *
 * newObject, replaceObject, and deleteObject all return a change_id.  This is
 * *only* an error-prevention mechanism; if we trust the rest of the codebase
 * works properly, then we could just use m_changeset.begin() instead of a
 * change_id that we assert always refers to the beginning of
 * m_changeset.begin().  I think this is an important extra layer of
 * verification, at least for now.
 *
 * newObject and deleteObject operations are the only "necessary" ones, for our
 * purpose of allowing "hey that object doesn't exist yet!" error checking
 * before evaluation-time.  But since the ObjectStore owns the memory of its
 * objects, it's also where we choose to invoke object constructors and
 * destructors.  That's why we also have replaceObject -- it's so the
 * ObjectStore can be responsible for calling the destructor of the replaced
 * object (also at evaluation-time aka commit-time, of course).
 */

#include "Common.h"
#include "Log.h"
#include "Type.h"

#include <deque>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace eval {

class Object;
class Type;

typedef int change_id;

class ObjectStore {
public:
  static const change_id NO_CHANGE = -1;

  ObjectStore(Log& log)
    : m_log(log),
      m_lastId(0) {}
  ~ObjectStore();

  void reset();
  void commit(change_id id);
  void commitAll();
  void revert(change_id id);
  void revertAll();

  // Lookup an object, returning NULL if it is not here.
  Object* getObject(const std::string& varname) const;
  // Construct a new object, as "pending" until it's either commit or revert
  change_id newObject(const std::string& varname, std::auto_ptr<Type> type);
  // Delete an object.  Calls the object's destructor when commit.
  change_id delObject(const std::string& varname);
  // Replace an object.  This should happen at evaluation time; no commit or
  // revert!  It just happens.  The destructor and constructor are called.
  void replaceObject(const std::string& varname, std::auto_ptr<Object> newObject);

  std::auto_ptr<ObjectStore> duplicate() const;

  size_t size() const { return m_objects.size(); }

private:
  enum ChangeType {
    OBJECT_ADD,
    OBJECT_DELETE,
  };

  typedef std::map<std::string,Object*> object_map;
  typedef std::pair<std::string,Object*> object_pair;
  typedef object_map::const_iterator object_iter;
  typedef object_map::iterator object_mod_iter;

  typedef std::pair<std::string,bool> ordering_pair;
  typedef std::deque<ordering_pair> order_vec;
  typedef order_vec::const_iterator ordering_iter;
  typedef order_vec::iterator ordering_mod_iter;
  typedef order_vec::const_reverse_iterator ordering_rev_iter;
  typedef order_vec::reverse_iterator ordering_rev_mod_iter;

  // TODO 6am engineering fail!
  struct ObjectChange {
    ObjectChange(change_id& lastId, std::string varname, ChangeType type,
                 Object* oldObject = NULL, bool newId = true)
      : varname(varname), type(type), oldObject(oldObject) {
      if (OBJECT_ADD == type && oldObject) {
        throw EvalError("Cannot create ADD ObjectChange for " + varname + " with an old object");
      } else if (OBJECT_DELETE == type && !oldObject) {
        throw EvalError("Cannot create DELETE ObjectChange for " + varname + " without an old object");
      }
      id = lastId;
      if (newId) {
        ++lastId;
        if (INT_MAX == lastId || lastId < 0) {
          lastId = 0;
        }
      }
    }
    change_id id;
    std::string varname;
    Object* oldObject;
    ChangeType type;
  };
  typedef std::deque<ObjectChange> changeset_vec;
  typedef changeset_vec::const_iterator changeset_iter;
  typedef changeset_vec::const_reverse_iterator changeset_rev_iter;

  change_id m_lastId;

  Log& m_log;
  // The store of objects.  Eager: includes the latest uncommit changeset
  object_map m_objects;
  // The object names in the order in which they are constructed (hence the
  // reverse order in which they should be deleted).  The flag indicates if the
  // object is pending deletion; used to help commit/revert deletions.
  order_vec m_ordering;
  // Changeset pending commit; changes can be rolled back
  changeset_vec m_changeset;
};

};

#endif // _ObjectStore_h_
