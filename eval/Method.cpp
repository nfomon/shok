// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Method.h"

#include "Arg.h"

#include <memory>
using std::auto_ptr;

using namespace compiler;

Method::Method(const arg_vec* args,
               auto_ptr<Type> returnType,
               auto_ptr<Block> body)
  : m_returnType(returnType), m_body(body) {
  if (args) {
    for (arg_iter i = args->begin(); i != args->end(); ++i) {
      m_args.push_back((*i)->getSpec().release());
    }
  }
}

Method::Method(const argspec_vec* args,
               auto_ptr<Type> returnType,
               auto_ptr<Block> body)
  : m_returnType(returnType), m_body(body) {
  if (args) {
    for (argspec_iter i = args->begin(); i != args->end(); ++i) {
      m_args.push_back((*i)->duplicate().release());
    }
  }
}

Method::~Method() {
  for (argspec_iter i = m_args.begin(); i != m_args.end(); ++i) {
    delete *i;
  }
}

/*
bool Method::isEquivalentTo(const Method& rhs) const {
  // TODO fix this to support named or optional arguments
  // TODO eventually we'll want smarter dispatch, where this will do something
  // like areArgsEquivalent() (not Identical, that's not correct).
  return m_args.size() == rhs.m_args.size();
}

// Check if the caller's list of argument types is compatible with this method.
// Note that m_args is an argspec_vec while rhs_args is a type_list.
bool Method::areArgsCompatible(const type_list& rhs_args) const {
  return m_args.size() == rhs_args.size();
}
*/
