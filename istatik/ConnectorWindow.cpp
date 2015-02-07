// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "ConnectorWindow.h"

#include "ISError.h"

using namespace istatik;

ConnectorWindow::ConnectorWindow(const statik::Rule& rule)
  : m_connector(rule) {
  g_log.info() << "Initialized ConnectorWindow for " << rule;
}

WindowResponse ConnectorWindow::Input(const statik::Hotlist& hotlist) {
  m_connector.ClearHotlist();
  m_connector.UpdateWithHotlist(hotlist.GetHotlist());
  WindowResponse response;
  response.hotlist = &m_connector.GetHotlist();
  return WindowResponse();
}
