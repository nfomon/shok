// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Regexp_h_
#define _Regexp_h_

/* Regexp rule */

#include "ConnectorAction.h"
#include "ComputeFunc.h"
#include "Rule.h"

#include <boost/regex.hpp>

#include <memory>
#include <string>

namespace statik {

std::auto_ptr<Rule> REGEXP(const std::string& name,
                           const boost::regex& regex);

class ComputeFunc_Regexp : public ComputeFunc {
public:
  ComputeFunc_Regexp(const boost::regex& regex);
  virtual ~ComputeFunc_Regexp() {}
  virtual void operator() (ConnectorAction::Action action, const IList& inode, const STree* initiator);
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Regexp(m_regex));
  }

private:
  const boost::regex m_regex;
};

std::auto_ptr<ComputeFunc> MakeComputeFunc_Regexp(const boost::regex& regex);

}

#endif // _Regexp_h_
