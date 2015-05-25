// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _istatik_ParserWindow_h_
#define _istatik_ParserWindow_h_

#include "ISError.h"
#include "WindowResponse.h"

#include "statik/Hotlist.h"
#include "statik/IncParser.h"
#include "statik/Rule.h"

#include <boost/noncopyable.hpp>

namespace istatik {

class ParserWindow : private boost::noncopyable {
public:
  ParserWindow(statik::Rule& rule, const std::string& graphdir);
  WindowResponse Input(const statik::Hotlist& hotlist);
private:
  statik::IncParser m_incParser;
  std::string m_str;
};

}

#endif // _istatik_ParserWindow_h_
