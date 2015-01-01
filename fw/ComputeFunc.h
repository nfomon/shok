// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ComputeFunc_h_
#define _ComputeFunc_h_

#include "IList.h"

#include "util/Log.h"

namespace fw {

class FWTree;

struct ComputeFunc {
public:
  ComputeFunc(Log& log)
    : m_log(log) {}
  virtual ~ComputeFunc() {}
  void Init(FWTree& x) {
    m_node = &x;
  }
  virtual void operator() () = 0;
  virtual std::auto_ptr<ComputeFunc> Clone() = 0;
protected:
  Log& m_log;
  FWTree* m_node;
};

}

#endif // _ComputeFunc_h_
