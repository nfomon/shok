// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _InputWindow_h_
#define _InputWindow_h_

#include "ISError.h"
#include "WindowResponse.h"

#include "statik/IList.h"
#include "statik/ObjectPool.h"

#include <boost/ptr_container/ptr_vector.hpp>

#include <ostream>
#include <string>

namespace istatik {

struct Char {
  Char(int ch);
  void SetNext(Char* next);
  void SetPrevious(Char* prev);
  void Unlink();
  operator std::string() const;

  int ch;
  statik::IList inode;
};
std::ostream& operator<< (std::ostream& out, const Char& node);

class Line {
public:
  Line(int y);
  bool HasChar(int x) const;
  int LastIndex() const;
  size_t Size() const;
  std::string GetString(int startx = 0) const;
  Char* GetFirst();
  Char* GetLast();
  Char* GetBefore(int x);
  Char* GetAtOrAfter(int x);
  Char* Insert(int x, int ch);
  Char* Delete(int x);
private:
  typedef std::vector<Char*> char_vec;
  int y;
  char_vec m_chars;
  statik::ObjectPool<Char> m_charpool;
};

class LineBuf {
public:
  LineBuf(size_t maxcols);
  bool HasChar(int y, int x) const;
  bool HasLine(int y) const;
  int LastIndexOfLine(int y) const;
  WindowResponse Insert(int y, int x, int ch);
  WindowResponse Enter(int y, int x, int ch);
  WindowResponse Delete(int y, int x);
  WindowResponse Backspace(int y, int x);
private:
  typedef boost::ptr_vector<Line> line_vec;
  size_t m_maxcols;
  line_vec m_lines;
  Char* FindBefore(int y, int x);
  Char* FindAtOrAfter(int y, int x);
};

class InputWindow {
public:
  InputWindow(size_t maxrows, size_t maxcols);
  WindowResponse Input(int y, int x, int ch);
private:
  size_t m_maxrows;
  LineBuf m_linebuf;
};

}

#endif // _InputWindow_h_
