// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _RestartFunc_h_
#define _RestartFunc_h_

#include "IList.h"

namespace fw {

class Rule;
class FWTree;

enum RestartFuncType {
  RF_None,
  RF_FirstChildOfNode,
  RF_AllChildrenOfNode
};

struct RestartFunc {
public:
  RestartFunc(Log& log, FWTree& x)
    : m_log(log),
      m_node(x) {}
  virtual ~RestartFunc() {}
  virtual void operator() (const IList& istart) = 0;
protected:
  Log& m_log;
  FWTree& m_node;
};

struct RestartFunc_None : public RestartFunc {
public:
  RestartFunc_None(Log& log, FWTree& x)
    : RestartFunc(log, x) {}
  virtual ~RestartFunc_None() {}
  virtual void operator() (const IList& istart) {}
};

struct RestartFunc_FirstChildOfNode : public RestartFunc {
  RestartFunc_FirstChildOfNode(Log& log, FWTree& x)
    : RestartFunc(log, x) {}
  virtual ~RestartFunc_FirstChildOfNode() {}
  virtual void operator() (const IList& istart);
};

struct RestartFunc_AllChildrenOfNode : public RestartFunc {
  RestartFunc_AllChildrenOfNode(Log& log, FWTree& x)
    : RestartFunc(log, x) {}
  virtual ~RestartFunc_AllChildrenOfNode() {}
  virtual void operator() (const IList& istart);
};

}

#endif // _RestartFunc_h_
