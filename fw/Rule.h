// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Rule_h_
#define _Rule_h_

/* Production rules
 *
 * A Rule represents a node of a parser-machine graph.  In essence it is a
 * factory for FWTree nodes which represent the process of the parse attempt.
 */

#include "ComputeFunc.h"
#include "IList.h"
#include "OutputFunc.h"
#include "RestartFunc.h"
#include "State.h"

#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>
#include <string>

namespace fw {

class Connector;
class FWTree;

class Rule {
public:
  typedef boost::ptr_vector<Rule> child_vec;
  typedef child_vec::const_iterator child_iter;

  Rule(Log& log, const std::string& debugName,
      std::auto_ptr<RestartFunc> = std::auto_ptr<RestartFunc>(),
      std::auto_ptr<ComputeFunc> = std::auto_ptr<ComputeFunc>(),
      std::auto_ptr<OutputFunc> = std::auto_ptr<OutputFunc>());

  Rule& SetRestartFunc(std::auto_ptr<RestartFunc> restartFunc);
  Rule& SetComputeFunc(std::auto_ptr<ComputeFunc> computeFunc);
  Rule& SetOutputFunc(std::auto_ptr<OutputFunc> outputFunc);
  void CapOutput(const std::string& cap);

  Rule* AddChild(std::auto_ptr<Rule> child);

  std::auto_ptr<FWTree> MakeRootNode(Connector& connector) const;
  FWTree* MakeNode(FWTree& parent, const IList& istart) const;

  std::string Name() const { return m_name; }
  const child_vec& GetChildren() const { return m_children; }

  operator std::string() const { return m_name; }
  std::string Print() const;
  std::string DrawNode(const std::string& context) const;

protected:
  void setParent(Rule* parent) {
    m_parent = parent;
  }

  Log& m_log;
  std::string m_name;
  std::auto_ptr<RestartFunc> m_restartFunc;
  std::auto_ptr<ComputeFunc> m_computeFunc;
  std::auto_ptr<OutputFunc> m_outputFunc;
  Rule* m_parent;
  child_vec m_children;
};

}

#endif // _Rule_h_
