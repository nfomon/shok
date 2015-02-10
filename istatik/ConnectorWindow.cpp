// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "ConnectorWindow.h"

#include "ISError.h"

#include <string>
#include <iostream>
using std::endl;
using std::string;

using namespace istatik;

ConnectorWindow::ConnectorWindow(const statik::Rule& rule, const string& graphdir)
  : m_connector(rule, "", graphdir) {
  g_log.info() << "Initialized ConnectorWindow for " << rule;
}

WindowResponse ConnectorWindow::Input(const statik::Hotlist& hotlist) {
  m_connector.ClearHotlist();
  m_connector.UpdateWithHotlist(hotlist.GetHotlist());
  WindowResponse response;
  // Find first item in connector's output list, and draw everything
  response.actions.push_back(WindowAction(WindowAction::MOVE, 0, 0, 0));
  if (!m_connector.GetHotlist().IsEmpty()) {
    const statik::Hotlist::hotlist_vec& hotlist = m_connector.GetHotlist().GetHotlist();
    const statik::IList* inode = hotlist.at(0).first;
    while (inode->left) {
      inode = inode->left;
    }
    int x = 0;
    while (inode) {
      string s = inode->name;
      for (size_t i = 0; i < s.size(); ++i) {
        response.actions.push_back(WindowAction(WindowAction::INSERT, 0, x, s[i]));
        ++x;
      }
      response.actions.push_back(WindowAction(WindowAction::INSERT, 0, x, ' '));
      ++x;
      inode = inode->right;
    }
    response.hotlist.Accept(m_connector.GetHotlist().GetHotlist());
  }
  return response;
}
