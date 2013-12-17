// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Function.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace eval;

Function::Function(Log& log, const string& name, auto_ptr<Type> type,
                   Signature initialSignature)
  : Object(log, name, type) {
  m_signatures.push_back(initialSignature);
}

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

bool Function::takesArgs(const type_list& args) const {
  for (signature_iter i = m_signatures.begin(); i != m_signatures.end(); ++i) {
    if (i->areArgsCompatible(args)) {
      return true;
    }
  }
  return false;
}

auto_ptr<Type> Function::getPossibleReturnTypes(const type_list& args) const {
  if (!takesArgs(args)) {
    throw EvalError("Function " + print() + " does not take these args");
  }
  auto_ptr<Type> returnTypes(NULL);
  for (signature_iter i = m_signatures.begin(); i != m_signatures.end(); ++i) {
    if (i->areArgsCompatible(args)) {
      returnTypes.reset(i->getReturnType());
      return returnTypes;
      /*
      if (!returnTypes.get()) {
        returnTypes.reset(i->getReturnType()->duplicate());
      } else {
        Type* rt = i->getReturnType(); //...  void??  uhhh...
        if (!rt) {
          throw EvalError("Void-returning functions not yet supported");
        }
        returnTypes.reset(OrUnion(*returnTypes.get(), *i->getReturnType()));
      }
      */
    }
  }
  return returnTypes;
}

auto_ptr<Object> Function::call(const object_list& args) const {
  // TODO
  return auto_ptr<Object>(NULL);
}


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
