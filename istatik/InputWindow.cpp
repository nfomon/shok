// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "InputWindow.h"

#include "ISError.h"

#include <curses.h>

#include <algorithm>
#include <iostream>
using std::endl;
using std::ostream;

using namespace istatik;

ostream& istatik::operator<< (ostream& out, const CharList& node) {
  out << "(" << node.y << "," << node.x << "):" << (char)node.ch;
  return out;
}

InputWindow::InputWindow(int maxrows, int maxcols)
  : m_maxrows(maxrows),
    m_maxcols(maxcols),
    m_start(NULL) {
  if (maxrows <= 0 || maxcols <= 0) {
    throw ISError("Cannot create InputWindow with invalid dimensions");
  }
}

InputWindow::~InputWindow() {
  CharList* node = m_start;
  while (node) {
    CharList* tmp = node;
    node = node->next;
    delete tmp;
  }
}

#define MOVE(y,x) response.actions.push_back(new MoveAction(y, x))
#define DELETE() response.actions.push_back(new DeleteAction())
#define INSERT(ch) response.actions.push_back(new InsertAction(ch))

WindowResponse InputWindow::Input(int y, int x, int ch) {
  WindowResponse response;

  // The input char might be a special character like movement or delete.
  if (KEY_DC == ch) {
    CharList* node = FindNode(y, x);
    if (node) {
      if (x != node->x && y != node->y) {
        throw ISError("Del mismatch");
      }
      if (node->prev) {
        node->prev->next = node->next;
      }
      if (node->next) {
        node->next->prev = node->prev;
      }
      // ADD DEL TO response HOTLIST
      node = node->next;
    }
    while (node) {
      if (node->y != node->prev->y) {
        // TODO clear to end of line
        break;
      }
      --node->x;
      MOVE(node->y, node->x);
      INSERT(node->ch);
      node = node->next;
    }
/*
  } else if (KEY_BACKSPACE == ch) {
    if (x > 0) {
      MOVE(y,x-1);
      DELETE();
      m_rows.at(y).erase(m_rows.at(y).begin() + x-1);
    }
*/
  } else if (KEY_LEFT == ch) {
    g_log.info() << " - LEFT";
    if (x > 0) {
      g_log.info() << " - MOVE to (" << y << "," << x-1 << ")";
      MOVE(y, x-1);
    }
  } else if (KEY_RIGHT == ch) {
    g_log.info() << " - RIGHT";
    CharList* node = FindNode(y, x+1);
    if (!node) {
      g_log.info() << "No node!";
    }
    if (node && (node->y < y || (node->y == y && node->x <= x)) && x < m_maxcols - 1) {
      g_log.info() << " - MOVE to (" << y << "," << x+1 << ")";
      MOVE(y, x+1);
    } else if (node) {
      g_log.info() << " - MOVE to (" << node->y << "," << node->x << ")";
      MOVE(node->y, node->x);
    }
  } else if(KEY_UP == ch) {
    g_log.info() << " - UP";
    if (y > 0) {
      CharList* node = FindNode(y-1, x);
      if (node && (y-1 == node->y)) {
        g_log.info() << " - MOVE to (" << node->y << "," << node->x << ")";
        MOVE(node->y, node->x);
      }
    }
/*
  } else if (KEY_DOWN == ch) {
    if (y < m_rows.size() - 1) {
      if (x > m_rows.at(y+1).size()) {
        x = std::max(m_rows.at(y+1).size(), (size_t)0);
      }
      MOVE(y+1, x);
    }
*/
/*
  } else if (KEY_ENTER == ch || '\n' == ch) {
    // TODO
    CharList* cl = new CharList(y, x, ch);
    g_log.info() << "Got enter: " << cl;
    CharList* node = FindNode(y, x-1);
    if (y < m_rows.size() - 1) {
      MOVE(y+1, 0);
    } else if (y == m_rows.size() - 1 && y < m_maxrows) {
      m_rows.push_back(col_vec());
      MOVE(y+1, 0);
    }
*/
  } else {
    CharList* cl = new CharList(y, x, ch);
    CharList* node = FindNode(y, x-1);
    if (node) {
      cl->prev = node;
      if (node->next) {
        cl->next = node->next;
        cl->next->prev = cl;
      }
      node->next = cl;
    } else if (m_start) {
      m_start->prev = cl;
      cl->next = m_start;
      m_start = cl;
    } else {
      m_start = cl;
    }
    MOVE(cl->y, cl->x);
    INSERT(cl->ch);
    if (KEY_ENTER == cl->ch || '\n' == cl->ch || cl->x == m_maxcols - 1) {
      ++y;
      x = 0;
    }
    int endy = y;
    int endx = cl->x+1;
    if (endx == m_maxcols - 1) {
      ++endy;
      endx = 0;
    }
    node = cl->next;
    while (node) {
      g_log.info() << " at " << *node;
      if (KEY_ENTER == node->prev->ch || '\n' == node->prev->ch || node->prev->x == m_maxcols - 1) {
        for (int i = node->prev->x+1; i < m_maxcols; ++i) {
          MOVE(y, i);
          INSERT(' ');    // clear rest of line
        }
        ++y;
        x = 0;
      } else {
        ++x;
      }
      node->y = y;
      node->x = x;
      MOVE(node->y, node->x);
      INSERT(node->ch);
      node = node->next;
    }
    MOVE(endy, endx);
  }

  return response;
}

CharList* InputWindow::FindNode(int y, int x) {
  CharList* prev = NULL;
  CharList* node = m_start;
  while (node) {
    if (node->y > y || (node->y == y && node->x > x)) {
      break;
    }
    g_log.info() << "  ... skipping " << *node;
    prev = node;
    node = node->next;
  }
  if (prev) {
    g_log.info() << "  found " << *prev;
  } else {
    g_log.info() << "  found <null>";
  }
  return prev;
}
