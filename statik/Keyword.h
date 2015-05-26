// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_Keyword_h_
#define _statik_Keyword_h_

/* Keyword rule */

#include "List.h"
#include "ParseAction.h"
#include "ParseFunc.h"
#include "Rule.h"
#include "STree.h"

#include <memory>
#include <string>

namespace statik {

std::auto_ptr<Rule> KEYWORD(const std::string& str);
std::auto_ptr<Rule> KEYWORD(const std::string& name,
                            const std::string& str);

class ParseFunc_Keyword : public ParseFunc {
public:
  ParseFunc_Keyword(const std::string& str);
  virtual ~ParseFunc_Keyword() {}
  virtual void operator() (ParseAction::Action action, const List& inode, const STree* initiator);
  virtual std::auto_ptr<ParseFunc> Clone() {
    return std::auto_ptr<ParseFunc>(new ParseFunc_Keyword(m_str));
  }

private:
  const std::string m_str;
};

std::auto_ptr<ParseFunc> MakeParseFunc_Keyword(const std::string& str);

}

#endif // _statik_Keyword_h_
