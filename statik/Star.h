// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_Star_h_
#define _statik_Star_h_

#include "List.h"
#include "ParseAction.h"
#include "ParseFunc.h"
#include "Rule.h"
#include "STree.h"

#include <memory>
#include <string>

namespace statik {

std::auto_ptr<Rule> STAR(const std::string& name);

struct ParseFunc_Star : public ParseFunc {
  virtual ~ParseFunc_Star() {}
  virtual void operator() (ParseAction::Action action, const List& inode, const STree* initiator);
  virtual std::auto_ptr<ParseFunc> Clone() {
    return std::auto_ptr<ParseFunc>(new ParseFunc_Star());
  }
};

std::auto_ptr<ParseFunc> MakeParseFunc_Star();

}

#endif // _statik_Star_h_
