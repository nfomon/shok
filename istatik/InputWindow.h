// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _InputWindow_h_
#define _InputWindow_h_

#include "ISError.h"
#include "WindowResponse.h"

#include "statik/IList.h"

#include <boost/lexical_cast.hpp>

#include <vector>

namespace istatik {

struct CharList {
  CharList(int y, int x, int ch)
    : y(y),
      x(x),
      ch(ch),
      inode(boost::lexical_cast<std::string>((char)ch)),
      prev(NULL),
      next(NULL) {}
  int y;
  int x;
  int ch;
  statik::IList inode;
  CharList* prev;
  CharList* next;
};

std::ostream& operator<< (std::ostream& out, const CharList& node);

class InputWindow {
public:
  InputWindow(int maxrows, int maxcols);
  ~InputWindow();
  WindowResponse Input(int y, int x, int ch);
private:
  CharList* FindNode(int y, int x);

  int m_maxrows;
  int m_maxcols;

  CharList* m_start;
};

}

#endif // _InputWindow_h_
