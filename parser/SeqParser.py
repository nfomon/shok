from Parser import Parser, MakeParser
from Rule import Rule
from StarParser import Star
from copy import copy
import logging

# The SeqParser's rule is a list of Rules that will be matched in
# sequence; advancing only when an individual rule is done and turns
# bad if given another token.  The SeqParser is done anytime that its
# last rule is done.
#
# If a not-the-last rule is fed a token and it moves from done to
# not-bad, not-done: we start accumulating all the tokens we feed it.
# If finally it turns bad instead of turning done again, we advance not
# only with the current token, but first with all the history of tokens
# that we tried to feed to this intermediary step but were rejected.
class SeqParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    self.parsers = []   # parsers constructed for each of rule.items
    self.active = None  # active parser (top of self.parsers)
    self.pos = 0        # position in self.rule.items
    self.displays = []  # display() of any completed sub-parsers
    self.accum = []     # tokens we've consumed since last rule-done-time
    logging.info("%s SeqParser initialized with rule: %s" % (self.name, self.rule))
    while len(self.displays) != len(self.rule.items):
      self.displays.append('')

  def restart(self):
    Parser.restart(self)
    self.parsers = []
    self.active = None
    self.pos = 0
    self.displays = []
    while len(self.displays) != len(self.rule.items):
      self.displays.append('')

  def parse(self,token):
    if self.signalRestart:
      self.restart()
      self.signalRestart = False
    logging.debug("%s SeqParser at pos=%s attempting to parse token '%s'" % (self.name, self.pos, token))

    if self.pos >= len(self.rule.items):
      logging.debug("%s SeqParser moved beyond rule items; now bad" % self.name)
      self.bad = True
      self.done = False
    elif not self.active:
      logging.info("%s SeqParser making parser for %s at pos=%s" % (self.name, self.rule.items[self.pos], self.pos))
      self.parsers.append(MakeParser(self.rule.items[self.pos], self))
      logging.info("%s SeqParser now parsers=%s, len=%s" % (self.name, self.parsers, len(self.parsers)))
      self.active = self.parsers[-1]
      self.bad = self.active.bad
    if self.pos != len(self.parsers)-1:
      raise Exception("%s SeqParser: parsers (%s) vs. self.pos=%s mismatch; len=%s, lenrule=%s" % (self.name, self.parsers, self.pos, len(self.parsers), len(self.rule.items)))
    if self.bad:
      self.done = False
      raise Exception("%s SeqParser is bad; can't accept token '%s'" % (self.name, token))
    logging.info("%s SeqParser parsing token '%s'; active=%s,%s" % (self.name, token, self.pos, self.active.name))

    tempdisp = ''
    wasdone = self.active.done    # was the current active parser already done
    if wasdone:
      logging.debug("%s SeqParser was done; get tempdisp '%s'" % (self.name, self.active.name))
      tempdisp = self.active.display()

    # Parse!
    self.active.parse(token)
    logging.debug("%s SeqParser parsed token '%s'; now evaluating" % (self.name, token))

    if self.pos == len(self.rule.items)-1:
      self.bad = self.bad or self.active.bad
      if self.bad:
        self.done = False
      else:
        self.done = self.active.done
        if self.done:
          self.displays[self.pos] = self.active.display()
      logging.debug("%s SeqParser in last state; now bad=%s, done=%s" % (self.name, self.bad, self.done))
      return

    if self.active.bad:
      if wasdone:
        # accept that state as having never parsed this token yet, and go
        # forward
        logging.info("%s SeqParser at token '%s' is force-advancing pos %s to %s" % (self.name, token, self.pos, self.pos+1))
        self.displays[self.pos] = tempdisp
        self.pos += 1
        self.active = None
        self.accum = []
        self.parse(token)
      elif self.accum:
        # Skip ahead to the next rule and run out our accum backlog
        self.pos += 1
        self.active = None
        self.accum.append(token)
        accum = copy(self.accum)
        self.accum = []
        for a in accum:
          logging.info("%s SeqParser is force-advancing pos %s to %s with accumulated token '%s'" % (self.name, self.pos, self.pos+1, a))
          if not self.bad:
            self.parse(a)
      else:
        self.bad = True
        self.done = False
        logging.debug("%s SeqParser wasn't done, now bad" % self.name)
      return

    if self.active.done:
      # stay here until we turn bad
      # if all upcoming rule items are Stars, then mark the whole Seq as done.
      logging.info("%s SeqParser active pos=%s went done; waiting until it turns bad before advancing" % (self.name, self.pos))
      self.accum = []
      self.displays[self.pos] = self.active.display()
      nonstars = False
      for item in self.rule.items[self.pos+1:]:
        if not isinstance(item, Star):
          nonstars = True
      if not nonstars:
        self.done = True
        logging.debug("%s SeqParser is earlydone with displays %s" % (self.name, self.displays))
      return

    # we're ok, stay put for now
    logging.debug("%s SeqParser is staying put after '%s'" % (self.name, token))
    self.accum.append(token)

  def display(self):
    if self.bad or not self.done:
      raise Exception("%s SeqParser is unfinished" % self.name)
    logging.debug("%s SeqParser is displaying; bad=%s done=%s displays=%s" % (self.name, self.bad, self.done, self.displays))
    logging.debug(" - will become: '%s'" % self.rule.display(self.displays))
    return self.rule.display(self.displays)

class Seq(Rule):
  def MakeParser(self,parent):
    return SeqParser(self, parent)

