// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "ParserWindow.h"

#include "ISError.h"

#include "statik/List.h"

#include <memory>
#include <string>
#include <iostream>
using std::auto_ptr;
using std::endl;
using std::string;

using namespace istatik;

ParserWindow::ParserWindow(auto_ptr<statik::Rule> rule,
                           const string& name,
                           const string& graphdir)
  : m_incParser(rule, name, graphdir) {
  g_log.info() << "Initialized ParserWindow " << name;
}

WindowResponse ParserWindow::Input(const statik::Hotlist& ihotlist) {
  g_log.info() << "Updating incParser " << m_incParser.Name() << " with hotlist: " << ihotlist.Print();
  m_incParser.UpdateWithHotlist(ihotlist.GetHotlist());
  WindowResponse response;
  // Find first item in incParser's output list, and draw everything
  response.actions.push_back(WindowAction(WindowAction::MOVE, 0, 0, 0));
  statik::Hotlist hotlist;
  m_incParser.ExtractHotlist(hotlist);
  g_log.info() << "Printing WindowResponse list.  Hotlist size is: " << hotlist.Size();
  if (!hotlist.IsEmpty()) {
    const statik::List* inode = m_incParser.GetFirstONode();
    string old_str = m_str;
    m_str = "";
    bool first = true;
    while (inode) {
      if (first) {
        first = false;
      } else {
        m_str += " ";
      }
      m_str += inode->name;
      inode = inode->right;
    }
    for (size_t i = 0; i < m_str.size(); ++i) {
      response.actions.push_back(WindowAction(WindowAction::INSERT, 0, i, m_str[i]));
    }
    // Clear out the rest of the line
    for (size_t i = m_str.size(); i < old_str.size(); ++i) {
      response.actions.push_back(WindowAction(WindowAction::INSERT, 0, i, ' '));
    }
    response.hotlist.Accept(hotlist.GetHotlist());
  }
  return response;
}
