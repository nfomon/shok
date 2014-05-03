// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Operator_h_
#define _Operator_h_

/* Operator */

#include "CompileError.h"
#include "Variable.h"

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace compiler {

/* Operators */

class Operator {
public:
  typedef int Priority;

  Operator(const std::string& name, Priority priority)
    : m_name(name), m_priority(priority) {}
  virtual ~Operator() {}

  virtual std::string methodName() const = 0;

  std::string name() const { return m_name; }
  virtual Priority priority() const { return m_priority; }

protected:
  std::string m_name;
  Priority m_priority;
};

class PrefixOperator : public Operator {
public:
  PrefixOperator(const std::string& name)
    : Operator(name, PrefixPriority(name)) {}

  std::string methodName() const { return "operator" + m_name; }

private:
  static Priority PrefixPriority(const std::string& name);
};

class InfixOperator : public Operator {
public:
  enum Assoc {
    LEFT_ASSOC,
    RIGHT_ASSOC
  };

  InfixOperator(const std::string& name)
    : Operator(name, InfixPriority(name)),
      m_assoc(assoc(name)) {}

  std::string methodName() const { return "operator" + m_name; }

  Assoc assoc() const { return m_assoc; }

private:
  static Priority InfixPriority(const std::string& name);
  static Assoc assoc(const std::string& name);

  Assoc m_assoc;
};

/* Operator tree nodes */

class OperatorNode {
public:
  virtual ~OperatorNode() {}
  virtual Operator::Priority priority() const = 0;
  virtual std::auto_ptr<OperatorNode> duplicate() const = 0;
  boost::shared_ptr<Type> type() const { return m_type; }

  std::string bytecode() const;
protected:
  std::string m_bytecode;
  boost::shared_ptr<Type> m_type;
};

class AtomOperatorNode : public OperatorNode {
public:
  AtomOperatorNode(const Variable& atom) {
    m_bytecode = atom.fullname();
    m_type = atom.type().duplicate();
  }

  Operator::Priority priority() const {
    throw CompileError("Cannot get priority of OperatorNode " + m_bytecode);
  }

  std::auto_ptr<OperatorNode> duplicate() const {
    return std::auto_ptr<OperatorNode>(new AtomOperatorNode(*this));
  }
};

class PrefixOperatorNode : public OperatorNode {
public:
  PrefixOperatorNode(const std::string& name)
    : m_op(name) {}

  void addChild(OperatorNode* child);
  Operator::Priority priority() const { return m_op.priority(); }

  std::auto_ptr<OperatorNode> duplicate() const {
    return std::auto_ptr<OperatorNode>(new PrefixOperatorNode(*this));
  }

private:
  PrefixOperator m_op;
  boost::shared_ptr<OperatorNode> m_child;
};

class InfixOperatorNode : public OperatorNode {
public:
  InfixOperatorNode(const std::string& name)
    : m_op(name) {}

  void addLeft(OperatorNode* left);
  void addRight(OperatorNode* right);
  Operator::Priority priority() const {
    return m_op.priority() +
      (InfixOperator::LEFT_ASSOC == m_op.assoc() ? 0 : 1);
  }

  std::auto_ptr<OperatorNode> duplicate() const {
    return std::auto_ptr<OperatorNode>(new InfixOperatorNode(*this));
  }

private:
  InfixOperator m_op;
  boost::shared_ptr<OperatorNode> m_left;
  boost::shared_ptr<OperatorNode> m_right;
};

inline OperatorNode* new_clone(const OperatorNode& opn) {
  return opn.duplicate().release();
}

}

#endif // _Operator_h_
