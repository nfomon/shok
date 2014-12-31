// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Keyword_h_
#define _Keyword_h_

/* Keyword rule
 *
 * Keywords recognize CharData input nodes, and match a specific string.
 */

#include "FWTree.h"
#include "Rule.h"

#include <string>

namespace fw {

class KeywordRule : public Rule {
public:
  KeywordRule(Log& log, const std::string& str);
  virtual ~KeywordRule() {}

  const std::string& GetString() const { return m_str; }

  virtual void Update(FWTree& x) const;

private:
  const std::string m_str;
};

}

#endif // _Keyword_h_
