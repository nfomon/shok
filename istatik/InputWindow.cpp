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

Char::Char(int ch)
  : ch(ch),
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

void Char::Unlink() {
  if (inode.left) {
    inode.left->right = inode.right;
  }
  if (inode.right) {
    inode.right->left = inode.left;
  }
}

Char::operator std::string() const {
  return string(1, ch);
}

ostream& istatik::operator<< (ostream& out, const Char& node) {
  out << (char)node.ch;
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

size_t Line::Size() const {
  return m_chars.size();
}

string Line::GetString(int startx) const {
  string s;
  for (size_t x = startx; x < m_chars.size(); ++x) {
    s += (char)m_chars.at(x)->ch;
  }
  return s;
}

Char* Line::GetFirst() {
  if (m_chars.empty()) {
    return NULL;
  }
  return m_chars.at(0);
}

Char* Line::GetLast() {
  if (m_chars.empty()) {
    return NULL;
  }
  return m_chars.back();
}

Char* Line::GetBefore(int x) {
  if (m_chars.empty() || 0 == x) {
    return NULL;
  } else if ((size_t)x >= m_chars.size()) {   // TODO is this right? lol
    return m_chars.back();
  }
  return m_chars.at(x-1);
}

Char* Line::GetAtOrAfter(int x) {
  if (m_chars.empty()) {
    return NULL;
  } else if ((size_t)x >= m_chars.size()) {
    return NULL;
  }
  return m_chars.at(x);
}

Char* Line::Insert(int x, int ch) {
  Char* c = m_charpool.Insert(auto_ptr<Char>(new Char(ch)));
  g_log.info() << "Line " << y << " inserting " << *c;
  if ((size_t)x > m_chars.size()) {
    throw ISError(string("Cannot insert ") + (char)ch + "; x out of bounds");
  } else if (m_chars.size() == (size_t)x) {
    m_chars.push_back(c);
  } else {
    m_chars.insert(m_chars.begin() + x, c);
  }
  return c;
}

Char* Line::Insert(int x, auto_ptr<Char> ac) {
  Char* c = m_charpool.Insert(ac);  // FIXME ObjectPool violation... ah well.
                                    // OP is dumb anyway.
  g_log.info() << "Line " << y << " inserting " << *c;
  if ((size_t)x > m_chars.size()) {
    std::stringstream s;
    s << "Cannot insert " << *c << "; x out of bounds";
    throw ISError(s.str());
  } else if (m_chars.size() == (size_t)x) {
    m_chars.push_back(c);
  } else {
    m_chars.insert(m_chars.begin() + x, c);
  }
  return c;
}

Char* Line::Delete(int x) {
  if ((size_t)x >= m_chars.size()) {
    throw ISError("Cannot delete; x out of bounds");
  }
  Char* c = m_chars.at(x);
  c->Unlink();
  m_charpool.Unlink(*c);
  m_chars.erase(m_chars.begin() + x);
  return c;
}

auto_ptr<Char> Line::Extract(int x) {
  if ((size_t)x >= m_chars.size()) {
    throw ISError("Cannot extract; x out of bounds");
  }
  Char* c = m_chars.at(x);
  g_log.debug() << " before extract: " << m_chars.size();
  m_chars.erase(m_chars.begin() + x);
  g_log.debug() << " after extract: " << m_chars.size();
  return m_charpool.Extract(*c);
}

/* LineBuf public */

LineBuf::LineBuf(size_t maxcols)
  : m_maxcols(maxcols) {
}

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
  } else if (m_lines.at(y).Size() == m_maxcols-2) {
    // Line too long; input ignored
    return response;
  }
  g_log.info() << "LineBuf inserting (" << y << "," << x << "):" << (char)ch;
  Char* prev = FindBefore(y, x);
  Char* next = FindAtOrAfter(y, x);
  Char* c = m_lines.at(y).Insert(x, ch);
  int x0 = x+1;
  int y0 = y;
  // Re-draw the window starting at the newly-entered character
  do {
    response.actions.push_back(WindowAction(WindowAction::MOVE, y, x));
    string s = m_lines.at(y).GetString(x);
    g_log.debug() << "Printing str: " << s;
    for (size_t i = 0; i < s.size(); ++i) {
      response.actions.push_back(WindowAction(WindowAction::INSERT, y, x + i, s[i]));
    }
    x = 0;
    ++y;
  } while ((size_t)y < m_lines.size());
  response.batch.Insert(c->inode, (prev ? &prev->inode : NULL));
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
  statik::List* istart = &c->inode;
  while (istart->left) {
    istart = istart->left;
  }
  response.actions.push_back(WindowAction(WindowAction::MOVE, y0, x0));
  g_log.info() << "window0 batch: " << response.batch.Print();
  return response;
}

WindowResponse LineBuf::Enter(int y, int x, int ch) {
  g_log.debug() << "Processing Enter at (" << y << "," << x << ")";
  g_log.debug();
  WindowResponse response;
  if ((size_t)y > m_lines.size()) {
    throw ISError(string("Cannot insert Enter; y out of bounds"));
  } else if (m_lines.size() == (size_t)y) {
    g_log.debug() << "Inserting blank line right here at end";
    m_lines.push_back(new Line(y));
  } else {
    g_log.debug() << "Inserting blank line in the middle";
    m_lines.insert(m_lines.begin()+y+1, new Line(y+1));
    for (line_vec::iterator i = m_lines.begin()+y+2; i != m_lines.end(); ++i) {
      ++i->y;
    }
  }
  // Clear end of this line
  while ((size_t)x < m_lines.at(y).Size()) {
    g_log.debug() << "Enter: clearing (" << y << "," << m_lines.at(y).LastIndex() << ")";
    response.actions.push_back(WindowAction(WindowAction::DELETE));
    auto_ptr<Char> c = m_lines.at(y).Extract(m_lines.at(y).LastIndex());
    m_lines.at(y+1).Insert(0, c);
  }

  // Next line is populated; draw it
  string s = m_lines.at(y+1).GetString(0);
  response.actions.push_back(WindowAction(WindowAction::MOVE, y+1, 0));
  for (size_t i = 0; i < s.size(); ++i) {
    g_log.debug() << "Enter: inserting " << y+1 << "," << i << ":" << s[i];
    response.actions.push_back(WindowAction(WindowAction::INSERT, y+1, i, s[i]));
  }
  response.actions.push_back(WindowAction(WindowAction::MOVE, y+1, 0));
  return response;
}

WindowResponse LineBuf::Delete(int y, int x) {
  WindowResponse response;
  if ((size_t)y > m_lines.size()) {
    throw ISError("Cannot delete; y out of bounds");
  } else if ((size_t)y == m_lines.size()) {
    return response;
  } else if ((size_t)x > m_lines.at(y).Size()) {
    throw ISError("Cannot delete; x out of bounds");
  } else if ((size_t)x == m_lines.at(y).Size()) {
    return response;
  }
  g_log.info() << "LineBuf deleting (" << y << "," << x << ")";
  Char* c = m_lines.at(y).Delete(x);
  response.actions.push_back(WindowAction(WindowAction::MOVE, y, x));
  response.actions.push_back(WindowAction(WindowAction::DELETE));
  response.batch.Delete(c->inode);
  return response;
}

WindowResponse LineBuf::Backspace(int y, int x) {
  if (0 == x) {
    return WindowResponse();
  }
  WindowResponse response = Delete(y, x-1);
  response.actions.push_back(WindowAction(WindowAction::MOVE, y, x-1));
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

InputWindow::InputWindow(size_t maxrows, size_t maxcols)
  : m_maxrows(maxrows),
    m_linebuf(maxcols) {
  if (maxrows <= 0 || maxcols <= 0) {
    throw ISError("Cannot create InputWindow with invalid dimensions");
  }
}

WindowResponse InputWindow::Input(int y, int x, int ch) {
  // The input char might be a special character like movement or delete.
  if (KEY_LEFT == ch) {
    WindowResponse response;
    if (m_linebuf.HasChar(y, x-1)) {
      response.actions.push_back(WindowAction(WindowAction::MOVE, y, x-1));
    }
    return response;
  } else if (KEY_RIGHT == ch) {
    WindowResponse response;
    if (m_linebuf.HasChar(y, x)) {
      response.actions.push_back(WindowAction(WindowAction::MOVE, y, x+1));
    }
    return response;
  } else if (KEY_UP == ch) {
    WindowResponse response;
    if (m_linebuf.HasLine(y-1)) {
      if (!m_linebuf.HasChar(y-1, x)) {
        x = std::max(m_linebuf.LastIndexOfLine(y-1), 0);
      }
      response.actions.push_back(WindowAction(WindowAction::MOVE, y-1, x));
    }
    return response;
  } else if (KEY_DOWN == ch) {
    WindowResponse response;
    if (m_linebuf.HasLine(y+1)) {
      if (!m_linebuf.HasChar(y+1, x)) {
        x = std::max(m_linebuf.LastIndexOfLine(y+1), 0);
      }
      response.actions.push_back(WindowAction(WindowAction::MOVE, y+1, x));
    } else if (m_linebuf.HasLine(y)) {
      response.actions.push_back(WindowAction(WindowAction::MOVE, y+1, 0));
    }
    return response;
  } else if (KEY_DC == ch) {
    return m_linebuf.Delete(y, x);
  } else if (KEY_BACKSPACE == ch) {
    return m_linebuf.Backspace(y, x);
  } else if (KEY_ENTER == ch || '\n' == ch) {
    return m_linebuf.Enter(y, x, ch);
  } else {
    return m_linebuf.Insert(y, x, ch);
  }
  throw ISError(string("Failed to accept input char ") + (char)ch);
}
