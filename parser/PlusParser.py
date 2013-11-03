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
    self.unconfirmed = ''
    self.neverGoBad = False
    self.debug = []

  def parse(self,token):
    if self.name in self.debug:
      print "%s parsing %s" % (self.name, token)
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
      if self.name in self.debug:
        print "%s re-establishing" % self.name
      self.active = MakeParser(self.rule.items, self)
      self.unconfirmed = ''
      disp += self.parse(token)
      if self.name in self.debug:
        print "%s is re-established" % self.name
        print "%s evil=%s" % (self.name, self.evil)
      if self.neverGoBad and self.bad:
        raise Exception("%s StarParser has gone bad (eagerly)" % self.name)
      return disp
    self.bad = self.active.bad
    self.evil = self.evil or self.active.evil
    if self.name in self.debug:
      if self.evil:
        print "%s is evil, unconfirmed='%s'" % (self.name, self.unconfirmed)
    self.done = self.active.done
    if self.done:
      self.unconfirmed = ''
    else:
      self.unconfirmed += disp
    if self.bad:
      if self.done:
        raise Exception("Plus noticed another terrible parser was both bad and done")
      if self.neverGoBad:
        raise Exception("%s StarParser has gone bad (eagerly)" % self.name)
      if self.unconfirmed:
        self.evil = True
        if self.name in self.debug:
          if self.evil:
            print "%s is evil2, unconfirmed='%s'" % (self.name, self.unconfirmed)
    if self.name in self.debug:
      print "%s evil=%s" % (self.name, self.evil)
    return disp

  def fakeEnd(self):
    if self.evil:
      raise Exception("%s is evil but fakeEnd() requested.  Is that ok?" % self.name)
    if self.active:
      return self.active.fakeEnd()
    return ''

class Plus(Rule):
  def MakeParser(self,parent):
    return PlusParser(self, parent)

