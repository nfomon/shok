# Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
# directory of this distribution and at http://lush-shell.org/copyright.html

from Parser import Parser, MakeParser
from Rule import Rule
from copy import copy
import logging

# PlusParser repeats its single parser at least once.  It's done anytime its
# sub-parser is done.  If it gets another token it re-creates itself with
# another instance, so we are memory- and stack-bounded (except we grab the
# segment's display text into a list).
# We only re-establish the sub-parser if it was done and becomes bad if given
# another token.
class PlusParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    self.active = None

  def parse(self,token):
    logging.debug("%s PlusParser parsing token '%s'" % (self.name, token))
    if not self.active:
      self.active = MakeParser(self.rule.items, self)
    self.bad = self.bad or self.active.bad
    if self.bad:
      raise Exception("%s PlusParser is bad" % self.name)
    wasdone = False
    if self.active.done:
      wasdone = True
    disp = self.active.parse(token)
    if wasdone and self.active.bad:
      # re-establish
      self.active = MakeParser(self.rule.items, self)
      disp += self.parse(token)
      return disp
    self.bad = self.active.bad
    self.done = self.active.done
    return disp

  def fakeEnd(self):
    if self.active:
      return self.active.fakeEnd()
    return ''

class Plus(Rule):
  def MakeParser(self,parent):
    return PlusParser(self, parent)

