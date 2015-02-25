// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Keyword_h_
#define _Keyword_h_

/* Keyword rule */

#include "ComputeFunc.h"
#include "Rule.h"

#include <memory>
#include <string>

namespace statik {

std::auto_ptr<Rule> KEYWORD(const std::string& str);
std::auto_ptr<Rule> KEYWORD(const std::string& name,
                            const std::string& str);

class ComputeFunc_Keyword : public ComputeFunc {
public:
  ComputeFunc_Keyword(const std::string& str);
  virtual ~ComputeFunc_Keyword() {}
  virtual void operator() ();
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Keyword(m_str));
  }

private:
  const std::string m_str;
};

std::auto_ptr<ComputeFunc> MakeComputeFunc_Keyword(const std::string& str);

}

#endif // _Keyword_h_
