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
 * The members currently are not owned by the Object; they must already exist
 * somewhere.  But I think that is broken.  It's maybe fine for objects being
 * created by the user from other objects, but it doesn't help us statically
 * put things in them.  Maybe that's ok -- they're static, so I just need to
 * create them somewhere statically.  Let's go with that for now....
 *
 * Seems like an Object needs both: members that it owns (acts like a scope
 * for), and members that are just links to Objects that will outlive this
 * Object.  Maybe we have a Member class that knows how to be destroyed, how to
 * hold a new Object incarnation or to point to an existing one. Hmmmmmmm.
 * Let's think carefully about Object vs. object etc...
 *
 * uhhh it should definitely own all its members, which are variables: copies
 * of some object.  The real question is: how does this relate to Scope?
 *
 * We'll split Scope in two.  The basis is "ObjectStore", an Object container
 * with revert/commit logic.  The other is Scope, which *has* an ObjectStore
 * and keeps it behind the "scope depth" logic (0 is root node, 1 is special).
 * Object will also *have* an ObjectStore behind it in the same way.
 */

#include "Log.h"
#include "Type.h"

#include <map>
#include <memory>
#include <string>

namespace eval {

class Expression;
class Type;

class Object {
public:
  Object(Log& log, const std::string& name, const Type& type);
  virtual ~Object() {}

  std::string print() const { return m_name; }
  const Type& getType() const {
    if (!m_type.get()) {
      throw EvalError("Object " + print() + " does not appear to have a Type");
    }
    return *m_type.get();
  }

  // Retrieve a member, deferring to the parent type(s) if it's not found.
  // Since the result should be considered in the context of the child, we
  // return a const Object* here and expect any mutation to go through us first
  // (allowing us to implement copy-on-write semantics).
  const Object* getMember(const std::string& name) const;
  std::auto_ptr<Type> getMemberType(const std::string& name) const;

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
  // Overridable constructor for children that want to set their own m_type
  // without duplicating an existing type (e.g. stdlib::object)
  Object(Log& log, const std::string& name);

  typedef std::map<std::string,Object*> member_list;
  typedef member_list::const_iterator member_iter;

  Log& m_log;
  std::string m_name;
  std::auto_ptr<Type> m_type;
  member_list m_members;
};

};

#endif // _Object_h_
