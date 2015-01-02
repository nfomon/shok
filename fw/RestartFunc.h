// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _RestartFunc_h_
#define _RestartFunc_h_

#include "IList.h"

#include "util/Log.h"

namespace fw {

class FWTree;
class RestartFunc;

std::auto_ptr<RestartFunc> MakeRestartFunc_None(Log& log);
std::auto_ptr<RestartFunc> MakeRestartFunc_FirstChildOfNode(Log& log);
std::auto_ptr<RestartFunc> MakeRestartFunc_AllChildrenOfNode(Log& log);

struct RestartFunc {
public:
  RestartFunc(Log& log)
    : m_log(log),
      m_node(NULL) {}
  virtual ~RestartFunc() {}
  void Init(FWTree& x) { m_node = &x; }
  virtual void operator() (const IList& istart) = 0;
  virtual std::auto_ptr<RestartFunc> Clone() = 0;
protected:
  Log& m_log;
  FWTree* m_node;
};

struct RestartFunc_None : public RestartFunc {
public:
  RestartFunc_None(Log& log)
    : RestartFunc(log) {}
  virtual ~RestartFunc_None() {}
  virtual void operator() (const IList& istart) {}
  virtual std::auto_ptr<RestartFunc> Clone() {
    return std::auto_ptr<RestartFunc>(new RestartFunc_None(m_log));
  }
};

struct RestartFunc_FirstChildOfNode : public RestartFunc {
  RestartFunc_FirstChildOfNode(Log& log)
    : RestartFunc(log) {}
  virtual ~RestartFunc_FirstChildOfNode() {}
  virtual void operator() (const IList& istart);
  virtual std::auto_ptr<RestartFunc> Clone() {
    return std::auto_ptr<RestartFunc>(new RestartFunc_FirstChildOfNode(m_log));
  }
};

struct RestartFunc_AllChildrenOfNode : public RestartFunc {
  RestartFunc_AllChildrenOfNode(Log& log)
    : RestartFunc(log) {}
  virtual ~RestartFunc_AllChildrenOfNode() {}
  virtual void operator() (const IList& istart);
  virtual std::auto_ptr<RestartFunc> Clone() {
    return std::auto_ptr<RestartFunc>(new RestartFunc_AllChildrenOfNode(m_log));
  }
};

}

#endif // _RestartFunc_h_
