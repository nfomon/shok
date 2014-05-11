// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Type.h"

#include "SymbolTable.h"

#include "util/Util.h"

#include <memory>
#include <string>
#include <utility>
using std::auto_ptr;
using std::string;

using namespace compiler;

/* RootType */

RootType::RootType() {
  // object members
  //m_members.insert("->str", auto_ptr<Type());   // TODO circular!
}

void RootType::addMember(const string& name, auto_ptr<Type> type) {
  m_members.insert(name, type);
}

const Type* RootType::findMember(const string& name) const {
  symbol_iter s = m_members.find(name);
  if (s != m_members.end()) return s->second;
  return NULL;
}

auto_ptr<Type> RootType::duplicate() const {
  RootType* rt = new RootType();
  auto_ptr<Type> art(rt);
  rt->m_members = m_members;
  return art;
}

std::string RootType::defaultValueBytecode() const {
  return " object";
}

/* BasicType */

BasicType::BasicType(auto_ptr<Type> parent, const string& parentName)
  : m_parent(parent),
    m_parentName(parentName) {
  if (!m_parent.get()) {
    throw CompileError("Cannot create BasicType that has NULL parent '" + parentName + "'");
  }
}

void BasicType::addMember(const string& name, auto_ptr<Type> type) {
  m_members.insert(name, type);
}

const Type* BasicType::findMember(const string& name) const {
  symbol_iter s = m_members.find(name);
  if (s != m_members.end()) return s->second;
  return m_parent->findMember(name);
}

auto_ptr<Type> BasicType::duplicate() const {
  BasicType* bt = new BasicType(m_parent->duplicate(), m_parentName);
  auto_ptr<Type> abt(bt);
  bt->m_members = m_members;
  return abt;
}

std::string BasicType::defaultValueBytecode() const {
  std::string bc(" (object");
  for (symbol_iter i = m_members.begin(); i != m_members.end(); ++i) {
    bc += " (member " + i->first + " " + i->second->defaultValueBytecode() + ")";
  }
  bc += ")";
  return bc;
}

bool BasicType::isParentOf(const Type& child) const {
  const BasicType* basicType = dynamic_cast<const BasicType*>(&child);
  const AndType* andType = dynamic_cast<const AndType*>(&child);
  const OrType* orType = dynamic_cast<const OrType*>(&child);
  if (basicType) {
    // If we're a parent, then nevermind what members the rhs has.  Just check
    // if our parent type is its parent type, and walk up its tree.
    // TODO we need a better concept of type-identity!!  (name:scope_id or w/e)
    if (m_parent.get() == &basicType->parent()) {
      // If we extended our parent with any members, the child had better have
      // them too
      for (symbol_iter i = m_members.begin(); i != m_members.end(); ++i) {
        if (!basicType->findMember(i->first)) {
          return false;
        }
        if (!i->second->isParentOf(*basicType->findMember(i->first))) {
          return false;
        }
      }
      return true;
    }
    return isParentOf(basicType->parent());
  } else if (andType) {
    return isParentOf(andType->left()) || isParentOf(andType->right());
  } else if (orType) {
    return isParentOf(andType->left()) && isParentOf(andType->right());
  }
  return false;
}

/* ArgsType */

ArgsType::ArgsType(const Type& froot, const boost::ptr_vector<Type>& args)
  : m_froot(froot) {
  for (boost::ptr_vector<Type>::const_iterator i = args.begin();
       i != args.end(); ++i) {
    m_args.push_back(i->duplicate());
  }
}

void ArgsType::addMember(const string& name, auto_ptr<Type> type) {
  throw CompileError("Cannot add member " + name + " to an ArgsType");
}

const Type* ArgsType::findMember(const string& name) const {
  return m_froot.findMember(name);
}

auto_ptr<Type> ArgsType::duplicate() const {
  return auto_ptr<Type>(new ArgsType(m_froot, m_args));
}

std::string ArgsType::defaultValueBytecode() const {
  return m_froot.defaultValueBytecode();
}

bool ArgsType::isParentOf(const Type& child) const {
  const ArgsType* argsType = dynamic_cast<const ArgsType*>(&child);
  const BasicType* basicType = dynamic_cast<const BasicType*>(&child);
  const AndType* andType = dynamic_cast<const AndType*>(&child);
  const OrType* orType = dynamic_cast<const OrType*>(&child);
  if (argsType) {
    if (argsType->m_args.size() != m_args.size()) {
      return false;
    }
    // each of the child's args is a parent of our corresponding arg
    for (size_t i=0; i < m_args.size(); ++i) {
      if (!argsType->m_args.at(i).isParentOf(m_args.at(i))) {
        return false;
      }
    }
    return true;
  } else if (basicType) {
    return isParentOf(basicType->parent());
  } else if (andType) {
    return isParentOf(andType->left()) || isParentOf(andType->right());
  } else if (orType) { 
    return isParentOf(orType->left()) && isParentOf(orType->right());
  }

  return false;
}

/* ReturnsType */

ReturnsType::ReturnsType(const Type& froot, auto_ptr<Type> returns)
  : m_froot(froot),
    m_returns(returns) {
}

void ReturnsType::addMember(const string& name, auto_ptr<Type> type) {
  throw CompileError("Cannot add member " + name + " to a ReturnsType");
}

const Type* ReturnsType::findMember(const string& name) const {
  return m_froot.findMember(name);
}

auto_ptr<Type> ReturnsType::duplicate() const {
  return auto_ptr<Type>(new ReturnsType(m_froot, m_returns->duplicate()));
}

std::string ReturnsType::defaultValueBytecode() const {
  return m_froot.defaultValueBytecode();
}

bool ReturnsType::isParentOf(const Type& child) const {
  const ReturnsType* retType = dynamic_cast<const ReturnsType*>(&child);
  const BasicType* basicType = dynamic_cast<const BasicType*>(&child);
  const AndType* andType = dynamic_cast<const AndType*>(&child);
  const OrType* orType = dynamic_cast<const OrType*>(&child);
  if (retType) {
    // our return type is a parent of the child's return type
    return m_returns->isParentOf(*retType->m_returns);
  } else if (basicType) {
    return isParentOf(basicType->parent());
  } else if (andType) {
    return isParentOf(andType->left()) || isParentOf(andType->right());
  } else if (orType) {
    return isParentOf(orType->left()) && isParentOf(orType->right());
  }

  return false;
}

/* AndType */

AndType::AndType(auto_ptr<Type> left, auto_ptr<Type> right)
  : m_left(left),
    m_right(right) {
  if (!left.get() || !right.get()) {
    throw CompileError("Cannot create AndType with missing children");
  }
  // TODO validate that left and right don't have colliding member names from
  // different parents
}

void AndType::addMember(const string& name, auto_ptr<Type> type) {
  throw CompileError("Cannot add member " + name + " to an AndType");
}

const Type* AndType::findMember(const string& name) const {
  const Type* type = m_left->findMember(name);
  if (type) return type;
  return m_right->findMember(name);
}

auto_ptr<Type> AndType::duplicate() const {
  return auto_ptr<Type>(new AndType(m_left->duplicate(), m_right->duplicate()));
}

std::string AndType::defaultValueBytecode() const {
  // TODO we need a sane merge of the members from the children (want a member
  // that comes through both via a shared parent to appear only once)
  // For now we just pretend the VM will figure it out.
  return " (and (" + m_left->defaultValueBytecode() + ") (" + m_right->defaultValueBytecode() + ")";
}

bool AndType::isParentOf(const Type& child) const {
  return m_left->isParentOf(child) && m_right->isParentOf(child);
}

/* OrType */

auto_ptr<Type> OrType::OrUnion(const Type& a, const Type& b) {
  // TODO
/*
  if (a.isParentOf(b)) return a.duplicate();
  if (b.isParentOf(a)) return b.duplicate();
  return auto_ptr<Type>(new OrType(log, a, b));
*/
  return auto_ptr<Type>(NULL);
}

OrType::OrType(auto_ptr<Type> left, auto_ptr<Type> right)
  : m_left(left),
    m_right(right) {
  if (!left.get() || !right.get()) {
    throw CompileError("Cannot create OrType with missing children");
  }
  // TODO anything to validate here?  can left and right be the same type?
}

void OrType::addMember(const string& name, auto_ptr<Type> type) {
  throw CompileError("Cannot add member " + name + " to an OrType");
}

const Type* OrType::findMember(const string& name) const {
  const Type* left = m_left->findMember(name);
  const Type* right = m_right->findMember(name);
  if (!left || !right) {
    throw CompileError("A child of OrType is missing member " + name);
  }
  // TODO we need findMember() to return an auto_ptr, so the receiver can hold
  // the newly-created OrUnion.
  //return OrUnion(*left, *right);
  return NULL;
}

auto_ptr<Type> OrType::duplicate() const {
  return auto_ptr<Type>(new OrType(m_left->duplicate(), m_right->duplicate()));
}

std::string OrType::defaultValueBytecode() const {
  throw CompileError("Cannot create object for OrType");
}

bool OrType::isParentOf(const Type& child) const {
  return m_left->isParentOf(child) || m_right->isParentOf(child);
}
