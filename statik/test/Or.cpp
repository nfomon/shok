// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Or.h"

#include "statik/Batch.h"
#include "statik/IncParser.h"
#include "statik/Keyword.h"
#include "statik/List.h"
#include "statik/Or.h"
#include "statik/Rule.h"
#include "statik/SError.h"
#include "statik/STree.h"
#include "statik/State.h"
using statik::Batch;
using statik::IncParser;
using statik::KEYWORD;
using statik::List;
using statik::OR;
using statik::Rule;
using statik::State;

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace statik_test;

void Or::run() {
  g_log.info();
  g_log.info() << "Blank Or";
  {
    bool p = false;
    string msg;
    try {
      auto_ptr<Rule> r(OR("Or"));
      IncParser ip(r, "Or test 1");
      List cx("cx", "x");
      ip.Insert(cx, NULL);
    } catch (const statik::SError& e) {
      p = true;
      msg = e.what();
    }
    test(p, "Empty Or not allowed: " + msg);
  }

  // I used to run this test ("wrong input") and then in the same IncParser do
  // the subsequent (reasonable input).  Separated them for now until the
  // latter test passes at least in isolation...
  {
    g_log.info();
    g_log.info() << "Or with one Keyword child, wrong input";
    {
      auto_ptr<Rule> kr(KEYWORD("a"));
      auto_ptr<Rule> r(OR("Or"));
      r->AddChild(kr);
      IncParser ip(r, "Or test 2");
      const statik::STree& root = ip.GetRoot();

      {
        Batch out_batch;
        List cx("cx", "x");
        ip.Insert(cx, NULL);
        test(root.GetState().IsBad(), "x");
        ip.ExtractChanges(out_batch);
        test(out_batch.IsEmpty(), "no output");
        out_batch.Clear();

        ip.Delete(cx);
        test(root.IsClear(), "clear");
        ip.ExtractChanges(out_batch);
        test(out_batch.IsEmpty(), "no output");
        out_batch.Clear();
      }
    }

    g_log.info();
    g_log.info() << "Or with one Keyword child";
    auto_ptr<Rule> kr(KEYWORD("a"));
    auto_ptr<Rule> r(OR("Or"));
    r->AddChild(kr);
    IncParser ip(r, "Or test 2");
    const statik::STree& root = ip.GetRoot();
    {
      Batch out_batch;
      List ca("a", "");
      ip.Insert(ca, NULL);
      test(root.GetState(), State(State::ST_DONE), "a");
      ip.ExtractChanges(out_batch);

      {
        Batch expected_batch;
        expected_batch.Insert(ca, NULL);
        test(out_batch, expected_batch, "output");
      }
      out_batch.Clear();

      List cb("cb", "b");
      ip.Insert(cb, &ca);
      test(root.GetState(), State(State::ST_COMPLETE), "ab");

      List cc("cc", "c");
      ip.Insert(cc, &cb);
      test(root.GetState().IsBad(), "abc");

      ip.Delete(cb);
      test(root.GetState().IsComplete(), "ac");

      ip.Delete(ca);
      test(root.GetState().IsBad(), "c");

      List ca2("2", "a");
      ip.Insert(ca2, NULL);
      test(root.GetState().IsComplete(), "ac");

      ip.Delete(cc);
      test(root.GetState().IsDone(), "a");

      ip.Delete(ca2);
      test(root.IsClear());
    }
  }

  g_log.info();
  g_log.info() << "Or with two Keyword children";
  {
    auto_ptr<Rule> kr1(KEYWORD("abcde"));
    auto_ptr<Rule> kr2(KEYWORD("abcfg"));
    auto_ptr<Rule> r(OR("Or"));
    r->AddChild(kr1);
    r->AddChild(kr2);
    IncParser ip(r, "Or test 3");
    const statik::STree& root = ip.GetRoot();

    {
      Batch out_batch;
      List ca("ca", "a");
      ip.Insert(ca, NULL);
      test(root.GetState().IsOK(), "a");

      List cb("cb", "b");
      ip.Insert(cb, &ca);
      test(root.GetState().IsOK(), "ab");

      List cc("cc", "c");
      ip.Insert(cc, &cb);
      test(root.GetState().IsOK(), "abc");

      List cd("cd", "d");
      ip.Insert(cd, &cc);
      test(root.GetState().IsOK(), "abcd");

      List ce("ce", "e");
      ip.Insert(ce, &cd);
      test(root.GetState().IsDone(), "abcde");
      ip.ExtractChanges(out_batch);
      {
        Batch expected_batch;
        List e("abcde", "");
        expected_batch.Insert(e, NULL);
        test(out_batch, expected_batch, "output");
      }
      out_batch.Clear();

      List cx("cx", "x");
      ip.Insert(cx, &ce);
      test(root.GetState().IsComplete(), "abcdex");

      ip.Delete(cx);
      test(root.GetState().IsDone(), "abcde");

      ip.Delete(ce);
      test(root.GetState().IsOK(), "abcd");

      ip.Delete(cd);
      test(root.GetState().IsOK(), "abc");

      List cf("cf", "f");
      ip.Insert(cf, &cc);
      test(root.GetState().IsOK(), "abcf");

      List cg("cg", "g");
      ip.Insert(cg, &cf);
      test(root.GetState().IsDone(), "abcfg");
    }
  }
}
