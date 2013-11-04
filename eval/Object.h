// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Object_h_
#define _Object_h_

/* Object
 *
 * Not to be confused with stdlib::object, which is a subtype of Object.  This
 * is our internal Object representation; it has a list of members, and a Type
 * which refers to its parents.
 *
 * The Object owns its members by way of an internal ObjectStore.
 */

#include "Log.h"
#include "ObjectStore.h"
#include "Type.h"

#include <map>
#include <memory>
#include <string>

namespace eval {

class Expression;
class ObjectStore;
class Type;

class Object {
public:
  Object(Log& log, const std::string& name, std::auto_ptr<Type> type);

  virtual ~Object() {}

  std::string print() const { return m_name; }
  const Type& getType() const {
    if (!m_type.get()) {
      throw EvalError("Object " + print() + " does not appear to have a Type");
    }
    return *m_type.get();
  }

  // Retrieve a member, deferring to the parent type(s) if it's not found.
  Object* getMember(const std::string& name) const;
  std::auto_ptr<Type> getMemberType(const std::string& name) const;
  // TODO should an initial value (object) be required?  by auto_ptr I guess?
  // Probably shouldn't allow creation of an OrType with no default value?
  Object& newMember(const std::string& varname, std::auto_ptr<Type> type);

  //Function& newSignature(const argspec_list& args, Type* returnType, (void*) builtinCode);

  // Does an object get "assigned" to?  I think not!
  //    x = y
  // means the object behind 'x' is destroyed, then 'x' is given a copy of the
  // object behind 'y'.  It's a change to the enclosing Scope, not the Object.
  // What about:
  //    x.y = z
  // Now we're thinking about the Object acting as a Scope, an ownership home
  // for the variable being retrieved and modified (replaced).  So we'll have
  // to support some operation to enable this.
  //void assign(const std::string& name, Object* value);

protected:
  Log& m_log;
  // This is an auto_ptr only to resolve a circular type dependency that
  // prevents us from keeping it by value  :/
  std::auto_ptr<ObjectStore> m_objectStore;
  std::string m_name;
  std::auto_ptr<Type> m_type;
};

};

#endif // _Object_h_
