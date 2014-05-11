// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Type_h_
#define _Type_h_

/* Type */

#include "CompileError.h"
#include "SymbolTable.h"

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/utility.hpp>

#include <memory>
#include <string>
#include <vector>

namespace compiler {

class Type : boost::noncopyable {
public:
  virtual ~Type() {}
  virtual std::auto_ptr<Type> duplicate() const = 0;
  virtual void addMember(const std::string& name, std::auto_ptr<Type> type) = 0;
  virtual const Type* findMember(const std::string& name) const = 0;
  virtual std::string defaultValueBytecode() const = 0;
  virtual bool isParentOf(const Type& child) const = 0;
  //virtual bool takesArgs(const paramtype_vec& paramtypes) const = 0;
  virtual bool isRoot() const { return false; }

protected:
};

class RootType : public Type {
public:
  RootType();
  std::auto_ptr<Type> duplicate() const;
  void addMember(const std::string& name, std::auto_ptr<Type> type);
  const Type* findMember(const std::string& name) const;
  std::string defaultValueBytecode() const;
  bool isParentOf(const Type& child) const { return true; }
  bool isRoot() const { return true; }

private:
  symbol_map m_members;
};

// A BasicType is a type wrapping an existing object.  If x has type
// BasicType(y), then x's immediate parent is y, and x additionally defines the
// members local to the BasicType(y).
class BasicType : public Type {
public:
  BasicType(std::auto_ptr<Type> parent, const std::string& parentName);
  std::auto_ptr<Type> duplicate() const;
  void addMember(const std::string& name, std::auto_ptr<Type> type);
  const Type* findMember(const std::string& name) const;
  std::string defaultValueBytecode() const;
  bool isParentOf(const Type& child) const;

  const Type& parent() const { return *m_parent; }

private:
  std::auto_ptr<Type> m_parent;
  std::string m_parentName;
  symbol_map m_members;
};

// Builtin type representing a function that takes certain arguments
class ArgsType : public Type {
public:
  ArgsType(const Type& froot, const boost::ptr_vector<Type>& args);
  std::auto_ptr<Type> duplicate() const;
  void addMember(const std::string& name, std::auto_ptr<Type> type);
  const Type* findMember(const std::string& name) const;
  std::string defaultValueBytecode() const;
  bool isParentOf(const Type& child) const;

private:
  const Type& m_froot;    // @
  boost::ptr_vector<Type> m_args;
};

// Builtin type representing a function with a specific return type
class ReturnsType : public Type {
public:
  ReturnsType(const Type& froot, std::auto_ptr<Type> returns);
  std::auto_ptr<Type> duplicate() const;
  void addMember(const std::string& name, std::auto_ptr<Type> type);
  const Type* findMember(const std::string& name) const;
  std::string defaultValueBytecode() const;
  bool isParentOf(const Type& child) const;

private:
  const Type& m_froot;    // @
  std::auto_ptr<Type> m_returns;
};

// AndType:  a&b
class AndType : public Type {
public:
  AndType(std::auto_ptr<Type> left, std::auto_ptr<Type> right);
  std::auto_ptr<Type> duplicate() const;
  void addMember(const std::string& name, std::auto_ptr<Type> type);
  const Type* findMember(const std::string& name) const;
  std::string defaultValueBytecode() const;
  bool isParentOf(const Type& child) const;

  const Type& left() const { return *m_left.get(); }
  const Type& right() const { return *m_right.get(); }

private:
  std::auto_ptr<Type> m_left;
  std::auto_ptr<Type> m_right;
};

// OrType:  a|b
class OrType : public Type {
public:
  // Perform the best |-union of two Types
  static std::auto_ptr<Type> OrUnion(const Type& a, const Type& b);

  OrType(std::auto_ptr<Type> left, std::auto_ptr<Type> right);
  std::auto_ptr<Type> duplicate() const;
  void addMember(const std::string& name, std::auto_ptr<Type> type);
  const Type* findMember(const std::string& name) const;
  std::string defaultValueBytecode() const;
  bool isParentOf(const Type& child) const;

  const Type& left() const { return *m_left.get(); }
  const Type& right() const { return *m_right.get(); }

private:
  std::auto_ptr<Type> m_left;
  std::auto_ptr<Type> m_right;
};

inline Type* new_clone(const Type& t) {
  return t.duplicate().release();
}

}

#endif // _Type_h_
