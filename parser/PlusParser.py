from Parser import Parser, MakeParser
from Rule import Rule
from copy import copy
import logging

# PlusParser repeats its single parser at least once.  It's done anytime its
# sub-parser is done.  If it gets another token it re-creates itself with
# another instance, so we are memory- and stack-bounded.
# We only re-establish the sub-parser if it was done and it's bad if it were
# given another token.
class PlusParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    self.active = None
    self.disps = []
    self.reps = 0

  def restart(self):
    Parser.restart(self)
    self.active = None
    self.disps = []

  def parse(self,token):
    if self.signalRestart:
      self.restart()
      self.signalRestart = False
    logging.debug("%s PlusParser parsing token '%s'" % (self.name, token))
    if not self.active:
      self.active = MakeParser(self.rule.items, self)
    self.bad = self.bad or self.active.bad
    if self.bad:
      raise Exception("%s PlusParser is bad" % self.name)
    wasdone = False
    tempdisp = ''
    if self.active.done:
      wasdone = True
      tempdisp = self.active.display()
    self.active.parse(token)
    if wasdone and self.active.bad:
      # re-establish
      self.active = MakeParser(self.rule.items, self)
      self.disps.append(tempdisp)
      self.reps += 1
      logging.debug("Re-establishing %s; reps=%s, disps=%s" % (self.name, self.reps, self.disps))
      self.parse(token)   # this is not recursive in the way that would hurt
    self.bad = self.active.bad
    self.done = self.active.done

  def display(self):
    if self.bad or not self.done:
      raise Exception("%s PlusParser is unfinished" % self.name)
    disps = copy(self.disps)
    disps.append(self.active.display())
    s = ''
    for m in range(0,len(disps)):
      s += self.rule.msg
    return s % tuple(disps)

# StarParser repeats its single rule 0 or more times.  That is, it always
# starts off 'done'.
class StarParser(PlusParser):
  def __init__(self,rule,parent):
    PlusParser.__init__(self, rule, parent)
    self.done = True
    self.anyparse = False
  def restart(self):
    PlusParser.restart(self)
    self.done = True
    self.anyparse = False
  def parse(self,token):
    logging.debug("%s StarParser parsing token '%s'" % (self.name, token))
    PlusParser.parse(self, token)
    if not self.bad:
      self.anyparse = True
  def display(self):
    if not self.anyparse:
      return ''
    return PlusParser.display(self)

class Plus(Rule):
  def MakeParser(self,parent):
    return PlusParser(self, parent)

