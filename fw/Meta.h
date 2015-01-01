// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Meta_h_
#define _Meta_h_

/* Meta rule
 *
 * Recognizes a node output by another rule, by its name.
 */

#include "ComputeFunc.h"
#include "Rule.h"

#include <memory>
#include <string>

namespace fw {

std::auto_ptr<Rule> MakeRule_Meta(Log& log, const std::string& searchName);
std::auto_ptr<Rule> MakeRule_Meta(Log& log,
    const std::string& name,
    const std::string& searchName);

class ComputeFunc_Meta : public ComputeFunc {
public:
  ComputeFunc_Meta(Log& log, const std::string& searchName)
    : ComputeFunc(log),
      m_searchName(searchName) {}
  virtual ~ComputeFunc_Meta() {}
  virtual void operator() ();
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Meta(m_log, m_searchName));
  }

private:
  std::string m_searchName;
};

}

#endif // _Meta_h_
