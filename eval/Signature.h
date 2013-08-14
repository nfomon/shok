// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Signature_h_
#define _Signature_h_

/* Function signatures (arguments and return types)
 *
 * TODO: Describe pattern-matching signature lookups here
 */

#include "Log.h"
#include "Type.h"

#include <string>
#include <vector>

namespace eval {

// The specification of a function argument; part of a function signature
// definition.
class ArgSpec {
public:
  ArgSpec(std::string name,
          Type* type,
          Object* defaultValue,
          bool optional = false)
    : m_name(name),
      m_type(type) {}
      //m_defaultValue(defaultValue),
      //m_optional(optional) {}

  /*
  bool isTypeIdentical(const ArgSpec& rhs) const {
    return rhs.m_type == m_type;
  }
  bool isTypeCompatible(const Type* rhs) const {
    return m_type->isCompatible(rhs);
  }
  TypeScore compatibilityScore(const Type* rhs) const {
    return m_type->compatibilityScore(rhs);
  }
  */

private:
  std::string m_name;
  Type* m_type;
  //Object* m_defaultValue;
  //bool m_optional;
};

// Argument list definition used in function signatures
typedef std::vector<ArgSpec> argspec_list;
typedef argspec_list::const_iterator argspec_iter;

// List of types of arguments that a caller wants to provide
typedef std::vector<const Type*> type_list;
typedef type_list::const_iterator type_iter;

// List of actual Object* arguments provided by a caller
typedef std::vector<Object*> object_list;
typedef object_list::const_iterator object_iter;

/*
class SignatureScore {
public:
  SignatureScore() {}
  void addTypeScore(TypeScore score) {
    m_scores.push_back(score);
  }
  bool isBetterThan(const SignatureScore& rhs) const;
private:
  std::vector<TypeScore> m_scores;
};
*/

class Signature {
public:
  Signature(argspec_list args, Type* returnType)
    : m_args(args), m_returnType(returnType) {}

  Type* getReturnType() const { return m_returnType; }

  bool isEquivalentTo(const Signature& rhs) const;
  //bool areArgsIdentical(const Signature& rhs) const;
  bool areArgsCompatible(const type_list& rhs_args) const;
  //SignatureScore compatibilityScore(const argspec_list& rhs_args) const;

private:
  argspec_list m_args;
  Type* m_returnType;
};

};

#endif // _Signature_h_
