// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_Opt_h_
#define _statik_Opt_h_

/* Optional rule */

#include "List.h"
#include "ParseAction.h"
#include "ParseFunc.h"
#include "Rule.h"
#include "STree.h"

#include <memory>
#include <string>

namespace statik {

std::auto_ptr<Rule> OPT(const std::string& name);

struct ParseFunc_Opt : public ParseFunc {
  virtual ~ParseFunc_Opt() {}
  virtual void operator() (ParseAction::Action action, const List& inode, const STree* initiator);
  virtual std::auto_ptr<ParseFunc> Clone() {
    return std::auto_ptr<ParseFunc>(new ParseFunc_Opt());
  }
};

std::auto_ptr<ParseFunc> MakeParseFunc_Opt();

}

#endif // _statik_Opt_h_
