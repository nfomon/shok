// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Meta_h_
#define _Meta_h_

/* Meta rule
 *
 * Recognizes a node output by another rule, by its name.
 */

#include "Rule.h"

#include <string>

namespace fw {

class MetaRule : public Rule {
public:
  MetaRule(Log& log, const std::string& searchName, const std::string& name = "")
    : Rule(log, (name.empty() ? searchName : name), RF_None, OS_SINGLE),
      m_searchName(searchName) {}
  virtual ~MetaRule() {}

  const std::string& GetString() const { return m_searchName; }

  virtual void Update(FWTree& x) const;

private:
  std::string m_searchName;
};

}

#endif // _Meta_h_
