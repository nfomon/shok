// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ConnectorWindow_h_
#define _ConnectorWindow_h_

#include "ISError.h"
#include "WindowResponse.h"

#include "statik/Connector.h"
#include "statik/Hotlist.h"
#include "statik/Rule.h"

namespace istatik {

class ConnectorWindow {
public:
  ConnectorWindow(const statik::Rule& rule);
  WindowResponse Input(const statik::Hotlist& hotlist);
private:
  statik::Connector m_connector;
};

}

#endif // _ConnectorWindow_h_
