// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Expression.h"

#include "EvalError.h"
#include "Operator.h"
#include "Variable.h"

#include <boost/lexical_cast.hpp>

#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace eval;

void Expression::setup() {
  if (children.size() < 1) {
    throw EvalError("Expression " + print() + " must have at least one child");
  }

  makeOperatorTree();

  Operator* op = dynamic_cast<Operator*>(children.at(0));
  if (op) {
    op->analyzeTree();
  }
  log.info("Expression " + print() + " has analyzed its tree");
  computeType();
}

// Nothing to do here
void Expression::evaluate() {
}

string Expression::cmdText() const {
  if (!isEvaluated) {
    throw EvalError("Cannot get cmdText of unevaluated Expression");
  }
  // TODO: call the resulting object's ->str.escape() (*UNIMPL*)
  //return children.front()->cmdText();
  return "Expression__cmdText unimplemented";
}

Object& Expression::getObject() const {
  if (!isEvaluated) {
    throw EvalError("Cannot get object from Expression " + print() + " before its evaluation");
  }
  Variable* var = dynamic_cast<Variable*>(children.at(0));
  if (var) {
    return var->getObject();
  }
  // TODO: Operator
  throw EvalError("Expression " + print() + " cannot retrieve Object from unsupported child type " + children.at(0)->print());
}

void Expression::makeOperatorTree() {
  log.info("Making operator tree: " + print());
  Operator* root = NULL;
  Variable* var = NULL;
  Operator* unary_op = NULL;  // only prefix unary operators are supported
  Operator* binary_op = NULL;
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    Variable* v = dynamic_cast<Variable*>(*i);
    Operator* o = dynamic_cast<Operator*>(*i);
    if (v) {
      if (var) {
        throw EvalError("Expression " + print() + " duplicate var");
      }
      log.debug(print() + " found var: " + v->print());
      var = v;
    } else if (o) {
      if (o->children.size() != 0) {
        throw EvalError("Expression " + print() + " has operator child that already has children");
      }
      if (binary_op) {
        if (unary_op) {
          if (var) {
            log.debug(print() + " has binary and unary");
            unary_op->addChild(var);
            var = NULL;
            o->addChild(unary_op);
            unary_op = NULL;
            binary_op->addChild(o);
            binary_op = o;
          } else {
            throw EvalError("Expression " + print() + " duplicate operator (binary case)");
          }
        } else if (var) {
          log.debug(print() + " has binary and var");
          o->addChild(var);
          var = NULL;
          binary_op->addChild(o);
          binary_op = o;
        } else {
          log.debug(print() + " has binary and no var");
          unary_op = o;
        }
      } else if (unary_op) {
        if (var) {
          log.debug(print() + " has unary");
          unary_op->addChild(var);
          var = NULL;
          binary_op = o;
          binary_op->addChild(unary_op);
          unary_op = NULL;
          if (!root) {
            root = binary_op;
          }
        } else {
          throw EvalError("Expression " + print() + " duplicate operator (unary case)");
        }
      } else if (var) {
        log.debug(print() + " has var, making first binary");
        binary_op = o;
        binary_op->addChild(var);
        var = NULL;
        if (!root) {
          root = binary_op;
        }
      } else {
        log.debug(print() + " has nada, making unary");
        unary_op = o;
      }
    } else {
      throw EvalError("Expression " + print() + " unexpected child type");
    }
  }
  // finish up
  if (!var) {
    throw EvalError("Expression " + print() + " has dangling operator");
  }
  if (binary_op && unary_op) {
    unary_op->addChild(var);
    var = NULL;
    binary_op->addChild(unary_op);
    unary_op = NULL;
    if (!root) {
      root = binary_op;
    }
  } else if (binary_op) {
    binary_op->addChild(var);
    var = NULL;
    if (!root) {
      root = binary_op;
    }
  } else if (unary_op) {
    if (root) {
      throw EvalError("Expression " + print() + " with single unary operator has surprise dangling root node");
    }
    unary_op->addChild(var);
    var = NULL;
    root = unary_op;
  } else if (root) {
    throw EvalError("Expression " + print() + " has surprise dangling root node");
  }
  children.clear();
  if (root) {
    addChild(root);
    setParents(root);
  } else {
    addChild(var);
  }
  if (children.size() != 1) {
    throw EvalError("Expression " + print() + " made tree but somehow has " + boost::lexical_cast<string>(children.size()) + " children != 1");
  }
}

void Expression::setParents(Node* node) {
  for (child_iter i = node->children.begin(); i != node->children.end(); ++i) {
    (*i)->parent = node;
    setParents(*i);
  }
}

void Expression::computeType() {
  TypedNode* child = dynamic_cast<TypedNode*>(children.at(0));
  if (!child) {
    throw EvalError("Child of Expression must be a TypedNode");
  }
  m_type = child->getType();
  if (!m_type.get()) {
    throw EvalError("Child of Expression must have a Type");
  }
}
