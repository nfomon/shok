// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_Meta_h_
#define _statik_Meta_h_

/* Meta rule
 *
 * Recognizes a node output by another rule, by its name.
 */

#include "List.h"
#include "ParseAction.h"
#include "ParseFunc.h"
#include "Rule.h"
#include "STree.h"

#include <memory>
#include <string>

namespace statik {

std::auto_ptr<Rule> META(const std::string& searchName);
std::auto_ptr<Rule> META(const std::string& name,
                         const std::string& searchName);

class ParseFunc_Meta : public ParseFunc {
public:
  ParseFunc_Meta(const std::string& searchName);
  virtual ~ParseFunc_Meta() {}
  virtual void operator() (ParseAction::Action action, const List& inode, const STree* initiator);
  virtual std::auto_ptr<ParseFunc> Clone() {
    return std::auto_ptr<ParseFunc>(new ParseFunc_Meta(m_searchName));
  }

private:
  std::string m_searchName;
};

std::auto_ptr<ParseFunc> MakeParseFunc_Meta(const std::string& searchName);

}

#endif // _statik_Meta_h_
