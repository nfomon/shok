// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Keyword.h"

#include "statik/IList.h"
#include "statik/IncParser.h"
#include "statik/Keyword.h"
#include "statik/Rule.h"
#include "statik/SError.h"
#include "statik/STree.h"
using statik::KEYWORD;

#include <memory>
using std::auto_ptr;

using namespace statik_test;

void Keyword::run() {

  // Blank keyword
  {
    bool p = false;
    try {
      auto_ptr<statik::Rule> r(KEYWORD(""));
      statik::IncParser ip(*r.get(), "test1");
    } catch (const statik::SError& e) {
      p = true;
    }
    test(p, "Empty keyword not allowed");
  }

  // Single-character keyword
  {
    auto_ptr<statik::Rule> r(KEYWORD("a"));
    statik::IncParser ip(*r.get(), "test2");

    statik::IList c1("1", "x");
    ip.Insert(c1);
    const statik::STree& root = ip.GetRoot();
    test(root.GetState().IsBad(), "x");

    ip.Delete(c1);
    test(root.IsClear());

    statik::IList c2("2", "a");
    ip.Insert(c2);
    test(root.GetState().IsDone(), "a");

    statik::IList c3("3", "b");
    c3.left = &c2;
    c2.right = &c3;
    ip.Insert(c3);
    test(root.GetState().IsComplete(), "ab");

    statik::IList c4("4", "c");
    c4.left = &c3;
    c3.right = &c4;
    ip.Insert(c4);
    test(root.GetState().IsBad(), "abc");

    c2.right = &c4;
    c4.left = &c2;
    ip.Delete(c3);
    test(root.GetState().IsComplete(), "ac");

    c4.left = NULL;
    ip.Delete(c2);
    g_log.info() << root << " ; " << root.GetState();
    test(root.GetState().IsBad(), "c");

    c4.left = &c2;
    ip.Insert(c2);
    test(root.GetState().IsComplete(), "ac");

    c2.right = NULL;
    ip.Delete(c4);
    test(root.GetState().IsDone(), "a");

    ip.Delete(c2);
    test(root.IsClear());
  }
}
