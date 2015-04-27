// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _RestartFunc_h_
#define _RestartFunc_h_

#include "IList.h"

#include <memory>

namespace statik {

class STree;
class RestartFunc;

std::auto_ptr<RestartFunc> MakeRestartFunc_Default();
std::auto_ptr<RestartFunc> MakeRestartFunc_Sequence();
std::auto_ptr<RestartFunc> MakeRestartFunc_AllChildrenOfNode();

struct RestartFunc {
public:
  RestartFunc()
    : m_node(NULL) {}
  virtual ~RestartFunc() {}
  void Init(STree& x) { m_node = &x; }
  virtual void operator() (const IList& istart) = 0;
  virtual std::auto_ptr<RestartFunc> Clone() = 0;
protected:
  STree* m_node;
};

struct RestartFunc_Default : public RestartFunc {
public:
  RestartFunc_Default()
    : RestartFunc() {}
  virtual ~RestartFunc_Default() {}
  virtual void operator() (const IList& istart);
  virtual std::auto_ptr<RestartFunc> Clone() {
    return std::auto_ptr<RestartFunc>(new RestartFunc_Default());
  }
};

struct RestartFunc_Sequence : public RestartFunc {
  RestartFunc_Sequence()
    : RestartFunc() {}
  virtual ~RestartFunc_Sequence() {}
  virtual void operator() (const IList& istart);
  virtual std::auto_ptr<RestartFunc> Clone() {
    return std::auto_ptr<RestartFunc>(new RestartFunc_Sequence());
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
