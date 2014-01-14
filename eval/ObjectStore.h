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
 * newObject and deleteObject operations are the only "necessary" ones, for our
 * purpose of allowing "hey that object doesn't exist yet!" error checking
 * before evaluation-time.  But since the ObjectStore owns the memory of its
 * objects, it's also where we choose to invoke object constructors and
 * destructors.  That's why we also have replaceObject -- it's so the
 * ObjectStore can be responsible for calling the destructor of the replaced
 * object (also at evaluation-time aka commit-time, of course).
 */

#include "Common.h"
#include "EvalError.h"
#include "Log.h"

#include <deque>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace eval {

class Object;

class ObjectStore {
public:
  ObjectStore(Log& log)
    : m_log(log) {}
  ~ObjectStore();

  void reset();
  void commitFirst();
  void commitAll();
  void revertLast();
  void revertAll();

  // Lookup an object, returning NULL if it is not here.
  Object* getObject(const std::string& varname) const;
  // Construct a new object, as "pending" until it's either commit or revert
  void newObject(const std::string& varname, std::auto_ptr<Type> type);
  // Delete an object.  Calls the object's destructor when commit.
  void delObject(const std::string& varname);
  // Initialize a new'd object with its first value.  (evaluation-time)
  void initObject(const std::string& varname, std::auto_ptr<Object> newObject);
  // Assign a new value to an object.  (evaluation-time)
  void replaceObject(const std::string& varname,
                     std::auto_ptr<Object> newObject);

  std::auto_ptr<ObjectStore> duplicate() const;

  size_t size() const { return m_symbols.size(); }

private:
  struct Symbol {
    Symbol(std::auto_ptr<Type> type)
      : type(type) {}
    std::auto_ptr<Type> type;
    std::auto_ptr<Object> object;
  };

  typedef std::map<std::string,Symbol*> symbol_map;
  typedef std::pair<std::string,Symbol*> symbol_pair;
  typedef symbol_map::const_iterator symbol_iter;
  typedef symbol_map::iterator symbol_mod_iter;

  typedef std::pair<std::string,bool> ordering_pair;
  typedef std::deque<ordering_pair> order_vec;
  typedef order_vec::const_iterator ordering_iter;
  typedef order_vec::iterator ordering_mod_iter;
  typedef order_vec::const_reverse_iterator ordering_rev_iter;
  typedef order_vec::reverse_iterator ordering_rev_mod_iter;

  struct SymbolChange {
    SymbolChange(const std::string& varname)
      : varname(varname) {}
    virtual ~SymbolChange() {}
    std::string varname;
  };
  struct AddSymbol : public SymbolChange {
    AddSymbol(const std::string& varname)
      : SymbolChange(varname) {}
    virtual ~AddSymbol() {}
  };
  struct DelSymbol : public SymbolChange {
    DelSymbol(const std::string& varname,
              std::auto_ptr<Type> oldType,
              std::auto_ptr<Object> oldObject)
      : SymbolChange(varname), oldType(oldType), oldObject(oldObject) {
    }
    virtual ~DelSymbol() {}
    std::auto_ptr<Type> oldType;
    std::auto_ptr<Object> oldObject;
  };

  typedef std::deque<SymbolChange*> changeset_vec;
  typedef changeset_vec::const_iterator changeset_iter;
  typedef changeset_vec::const_reverse_iterator changeset_rev_iter;

  Log& m_log;
  // The store of symbols (typed objects).
  // Eager: includes the latest uncommit changeset
  symbol_map m_symbols;
  // The object names in the order in which they are constructed (hence the
  // reverse order in which they should be deleted).  The flag indicates if the
  // object is pending deletion; used to help commit/revert deletions.
  order_vec m_ordering;
  // Changeset pending commit; changes can be rolled back
  changeset_vec m_changeset;
};

}

#endif // _ObjectStore_h_
