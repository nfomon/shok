# Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
# directory of this distribution and at http://shok.io/code/copyright.html

from OrParser import OrParser
from Parser import Parser, MakeParser
from Rule import Rule, IsRuleDone
from copy import copy, deepcopy
import logging

# TODO; UNIMPLEMENTED

# The SeqParser's rule is a list of Rules that will be matched in sequence;
# advancing only when an individual rule is done and turns bad if given another
# token.  The SeqParser is done anytime that its last rule is done.
#
# This is the quick+simple SeqParser.  It will not backtrack prior to the last
# token that brought us to a done state.  Equivalently, it requires that the
# first ok (not done) token accepted by a parser that directly follows a
# done-token (token that puts the parser in a done state) is not a token that
# is accepted by the next parser that accepts a token.  We check this as a way
# to identify if this SeqParser is being used in violation of the condition.
# This is a runtime counterexample-check, kind of ugly, but better than not
# knowing :)  If a sequence cannot obey this constraint (and you're sure you
# can't just use an Opt to get around it), use a DecentSeqParser or a
# StableSeqParser instead.
#
# Note that this scheme accompanies an explicit prioritization of getting
# displaytext output from a parser ahead of correctness in a specific way.  To
# be more correct we could hold off outputting a position of the SeqParser
# until we move past it.  Instead here we are outputting each token of a
# position until it gets to its first done state.  From there, we output only
# at each time the state becomes done again.  This is a bit greedy but I think
# still easy enough to manage.  Perhaps the DecentSeqParser would be where we
# wait until we actually move past a position to return all its displaytext.
class SeqParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    self.parsers = []   # parsers constructed for each of rule.items
    self.active = None  # main active parser (top of self.parsers)
    self.pos = 0        # position in self.rule.items

    # The not-done tokens that immediately followed done-tokens by the last
    # parser that accepted any tokens.  If the next parser that accepts a
    # token's very first token is in this set, then this SeqParser was used
    # erroneously!
    self.firstnotdonetokens = set()
    self.everdone = []  # True for each position that was ever done
    self.tokenssincedone = []   # each token observed since the last done state
    self.display = ''   # buffer of text to emit at the next emittable state
    self.lastdonetext = ''    # text from the last time we were done

  def parse(self,token):
    firsttoken = False
    if self.pos >= len(self.rule.items):
      self.bad = True
      self.done = False
    elif not self.active:
      item = self.rule.items[self.pos]
      self.parsers.append(MakeParser(item, self))
      self.active = self.parsers[-1]
      self.bad = self.active.bad
      self.everdone.append(self.active.done)
      firsttoken = True
    if self.pos != len(self.parsers)-1:
      raise Exception("%s SeqParser: parsers (%s) vs. self.pos=%s mismatch; len=%s" % (self.name, self.parsers, self.pos, len(self.parsers)))
    if self.bad:
      self.done = False
      raise Exception("%s SeqParser is bad; can't accept token '%s'" % (self.name, token))

    wasdone = self.active.done    # was the current active parser already done

    # Parse!
    olddisp = copy(self.display)
    self.display = ''
    newdisp = self.active.parse(token)
    if self.name == 'cmdcodeblock' or self.name == 'cmdblock':
      print "%s parsed %s olddisp='%s' newdisp='%s'" % (self.name, token, olddisp, newdisp)

    # if in last pos
    if self.pos == len(self.rule.items)-1:
      self.bad = self.bad or self.active.bad
      if self.bad:
        self.done = False
      else:
        self.done = self.active.done
      logging.info("%s SeqParser received '%s', last state, olddisp '%s', newdisp '%s'" % (self.name, token, olddisp, newdisp))
      return olddisp + newdisp

    if self.active.bad:
      self.firstnotdonetokens = set()
      if wasdone:
        # accept that state as having never parsed this token yet, and go
        # forward
        self.pos += 1
        self.active = None
        self.lastdonetext = ''
        newdisp += self.parse(token)
        if self.bad:
          return olddisp
        return olddisp + newdisp

      # if this state was ever done, feed forward all the tokens it has been
      # fed since the last time it was done.  Instead of accumulated display
      # text (olddisp) we want to predicate the current and all the new
      # (forward-fed accum token) display text with just the output of the last
      # *done* state, lastdonetext.
      olddisp = copy(self.lastdonetext)
      self.lastdonetext = ''
      if self.everdone[self.pos]:
        self.pos += 1
        self.active = None
        tokenssincedone = copy(self.tokenssincedone)
        self.tokenssincedone = []
        for t in tokenssincedone:
          if not self.bad:
            newdisp += self.parse(t)
        if not self.bad:
          newdisp += self.parse(token)
      else:
        self.bad = True
        self.done = False
      if self.bad:
        return olddisp
      return olddisp + newdisp

    # We accepted a token.  If it's the first token for this parser, ensure
    # it's not in the firstnotdonetokens set!
    if firsttoken:
      if token.ttype in self.firstnotdonetokens:
        raise Exception("%s SeqParser pos=%s accepts first token \"%s\" which was the first not done token in the previous parser that accepted tokens!  This violates a condition for usage of SeqParser.  Please use a DecentSeqParser or StableSeqParser instead" % (self.name, self.pos, token));
      self.firstnotdonetokens = set()

    if self.active.done:
      # if all upcoming rule items are done, then mark the whole Seq as done.
      # Note that we're not ready to "commit" to done as much as we can in the
      # "last pos, wasdone" state further above.
      self.done = True    # a bit eager, n'est-ce pas?
      for item in self.rule.items[self.pos+1:]:
        if not IsRuleDone(item):
          self.done = False   # il n'est pas
          break
      self.everdone[self.pos] = True
      self.tokenssincedone = []
      self.lastdonetext = self.fakeEnd()
      return olddisp + newdisp

    if wasdone:
      self.firstnotdonetokens.add(token.ttype)

    # This state is ok.  If the active parser has ever been done, accumulate
    # the text from this parse; we don't know if we'll actually use it, or if
    # it will be fed forward.  Return ''.  If this state has never been done,
    # alternatively, return whatever we can (it *must* be accepted for the
    # whole sequence to ever work).  What if it starts done?  Then indeed it
    # has ever been done, that counts too, make sure we accumulate in that case
    # and output nothing right now -- until we arrive at a done state again.
    if self.everdone[self.pos]:
      self.tokenssincedone.append(token)
      self.display = olddisp + newdisp
      print " - %s yield nuthin" % self.name
      return ''
    return olddisp + newdisp

  def fakeEnd(self):
    if self.active:
      return self.active.fakeEnd()
    return ''

class Seq(Rule):
  def MakeParser(self,parent):
    return SeqParser(self, parent)

