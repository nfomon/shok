// Copyright (C) 2013 Michael Biggs.  See the COPYRIGHT file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _EvalError_h_
#define _EvalError_h_

#include <stdexcept>

namespace eval {

class EvalError : public std::runtime_error {
public:
  EvalError(const std::string& what) : std::runtime_error(what) {}
};

};

#endif // _EvalError_h_
