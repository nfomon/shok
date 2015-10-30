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
  : m_incParser(rule, name, graphdir),
    m_nodes(NULL) {
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
  const statik::STree& root = m_incParser.GetRoot();
  WindowResponse response;
  if (root.GetState().IsBad()) {
    g_log.debug() << " - Not extracting output, because root is bad!";
    return response;
  }
  // Find first item in incParser's output list, and draw everything
  response.actions.push_back(WindowAction(WindowAction::MOVE, 0, 0, 0));
  Batch batch;
  g_log.debug() << "Extracting changes from IncParser";
  m_incParser.ExtractChanges(batch);
  g_log.info() << "Printing WindowResponse list for batch of size " << batch.Size() << ": " << batch;
  if (m_nodes) {
    g_log.debug() << "m_nodes is: " << *m_nodes;
  }
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
            if (node->right) {
              node->right->left = node;
            }
            g_log.debug() << "Inserted " << *node;
            if (node->left) {
              g_log.debug() << " - with left: " << *node->left;
              if (node->left->right) {
                g_log.debug() << " - - which has right: " << *node->left->right;
              }
            }
            if (node->right) {
              g_log.debug() << " - with right: " << *node->right;
              if (node->right->left) {
                g_log.debug() << " - - which has left: " << *node->right->left;
              }
            }
          } else {
            g_log.debug() << "Setting this node as m_nodes";
            node->right = m_nodes;
            if (node->right) {
              node->right->left = node;
            }
            m_nodes = node;
          }
          m_nodeMap.insert(std::make_pair(i->node, node));
          g_log.debug() << "Done inserting node";
        }
        break;

      case Batch::OP_DELETE: {
          // Careful!  i->node is a pointer to dead data, it's just a lookup
          // handle for us.
          node_mod_iter node_i = m_nodeMap.find(i->node);
          if (m_nodeMap.end() == node_i) {
            throw ISError("Received invalid node for Delete");
          }
          List* node = node_i->second;
          g_log.debug() << "Delete node: " << node->name << ":" << node->value;
          List* left = node->left;
          List* right = node->right;
          if (left && left->right == node) {
            left->right = right;
          }
          if (right && right->left == node) {
            right->left = left;
          }
          if (m_nodes == node) {
            g_log.debug() << "Deleted node is m_nodes";
            if (left) {
              g_log.debug() << "going left";
              g_log.debug() << "going left to " << *left << " from " << *node;
              m_nodes = left;
            } else if (right) {
              g_log.debug() << "going right";
              g_log.debug() << "going right to " << *right << " from " << *node;
              m_nodes = right;
            } else {
              g_log.debug() << "nowhere to go; m_nodes is clear";
              m_nodes = NULL;
            }
          }
          g_log.debug() << "Deleted " << *node;
          if (node->left) {
            g_log.debug() << " - with left: " << *node->left;
          }
          if (node->right) {
            g_log.debug() << " - with right: " << *node->right;
          }
          delete node;
          m_nodeMap.erase(node_i);
          g_log.debug() << "Done deleting node";
        }
        break;

      case Batch::OP_UPDATE: {
          g_log.debug() << "Update node: " << i->node->name << ":" << i->node->value;
/*
          node_mod_iter node_i = m_nodeMap.find(i->node);
          if (m_nodeMap.end() == node_i) {
            throw ISError("Received invalid node for Update");
          }
          List* node = node_i->second;
          g_log.debug() << "Update node: " << node->name << ":" << node->value << " -> " << i->node->name << ":" << i->node->value;
          node->value = i->node->value;
*/
        }
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
      /*if (!node->value.empty()) {
        m_str += "[" + node->value + "]";
      }*/
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
