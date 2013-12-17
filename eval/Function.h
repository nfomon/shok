// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Function_h_
#define _Function_h_

/* Function
 *
 * All methods are Functions, and all Functions are methods.  woosh.
 * Also, Functions are Objects.
 *
 * A function may have multiple signatures, but this Function object represents
 * exactly one of them.  Here we add only a single member to our ObjectStore, a
 * builtin-Block-type that holds the function body.  When this Function is &'d
 * together with other Functions or Objects, this will be merged and will
 * conflict if there's a duplicate builtin-function-taking-compatible-args.
 *
 * Two signatures on the same function, for now, must have different #
 * arguments.  This will eventually be relaxed to: lining up the arguments, no
 * OR-item from one can be identical to an OR-item of the other.   e.g.
 * @(A|B)&X  <=>  @(C|A)&X  (not allowed) Different overloads may have the same
 * or different return types.
 *
 * The function does not take ownership of any Type* or Object* it is given as
 * part of an Arg.  These must and will outlive the Function.
 *
 * Note that a function that "returns void" does not leave its returntype NULL,
 * it actually should have a void object that presently does not exist...
 */

#include "Log.h"
#include "Signature.h"
#include "Type.h"
#include "Variable.h"

#include <memory>
#include <string>

namespace eval {

class Block;

class Function : public Object {
public:
  // What is the function's type?  something like @(A)->B  ?  or just @->B?  or
  // just @?
  // Answer:  @(A) & @->B which both have type @
  Function(Log& log, const std::string& name, std::auto_ptr<Type> type,
           Signature initialSignature);

  void addSignature(Signature signature);

  bool takesArgs(const type_list& args) const;

  // Returns the best-match signature for the given arg-type list, or NULL if
  // it doesn't match
  // Who wants to call this and why??  We should give them what they actually
  // want, which I doubt is a Signature...  for now it's just Operator wants
  // the possible return type(s) for the best matching signature.
  // ... whatever just do this for now
  // Wait, if we're only providing rhs types (which could be from say
  // @()->A|B), then we may not have a single best signature.  If the possible
  // return types are requested, then we'll need to find all the possible
  // best-matching signatures and get their return-type OR-union.
  //const Signature* getSignature(const type_list& args) const;
  std::auto_ptr<Type> getPossibleReturnTypes(const type_list& args) const;

  std::auto_ptr<Object> call(const object_list& args) const;

private:
  typedef std::vector<Signature> signature_list;
  typedef signature_list::const_iterator signature_iter;

  signature_list m_signatures;
  Block* m_body;  // ownership of this?? nah make it a legit member!
};

};

#endif // _Function_h_
