// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Object_h_
#define _Object_h_

/* Object
 *
 * This is our internal Object representation; it has a store of members, and a
 * Type which refers to its parents.
 *
 * Object is not a Node; it's a thing created by ObjectStores, in blocks or
 * other Objects.  The Object owns its members by way of its own internal
 * ObjectStore.
 *
 * An Object might be a function, meaning simply that it has at least one
 * member of type "function builtin".  So any object can *attempt* to be called
 * like a function, meaning it will look up an appropriate member (a Function)
 * that has the codeblock for the provided parameters.
 */

#include "Common.h"
#include "Log.h"
#include "Type.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace eval {

class Expression;
class ObjectStore;
class Type;

class Object {
public:
  Object(Log& log, const std::string& name, std::auto_ptr<Type> type);
  ~Object() {}

  std::string getName() const { return m_name; }
  std::string print() const { return m_name; }
  const Type& getType() const;

  void reset();
  void commit(change_id id);
  void commitAll();
  void revert(change_id id);
  void revertAll();

  // Retrieve a member, deferring to the parent type(s) if it's not found.
  Object* getMember(const std::string& name) const;
  std::auto_ptr<Type> getMemberType(const std::string& name) const;
  // TODO should an initial value (object) be required?  by auto_ptr I guess?
  // Probably shouldn't allow creation of an OrType with no default value,
  // unless this is an abstract.
  change_id newMember(const std::string& varname, std::auto_ptr<Type> type);

  // Does an object get "assigned" to?  I think not!
  //    x = y
  // means the object behind 'x' is destroyed, then 'x' is given a copy of the
  // object behind 'y'.  It's a change to the enclosing Scope, not the Object.
  // What about:
  //    x.y = z
  // Now we're thinking about the Object acting as a Scope, an ownership home
  // for the variable being retrieved and modified (replaced).  So we'll have
  // to support some operation to enable this.
  void assignMember(const std::string& name, Object* value);

  // Function
  bool isFunction() const { return false; }   // TODO
  // bool isFunction() const { return m_signatures.empty(); }
  bool takesArgs(const paramtype_vec& params) const;
  std::auto_ptr<Type> getPossibleReturnTypes(const paramtype_vec& params) const;
  std::auto_ptr<Object> call(const param_vec& params) const;

  // Constructor/destructor functions.
  void construct();
  void destruct();

private:
  Log& m_log;
  // This is an auto_ptr only to resolve a circular type dependency that
  // prevents us from keeping it by value  :/
  std::auto_ptr<ObjectStore> m_objectStore;
  std::string m_name;
  std::auto_ptr<Type> m_type;
  // An abstract is an object with any non-Function Signatures, and/or any
  // OrType members that do not also have an initial value.
  bool m_isAbstract;
  // Function signatures; TODO
  //typedef std::vector<Signature> m_signatures;
};

};

#endif // _Object_h_
