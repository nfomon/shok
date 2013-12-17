# Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
# directory of this distribution and at http://shok.io/code/copyright.html

import unittest
from ShokParser import ShokParser
from LexToken import LexToken

class ShokTester(unittest.TestCase):
  def shokTestAll(self,tests):
    for test in tests:
      self.shokTest(test[0], test[1])

  def shokTest(self,inp,out,bad=False,incomplete=False):
    parser = ShokParser()
    for word in inp.split():
      tok = '1:%s' % word
      parser.parse(LexToken(tok))
      if parser.bad:
        self.assertTrue(bad)
        break
    if not parser.done:
      self.assertTrue(incomplete)
      return
    self.assertEqual(parser.ast, out)

class TestCmdLine(ShokTester):
  def test_Cmd(self):
    self.shokTestAll([
      ("ID:'ls' NEWL",
        "[ls]"),
      ("WS ID:'ls' WS MINUS ID:'al' WS ID:'foo.txt' WS NEWL",
        "[ls -al foo.txt]"),
    ])

  def test_ExpBlock(self):
    self.shokTestAll([
      ("LBRACE ID:'foo' RBRACE WS ID:'lolz' NEWL",
        "[{(exp ID:'foo')} lolz]"),
      ("LBRACE ID:'foo' WS RBRACE WS ID:'one' WS LBRACE ID:'two' RBRACE NEWL",
        "[{(exp ID:'foo')} one {(exp ID:'two')}]"),
      ("WS LBRACE WS ID:'foo' WS RBRACE WS ID:'one' WS LBRACE WS ID:'two' WS RBRACE WS NEWL",
        "[{(exp ID:'foo')} one {(exp ID:'two')}]"),
    ])

  def test_CodeBlock(self):
    self.shokTestAll([
      ("WS LBRACE WS NEWL WS NEWL NEWL WS NEWL WS RBRACE WS NEWL",
        "[{}]"),
    ])


def suite():
  suite = unittest.TestSuite()
  testcases = [TestCmdLine]
  suite.addTests([unittest.TestLoader().loadTestsFromTestCase(x) for x in testcases])
  return suite

if __name__ == '__main__':
  unittest.TextTestRunner(verbosity=2).run(suite())
