// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Regexp_h_
#define _Regexp_h_

/* Regexp rule */

#include "Rule.h"

#include <boost/regex.hpp>

namespace fw {

class RegexpRule : public Rule {
public:
  RegexpRule(Log& log, const std::string& name, const boost::regex& regex);
  virtual ~RegexpRule() {}

  virtual void Update(Connector& connector, FWTree& x) const;

private:
  const boost::regex m_regex;
};

}

#endif // _Regexp_h_
