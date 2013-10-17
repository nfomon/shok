# Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
# directory of this distribution and at http://lush-shell.org/copyright.html

from Parser import Parser, MakeParser
from Rule import Rule
from copy import copy
import logging

# The OrParser's rule is a list of Rules that will be matched in
# parallel.  The OrParser is done when a single one of its rules is
# done.  But that doesn't mean it has emit!  It can't emit until only a single
# one of its rules is not bad.
class OrParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    self.parsers = []
    self.displays = {}
    for item in rule.items:
      parser = MakeParser(item, self)
      self.parsers.append(parser)
      self.displays[parser] = ''
    self.lastHappyEnding = ''

  def parse(self,token):
    logging.debug("%s OrParser parsing '%s'" % (self.name, token))
    if len(self.parsers) == 0:
      logging.warning("%s OrParser can't parse; no productions" % self.name)
      self.bad = True
    if self.bad:
      raise Exception("State '%s' is bad" % self.name)
    badparsers = []
    doneparsers = []
    wasdoneparsers = []
    # all of self.parsers are ok, none are bad
    # feed them each the token and see who turns bad or done
    for prod in self.parsers:
      logging.info("%s OrParser parsing prod %s" % (self.name, prod.name))
      if prod.bad:
        raise Exception("%s OrParser already has bad prod '%s'" % (self.name, prod.name))
      elif prod.done:
        wasdoneparsers.append(prod)
      self.displays[prod] += prod.parse(token)
      if prod.bad:
        logging.info("%s OrParser prod '%s' went bad" % (self.name, prod.name))
        badparsers.append(prod)
      elif prod.done:
        logging.info("%s OrParser has doneparser '%s'" % (self.name, prod.name))
        doneparsers.append(prod)

    for prod in badparsers:
      self.parsers.remove(prod)

    if len(self.parsers) == 0:
      logging.info("%s OrParser turned bad" % self.name)
      self.bad = True
      self.done = False
      if len(wasdoneparsers) >= 1:
        # emit the wasdoneparsers output if they all agree
        disp = None
        for wdp in wasdoneparsers:
          if disp is None:
            disp = self.displays[wdp]
          elif disp != self.displays[wdp]:
            for i in wasdoneparsers:
              logging.debug('%s wdp: %s' % (self.name, self.displays[i]))
            return ''
        disp = copy(self.displays[wasdoneparsers[0]])
        self.displays[wasdoneparsers[0]] = ''
        for prod in badparsers:   # unnecessary cleanup
          del self.displays[prod]
        return disp
      for prod in badparsers:   # unnecessary cleanup
        del self.displays[prod]
      return ''

    if len(doneparsers) >= 1:
      self.done = True
      if len(doneparsers) == 1:
        self.lastHappyEnding = copy(self.displays[doneparsers[0]])
    else:
      self.done = False

    if len(self.parsers) == 1:
      disp = copy(self.displays[self.parsers[0]])
      self.displays[self.parsers[0]] = ''
      return disp
    # eagerness: if all parsers have same display, return it, otherwise ''.
    disp = None
    for prod in self.parsers:
      if disp is None:
        disp = self.displays[prod]
      elif disp != self.displays[prod]:
        return ''
    disp = copy(self.displays[self.parsers[0]])
    for prod in self.parsers:
      self.displays[prod] = ''
    return disp

  def fakeEnd(self):
    if len(self.parsers) == 1:
      return self.parsers[0].fakeEnd()
    return self.lastHappyEnding

class Or(Rule):
  def MakeParser(self,parent):
    return OrParser(self, parent)

