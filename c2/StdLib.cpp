// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "StdLib.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace compiler;

void StdLib::Init(Scope& scope) {
  scope.insert(StdLib::OBJECT, auto_ptr<Type>(new RootType()));
  scope.insert(StdLib::FUNCTION, auto_ptr<Type>(new BasicType(
      scope.find(StdLib::OBJECT)->duplicate(), StdLib::OBJECT)));
  scope.insert(StdLib::INTEGER, auto_ptr<Type>(new BasicType(
      scope.find(StdLib::OBJECT)->duplicate(), StdLib::OBJECT)));
  scope.insert(StdLib::STRING, auto_ptr<Type>(new BasicType(
      scope.find(StdLib::OBJECT)->duplicate(), StdLib::OBJECT)));
  scope.insert("print", auto_ptr<Type>(new BasicType(
      scope.find(StdLib::FUNCTION)->duplicate(), StdLib::FUNCTION)));
}
