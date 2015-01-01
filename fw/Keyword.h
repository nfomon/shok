// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Keyword_h_
#define _Keyword_h_

/* Keyword rule
 *
 * Keywords recognize CharData input nodes, and match a specific string.
 */

#include "ComputeFunc.h"
#include "Rule.h"

#include "util/Log.h"

#include <memory>
#include <string>

namespace fw {

std::auto_ptr<Rule> MakeRule_Keyword(Log& log, const std::string& str);
std::auto_ptr<Rule> MakeRule_Keyword(Log& log,
    const std::string& name,
    const std::string& str);

class ComputeFunc_Keyword : public ComputeFunc {
public:
  ComputeFunc_Keyword(Log& log, const std::string& str);
  virtual ~ComputeFunc_Keyword() {}
  virtual void operator() ();
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Keyword(m_log, m_str));
  }

private:
  const std::string m_str;
};

}

#endif // _Keyword_h_
