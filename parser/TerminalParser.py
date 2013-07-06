# Copyright (C) 2013 Michael Biggs.  See the COPYRIGHT file at the top-level
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

  def restart(self):
    Parser.restart(self)
    self.tname = self.rule.items[0]
    self.value = None

  def parse(self,token):
    if self.signalRestart:
      self.restart()
      self.signalRestart = False
    logging.debug("%s TerminalParser parsing '%s'" % (self.name, token))
    if self.bad:
      raise Exception("%s TerminalParser already bad" % self.name)
    if self.done:
      self.done = False
      self.bad = True
      self.neverGoBadCheck(token)
      return
    if token.ttype == self.tname:
      self.done = True
      if token.tvalue != None and token.tvalue != '':
        self.value = token.tvalue
    else:
      self.done = False
      self.bad = True
    self.neverGoBadCheck(token)

  def display(self):
    if self.bad or not self.done:
      raise Exception("Terminal '%s' is unmatched" % self.name)
    if self.value == None:
      if self.rule.msg.find('%s') == -1:
        return self.rule.msg
      return self.rule.msg % self.tname
    return self.rule.msg % ("%s:%s" % (self.tname, self.value))

class Terminal(Rule):
  def __init__(self,name,items=None,msg='%s',inds=[0]):
    if not items:
      items = [name]
    Rule.__init__(self, name, items, msg, inds)

  def MakeParser(self,parent):
    return TerminalParser(self, parent)

