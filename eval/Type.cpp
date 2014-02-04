// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Type.h"

#include "Arg.h"
#include "Object.h"

#include "boost/lexical_cast.hpp"

#include <algorithm>
#include <string>
#include <utility>
using std::auto_ptr;
using std::make_pair;
using std::string;

using namespace eval;

/* RootType */

RootType::~RootType() {
  for (member_iter i = m_members.begin(); i != m_members.end(); ++i) {
    delete i->second;
  }
}

void RootType::addMemberType(const string& name, auto_ptr<Type> type) {
  m_members.insert(make_pair(name, type.release()));
}

bool RootType::hasMember(const string& name) const {
  return m_members.find(name) != m_members.end();
}

auto_ptr<Type> RootType::getMemberType(const string& name) const {
  member_iter i = m_members.find(name);
  if (m_members.end() == i) {
    return auto_ptr<Type>(NULL);
  }
  return i->second->duplicate();
}

auto_ptr<Object> RootType::makeDefaultObject(const string& newName) const {
  auto_ptr<Object> o(new Object(m_log, newName));
  // Careful here!  Possibility of infinite loops!
  for (member_iter i = m_members.begin(); i != m_members.end(); ++i) {
    o->newMember(i->first, i->second->makeDefaultObject(i->first));
  }
  return o;
}

bool RootType::isParentOf(const Type& child) const {
  return true;
}

bool RootType::takesArgs(const paramtype_vec& paramtypes) const {
  return false;
}

auto_ptr<Type> RootType::duplicate() const {
  RootType* rt = new RootType(m_log);
  auto_ptr<Type> art(rt);
  for (member_iter i = m_members.begin(); i != m_members.end(); ++i) {
    rt->m_members.insert(make_pair(i->first, i->second->duplicate().release()));
  }
  return art;
}

string RootType::getName() const {
  return "<no type>";
}

string RootType::print() const {
  return "<no type>";
}

/* FunctionArgsType */

FunctionArgsType::FunctionArgsType(Log& log,
                                   const Symbol& froot,
                                   const arg_vec& args)
  : Type(log), m_froot(froot) {
  for (arg_iter i = args.begin(); i != args.end(); ++i) {
    m_args.push_back(new ArgSpec((*i)->getName(), *(*i)->getType().release()));
  }
}

FunctionArgsType::FunctionArgsType(Log& log,
                                   const Symbol& froot,
                                   const argspec_vec& args)
  : Type(log), m_froot(froot) {
  for (argspec_iter i = args.begin(); i != args.end(); ++i) {
    m_args.push_back((*i)->duplicate().release());
  }
}

FunctionArgsType::~FunctionArgsType() {
  for (argspec_iter i = m_args.begin(); i != m_args.end(); ++i) {
    delete *i;
  }
}

void FunctionArgsType::addMemberType(const string& name, auto_ptr<Type> type) {
  throw EvalError("Cannot add member type for " + name + " to FunctionArgsType " + print());
}

auto_ptr<Type> FunctionArgsType::getMemberType(const string& name) const {
  return m_froot.type->getMemberType(name);
}

auto_ptr<Object> FunctionArgsType::makeDefaultObject(const std::string& newName) const {
  return m_froot.type->makeDefaultObject(newName);
}

bool FunctionArgsType::isParentOf(const Type& child) const {
  const FunctionArgsType* funargsType = dynamic_cast<const FunctionArgsType*>(&child);
  const BasicType* basicType = dynamic_cast<const BasicType*>(&child);
  const AndType* andType = dynamic_cast<const AndType*>(&child);
  const OrType* orType = dynamic_cast<const OrType*>(&child);
  if (funargsType) {
    if (funargsType->m_args.size() != m_args.size()) {
      return false;
    }
    // each of the child's args is a parent of our corresponding arg
    for (size_t i=0; i < m_args.size(); ++i) {
      if (!funargsType->m_args.at(i)->type().isParentOf(m_args.at(i)->type())) {
        return false;
      }
    }
    return true;
  } else if (basicType) {
    return isParentOf(*basicType->parent().type);
  } else if (andType) {
    return isParentOf(andType->left()) || isParentOf(andType->right());
  } else if (orType) {
    return isParentOf(orType->left()) && isParentOf(orType->right());
  }

  return false;
}

bool FunctionArgsType::takesArgs(const paramtype_vec& paramtypes) const {
  // compare against m_args, an ArgSpec* vec
  if (paramtypes.size() != m_args.size()) {
    return false;
  }
  for (size_t i=0; i < paramtypes.size(); ++i) {
    if (!m_args.at(i)->type().isParentOf(*paramtypes.at(i))) {
      return false;
    }
  }
  return true;
}

auto_ptr<Type> FunctionArgsType::duplicate() const {
  argspec_vec new_args;
  for (argspec_iter i = m_args.begin(); i != m_args.end(); ++i) {
    new_args.push_back((*i)->duplicate().release());
  }
  return auto_ptr<Type>(new FunctionArgsType(m_log, m_froot, new_args));
}

string FunctionArgsType::getName() const {
  string s = "@(";
  bool first = true;
  for (argspec_iter i = m_args.begin(); i != m_args.end(); ++i) {
    if (first) {
      s += (*i)->type().getName();
      first = false;
    } else {
      s += "," + (*i)->type().getName();
    }
  }
  s += ")";
  return s;
}

string FunctionArgsType::print() const {
  return getName();
}

/* FunctionReturnsType */

void FunctionReturnsType::addMemberType(const string& name, auto_ptr<Type> type) {
  throw EvalError("Cannot add member type for " + name + " to FunctionReturnsType " + print());
}

auto_ptr<Type> FunctionReturnsType::getMemberType(const string& name) const {
  return m_froot.type->getMemberType(name);
}

auto_ptr<Object> FunctionReturnsType::makeDefaultObject(const std::string& newName) const {
  return m_froot.type->makeDefaultObject(newName);
}

bool FunctionReturnsType::isParentOf(const Type& child) const {
  const FunctionReturnsType* funretType = dynamic_cast<const FunctionReturnsType*>(&child);
  const BasicType* basicType = dynamic_cast<const BasicType*>(&child);
  const AndType* andType = dynamic_cast<const AndType*>(&child);
  const OrType* orType = dynamic_cast<const OrType*>(&child);
  if (funretType) {
    // our return type is a parent of the child's return type
    return m_returns->isParentOf(*funretType->m_returns);
  } else if (basicType) {
    return isParentOf(*basicType->parent().type);
  } else if (andType) {
    return isParentOf(andType->left()) || isParentOf(andType->right());
  } else if (orType) {
    return isParentOf(orType->left()) && isParentOf(orType->right());
  }

  return false;
}

bool FunctionReturnsType::takesArgs(const paramtype_vec& paramtypes) const {
  return false;
}

auto_ptr<Type> FunctionReturnsType::duplicate() const {
  return auto_ptr<Type>(new FunctionReturnsType(m_log,
                                                m_froot,
                                                m_returns->duplicate()));
}

string FunctionReturnsType::getName() const {
  return "@->" + m_returns->getName();
}

string FunctionReturnsType::print() const {
  return getName();
}

/* BasicType */

BasicType::~BasicType() {
  for (member_iter i = m_members.begin(); i != m_members.end(); ++i) {
    delete i->second;
  }
}

void BasicType::addMemberType(const string& name, auto_ptr<Type> type) {
  m_members.insert(make_pair(name, type.release()));
}

bool BasicType::hasMember(const string& name) const {
  return m_members.find(name) != m_members.end();
}

auto_ptr<Type> BasicType::getMemberType(const string& name) const {
  member_iter i = m_members.find(name);
  if (m_members.end() == i) {
    return m_parent.type->getMemberType(name);
  }
  return i->second->duplicate();
}

auto_ptr<Object> BasicType::makeDefaultObject(const string& newName) const {
  // TODO it's really not clear what the right logic here is.
  // Let's carefully define what it means for  new x : foo.bar = lol.wut
  if (!m_select.empty()) {
    throw EvalError("BasicType::makeDefaultObject(const string&) does not support selectvecs yet");
  }
  auto_ptr<Object> o;
  if (m_parent.object.get()) {
    o = m_parent.object->clone(newName);
  } else {
    o = m_parent.type->makeDefaultObject(newName);
  }
  for (member_iter i = m_members.begin(); i != m_members.end(); ++i) {
    o->newMember(i->first, i->second->makeDefaultObject(i->first));
  }
  return o;
}

bool BasicType::isParentOf(const Type& child) const {
  const BasicType* basicType = dynamic_cast<const BasicType*>(&child);
  const AndType* andType = dynamic_cast<const AndType*>(&child);
  const OrType* orType = dynamic_cast<const OrType*>(&child);
  if (basicType) {
    // if we're a parent, then nevermind what members the rhs has.  just check
    // if our parent symbol is its parent symbol, and walk up its tree.
    if (&m_parent == &basicType->parent()) {
      // If we extended our parent with any members, the child had better have
      // them too
      for (member_iter i = m_members.begin(); i != m_members.end(); ++i) {
        if (!basicType->hasMember(i->first)) {
          return false;
        }
        if (!i->second->isParentOf(*basicType->getMemberType(i->first))) {
          return false;
        }
      }
      return true;
    }
    return isParentOf(*basicType->parent().type);
  } else if (andType) {
    return isParentOf(andType->left()) || isParentOf(andType->right());
  } else if (orType) {
    return isParentOf(andType->left()) && isParentOf(andType->right());
  }
  return false;
}

bool BasicType::takesArgs(const paramtype_vec& paramtypes) const {
  // TODO also check if we have an op() member??
  return m_parent.type->takesArgs(paramtypes);
}

auto_ptr<Type> BasicType::duplicate() const {
  BasicType* bt = new BasicType(m_log, m_parent);
  auto_ptr<Type> t(bt);
  for (member_iter i = m_members.begin(); i != m_members.end(); ++i) {
    bt->m_members.insert(make_pair(i->first, i->second->duplicate().release()));
  }
  return t;
}

string BasicType::getName() const {
  if (!m_parent.object.get()) {
    return "(uninitialized BasicType:" + m_parent.type->getName() + ")";
  }
  return m_parent.object->getName();
}

string BasicType::print() const {
  if (!m_parent.object.get()) {
    return "(uninitialized BasicType:" + m_parent.type->print() +
           string(m_members.size() > 0 ? " with " +
             boost::lexical_cast<string>(m_members.size()) + " members)" : ")");
  }
  return "(BasicType:" + m_parent.object->print() +
         string(m_members.size() > 0 ? " with " +
           boost::lexical_cast<string>(m_members.size()) + " members)" : ")");
}

/* AndType */

void AndType::addMemberType(const string& name, auto_ptr<Type> type) {
  throw EvalError("Cannot add type of member " + name + " to AndType " + print());
}

auto_ptr<Type> AndType::getMemberType(const string& name) const {
  if (!m_left.get() || !m_right.get()) {
    throw EvalError("Cannot get member " + name + " type from deficient AndType " + print());
  }
  auto_ptr<Type> type = m_left->getMemberType(name);
  if (type.get()) return type;
  return m_right->getMemberType(name);
}

auto_ptr<Object> AndType::makeDefaultObject(const string& newName) const {
  if (!m_left.get() || !m_right.get()) {
    throw EvalError("Cannot make default object for deficient AndType " + print());
  }
  auto_ptr<Object> left = m_left->makeDefaultObject(newName);
  auto_ptr<Object> right = m_right->makeDefaultObject(newName);
  // TODO &-merge the objects
  throw EvalError("AndType::makeDefaultObject(const string&) is unimplemented");
}

bool AndType::isParentOf(const Type& child) const {
  return m_left->isParentOf(child) && m_right->isParentOf(child);
}

bool AndType::takesArgs(const paramtype_vec& paramtypes) const {
  return m_left->takesArgs(paramtypes) || m_right->takesArgs(paramtypes);
}

auto_ptr<Type> AndType::duplicate() const {
  if (!m_left.get() || !m_right.get()) {
    throw EvalError("Cannot duplicate deficient AndType " + print());
  }
  return auto_ptr<Type>(new AndType(m_log, *m_left.get(), *m_right.get()));
}

string AndType::getName() const {
  string name;
  if (dynamic_cast<BasicType*>(m_left.get())) {
    name = m_left->getName() + "&";
  } else {
    name = "(" + m_left->getName() + ")&";
  }
  if (dynamic_cast<BasicType*>(m_right.get())) {
    name += m_right->getName();
  } else {
    name += "(" + m_right->getName() + ")";
  }
  return name;
}

string AndType::print() const {
  return "&(" + m_left->print() + "," + m_right->print() + ")";
}

/* OrType */

auto_ptr<Type> OrType::OrUnion(Log& log, const Type& a, const Type& b) {
  if (a.isParentOf(b)) return a.duplicate();
  if (b.isParentOf(a)) return b.duplicate();
  return auto_ptr<Type>(new OrType(log, a, b));
}

void OrType::addMemberType(const string& name, auto_ptr<Type> type) {
  throw EvalError("Cannot add type of member " + name + " to OrType " + print());
}

auto_ptr<Type> OrType::getMemberType(const string& name) const {
  // It must exist in both children.  We return the best |-union of the member
  // types.
  if (!m_left.get() || !m_right.get()) {
    throw EvalError("Cannot get member type of deficient OrType " + print());
  }
  auto_ptr<Type> leftType(m_left->getMemberType(name));
  auto_ptr<Type> rightType(m_right->getMemberType(name));
  if (!leftType.get() || !rightType.get()) {
    throw EvalError("A child of OrType " + print() + " has a deficient Type.");
  }
  return OrUnion(m_log, *leftType.get(), *rightType.get());
}

auto_ptr<Object> OrType::makeDefaultObject(const string& newName) const {
  throw EvalError("Cannot make default object " + newName + " for OrType " + print());
}

bool OrType::isParentOf(const Type& child) const {
  return m_left->isParentOf(child) || m_right->isParentOf(child);
}

bool OrType::takesArgs(const paramtype_vec& paramtypes) const {
  return m_left->takesArgs(paramtypes) && m_right->takesArgs(paramtypes);
}

auto_ptr<Type> OrType::duplicate() const {
  if (!m_left.get() || !m_right.get()) {
    throw EvalError("Cannot duplicate deficient OrType " + print());
  }
  return auto_ptr<Type>(new OrType(m_log, *m_left.get(), *m_right.get()));
}

string OrType::getName() const {
  string name;
  if (dynamic_cast<BasicType*>(m_left.get())) {
    name = m_left->getName() + "|";
  } else {
    name = "(" + m_left->getName() + ")|";
  }
  if (dynamic_cast<BasicType*>(m_right.get())) {
    name += m_right->getName();
  } else {
    name += "(" + m_right->getName() + ")";
  }
  return name;
}

string OrType::print() const {
  if (!m_left.get() || !m_right.get()) return "|(uninitialized)";
  return "|(" + m_left->print() + "," + m_right->print() + ")";
}
