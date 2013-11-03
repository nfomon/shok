# Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
# directory of this distribution and at http://lush-shell.org/copyright.html

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
# not-done: we greedily assume things will pan out in our favour, and emit what
# text we can.  If you cared about deeper correctness, you should have used a
# Nice or Decent or StableSeqParser.  Although, we also accumulate all the
# tokens we feed to any intermediary parser.  If it goes bad, we pretend that
# we haven't fed it (since its last done state; no deeper backtracking than
# that!) and run the tokens forward.  In this case, I think we currently don't
# allow that intermediary parser to have output any text... but eventually we
# can make it so we're still ok if marching forward past it we'll output all
# the same text that it gave us in "evilness".
#
# We turn 'evil' if:
# - we have returned non-empty text and go bad before ever being done
#   -- i.e. go bad with unconfirmed
# - after ever being done, we go ok (with non-empty text) and then bad instead
# of getting done again
#   -- i.e. go bad with unconfirmed
# - we ever return text from a parser that then turns evil, and running the
# accumulated tokens forward gives us different text than what we already
# returned (currently I'm not sure we do this, we might only allow an
# intermediary parser to go evil if it gave us no text since it was last done).
#
# --- begin old thoughts:
# HRRRM or wait, let's rethink that.  We turn evil if we realize we have ever
# emit text that was cursed.  What is our normal process?
# We emit every result from every parse that is accepted by a parser until it
# goes bad, and then we either go bad ourselves or advance.  When active goes
# from done to bad, we advance.  We go bad if we go from not-done to bad, ever,
# or if the last state goes bad for any reason.
#
# Thus, we go evil if self.unconfirmed and self.bad.  OR if any parser goes
# evil and we may have used its text ever.
#
# when we do:
#   disp = self.active.parse(token)
#
# well, we should check if active was evil before the parse.
# If it is evil only after the parse, hmm, have we used its output before?  how
# to know?  um because it was active.  also, we could buffer the text and state
# from each active parser to make our own determination about whether or not
# it's evil.  But certainly if it's ever evil then we're evil, if it ever gave
# us any text.
#
# 2k13-10-27 let's return '' if we go bad.  See what this breaks.  It means we
# can trust "evil" means "something previous was evil".  Note that evil always
# means "previously provided bad non-empty text".
# 2k13-10-28: ^^ careful.  When a sub-parser goes from done to bad, we do want
# to use the "going bad" text!  If that's in the last pos, then do return it.
# So easier: let's always return the text from the moment we turn bad.  But
# what if we turned evil?
# Evil means that *previous* text was bad.  If evil, then choose not to return
# the current display text.  We can make this true everywhere.
# --- end old thoughts

class SeqParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    self.parsers = []   # parsers constructed for each of rule.items
    self.active = None  # main active parser (top of self.parsers)
    self.pos = 0        # position in self.rule.items
    self.unconfirmed = '' # text we have output since starting or being done
    self.activeused = False # have we ever previously returned text from active
    self.everdone = []  # true for each position that was ever done
    self.tokenssincedone = [] # each token observed since the last done state
    self.everevil = False # have we ever emit stuff from a parser that went evil
    self.debug = []

  def parse(self,token):
    if self.name in self.debug:
      print
      print "%s parsing %s at pos=%s" % (self.name, token, self.pos)
    if self.pos >= len(self.rule.items):
      self.bad = True
      self.done = False
    elif not self.active:
      item = self.rule.items[self.pos]
      self.parsers.append(MakeParser(item, self))
      self.active = self.parsers[-1]
      self.bad = self.active.bad
      self.everdone.append(self.active.done)
      self.activeused = False
    if self.pos != len(self.parsers)-1:
      raise Exception("%s SeqParser: parsers (%s) vs. self.pos=%s mismatch; len=%s" % (self.name, self.parsers, self.pos, len(self.parsers)))
    if self.evil and not self.bad:
      raise Exception("%s SeqParser in bad-evil inconsistency")
    if self.bad:
      self.done = False
      raise Exception("%s SeqParser is bad; can't accept token '%s'" % (self.name, token))

    wasdone = self.active.done    # was the current active parser already done
    wasevil = self.active.evil    # this should be unnecessary I think..
    if wasdone and wasevil:
      raise Exception("%s SeqParser's active %s is both done and evil at %s" % (self.name, self.active.name, token))
    elif wasevil:
      raise Exception("%s SeqParser refuses to pass %s to already evil %s" % (self.name, token, self.active.name))

    # Parse!
    disp = self.active.parse(token)

    # just in case checks
    if self.active.evil and not self.active.bad:
      raise Exception("SeqParser sanity 1 fail")
    elif self.active.bad and self.active.done:
      raise Exception("SeqParser sanity 2 fail")

#    if self.active.evil and disp:
#      raise Exception("%s's active %s went evil but gave '%s'; it this allowed??" % (self.name, self.active.name, disp))

    # if in last pos
    if self.pos == len(self.rule.items)-1:
      self.bad = self.bad or self.active.bad
      if self.bad:
        self.done = False
        if self.unconfirmed:
          self.evil = True
        elif self.active.evil:
          raise Exception("last pos active is evil but no %s.unconfirmed, how is this possible" % self.name)
      else:
        self.done = self.active.done
      logging.info("%s SeqParser received '%s', last state, bad=%s, emits '%s'" % (self.name, token, self.bad, disp))
      if self.name in self.debug:
        print "%s in lastpos, now bad=%s, evil=%s, done=%s" % (self.name, self.bad, self.evil, self.done)
      if self.done:
        self.unconfirmed = ''
      else:
        self.unconfirmed += disp
      if disp:
        self.activeused = True
      return disp

    if self.active.bad:
      if wasdone:
        # accept that state as having never parsed this token yet, and go
        # forward.  We don't even care if it thinks it went evil.
        self.pos += 1
        if self.name in self.debug:
          print "%s active %s wasdone, its bad=%s evil=%s" % (self.name, self.active.name, self.active.bad, self.active.done)
          print "%s at %s evil=%s reparsing %s with unconfirmed '%s'" % (self.name, self.pos, self.evil, token, self.unconfirmed)
        self.active = None
        if self.name in self.debug:
          print "%s reparsing %s!" % (self.name, token)
        disp += self.parse(token)
      elif self.everdone[self.pos]:
        # we can try to feed forward tokens since last done state
        self.pos += 1
        self.active = None
        newdisp = ''
        tokenssincedone = copy(self.tokenssincedone)
        self.tokenssincedone = []
        for t in tokenssincedone:
          if not self.bad:
            newdisp += self.parse(t)
        if not self.bad:
          newdisp += self.parse(token)
        disp += newdisp
      else:
        self.bad = True
        self.done = False
        if self.unconfirmed:
          self.evil = True
        elif self.active.evil:
          raise Exception("active is evil but no %s.unconfirmed, how is this possible" % self.name)
        # failing above check, let active.evil => self.evil   ?????MAYBE??
      if self.name in self.debug:
        print "%s at %s token=%s done=%s bad=%s evil=%s" % (self.name, self.pos, token, self.done, self.bad, self.evil)
      return disp

    if self.active.done:
      # if all upcoming rule items are done, then mark the whole Seq as done.
      self.done = True    # a bit eager, n'est-ce pas?
      for item in self.rule.items[self.pos+1:]:
        if not IsRuleDone(item):
          self.done = False   # il n'est pas
          break
      self.everdone[self.pos] = True
      self.tokenssincedone = []
      if self.done:
        self.unconfirmed = ''
      else:
        self.unconfirmed += disp
      if self.active.evil:
        raise Exception("Terror")
      if self.name in self.debug:
        print "%s done=%s, active %s done=%s unc='%s' disp='%s'" % (self.name, self.done, self.active.name, self.active.done, self.unconfirmed, disp)
      return disp

    if self.everdone[self.pos]:
      self.tokenssincedone.append(token)

    self.done = False   # in case we started done; not true now after a parse
    if self.active.evil:
      raise Exception("Awful")
    self.unconfirmed += disp
    if self.name in self.debug:
      print "%s is ok, unconfirmed='%s', disp='%s', done=%s" % (self.name, self.unconfirmed, disp, self.done)
    return disp

  def fakeEnd(self):
    if self.active:
      if self.active.evil:
        raise Exception("no fakeend when active.evil I guess")
      return self.active.fakeEnd()
    return ''

class Seq(Rule):
  def MakeParser(self,parent):
    return SeqParser(self, parent)

