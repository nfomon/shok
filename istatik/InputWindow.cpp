// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "InputWindow.h"

#include "ISError.h"

#include <curses.h>

#include <algorithm>
#include <memory>
#include <ostream>
#include <string>
using std::auto_ptr;
using std::endl;
using std::ostream;
using std::string;

using namespace istatik;

/* Char */

Char::Char(int x, int ch)
  : x(x),
    ch(ch),
    inode("", string(1, (char)ch)) {
}

void Char::SetNext(Char* next) {
  if (next) {
    inode.right = &next->inode;
    next->inode.left = &inode;
  }
}

void Char::SetPrevious(Char* prev) {
  if (prev) {
    inode.left = &prev->inode;
    prev->inode.right = &inode;
  }
}

ostream& istatik::operator<< (ostream& out, const Char& node) {
  out << "(" << node.x << "):" << (char)node.ch;
  return out;
}

/* Line */

Line::Line(int y)
  : y(y) {
}

bool Line::HasChar(int x) const {
  return (size_t)x < m_chars.size();
}

int Line::LastIndex() const {
  return m_chars.size()-1;
}

Char* Line::GetFirst() {
  if (m_chars.empty()) {
    return NULL;
  }
  return &m_chars.at(0);
}

Char* Line::GetLast() {
  if (m_chars.empty()) {
    return NULL;
  }
  return &m_chars.back();
}

Char* Line::GetBefore(int x) {
  if (m_chars.empty() || 0 == x) {
    return NULL;
  } else if ((size_t)x >= m_chars.size()) {   // TODO is this right? lol
    return &m_chars.back();
  }
  return &m_chars.at(x-1);
}

Char* Line::GetAtOrAfter(int x) {
  if (m_chars.empty()) {
    return NULL;
  } else if ((size_t)x >= m_chars.size()) {
    return NULL;
  }
  return &m_chars.at(x);
}

Char* Line::Insert(int x, int ch) {
  auto_ptr<Char> c(new Char(x, ch));
  Char* cp = c.get();
  g_log.info() << "Line " << y << " inserting " << *c;
  if ((size_t)x > m_chars.size()) {
    throw ISError(string("Cannot insert ") + (char)ch + "; x out of bounds");
  } else if (m_chars.size() == (size_t)x) {
    m_chars.push_back(c);
  } else {
    m_chars.insert(m_chars.begin() + x, c);
  }
  return cp;
}

/* LineBuf public */

bool LineBuf::HasChar(int y, int x) const {
  if ((size_t)y >= m_lines.size()) {
    return false;
  }
  return m_lines.at(y).HasChar(x);
}

bool LineBuf::HasLine(int y) const {
  return (size_t)y < m_lines.size();
}

int LineBuf::LastIndexOfLine(int y) const {
  if (!HasLine(y)) {
    throw ISError("Cannot get last index of line; line out of bounds");
  }
  return m_lines.at(y).LastIndex();
}

WindowResponse LineBuf::Insert(int y, int x, int ch) {
  WindowResponse response;
  if ((size_t)y > m_lines.size()) {
    throw ISError(string("Cannot insert ") + (char)ch + "; y out of bounds");
  } else if (m_lines.size() == (size_t)y) {
    g_log.info() << "LineBuf creating line for y=" << y;
    m_lines.push_back(new Line(y));
  }
  g_log.info() << "LineBuf inserting (" << y << "," << x << "):" << (char)ch;
  Char* prev = FindBefore(y, x);
  Char* next = FindAtOrAfter(y, x+1);
  Char* c = m_lines.at(y).Insert(x, ch);
  response.hotlist.Insert(c->inode);
  if (prev) {
    g_log.info() << " found before: " << *prev;
    prev->SetNext(c);
    c->SetPrevious(prev);
  }
  if (next) {
    g_log.info() << " found next: " << *next;
    next->SetPrevious(c);
    c->SetNext(next);
  }
  statik::IList* istart = &c->inode;
  while (istart->left) {
    istart = istart->left;
  }
  g_log.info() << "window0 olist: " << istart->Print();
  g_log.info() << "window0 hotlist: " << response.hotlist.Print();

  response.actions.push_back(WindowAction(WindowAction::INSERT, y, x, ch));
  // TODO uhoh, we need to know ALL the nodes that changed (for display), which
  // might be many, across lines.
  return response;
}

WindowResponse LineBuf::Enter(int y, int x, int ch) {
  WindowResponse response;
  return response;
}

WindowResponse LineBuf::Delete(int y, int x) {
  WindowResponse response;
  return response;
}

WindowResponse LineBuf::Backspace(int y, int x) {
  WindowResponse response;
  return response;
}

/* LineBuf private */

Char* LineBuf::FindBefore(int y, int x) {
  g_log.info() << "LineBuf: FindBefore (" << y << "," << x << ")";
  if ((size_t)y >= m_lines.size()) {
    throw ISError("Cannot find char before (y,x); y out of bounds");
  }
  Char* c = m_lines.at(y).GetBefore(x);
  if (c) {
    return c;
  }
  for (int yi = y-1; yi >= 0; --yi) {
    Char* c = m_lines.at(yi).GetLast();
    if (c) {
      return c;
    }
  }
  return NULL;
}

Char* LineBuf::FindAtOrAfter(int y, int x) {
  g_log.info() << "LineBuf: FindAtOrAfter (" << y << "," << x << ")";
  if ((size_t)y >= m_lines.size()) {
    throw ISError("Cannot find char at or after (y,x); y out of bounds");
  }
  Char* c = m_lines.at(y).GetAtOrAfter(x);
  if (c) {
    return c;
  }
  for (size_t yi = y+1; yi < m_lines.size(); ++yi) {
    Char* c = m_lines.at(yi).GetFirst();
    if (c) {
      return c;
    }
  }
  return NULL;
}

/* InputWindow */

InputWindow::InputWindow(int maxrows, int maxcols)
  : m_maxrows(maxrows),
    m_maxcols(maxcols) {
  if (maxrows <= 0 || maxcols <= 0) {
    throw ISError("Cannot create InputWindow with invalid dimensions");
  }
}

WindowResponse InputWindow::Input(int y, int x, int ch) {
  // The input char might be a special character like movement or delete.
  if (KEY_LEFT == ch) {
    WindowResponse response;
    if (m_linebuf.HasChar(y, x-1)) {
      response.actions.push_back(WindowAction(WindowAction::MOVE, y, x-1, ch));
    }
    return response;
  } else if (KEY_RIGHT == ch) {
    WindowResponse response;
    if (m_linebuf.HasChar(y, x+1)) {
      response.actions.push_back(WindowAction(WindowAction::MOVE, y, x+1, ch));
    }
    return response;
  } else if (KEY_UP == ch) {
    WindowResponse response;
    if (m_linebuf.HasLine(y-1)) {
      if (!m_linebuf.HasChar(y-1, x)) {
        x = m_linebuf.LastIndexOfLine(y-1);
      }
      response.actions.push_back(WindowAction(WindowAction::MOVE, y-1, x, ch));
    }
    return response;
  } else if (KEY_DOWN == ch) {
    WindowResponse response;
    if (m_linebuf.HasLine(y+1)) {
      if (!m_linebuf.HasChar(y+1, x)) {
        x = m_linebuf.LastIndexOfLine(y+1);
      }
      response.actions.push_back(WindowAction(WindowAction::MOVE, y+1, x, ch));
    }
    return response;
  } else if (KEY_DC == ch) {
    return m_linebuf.Delete(y, x);
  } else if (KEY_BACKSPACE == ch) {
    return m_linebuf.Backspace(y, x);
  } else if (KEY_ENTER == ch || '\n' == ch) {
    //return m_linebuf.Enter(y, x, ch);
  } else {
    return m_linebuf.Insert(y, x, ch);
  }
  throw ISError(string("Failed to accept input char ") + (char)ch);
}
