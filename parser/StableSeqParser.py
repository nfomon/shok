# Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
# directory of this distribution and at http://shok.io/code/copyright.html

from OrParser import OrParser
from Parser import Parser, MakeParser
from Rule import Rule, IsRuleDone
from copy import copy, deepcopy
import logging

# The SeqParser's rule is a list of Rules that will be matched in sequence;
# advancing only when an individual rule is done and turns bad if given another
# token.  The SeqParser is done anytime that its last rule is done.
#
# If a not-the-last rule is fed a token and it moves from done to not-bad,
# not-done: we turn ourselves into an OrParser with two branches.  Each branch
# is a duplicate of our previous state, except one branch assumes that the
# current state will eventually turn bad and instead thunders on from pos+1.
class StableSeqParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    self.parsers = []   # parsers constructed for each of rule.items
    self.active = None  # main active parser (top of self.parsers)
    self.pos = 0        # position in self.rule.items
    self.or_parser = None   # In use if we split into an OrParser
    self.fakePrefix = ''    # Text to prepend to the next parse()'s output
    self.justSplit = False  # True for left branch immediately after a split
                            # We should go bad if current pos fails next token

  def parse(self,token):
    if self.or_parser:
      if self.bad:
        self.done = False
        raise Exception("%s SeqParser (split) is bad; can't accept token '%s'" % (self.name, token))
      disp = self.or_parser.parse(token)
      self.bad = self.or_parser.bad
      self.done = self.or_parser.done
      return disp

    if self.pos >= len(self.rule.items):
      self.bad = True
      self.done = False
    elif not self.active:
      item = self.rule.items[self.pos]
      self.parsers.append(MakeParser(item, self))
      self.active = self.parsers[-1]
      self.bad = self.active.bad
    if self.pos != len(self.parsers)-1:
      raise Exception("%s SeqParser: parsers (%s) vs. self.pos=%s mismatch; len=%s" % (self.name, self.parsers, self.pos, len(self.parsers)))
    if self.bad:
      self.done = False
      raise Exception("%s SeqParser is bad; can't accept token '%s'" % (self.name, token))

    wasdone = self.active.done    # was the current active parser already done

    # Parse!
    disp = self.fakePrefix + self.active.parse(token)
    self.fakePrefix = ''

    justSplit = copy(self.justSplit)
    self.justSplit = False

    # if in last pos
    if self.pos == len(self.rule.items)-1:
      self.bad = self.bad or self.active.bad
      if self.bad:
        self.done = False
      else:
        self.done = self.active.done
      logging.info("%s SeqParser received '%s', last state, emits '%s'" % (self.name, token, disp))
      return disp

    if self.active.bad:
      if wasdone and not justSplit:
        # accept that state as having never parsed this token yet, and go
        # forward
        self.pos += 1
        self.active = None
        disp += self.parse(token)
      else:
        self.bad = True
        self.done = False
      return disp

    if self.active.done:
      # if all upcoming rule items are done, then mark the whole Seq as done.
      self.done = True    # a bit eager, n'est-ce pas?
      for item in self.rule.items[self.pos+1:]:
        if not IsRuleDone(item):
          self.done = False   # il n'est pas
          break

    # Split ourselves in two.  First branch is as we are.  Right branch
    # pretends our pos will go bad, so it starts at pos+1.
    logging.info("Splitting %s at pos=%s" % (self.name, self.pos))
    left = deepcopy(self)
    left.name += '.1'
    left.justSplit = True
    right = deepcopy(self)
    right.name += '.2'
    right.pos += 1
    right.active = None
    right.fakePrefix = left.fakeEnd()
    rule = Rule('<%s:seq_or>' % self.name, [left, right])
    self.or_parser = OrParser(rule, self)
    del self.parsers
    del self.active
    del self.pos
    del self.fakePrefix
    del self.justSplit
    return disp

  def fakeEnd(self):
    if self.or_parser:
      return self.or_parser.fakeEnd()
    if self.active:
      return self.active.fakeEnd()
    return ''

class StableSeq(Rule):
  def MakeParser(self,parent):
    return StableSeqParser(self, parent)

