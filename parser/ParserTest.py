import unittest
from LexToken import LexToken
from MakeRule import MakeRule
from Parser import MakeParser
from PlusParser import Plus
from TerminalParser import Terminal

# Initial state for a parser
def ini(bad,done):
  return {
    'bad': bad,
    'done': done,
  }

# A token and its expected results from a parser
def tok(tokstr,bad,done,display=None):
  return {
    'tok': LexToken(tokstr),
    'bad': bad,
    'done': done,
    'display': display,
  }

# TestCase that will validate a parser's state as it advances through a
# series of token sequences (each starting from scratch)
class RuleTester(unittest.TestCase):
  def RuleTest(self,rule,series):
    for s in series:
      #print "s: %s" % s
      parser = MakeParser(rule)
      self.assertEqual(parser.bad, s[0]['bad'])
      self.assertEqual(parser.done, s[0]['done'])
      for i, t in enumerate(s[1:]):
        #print "sending token: %s,%s" % (i, t)
        st = s[i+1]
        parser.parse(t['tok'])
        self.assertEqual(parser.bad, st['bad'])
        self.assertEqual(parser.done, st['done'])
        if parser.done and st['done'] and st['display'] != None:
          self.assertEqual(parser.display(), st['display'])


class TestTerminals(RuleTester):
  def setUp(self):
    self.series = [
      [ini(False, False), tok('1:A', False, True, 'A'),
                          tok('1:A', True, False)],
      [ini(False, False), tok('1:X', True, False)],
    ]

  # Terminal rules made from strings
  def test_Str(self):
    token = 'A'
    rule = MakeRule(token)
    self.RuleTest(rule, self.series)

  # Terminal rules made directly as Terminal rule instances
  def test_Term(self):
    rule = Terminal('a', 'A')
    self.RuleTest(rule, self.series)

  # Terminal rule that has its own display representation
  def test_TermDisp(self):
    rule = Terminal('a', 'A', 'foo:%s')
    self.RuleTest(rule, self.series)


class TestPlus(RuleTester):
  # Plus with a terminal
  def test_Term(self):
    rule = Plus('term', 'A', 'foo:%s')
    series = [
      [ini(False, False), tok('1:A', False, True, 'foo:A'),
                          tok('1:A', False, True, 'foo:Afoo:A'),
                          tok('1:A', False, True, 'foo:Afoo:Afoo:A'),
                          tok('1:X', True, False)],
      [ini(False, False), tok('1:X', True, False)],
    ]
    self.RuleTest(rule, series)


def suite():
  suite = unittest.TestSuite()
  testcases = [TestTerminals, TestPlus]
  suite.addTests([unittest.TestLoader().loadTestsFromTestCase(x) for x in testcases])
  return suite

if __name__ == '__main__':
  unittest.TextTestRunner(verbosity=2).run(suite())
