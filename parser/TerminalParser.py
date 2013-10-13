# Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
# directory of this distribution and at http://lush-shell.org/copyright.html

from Parser import Parser
from Rule import Rule
import logging

class TerminalParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    if isinstance(self.rule.items, str):
      self.rule.items = [self.rule.items]
    if len(self.rule.items) != 1:
      raise Exception("TerminalParser's Terminal must have one rule.item; instead got '%s'" % self.rule.items)
    self.tname = self.rule.items[0]
    self.value = None

  def parse(self,token):
    logging.debug("%s TerminalParser parsing '%s'" % (self.name, token))
    if self.bad:
      raise Exception("%s TerminalParser already bad" % self.name)
    if self.done:
      self.done = False
      self.bad = True
      return ''
    if token.ttype == self.tname:
      self.done = True
      if token.tvalue != None and token.tvalue != '':
        self.value = token.tvalue
    else:
      self.done = False
      self.bad = True
      return ''

    return self.display()

  def display(self):
    if self.value == None:
      return self.tname
    return "%s:%s" % (self.tname, self.value)

  def fakeEnd(self):
    return ''

class Terminal(Rule):
  def __init__(self,name):
    Rule.__init__(self, name, [name])

  def MakeParser(self,parent):
    return TerminalParser(self, parent)

