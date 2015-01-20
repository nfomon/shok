// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ComputeFunc_h_
#define _ComputeFunc_h_

#include "IList.h"

#include <memory>

namespace statik {

class STree;

struct ComputeFunc {
public:
  virtual ~ComputeFunc() {}
  void Init(STree& x) { m_node = &x; }
  virtual void operator() () = 0;
  virtual std::auto_ptr<ComputeFunc> Clone() = 0;
protected:
  STree* m_node;
};

}

#endif // _ComputeFunc_h_
