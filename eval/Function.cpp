// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Function.h"

#include "Args.h"
#include "EvalError.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace eval;

void Function::setup() {
  if (children.size() > 2) {
    throw EvalError("Function " + print() + " must have <= 2 children");
  }
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    if (m_body) {
      throw EvalError("Function " + print() + " setup found children after the function body");
    }
    if (!m_args) {
      m_args = dynamic_cast<Args*>(*i);
      if (m_args) continue;
    }
    if (!m_returns) {
      m_returns = dynamic_cast<Returns*>(*i);
      if (m_returns) continue;
    }
    if (!m_body) {
      m_body = dynamic_cast<Block*>(*i);
    }
  }
  computeType();
}

void Function::evaluate() {
}

void Function::computeType() {
  if (m_type.get()) {
    throw EvalError("Cannot compute type of Function " + print() + " that already has a type");
  }
  const Object* p_function = parentScope->getObject("@");
  if (!p_function) {
    throw EvalError("Cannot find the @ object.");
  }
  const Object& function = *p_function;
  // Lookup or construct @(type1,type2,...) parent type
  if (m_args) {
    string function_args_name;
    Args::args_vec args = m_args->getArgs();
    for (Args::args_iter i = args.begin(); i != args.end(); ++i) {
      if (function_args_name.empty()) {
        function_args_name = "@(" + (*i)->getName();
      } else {
        function_args_name += "," + (*i)->getName();
      }
    }
    function_args_name += ")";
    const Object* function_args = parentScope->getObject(function_args_name);
    if (!function_args) {
      auto_ptr<Type> funcType(new BasicType(function));
      function_args = &parentScope->newObject(function_args_name, funcType);
    }
    m_type.reset(new BasicType(*function_args));
  }
  // Lookup or construct @->(return type) parent type
  if (m_returns) {
    string function_returns_name = "@->" + m_returns->getName();
    const Object* function_returns = parentScope->getObject(function_returns_name);
    if (!function_returns) {
      auto_ptr<Type> funcType(new BasicType(function));
      function_returns = &parentScope->newObject(function_returns_name, funcType);
    }
    Type* argsType = m_type.get();
    if (argsType) {
      auto_ptr<Type> returnType(new BasicType(*function_returns));
      m_type.reset(new AndType(m_type, returnType));
    } else {
      m_type.reset(new BasicType(*function_returns));
    }
  }
  // With no args or return type, our parent is just @
  if (!m_args && !m_returns) {
    m_type.reset(new BasicType(function));
  } else if (!m_type.get()) {
    throw EvalError("Function " + print() + " failed to compute a type for itself");
  }
}

/*
void Function::addSignature(Signature signature) {
  // Disallow exact matches of arg lists
  // For now: signatures must not have same # arguments
  for (signature_iter i = m_signatures.begin(); i != m_signatures.end(); ++i) {
    if (i->isEquivalentTo(signature)) {
      throw EvalError("Cannot (yet) overload function signature with same number of arguments");
    }
  }
  m_signatures.push_back(signature);
}
*/


/*
const Signature* Function::getSignature(const argtype_list& args) const {
  if (m_signatures.size() < 1) {
    throw EvalError("Function must have at least one signature");
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
