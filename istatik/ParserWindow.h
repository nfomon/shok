// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _istatik_ParserWindow_h_
#define _istatik_ParserWindow_h_

#include "ISError.h"
#include "WindowResponse.h"

#include "statik/Batch.h"
#include "statik/IncParser.h"
#include "statik/Rule.h"

#include <boost/noncopyable.hpp>

#include <memory>

namespace istatik {

class ParserWindow : private boost::noncopyable {
public:
  ParserWindow(std::auto_ptr<statik::Rule> rule,
               const std::string& name,
               const std::string& graphdir);
  WindowResponse Input(const statik::Batch& batch);
private:
  statik::IncParser m_incParser;
  std::string m_str;
};

}

#endif // _istatik_ParserWindow_h_
