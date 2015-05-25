// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Keyword.h"

#include "statik/Connector.h"
#include "statik/IList.h"
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
      statik::Connector c(*r.get(), "test1");
    } catch (const statik::SError& e) {
      p = true;
    }
    test(p, "Empty keyword not allowed");
  }

  // Single-character keyword
  {
    auto_ptr<statik::Rule> r(KEYWORD("a"));
    statik::Connector c(*r.get(), "test2");

    statik::IList c1("1", "x");
    c.Insert(c1);
    const statik::STree& root = c.GetRoot();
    test(root.GetState().IsBad(), "x");

    c.Delete(c1);
    test(root.IsClear());

    statik::IList c2("2", "a");
    c.Insert(c2);
    test(root.GetState().IsDone(), "a");

    statik::IList c3("3", "b");
    c3.left = &c2;
    c2.right = &c3;
    c.Insert(c3);
    test(root.GetState().IsComplete(), "ab");

    statik::IList c4("4", "c");
    c4.left = &c3;
    c3.right = &c4;
    c.Insert(c4);
    test(root.GetState().IsBad(), "abc");

    c2.right = &c4;
    c4.left = &c2;
    c.Delete(c3);
    test(root.GetState().IsComplete(), "ac");

    c4.left = NULL;
    c.Delete(c2);
    g_log.info() << root << " ; " << root.GetState();
    test(root.GetState().IsBad(), "c");

    c4.left = &c2;
    c.Insert(c2);
    test(root.GetState().IsComplete(), "ac");

    c2.right = NULL;
    c.Delete(c4);
    test(root.GetState().IsDone(), "a");

    c.Delete(c2);
    test(root.IsClear());
  }
}
