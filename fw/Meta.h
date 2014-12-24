// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Meta_h_
#define _Meta_h_

/* Meta rule
 *
 * Recognizes a node output by another rule, by its name.
 */

#include "Connector.h"
#include "FWError.h"
#include "FWTree.h"
#include "OData.h"
#include "Rule.h"
#include "State.h"

#include <memory>
#include <string>

namespace fw {

class MetaRule : public Rule {
public:
  MetaRule(Log& log, const std::string& searchName, const std::string& name = "")
    : Rule(log, (name.empty() ? searchName : name)),
      m_searchName(searchName) {}
  virtual ~MetaRule() {}

  const std::string& GetString() const { return m_searchName; }

  virtual void Reposition(Connector& connector, FWTree& x, const IList& inode) const;
  virtual void Update(Connector& connector, FWTree& x) const;

  virtual std::auto_ptr<OConnection> MakeOConnection(const FWTree& x) const;

private:
  std::string m_searchName;
};

}

#endif // _Meta_h_
