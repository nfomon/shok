// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_Meta_h_
#define _statik_Meta_h_

/* Meta rule
 *
 * Recognizes a node output by another rule, by its name.
 */

#include "ComputeFunc.h"
#include "List.h"
#include "ParseAction.h"
#include "Rule.h"
#include "STree.h"

#include <memory>
#include <string>

namespace statik {

std::auto_ptr<Rule> META(const std::string& searchName);
std::auto_ptr<Rule> META(const std::string& name,
                         const std::string& searchName);

class ComputeFunc_Meta : public ComputeFunc {
public:
  ComputeFunc_Meta(const std::string& searchName)
    : m_searchName(searchName) {}
  virtual ~ComputeFunc_Meta() {}
  virtual void operator() (ParseAction::Action action, const List& inode, const STree* initiator);
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Meta(m_searchName));
  }

private:
  std::string m_searchName;
};

std::auto_ptr<ComputeFunc> MakeComputeFunc_Meta(const std::string& searchName);

}

#endif // _statik_Meta_h_
