// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_Rule_h_
#define _statik_Rule_h_

/* Production rules
 *
 * A Rule represents a node of a parser-machine graph.  In essence it is a
 * factory for STree nodes which represent the process of the parse attempt.
 */

#include "ComputeFunc.h"
#include "List.h"
#include "OutputFunc.h"
#include "STree.h"
#include "State.h"

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace statik {

class Rule {
public:
  typedef std::vector<Rule*> child_vec;
  typedef child_vec::const_iterator child_iter;
  typedef std::vector<bool> childOwnership_vec;
  typedef childOwnership_vec::const_iterator childOwnership_iter;

  Rule(const std::string& name,
      std::auto_ptr<ComputeFunc>,
      std::auto_ptr<OutputFunc>);
  ~Rule();

  Rule& SetComputeFunc(std::auto_ptr<ComputeFunc> computeFunc);
  Rule& SetOutputFunc(std::auto_ptr<OutputFunc> outputFunc);
  Rule* SilenceOutput();
  Rule* CapOutput(const std::string& cap);

  Rule* AddChild(std::auto_ptr<Rule> child);
  Rule* AddChildRecursive(Rule* child);

  STree* MakeNode(STree& parent, const List& istart) const;
  STree* MakeNode(STree& parent, const List& istart, STree::child_mod_iter insertPos) const;

  std::string Name() const { return m_name; }
  const child_vec& GetChildren() const { return m_children; }

  std::string Print() const;
  std::string DrawNode(const std::string& context) const;

protected:
  void setParent(Rule* parent) {
    m_parent = parent;
  }

  std::string m_name;
  std::auto_ptr<ComputeFunc> m_computeFunc;
  std::auto_ptr<OutputFunc> m_outputFunc;
  Rule* m_parent;
  child_vec m_children;
  // Same length as m_children.  True where a child is owned (should be deleted
  // at destruction time), false otherwise.
  childOwnership_vec m_childOwnership;
};

std::ostream& operator<< (std::ostream& out, const Rule& rule);

}

#endif // _statik_Rule_h_
