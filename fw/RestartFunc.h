// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _RestartFunc_h_
#define _RestartFunc_h_

#include "IList.h"

#include <memory>

namespace fw {

class FWTree;
class RestartFunc;

std::auto_ptr<RestartFunc> MakeRestartFunc_None();
std::auto_ptr<RestartFunc> MakeRestartFunc_FirstChildOfNode();
std::auto_ptr<RestartFunc> MakeRestartFunc_AllChildrenOfNode();

struct RestartFunc {
public:
  RestartFunc()
    : m_node(NULL) {}
  virtual ~RestartFunc() {}
  void Init(FWTree& x) { m_node = &x; }
  virtual void operator() (const IList& istart) = 0;
  virtual std::auto_ptr<RestartFunc> Clone() = 0;
protected:
  FWTree* m_node;
};

struct RestartFunc_None : public RestartFunc {
public:
  RestartFunc_None()
    : RestartFunc() {}
  virtual ~RestartFunc_None() {}
  virtual void operator() (const IList& istart) {}
  virtual std::auto_ptr<RestartFunc> Clone() {
    return std::auto_ptr<RestartFunc>(new RestartFunc_None());
  }
};

struct RestartFunc_FirstChildOfNode : public RestartFunc {
  RestartFunc_FirstChildOfNode()
    : RestartFunc() {}
  virtual ~RestartFunc_FirstChildOfNode() {}
  virtual void operator() (const IList& istart);
  virtual std::auto_ptr<RestartFunc> Clone() {
    return std::auto_ptr<RestartFunc>(new RestartFunc_FirstChildOfNode());
  }
};

struct RestartFunc_AllChildrenOfNode : public RestartFunc {
  RestartFunc_AllChildrenOfNode()
    : RestartFunc() {}
  virtual ~RestartFunc_AllChildrenOfNode() {}
  virtual void operator() (const IList& istart);
  virtual std::auto_ptr<RestartFunc> Clone() {
    return std::auto_ptr<RestartFunc>(new RestartFunc_AllChildrenOfNode());
  }
};

}

#endif // _RestartFunc_h_
