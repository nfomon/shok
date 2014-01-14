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
#include "Method.h"
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
  Object(Log& log, const std::string& name, std::auto_ptr<Type> parentType);
  ~Object();

  std::string getName() const { return m_name; }
  std::string print() const { return m_name; }
  std::auto_ptr<Type> getType() const;
  const Type& type() const;

  void reset();
  void commitFirst();
  void commitAll();
  void revertLast();
  void revertAll();

  // Retrieve a member, deferring to the parent type(s) if it's not found.
  Object* getMember(const std::string& name) const;
  std::auto_ptr<Type> getMemberType(const std::string& name) const;
  void newMember(const std::string& varname, std::auto_ptr<Type> type);
  // TODO initMember, replaceMember, delMember
  void newMethod(const arg_vec* args,
                 std::auto_ptr<Type> returnType,
                 std::auto_ptr<Block> body);

  // Does an object get "assigned" to?  I think not!
  //    x = y
  // means the object behind 'x' is destroyed, then 'x' is given a copy of the
  // object behind 'y'.  It's a change to the enclosing Scope, not the Object.
  // What about:
  //    x.y = z
  // Now we're thinking about the Object acting as a Scope, an ownership home
  // for the variable being retrieved and modified (replaced).  So we'll have
  // to support some operation to enable this.
  //void assignMember(const std::string& name, Object* value);

  // Function
  bool isFunction() const { return !m_methods.empty(); }
  bool takesArgs(const paramtype_vec& params) const;
  std::auto_ptr<Type> getPossibleReturnTypes(const paramtype_vec& params) const;
  std::auto_ptr<Object> call(const param_vec& params) const;

  // Constructor/destructor functions.
  void construct();
  void destruct();
  std::auto_ptr<Object> clone(const std::string& newName) const;

private:
  typedef std::vector<Method*> method_vec;
  typedef method_vec::const_iterator method_iter;

  Log& m_log;
  // m_objectStore and m_type are auto_ptrs only to resolve a circular type
  // dependency that prevents us from keeping them by value  :/
  std::auto_ptr<ObjectStore> m_objectStore;
  std::string m_name;
  std::auto_ptr<Type> m_type;
  // An abstract is an object with any non-Function Signatures, and/or any
  // OrType members that do not also have an initial value.
  bool m_isAbstract;
  method_vec m_methods;
};

}

#endif // _Object_h_
