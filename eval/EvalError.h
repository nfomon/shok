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
