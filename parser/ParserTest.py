import unittest
from LexToken import LexToken
from MakeRule import MakeRule
from Parser import MakeParser
from ActionParser import Action
from OrParser import Or
from PlusParser import Plus
from SeqParser import Seq
from StarParser import Star
from TerminalParser import Terminal

# Initial state for a parser
def ini(bad,done,display=None):
  return {
    'bad': bad,
    'done': done,
    'display': display,
  }
init = ini(False, False)
initDone = ini(False, True)

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
  def stateTest(self,parser,t):
    self.assertEqual(parser.bad, t['bad'])
    self.assertEqual(parser.done, t['done'])
    if parser.done and t['done'] and t['display'] != None:
      self.assertEqual(parser.display(), t['display'])

  def ruleTest(self,rule,series):
    for s in series:
      #print "s: %s" % s
      parser = MakeParser(rule)
      self.stateTest(parser, s[0])
      for i, t in enumerate(s[1:]):
        #print "sending token: %s,%s" % (i, t)
        parser.parse(t['tok'])
        self.stateTest(parser, t)


class TestTerminals(RuleTester):
  # Terminal rules made from strings
  def test_Str(self):
    token = 'A'
    rule = MakeRule(token)
    self.series = [
      [init, tok('1:A', False, True, 'A'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, self.series)

  # Terminal rules made directly as Terminal rule instances
  def test_Term(self):
    rule = Terminal('a', ['A'])
    self.series = [
      [init, tok('1:A', False, True, 'A'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, self.series)

  # Terminal rule that has its own display representation
  def test_TermDisp(self):
    rule = Terminal('a', ['A'], 'foo:%s')
    self.series = [
      [init, tok('1:A', False, True, 'foo:A'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, self.series)


class TestSeq(RuleTester):
  def test_One(self):
    rule = Seq('one', ['A'], 'foo:%s')
    series = [
      [init, tok('1:A', False, True, 'foo:A'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Two(self):
    rule = Seq('two', ['A', 'B'], ':%s:%s:foo', [1, 0])
    series = [
      [init, tok('1:A', False, False),
             tok('1:B', False, True, ':B:A:foo'),
             tok('1:B', True, False)],
      [init, tok('1:A', False, False),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Plus(self):
    rule = Seq('seq', [
        Plus('+1', Seq('p1', ['A', 'B', 'C'], 'p1 %s%s%s', [0, 1, 2]), '+1 %s'),
        Plus('+2', Seq('p2', ['C', 'D', 'E'], 'p2 %s%s%s', [0, 1, 2]), '+2 %s'),
      ], '(seq %s %s)', [0, 1])
    series = [
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False),
             tok('1:Q', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False),
             tok('1:C', False, False),
             tok('1:D', False, False),
             tok('1:E', False, True, '(seq +1 p1 ABC +2 p2 CDE)')],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False),
             tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False),
             tok('1:C', False, False),
             tok('1:D', False, False),
             tok('1:E', False, True, '(seq +1 p1 ABC+1 p1 ABC +2 p2 CDE)'),
             tok('1:C', False, False),
             tok('1:D', False, False),
             tok('1:E', False, True, '(seq +1 p1 ABC+1 p1 ABC +2 p2 CDE+2 p2 CDE)')],
    ]
    self.ruleTest(rule, series)

  def test_PlusLimited(self):
    # Old limitation of SeqParser: if one stage is done, but then accepts a
    # token (does not become bad), the SeqParser would not advance even if it
    # would have needed to do so to ultimately parse the whole sequence.
    #
    # This rule used to be impossible to parse.
    rule = Seq('seq', [
        Plus('+1',
          Seq('p1', ['A', 'B', 'C'], 'p1 %s%s%s', [0, 1, 2]),
          '+1 %s'),
        Plus('+2',
          Seq('p2', ['A', 'D', 'E'], 'p2 %s%s%s', [0, 1, 2]),
          '+2 %s'),
      ], '(seq %s %s)', [0, 1])
    series = [
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False),
             tok('1:A', False, False),
#             tok('1:D', True, False)],   # Old limitation!
             tok('1:D', False, False),
             tok('1:E', False, True, '(seq +1 p1 ABC +2 p2 ADE)')],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False),
             tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False),
             tok('1:A', False, False),
             tok('1:D', False, False),
             tok('1:E', False, True, '(seq +1 p1 ABC+1 p1 ABC +2 p2 ADE)'),
             tok('1:A', False, False),
             tok('1:D', False, False),
             tok('1:E', False, True, '(seq +1 p1 ABC+1 p1 ABC +2 p2 ADE+2 p2 ADE)')],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False),
             tok('1:C', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_StarStartEnd(self):
    rule = Seq('seq', [
        Star('*1',
          Seq('p1', ['A', 'B', 'C'], 'p1 %s%s%s', [0, 1, 2]),
          '*1 %s'),
        'D',
        'E',
        Star('*2',
          Seq('p2', ['F', 'G', 'H'], 'p2 %s%s%s', [0, 1, 2]),
          '*2 %s'),
      ], '(seq %s <%s%s> %s)', [0, 1, 2, 3])
    series = [
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False),
             tok('1:D', False, False),
             tok('1:E', False, True, '(seq *1 p1 ABC <DE> )'),
             tok('1:F', False, False),
             tok('1:G', False, False),
             tok('1:H', False, True, '(seq *1 p1 ABC <DE> *2 p2 FGH)')],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False),
             tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False),
             tok('1:D', False, False),
             tok('1:E', False, True, '(seq *1 p1 ABC*1 p1 ABC <DE> )'),
             tok('1:F', False, False),
             tok('1:G', False, False),
             tok('1:H', False, True, '(seq *1 p1 ABC*1 p1 ABC <DE> *2 p2 FGH)'),
             tok('1:F', False, False),
             tok('1:G', False, False),
             tok('1:H', False, True, '(seq *1 p1 ABC*1 p1 ABC <DE> *2 p2 FGH*2 p2 FGH)')],
    ]
    self.ruleTest(rule, series)

 
class TestOr(RuleTester):
  def test_One(self):
    rule = Or('one', ['A'], 'foo:%s')
    series = [
      [init, tok('1:A', False, True, 'foo:A'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Two(self):
    rule = Or('two', ['A', 'B'], 'foo:%s')
    series = [
      [init, tok('1:A', False, True, 'foo:A'),
             tok('1:B', True, False)],
      [init, tok('1:B', False, True, 'foo:B'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Many(self):
    rule = Or('many', [
      Seq('seq1', ['A', 'B'], '(seq1:%s:%s)', [0, 1]),
      'Q',
      Terminal('qq', ['QQ'], '(term %s)'),
      Seq('seq2', ['B', 'C', 'D'], '(seq2:%s:%s:%s)', [0, 1, 2]),
      Seq('seq3', ['A', 'D'], '(seq3:%s:%s)', [0, 1]),
      Seq('seq4', ['A', 'D', 'E'], '(seq4:%s:%s:%s)', [0, 1, 2]),
      Seq('seq5', ['B', 'C', 'D', 'E', 'F'], '(seq5:%s..%s)', [0, 4]),
    ], '! %s')
    series = [
      [init, tok('1:X', True, False)],
      [init, tok('1:Q', False, True, '! Q'),
             tok('1:QQ', True, False)],
      [init, tok('1:QQ', False, True, '! (term QQ)'),
             tok('1:Q', True, False)],
      [init, tok('1:A', False, False),
             tok('1:C', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, True, '! (seq1:A:B)'),
             tok('1:D', True, False)],
      [init, tok('1:A', False, False),
             tok('1:D', False, True, '! (seq3:A:D)'),
             tok('1:E', False, True, '! (seq4:A:D:E)'),
             tok('1:E', True, False)],
      [init, tok('1:B', False, False),
             tok('1:C', False, False),
             tok('1:D', False, True, '! (seq2:B:C:D)'),
             tok('1:E', False, False),
             tok('1:F', False, True, '! (seq5:B..F)'),
             tok('1:E', True, False)],
    ]
    self.ruleTest(rule, series)


class TestPlus(RuleTester):
  # Plus with a terminal
  def test_Term(self):
    rule = Plus('term', 'A', 'foo:%s')
    series = [
      [init, tok('1:A', False, True, 'foo:A'),
             tok('1:A', False, True, 'foo:Afoo:A'),
             tok('1:A', False, True, 'foo:Afoo:Afoo:A'),
             tok('1:X', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Seq(self):
    rule = Plus('plus',
      Seq('seq',
        ['A', 'B', 'C'],
        '(seq %s,%s,%s)', [0, 1, 2]),
      '(plus %s)')
    series = [
      [init, tok('1:X', True, False)],
      [init, tok('1:A', False, False),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, True, '(plus (seq A,B,C))'),
             tok('1:C', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, True, '(plus (seq A,B,C))'),
             tok('1:A', False, False),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, True, '(plus (seq A,B,C))'),
             tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, True, '(plus (seq A,B,C))(plus (seq A,B,C))'),
             tok('1:C', True, False)],
    ]
    self.ruleTest(rule, series)


class TestStar(RuleTester):
  # Plus with a terminal
  def test_Term(self):
    start = ini(False, True, '')
    rule = Star('term', 'A', 'foo:%s')
    series = [
      [start, tok('1:A', False, True, 'foo:A'),
              tok('1:A', False, True, 'foo:Afoo:A'),
              tok('1:A', False, True, 'foo:Afoo:Afoo:A'),
              tok('1:X', True, False)],
      [start, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Seq(self):
    start = ini(False, True, '')
    rule = Star('star',
      Seq('seq',
        ['A', 'B', 'C'],
        '(seq %s,%s,%s)', [0, 1, 2]),
      '(* %s)')
    series = [
      [start, tok('1:X', True, False)],
      [start, tok('1:A', False, False),
              tok('1:X', True, False)],
      [start, tok('1:A', False, False),
              tok('1:B', False, False),
              tok('1:X', True, False)],
      [start, tok('1:A', False, False),
              tok('1:B', False, False),
              tok('1:C', False, True, '(* (seq A,B,C))'),
              tok('1:C', True, False)],
      [start, tok('1:A', False, False),
              tok('1:B', False, False),
              tok('1:C', False, True, '(* (seq A,B,C))'),
              tok('1:A', False, False),
              tok('1:X', True, False)],
      [start, tok('1:A', False, False),
              tok('1:B', False, False),
              tok('1:C', False, True, '(* (seq A,B,C))'),
              tok('1:A', False, False),
              tok('1:B', False, False),
              tok('1:C', False, True, '(* (seq A,B,C))(* (seq A,B,C))'),
              tok('1:C', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Double(self):
    start = ini(False, True, '')
    rule = Star('star',
      Seq('seq',
        [Star('*1', 'A', '(*1%s)'),
        Star('*2', 'B', '(*2%s)')],
        '(seq %s %s)', [0, 1]
      ), '(star %s)')
    series = [
      [start, tok('1:X', True, False)],
      [start, tok('1:A', False, True, '(star (seq (*1A) ))'),
              tok('1:A', False, True, '(star (seq (*1A)(*1A) ))'),
              tok('1:B', False, True, '(star (seq (*1A)(*1A) (*2B)))'),
              tok('1:B', False, True, '(star (seq (*1A)(*1A) (*2B)(*2B)))'),
              tok('1:X', True, False)],
      [start, tok('1:B', False, True, '(star (seq  (*2B)))'),
              tok('1:A', False, True, '(star (seq  (*2B)))(star (seq (*1A) ))'),
              tok('1:B', False, True, '(star (seq  (*2B)))(star (seq (*1A) (*2B)))')],
    ]
    self.ruleTest(rule, series)


def suite():
  suite = unittest.TestSuite()
  testcases = [TestTerminals, TestPlus, TestStar, TestSeq, TestOr]
  suite.addTests([unittest.TestLoader().loadTestsFromTestCase(x) for x in testcases])
  return suite

if __name__ == '__main__':
  unittest.TextTestRunner(verbosity=2).run(suite())
