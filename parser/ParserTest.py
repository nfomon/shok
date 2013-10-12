# Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
# directory of this distribution and at http://lush-shell.org/copyright.html

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
def ini(bad,done,display=''):
  return {
    'bad': bad,
    'done': done,
    'display': display,
  }
init = ini(False, False)
initDone = ini(False, True)

# A token and its expected results from a parser
def tok(tokstr,bad,done,display=''):
  return {
    'tok': LexToken(tokstr),
    'bad': bad,
    'done': done,
    'display': display,
  }

# TestCase that will validate a parser's state as it advances through a
# series of token sequences (each starting from scratch)
class RuleTester(unittest.TestCase):
  def stateTest(self,parser,t,disp):
    self.assertEqual(parser.bad, t['bad'])
    self.assertEqual(parser.done, t['done'])
    self.assertEqual(disp, t['display'])

  def ruleTest(self,rule,series):
    for s in series:
      #print "s: %s" % s
      parser = MakeParser(rule)
      self.stateTest(parser, s[0], '')
      for i, t in enumerate(s[1:]):
        #print "sending token: %s,%s" % (i, t)
        disp = parser.parse(t['tok'])
        self.stateTest(parser, t, disp)


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
    rule = Terminal('a', ['A'], '<%s>')
    self.series = [
      [init, tok('1:A', False, True, '<A>'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, self.series)

  def test_TermDisp2(self):
    rule = Terminal('a', ['A'], 'lolz')
    self.series = [
      [init, tok('1:A', False, True, 'lolz'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, self.series)


class TestSeq(RuleTester):
  def test_One(self):
    rule = Seq('one', [('A','1%s2')])
    series = [
      [init, tok('1:A', False, True, '1A'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Two(self):
    rule = Seq('two', [('A','1%s2'), ('B','3%s4')])
    series = [
      [init, tok('1:A', False, False, '1A'),
             tok('1:B', False, True, '23B'),
             tok('1:B', True, False)],
      [init, tok('1:A', False, False, '1A'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Three(self):
    rule = Seq('three', [('A','1%s2'), ('B','3%s4'), ('C','5%s6')])
    series = [
      [init, tok('1:A', False, False, '1A'),
             tok('1:B', False, False, '23B'),
             tok('1:C', False, True, '45C'),
             tok('1:D', True, False)],
      [init, tok('1:A', False, False, '1A'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Four(self):
    rule = Seq('three', [('A','1%s2'), ('B','3%s4'), ('C','5%s6'), ('D','7%s8')])
    series = [
      [init, tok('1:A', False, False, '1A'),
             tok('1:B', False, False, '23B'),
             tok('1:C', False, False, '45C'),
             tok('1:D', False, True, '67D'),
             tok('1:E', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_MiddlePlusTerm(self):
    rule = Seq('main', [
        ('A','1%s2'),
        (Plus('+', 'B', '<%s>'), '3%s4'),
        ('C', '5%s6'),
      ])
    series = [
      [init, tok('1:A', False, False, '1A'),
             tok('1:B', False, False, '23<B'),
             tok('1:B', False, False, '><B'),
             tok('1:B', False, False, '><B'),
             tok('1:C', False, True, '>45C'),
             tok('1:X', True, False)],
      ]
    self.ruleTest(rule, series)

  def test_MiddlePlusTerm(self):
    rule = Seq('main', [
        ('A','1%s2'),
        (Plus('+', 'B', '<%s>'), '3%s4'),
        ('C', '5%s6'),
      ])
    series = [
      [init, tok('1:A', False, False, '1A'),
             tok('1:B', False, False, '23<B'),
             tok('1:B', False, False, '><B'),
             tok('1:B', False, False, '><B'),
             tok('1:C', False, True, '>45C'),
             tok('1:X', True, False)],
      ]
    self.ruleTest(rule, series)

  def test_Plus(self):
    rule = Seq('giant', [
        (Plus('+1', Seq('seq1', [('A','1%s2'), ('B','3%s4'), ('C','5%s6')]), 'a%sb'),'!%s@'),
        (Plus('+2', Seq('seq2', [('C','7%s8'), ('D','9%s0'), ('E','1%s2')]), 'c%sd'), ' #%s$'),
      ])
    series = [
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False, '!a1A23B45C'),
             tok('1:Q', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False, '!a1A23B45C'),
             tok('1:C', False, False, '6b@ #c7C'),
             tok('1:D', False, False, '89D'),
             tok('1:E', False, True, '01E')],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False, '!a1A23B45C'),
             tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False, '6ba1A23B45C'),
             tok('1:C', False, False, '6b@ #c7C'),
             tok('1:D', False, False, '89D'),
             tok('1:E', False, True, '01E'),
             tok('1:C', False, False, '2dc7C'),
             tok('1:D', False, False, '89D'),
             tok('1:E', False, True, '01E')],
    ]
    self.ruleTest(rule, series)

  def test_PlusLimited(self):
    # Old limitation of SeqParser: if one stage is done, but then accepts a
    # token (does not become bad), the SeqParser would not advance even if it
    # would have needed to do so to ultimately parse the whole sequence.
    #
    # This rule used to be impossible to parse.
    rule = Seq('seq', [
        (Plus('+1',
          Seq('p1', [('A','1%s2'), ('B','3%s4'), ('C','5%s6')]),
          'a%sb'), '!%s@'),
        (Plus('+2',
          Seq('p2', [('A','1%s2'), ('D','3%s4'), ('E','5%s6')]),
          'c%sd'), ' #%s$')])
    series = [
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False, '!a1A23B45C'),
             tok('1:A', False, False),
#             tok('1:D', True, False)],   # Old limitation!
             tok('1:D', False, False, '6b@ #c1A23D'),
             tok('1:E', False, True, '45E')],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False, '!a1A23B45C'),
             tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False, '6ba1A23B45C'),
             tok('1:A', False, False),
             tok('1:D', False, False, '6b@ #c1A23D'),
             tok('1:E', False, True, '45E'),
             tok('1:A', False, False, '6dc1A'),
             tok('1:D', False, False, '23D'),
             tok('1:E', False, True, '45E')],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False, '!a1A23B45C'),
             tok('1:C', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_StarStartEnd(self):
    rule = Seq('seq', [
        (Star('*1',
          Seq('p1', [('A','1%s2'), ('B','3%s4'), ('C','5%s6')]),
          'a%sb'), '<%s>'),
        ('D','(%s)'),
        ('E','{%s}'),
        (Star('*2',
          Seq('p2', [('F','e%sf'), ('G','g%sh'), ('H','i%sj')]),
          'c%sd'), '[%s]'),
      ])
    series = [
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False, '<a1A23B45C'),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False, '<a1A23B45C'),
             tok('1:D', False, False, '6b>(D'),
             tok('1:E', False, True, '){E'),
             tok('1:F', False, False, '}[ceF'),
             tok('1:G', False, False, 'fgG'),
             tok('1:H', False, True, 'hiH'),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False, '<a1A23B45C'),
             tok('1:A', False, False),
             tok('1:B', False, False),
             tok('1:C', False, False, '6ba1A23B45C'),
             tok('1:D', False, False, '6b>(D'),
             tok('1:E', False, True, '){E'),
             tok('1:F', False, False, '}[ceF'),
             tok('1:G', False, False, 'fgG'),
             tok('1:H', False, True, 'hiH'),
             tok('1:F', False, False, 'jdceF'),
             tok('1:G', False, False, 'fgG'),
             tok('1:H', False, True, 'hiH')],
    ]
    self.ruleTest(rule, series)

  def test_Disp(self):
    rule1 = Seq('disp', ['A'])
    series1 = [
      [init, tok('1:A', False, True, 'A'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule1, series1)

    rule2 = Seq('disp', [('A','hello')])
    series2 = [
      [init, tok('1:A', False, True, 'hello'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule2, series2)

    rule3 = Seq('disp', [('A','he%sllo')])
    series3 = [
      [init, tok('1:A', False, True, 'heA'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule3, series3)

    rule4 = Seq('disp', [('A','')])
    series4 = [
      [init, tok('1:A', False, True),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule4, series4)

  def test_Or(self):
    rule = Seq('seq',
      [Or('one', ['A', 'B', 'C'], '1%s2'),
      Or('two', ['D', 'E', 'F'], '3%s4'),
      Or('three', ['G', 'H', 'I'], '5%s6'),
      ])
    series = [
      [init, tok('1:A', False, False, '1A'),
             tok('1:D', False, False, '23D'),
             tok('1:G', False, True, '45G'),
             tok('1:H', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  # We need a test for OrParsers where
  # - multiple subparsers will be done at the same time
  # - one subparser is done, one subparser is ok not done or bad yet
  # while simultaneously the SeqParser is either done, or needs to
  # move on.
  def test_Or2(self):
    pass
 
class TestOr(RuleTester):
  def test_One(self):
    rule = Or('one', ['A'], '<%s>')
    series = [
      [init, tok('1:A', False, True, '<A'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False, '')],
    ]
    self.ruleTest(rule, series)

  def test_Two(self):
    rule = Or('two', ['A', 'B'], '<%s>')
    series = [
      [init, tok('1:A', False, True, '<A'),
             tok('1:B', True, False)],
      [init, tok('1:B', False, True, '<B'),
             tok('1:A', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Many(self):
    rule = Or('many', [
      Seq('seq1', [('A','(seq1:%s:'), ('B','%s)')]),
      'Q',
      Terminal('qq', ['QQ'], '(term %s)'),
      Seq('seq2', [('B','(seq2:%s:'), ('C','%s:'), ('D','%s)')]),
      Seq('seq3', [('A','(seq3:%s:'), ('D','%s)')]),
      Seq('seq4', [('A','(seq4:%s'), ('D',':%s'), ('E',':%s)')]),
      Seq('seq5', [('B','(seq5:%s..'), 'C', 'D', 'E', ('F','..%s)')]),
    ], '<%s>')
    series = [
      [init, tok('1:X', True, False)],
      [init, tok('1:Q', False, True, '<Q'),
             tok('1:QQ', True, False)],
      [init, tok('1:QQ', False, True, '<(term QQ)'),
             tok('1:Q', True, False)],
      [init, tok('1:A', False, False, '<'),
             tok('1:C', True, False)],
      [init, tok('1:A', False, False, '<'),
             tok('1:B', False, True, '(seq1:A:B'),
             tok('1:D', True, False)],
      [init, tok('1:A', False, False, '<'),
             tok('1:D', False, True),
             tok('1:E', False, True, '(seq4:A:D:E'),
             tok('1:E', True, False)],
      [init, tok('1:B', False, False, '<'),
             tok('1:C', False, False),
             tok('1:D', False, True),
             tok('1:E', False, False, '(seq5:B..CDE'),
             tok('1:F', False, True, '..F'),
             tok('1:E', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_PathSadness(self):
    rule = Seq('master', [Or('literal', [
        'A',
        Seq('seq', [
          'A',
          'B',
        ]),
      ], '1%s2'), 'Q'])
    series = [
      [init, tok('1:A', False, False, '1'),
             tok('1:B', False, False, 'AB'),
             tok('1:Q', False, True, '2Q'),
             tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_PathSadness2(self):
    start = ini(False, True, '')
    rule = Star('pathpart', Or('pathtoken', [
                'A',
                'Q',
              ], 'a%sb')
            )
    series = [
      [start, tok('1:A', False, True, 'aA'),
             tok('1:A', False, True, 'baA'),
             tok('1:A', False, True, 'baA'),
             tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)


class TestPlus(RuleTester):
  # Plus with a terminal
  def test_Term(self):
    rule = Plus('term', 'A', '1%s2')
    series = [
      [init, tok('1:A', False, True, '1A'),
             tok('1:A', False, True, '21A'),
             tok('1:A', False, True, '21A'),
             tok('1:X', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_DefaultDisp(self):
    rule = Plus('term', 'A')
    series = [
      [init, tok('1:A', False, True, 'A'),
             tok('1:A', False, True, 'A'),
             tok('1:A', False, True, 'A'),
             tok('1:X', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_StaticMsg(self):
    rule = Plus('term', 'A', 'hello')
    series = [
      [init, tok('1:A', False, True, 'hello'),
             tok('1:A', False, True, 'hello'),
             tok('1:A', False, True, 'hello'),
             tok('1:X', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Silent(self):
    rule = Plus('term', 'A', '')
    series = [
      [init, tok('1:A', False, True),
             tok('1:A', False, True),
             tok('1:A', False, True),
             tok('1:X', True, False)],
      [init, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Seq(self):
    rule = Plus('plus',
      Seq('seq',
        [('A','1%s2'), ('B','3%s4'), ('C','5%s6')]),
      'x%sy')
    series = [
      [init, tok('1:X', True, False)],
      [init, tok('1:A', False, False, 'x1A'),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False, 'x1A'),
             tok('1:B', False, False, '23B'),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False, 'x1A'),
             tok('1:B', False, False, '23B'),
             tok('1:C', False, True, '45C'),
             tok('1:C', True, False, '')],
      [init, tok('1:A', False, False, 'x1A'),
             tok('1:B', False, False, '23B'),
             tok('1:C', False, True, '45C'),
             tok('1:A', False, False, '6yx1A'),
             tok('1:X', True, False)],
      [init, tok('1:A', False, False, 'x1A'),
             tok('1:B', False, False, '23B'),
             tok('1:C', False, True, '45C'),
             tok('1:A', False, False, '6yx1A'),
             tok('1:B', False, False, '23B'),
             tok('1:C', False, True, '45C'),
             tok('1:C', True, False)],
    ]
    self.ruleTest(rule, series)


class TestStar(RuleTester):
  # Star with a terminal
  def test_Term(self):
    start = ini(False, True, '')
    rule = Star('term', 'A', msg='x%sy')
    series = [
      [start, tok('1:A', False, True, 'xA'),
              tok('1:A', False, True, 'yxA'),
              tok('1:A', False, True, 'yxA'),
              tok('1:X', True, False)],
      [start, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_StaticMsg(self):
    start = ini(False, True, '')
    rule = Star('term', 'A', msg='hello')
    series = [
      [start, tok('1:A', False, True, 'hello'),
              tok('1:A', False, True, 'hello'),
              tok('1:A', False, True, 'hello'),
              tok('1:X', True, False)],
      [start, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Silent(self):
    start = ini(False, True, '')
    rule = Star('term', 'A', msg='')
    series = [
      [start, tok('1:A', False, True),
              tok('1:A', False, True),
              tok('1:A', False, True),
              tok('1:X', True, False)],
      [start, tok('1:X', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Seq(self):
    start = ini(False, True, '')
    rule = Star('star',
      Seq('seq',
        [('A','1%s2'), ('B','3%s4'), ('C','5%s6')]),
      'x%sy')
    series = [
      [start, tok('1:X', True, False, '')],
      [start, tok('1:A', False, False, 'x1A'),
              tok('1:X', True, False)],
      [start, tok('1:A', False, False, 'x1A'),
              tok('1:B', False, False, '23B'),
              tok('1:X', True, False)],
      [start, tok('1:A', False, False, 'x1A'),
              tok('1:B', False, False, '23B'),
              tok('1:C', False, True, '45C'),
              tok('1:C', True, False)],
      [start, tok('1:A', False, False, 'x1A'),
              tok('1:B', False, False, '23B'),
              tok('1:C', False, True, '45C'),
              tok('1:A', False, False, '6yx1A'),
              tok('1:X', True, False)],
      [start, tok('1:A', False, False, 'x1A'),
              tok('1:B', False, False, '23B'),
              tok('1:C', False, True, '45C'),
              tok('1:A', False, False, '6yx1A'),
              tok('1:B', False, False, '23B'),
              tok('1:C', False, True, '45C'),
              tok('1:C', True, False)],
    ]
    self.ruleTest(rule, series)

  def test_Double(self):
    start = ini(False, True, '')
    rule = Star('star',
      Seq('seq',
        [(Star('*1', 'A', 'a%sb'), '1%s2'),
        (Star('*2', 'B', 'c%sd'), '3%s4')]
      ), '<%s>')
    series = [
      [start, tok('1:X', True, False)],
      [start, tok('1:A', False, True, '<1aA'),
              tok('1:A', False, True, 'baA'),
              tok('1:B', False, True, 'b23cB'),
              tok('1:B', False, True, 'dcB'),
              tok('1:X', True, False)],
      [start, tok('1:B', False, True, '<3cB'),
              tok('1:A', False, True, 'd4><1aA'),
              tok('1:B', False, True, 'b23cB')],
    ]
    self.ruleTest(rule, series)


def suite():
  suite = unittest.TestSuite()
  testcases = [TestTerminals, TestPlus, TestStar, TestSeq, TestOr]
  suite.addTests([unittest.TestLoader().loadTestsFromTestCase(x) for x in testcases])
  return suite

if __name__ == '__main__':
  unittest.TextTestRunner(verbosity=2).run(suite())
