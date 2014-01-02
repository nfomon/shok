// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Type_h_
#define _Type_h_

/* Variable type
 *
 * A Type is a binary tree, where each node is either an OrList of nodes, an
 * AndList of nodes, or an Object*.
 *
 * We do not own these Object*s; the Object should always outlive a child's
 * Type.  Types are typically owned by a TypedNode, which can give you its
 * Type or a duplicate with its getType() or type() methods.
 *
 * A Type can be used to query the members of its underlying object(s), or just
 * their types.  Note that some of the semantics of |-types and &-types are not
 * yet determined, so some of the lookup rules may not be stable/trustworthy.
 */

#include "EvalError.h"
#include "Log.h"
#include "Object.h"

#include <limits.h>
#include <memory>
#include <string>
#include <vector>

namespace eval {

class Object;

// Pure virtual base class
class Type {
public:
  virtual ~Type() {}

  // Query our underlying object(s) for a member.
  // Returns NULL if the member does not exist or is otherwise inaccessible
  // (e.g. a disallowed aggregate).  This can NOT be used on an OrType (not
  // sure if that should just return NULL instead?)
  virtual Object* getMember(const std::string& name) const = 0;

  // Query directly for the Type of a member of the underlying object(s).
  // Use this if you're just doing type-checking analysis rather than
  // retrieving the Object* itself, please.
  // Caller gets a duplicate (takes ownership) because in the case of OrType,
  // it might need to create a brand new type to represent this.
  // Note that for a function member (method), the type is the type of this
  // method, e.g. @(A)->B, and not just the (set of) return type(s).
  virtual std::auto_ptr<Type> getMemberType(const std::string& name) const = 0;

  // Checks if the provided Type is a compatible subtype of this Type
  virtual bool isCompatible(const Type& rhs) const = 0;

  // Returns a duplicate of the Type.  Caller takes ownership.
  virtual std::auto_ptr<Type> duplicate() const = 0;
  virtual std::string getName() const = 0;
  virtual std::string print() const = 0;
  virtual bool isNull() const { return false; }
};

// NullType is the Type of stdlib::object, the root of all objects.
class NullType : public Type {
public:
  NullType() {}
  virtual Object* getMember(const std::string& name) const;
  virtual std::auto_ptr<Type> getMemberType(const std::string& name) const;
  virtual bool isCompatible(const Type& rhs) const;
  virtual std::auto_ptr<Type> duplicate() const;
  virtual std::string getName() const;
  virtual std::string print() const;
  virtual bool isNull() const { return true; }
};

// A BasicType wraps a single Object.  Note that a Variable's Type will be the
// BasicType of its Object, whereas an Object's Type represents its parents.
class BasicType : public Type {
public:
  BasicType(const Object& o)
    : m_object(o) {}
  virtual Object* getMember(const std::string& name) const;
  virtual std::auto_ptr<Type> getMemberType(const std::string& name) const;
  virtual bool isCompatible(const Type& rhs) const;
  virtual std::auto_ptr<Type> duplicate() const;
  virtual std::string getName() const;
  virtual std::string print() const;
private:
  const Object& m_object;
};

// AndType:  a&b
class AndType : public Type {
public:
  AndType(const Type& left, const Type& right)
    : m_left(left.duplicate()), m_right(right.duplicate()) {}
  AndType(std::auto_ptr<Type> left, std::auto_ptr<Type> right)
    : m_left(left), m_right(right) {}
  const Type& left() const { return *m_left.get(); }    // must exist
  const Type& right() const { return *m_right.get(); }  // must exist
  virtual Object* getMember(const std::string& name) const;
  virtual std::auto_ptr<Type> getMemberType(const std::string& name) const;
  virtual bool isCompatible(const Type& rhs) const;
  virtual std::auto_ptr<Type> duplicate() const;
  virtual std::string getName() const;
  virtual std::string print() const;
private:
  std::auto_ptr<Type> m_left;
  std::auto_ptr<Type> m_right;
};

// OrType:  a|b
class OrType : public Type {
public:
  // Perform the best |-union of two Types
  static std::auto_ptr<Type> OrUnion(const Type& a, const Type& b);

  OrType(const Type& left, const Type& right)
    : m_left(left.duplicate()), m_right(right.duplicate()) {}
  const Type& left() const { return *m_left.get(); }
  const Type& right() const { return *m_right.get(); }
  virtual Object* getMember(const std::string& name) const;
  virtual std::auto_ptr<Type> getMemberType(const std::string& name) const;
  virtual bool isCompatible(const Type& rhs) const;
  virtual std::auto_ptr<Type> duplicate() const;
  virtual std::string getName() const;
  virtual std::string print() const;
private:
  std::auto_ptr<Type> m_left;
  std::auto_ptr<Type> m_right;
};

};

#endif // _Type_h_
