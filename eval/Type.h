// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Type_h_
#define _Type_h_

/* Variable (symbol) type
 *
 * A Type specifies the set of possible objects that a symbol is allowed to
 * take on as values.  It is a binary tree of BasicType nodes joined by
 * AndTypes (&) and OrTypes (|).  A BasicType links to a parent type and also
 * has a set of members that it provides.
 */

#include "Common.h"
#include "EvalError.h"
#include "Log.h"
#include "Symbol.h"

#include <limits.h>
#include <memory>
#include <string>
#include <vector>

namespace eval {

class Object;

// Pure virtual base class
class Type {
public:
  Type(Log& log)
    : m_log(log) {}
  virtual ~Type() {}
  virtual void addMemberType(const std::string& name, std::auto_ptr<Type> type) = 0;
  virtual std::auto_ptr<Type> getMemberType(const std::string& name) const = 0;
  virtual std::auto_ptr<Object> makeDefaultObject(const std::string& newName) const = 0;
  virtual bool isParentOf(const Type& child) const = 0;
  virtual bool takesArgs(const paramtype_vec& paramtypes) const = 0;
  // Returns a duplicate of the Type.  Caller takes ownership.
  virtual std::auto_ptr<Type> duplicate() const = 0;
  virtual std::string getName() const = 0;
  virtual std::string print() const = 0;
  virtual bool isRoot() const { return false; }

protected:
  Log& m_log;
};

// RootType is the Type of std.object, the root of all objects.
// It has members but no parent type.
class RootType : public Type {
public:
  RootType(Log& log) : Type(log) {}
  virtual ~RootType();
  virtual void addMemberType(const std::string& name, std::auto_ptr<Type> type);
  virtual bool hasMember(const std::string& name) const;
  virtual std::auto_ptr<Type> getMemberType(const std::string& name) const;
  virtual std::auto_ptr<Object> makeDefaultObject(const std::string& newName) const;
  virtual bool isParentOf(const Type& child) const;
  virtual bool takesArgs(const paramtype_vec& paramtypes) const;
  virtual std::auto_ptr<Type> duplicate() const;
  virtual std::string getName() const;
  virtual std::string print() const;
  virtual bool isRoot() const { return true; }
private:
  typedef std::map<std::string,Type*> member_map;
  typedef std::pair<std::string,Type*> member_pair;
  typedef member_map::const_iterator member_iter;
  member_map m_members;
};

// Builtin type representing a function that takes certain arguments
class FunctionArgsType : public Type {
public:
  FunctionArgsType(Log& log, const Symbol& froot, const arg_vec& args);
  FunctionArgsType(Log& log, const Symbol& froot, const argspec_vec& args);
  virtual ~FunctionArgsType();
  virtual void addMemberType(const std::string& name, std::auto_ptr<Type> type);
  virtual std::auto_ptr<Type> getMemberType(const std::string& name) const;
  virtual std::auto_ptr<Object> makeDefaultObject(const std::string& newName) const;
  virtual bool isParentOf(const Type& child) const;
  virtual bool takesArgs(const paramtype_vec& paramtypes) const;
  virtual std::auto_ptr<Type> duplicate() const;
  virtual std::string getName() const;
  virtual std::string print() const;
private:
  const Symbol& m_froot;    // @
  argspec_vec m_args;
};

// Builtin type representing a function with a specific return type
class FunctionReturnsType : public Type {
public:
  FunctionReturnsType(Log& log,
                      const Symbol& froot,
                      std::auto_ptr<Type> returns)
    : Type(log), m_froot(froot), m_returns(returns) {}
  virtual void addMemberType(const std::string& name, std::auto_ptr<Type> type);
  virtual std::auto_ptr<Type> getMemberType(const std::string& name) const;
  virtual std::auto_ptr<Object> makeDefaultObject(const std::string& newName) const;
  virtual bool isParentOf(const Type& child) const;
  virtual bool takesArgs(const paramtype_vec& paramtypes) const;
  virtual std::auto_ptr<Type> duplicate() const;
  virtual std::string getName() const;
  virtual std::string print() const;
private:
  const Symbol& m_froot;    // @
  std::auto_ptr<Type> m_returns;
};

// A BasicType has a parent Symbol and maybe some member types.
class BasicType : public Type {
public:
  BasicType(Log& log, const Symbol& parent)
    : Type(log), m_parent(parent) {}
  ~BasicType();
  virtual void addMemberType(const std::string& name, std::auto_ptr<Type> type);
  virtual bool hasMember(const std::string& name) const;
  virtual std::auto_ptr<Type> getMemberType(const std::string& name) const;
  virtual std::auto_ptr<Object> makeDefaultObject(const std::string& newName) const;
  virtual bool isParentOf(const Type& child) const;
  virtual bool takesArgs(const paramtype_vec& paramtypes) const;
  virtual std::auto_ptr<Type> duplicate() const;
  virtual std::string getName() const;
  virtual std::string print() const;
  const Symbol& parent() const { return m_parent; }
private:
  typedef std::map<std::string,Type*> member_map;
  typedef std::pair<std::string,Type*> member_pair;
  typedef member_map::const_iterator member_iter;
  typedef std::vector<std::string> select_vec;
  typedef select_vec::const_iterator select_iter;
  const Symbol& m_parent;
  select_vec m_select;  // nested member names to lookup in the parent
  member_map m_members;
};

// AndType:  a&b
class AndType : public Type {
public:
  AndType(Log& log, const Type& left, const Type& right)
    : Type(log), m_left(left.duplicate()), m_right(right.duplicate()) {}
  AndType(Log& log, std::auto_ptr<Type> left, std::auto_ptr<Type> right)
    : Type(log), m_left(left), m_right(right) {}
  const Type& left() const { return *m_left.get(); }    // must exist
  const Type& right() const { return *m_right.get(); }  // must exist
  virtual void addMemberType(const std::string& name, std::auto_ptr<Type> type);
  virtual std::auto_ptr<Type> getMemberType(const std::string& name) const;
  virtual std::auto_ptr<Object> makeDefaultObject(const std::string& newName) const;
  virtual bool isParentOf(const Type& child) const;
  virtual bool takesArgs(const paramtype_vec& paramtypes) const;
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
  static std::auto_ptr<Type> OrUnion(Log& log, const Type& a, const Type& b);

  OrType(Log& log, const Type& left, const Type& right)
    : Type(log), m_left(left.duplicate()), m_right(right.duplicate()) {}
  const Type& left() const { return *m_left.get(); }
  const Type& right() const { return *m_right.get(); }
  virtual void addMemberType(const std::string& name, std::auto_ptr<Type> type);
  virtual std::auto_ptr<Type> getMemberType(const std::string& name) const;
  virtual std::auto_ptr<Object> makeDefaultObject(const std::string& newName) const;
  virtual bool isParentOf(const Type& child) const;
  virtual bool takesArgs(const paramtype_vec& paramtypes) const;
  virtual std::auto_ptr<Type> duplicate() const;
  virtual std::string getName() const;
  virtual std::string print() const;
private:
  std::auto_ptr<Type> m_left;
  std::auto_ptr<Type> m_right;
};

}

#endif // _Type_h_
