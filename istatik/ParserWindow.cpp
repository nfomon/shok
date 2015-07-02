// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "ParserWindow.h"

#include "ISError.h"

#include "statik/Batch.h"
#include "statik/List.h"
using statik::Batch;
using statik::List;

#include <memory>
#include <string>
#include <utility>
using std::auto_ptr;
using std::string;

using namespace istatik;

ParserWindow::ParserWindow(auto_ptr<statik::Rule> rule,
                           const string& name,
                           const string& graphdir)
  : m_incParser(rule, name, graphdir) {
  g_log.info() << "Initialized ParserWindow " << name;
}

ParserWindow::~ParserWindow() {
  List* n = m_nodes;
  while (n) {
    List* prev = n;
    n = n->right;
    delete prev;
  }
}

WindowResponse ParserWindow::Input(const Batch& ibatch) {
  g_log.info() << "Updating incParser " << m_incParser.Name() << " with batch: " << ibatch.Print();
  m_incParser.ApplyBatch(ibatch);
  WindowResponse response;
  // Find first item in incParser's output list, and draw everything
  response.actions.push_back(WindowAction(WindowAction::MOVE, 0, 0, 0));
  Batch batch;
  m_incParser.ExtractChanges(batch);
  g_log.info() << "Printing WindowResponse list.  Batch size is: " << batch.Size();
  if (!batch.IsEmpty()) {
    for (Batch::batch_iter i = batch.begin(); i != batch.end(); ++i) {
      switch (i->op) {
      case Batch::OP_INSERT: {
          g_log.debug() << "Insert node: " << i->node->name << ":" << i->node->value;
          List* node = new List(i->node->name, i->node->value);
          if (i->pos) {
            node_iter pos_i = m_nodeMap.find(i->pos);
            if (m_nodeMap.end() == pos_i) {
              throw ISError("Received invalid pos for Insert");
            }
            List* pos = pos_i->second;
            g_log.debug() << " - at pos: " << pos->name << ":" << pos->value;
            node->right = pos->right;
            node->left = pos;
            pos->right = node;
          } else {
            node->right = m_nodes;
            m_nodes = node;
          }
          m_nodeMap.insert(std::make_pair(i->node, node));
        }
        break;
      case Batch::OP_DELETE: {
          g_log.debug() << "Delete node: " << i->node->name << ":" << i->node->value;
          node_mod_iter node_i = m_nodeMap.find(i->node);
          if (m_nodeMap.end() == node_i) {
            throw ISError("Received invalid node for Delete");
          }
          List* node = node_i->second;
          if (node->left) {
            node->left->right = node->right;
            if (node->right) {
              node->right->left = node->left;
            }
            delete node;
          } else {
            if (m_nodes != node) {
              throw ISError("Attempt to delete node which has no left, but is not m_nodes either");
            }
            m_nodes = node->right;
            delete node;
          }
          m_nodeMap.erase(node_i);
        }
        break;
      case Batch::OP_UPDATE:
        g_log.debug() << "Update node: " << i->node->name << ":" << i->node->value;
        break;
      default:
        throw ISError("Unknown batch operation " + Batch::UnMapBatchOp(i->op));
      }
    }

    string old_str = m_str;
    m_str = "";
    bool first = true;
    List* node = m_nodes;
    while (node) {
      if (first) {
        first = false;
      } else {
        m_str += " ";
      }
      m_str += node->name;
      node = node->right;
    }
    g_log.info() << "Printing str: '" << m_str << "'";
    for (size_t i = 0; i < m_str.size(); ++i) {
      response.actions.push_back(WindowAction(WindowAction::INSERT, 0, i, m_str[i]));
    }
    // Clear out the rest of the line
    for (size_t i = m_str.size(); i < old_str.size(); ++i) {
      response.actions.push_back(WindowAction(WindowAction::INSERT, 0, i, ' '));
    }
    response.batch.Accept(batch);
  }
  return response;
}
