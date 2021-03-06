// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Function.h"

#include "Args.h"
#include "CompileError.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace compiler;

// Determine the Function's type before we look into its { child scope
void Function::initChild(Node* child) {
  log.debug("Function " + print() + " initChild " + child->print());
  Args* args = dynamic_cast<Args*>(child);
  Returns* returns = dynamic_cast<Returns*>(child);
  Block* body = dynamic_cast<Block*>(child);
  if (!args && !returns && !body) {
    throw CompileError("Function " + print() + " initChild " + child->print() + " child is inappropriate");
  } else if ((args && returns) || (args && body) || (returns && body)) {
    throw CompileError("Function " + print() + " initChild " + child->print() + " child fits multiple child types");
  } else if ((args && m_args) || (returns && m_returns) || (body && m_body)) {
    throw CompileError("Function " + print() + " initChild " + child->print() + " already has that child");
  } else if (args && (m_returns || m_body)) {
    throw CompileError("Function " + print() + " initChild " + child->print() + " cannot set args when it already has returns or body");
  } else if (returns && m_body) {
    throw CompileError("Function " + print() + " initChild " + child->print() + " cannot set returns when it already has body");
  }
  if (args) {
    m_args = args;
  } else if (returns) {
    m_returns = returns;
  } else if (body) {
    m_body = body;
    computeType();
  }
}

void Function::setup() {
  if (children.size() > 3) {
    throw CompileError("Function " + print() + " must have <= 2 children");
  }
  if (m_body) {
    m_body->defer();
  } else {
    computeType();
  }
}

void Function::compile() {
  if (m_preparedArgs) {
    parentScope->commitFirst();
    m_preparedArgs = false;
  }
  if (m_preparedReturns) {
    parentScope->commitFirst();
    m_preparedReturns = false;
  }
}

arg_vec Function::getArgs() const {
  if (!m_args) {
    return arg_vec();
  }
  return m_args->getArgs();
}

auto_ptr<Object> Function::makeObject(const string& newName) {
  if (!isSetup || !isCompiled) {
    throw CompileError("Cannot make object from Function " + print() + " before it is setup and compiled");
  } else if (m_isObjectified) {
    throw CompileError("Cannot make object from Function " + print() + " that has already been made into an object");
  }
  const Symbol* functionSymbol = root->getScope()->getSymbol("@");
  if (!functionSymbol || !functionSymbol->object.get()) {
    throw CompileError("Cannot find the @ object");
  }
  auto_ptr<Object> o(new Object(log, newName));
  const arg_vec* args = NULL;
  if (m_args) {
    args = &m_args->getArgs();
  }
  auto_ptr<Type> returnType(NULL);
  if (m_returns) {
    returnType = m_returns->getType();
  }
  // Carefully transfer body (code block) to the function object
  auto_ptr<Block> body(m_body);
  m_body = NULL;
  children.pop_back();
  log.debug("Creating method object for Function " + print());
  o->newMethod(args, returnType, body);
  log.debug("methodness done");
  m_isObjectified = true;
  return o;
}

void Function::computeType() {
  if (m_type.get()) {
    throw CompileError("Cannot compute type of Function " + print() + " that already has a type");
  }
  const Symbol* p_function = root->getScope()->getSymbol("@");
  if (!p_function) {
    throw CompileError("Cannot find symbol for the @ object");
  }
  log.debug("Computing type of function " + print());
  const Symbol& function = *p_function;
  // Set builtin-function-type @(type1,type2,...) as a parent type
  if (m_args) {
    m_type.reset(new FunctionArgsType(log, function, m_args->getArgs()));
  }
  // Set builtin-function-type @->return_type as a parent type
  if (m_returns) {
    auto_ptr<Type> returnType(new FunctionReturnsType(log, function, m_returns->getType()));
    if (m_type.get()) {
      m_type.reset(new AndType(log, m_type, returnType));
    } else {
      m_type = returnType;
    }
  }
  // With no args or return type, our parent is just @
  if (!m_args && !m_returns) {
    m_type.reset(new BasicType(log, function));
  } else if (!m_type.get()) {
    throw CompileError("Function " + print() + " failed to compute a type for itself");
  }
  log.info("Type of Function " + print() + ": " + m_type->print() + " -- " + m_type->getName());
}

/*
void Function::addSignature(Signature signature) {
  // Disallow exact matches of arg lists
  // For now: signatures must not have same # arguments
  for (signature_iter i = m_signatures.begin(); i != m_signatures.end(); ++i) {
    if (i->isEquivalentTo(signature)) {
      throw CompileError("Cannot (yet) overload function signature with same number of arguments");
    }
  }
  m_signatures.push_back(signature);
}
*/


/*
const Signature* Function::getSignature(const argtype_list& args) const {
  if (m_signatures.size() < 1) {
    throw CompileError("Function must have at least one signature");
  }
  // For now, just pick the signature with the matching # arguments
  for (signature_iter i = m_signatures.begin(); i != m_signatures.end(); ++i) {
    if (i->areArgsCompatible(args)) {
      return &*i;
    }
  }
  return NULL;
*/

  /*
  const Signature* bestSignature = NULL;
  SignatureScore bestScore;
  for (signature_iter i = m_signatures.begin(); i != m_signatures.end(); ++i) {
    if (i->areArgsCompatible(args)) {
      if (!bestSignature) {
        bestSignature = &*i;
        bestScore = i->compatibilityScore(args);
      } else {
        SignatureScore score = i->compatibilityScore(args);
        if (score.isBetterThan(bestScore)) {
          bestSignature = &*i;
          bestScore = score;
        }
      }
    }
  }
  return bestSignature;
  */
/*
}
*/
