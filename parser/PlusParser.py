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
    self.reps = 0
    self.firstparse = True
    self.msg_parts = self.rule.msg.split('%s', 2)
    self.msg_start = self.msg_parts[0]
    self.msg_end = ''
    if len(self.msg_parts) > 1:
      self.msg_end = self.msg_parts[1]
    self.quiet = False
    if self.rule.msg.count('%s') == 0:
      self.quiet = True

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
      if disp != '':
        raise Exception("Bad parse should have given no return value; at %s" % self.name)
      # re-establish
      fdisp = self.active.finish()
      edisp = self.msg_end
      self.firstparse = True
      self.active = MakeParser(self.rule.items, self)
      self.reps += 1
      disp = self.parse(token)
      if self.bad:
        return ''
      return fdisp + edisp + self.display(disp)
    self.bad = self.active.bad
    self.done = self.active.done
    if self.bad:
      return ''
    if self.quiet:
      return self.display('')
    return self.display(disp)

  def display(self,disp):
    if self.firstparse:
      disp = self.msg_start + disp
      self.firstparse = False
    return disp

  def finish(self):
    sdisp = ''
    fdisp = ''
    edisp = ''
    if self.firstparse:
      self.firstparse = False
      sdisp = self.msg_start
    if self.active and self.active.done and not self.active.bad:
      fdisp = self.active.finish()
      edisp = self.msg_end
    if not self.done:   # EXPERIMENT TIMEZ
      return ''
    if self.quiet:
      return sdisp + edisp
    return sdisp + fdisp + edisp

class Plus(Rule):
  def MakeParser(self,parent):
    return PlusParser(self, parent)

