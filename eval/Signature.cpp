// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Signature.h"

using namespace eval;

/* SignatureScore */

/*
bool SignatureScore::isBetterThan(const SignatureScore& rhs) const {
  if (rhs.m_scores.size() != m_scores.size()) {
    throw EvalError("Cannot compare SignatureScores for signatures with differing number of arguments");
  }
  for (int i=0; i < m_scores.size(); ++i) {
    if (m_scores.at(i) < rhs.m_scores.at(i)) {
      return true;
    } else if (m_scores.at(i) > rhs.m_scores.at(i)) {
      return false;
    }
  }
  return false; // equal scores
}
*/

/* Signature */

bool Signature::isEquivalentTo(const Signature& rhs) const {
  // TODO fix this to support named or optional arguments
  // TODO eventually we'll want smarter dispatch, where this will do something
  // like areArgsEquivalent() (not Identical, that's not correct).
  return m_args.size() == rhs.m_args.size();
}

/*
bool Signature::areArgsIdentical(const Signature& rhs) const {
  if (m_args.size() != rhs.m_args.size()) return false;
  for (size_t i=0; i < m_args.size(); ++i) {
    if (!m_args.at(i).isTypeIdentical(rhs.m_args.at(i))) {
      return false;
    }
  }
  return true;
}
*/

// Check if the caller's list of argument types is compatible with this
// function signature.  Note that m_args is an argspec_list while rhs_args is a
// type_list.
bool Signature::areArgsCompatible(const type_list& rhs_args) const {
  return m_args.size() == rhs_args.size();
  /*
  if (m_args.size() != rhs_args.size()) return false;
  for (size_t i=0; i < m_args.size(); ++i) {
    if (!m_args.at(i).isTypeCompatible(rhs_args.at(i))) {
      return false;
    }
  }
  return true;
  */
}

/*
SignatureScore Signature::compatibilityScore(const argtype_list& rhs_args) const {
  SignatureScore score;
  if (m_args.size() != rhs_args.size()) {
    throw EvalError("Must have same # args in order to compute a Signature compatibility score");
  }
  for (size_t i=0; i < m_args.size(); ++i) {
    score.addTypeScore(m_args.at(i).compatibilityScore(rhs_args.at(i)));
  }
  return score;
}
*/
