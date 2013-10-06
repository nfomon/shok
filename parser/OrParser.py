# Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
# directory of this distribution and at http://lush-shell.org/copyright.html

from Parser import Parser, MakeParser
from Rule import Rule
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
    self.firstparse = True
    # validation: self.rule.inds: must be [0] or []
    if len(self.rule.inds) > 1 or (len(self.rule.inds) == 1 and self.rule.inds[0] != 0):
      raise Exception("Bad msg/inds for rule %s" % self.name)
    self.msg_start,self.msg_end = self.rule.msg.split('%s', 2)

  def parse(self,token):
    logging.debug("%s OrParser parsing '%s'" % (self.name, token))
    if len(self.parsers) == 0:
      logging.warning("%s OrParser can't parse; no productions" % self.name)
      self.bad = True
    if self.bad:
      raise Exception("State '%s' is bad" % self.name)
    bad = []
    self.doneparsers = []
    # all of self.parsers are ok, none are bad
    # feed them each the token and see who turns bad or done
    for prod in self.parsers:
      logging.info("%s OrParser parsing prod %s" % (self.name, prod.name))
      if prod.bad:
        raise Exception("%s OrParser already has bad prod '%s'" % (self.name, prod.name))
      tmp = prod.parse(token)
      self.displays[prod] = self.displays[prod] + tmp
      if prod.bad:
        logging.info("%s OrParser went bad at '%s'" % (self.name, prod.name))
        bad.append(prod)
      elif prod.done:
        logging.info("%s OrParser has doneparser '%s'" % (self.name, prod.name))
        self.doneparsers.append(prod)

    for prod in bad:
      self.parsers.remove(prod)
      del self.displays[prod]

    if len(self.parsers) == 0:
      logging.info("%s OrParser turned bad" % self.name)
      self.bad = True
      self.done = False
      return ''
    elif len(self.doneparsers) == 1:
      self.done = True
    else:
      self.done = False

    if len(self.parsers) == 1:
      disp = self.displays[self.parsers[0]]
      self.displays[self.parsers[0]] = ''
      return self.display(disp, self.firstparse)
    return ''

  def display(self,disp,firstparse):
    # self.rule.msg: 'start%send'
    # use self.firstparse to determine if left should be output
    if self.firstparse:
      disp = self.msg_start + disp
      self.firstparse = False
    return disp

  def finish(self):
    if len(self.doneparsers) == 0:
      raise Exception("Not sure this is actually bad, but: OrParser %s requested finish() without having a doneparser" % self.name)
    elif len(self.doneparsers) == 1:
      return self.doneparsers[0].finish() + self.msg_end
    else:
      raise Exception("Not sure this is actually bad, but: OrParser %s requested finish() without having a SINGLE doneparser" % self.name)

class Or(Rule):
  def MakeParser(self,parent):
    return OrParser(self, parent)

