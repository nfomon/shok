# Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
# directory of this distribution and at http://shok.io/code/copyright.html

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
    self.parsedtodone = False # Have we already parsed the token?
                              # Separate from done because we might be optional
    self.displayValue = False
    if hasattr(self.rule, 'displayValue') and self.rule.displayValue:
      self.displayValue = self.rule.displayValue

  def parse(self,token):
    logging.debug("%s TerminalParser parsing '%s'" % (self.name, token))
    if self.bad:
      raise Exception("%s TerminalParser already bad" % self.name)
    if self.parsedtodone:
      self.done = False
      self.bad = True
      return ''
    if token.ttype == self.tname:
      self.done = True
      self.parsedtodone = True
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
    if self.displayValue:
      return '%s' % self.value[1:-1]
    return "%s:%s" % (self.tname, self.value)

  def fakeEnd(self):
    return ''

class Terminal(Rule):
  def __init__(self,name):
    Rule.__init__(self, name, [name])

  def MakeParser(self,parent):
    return TerminalParser(self, parent)

# A Terminal that displays the original text (value) verbatim, and not the
# terminal name.
class ValueTerminal(Terminal):
  def __init__(self,name):
    Terminal.__init__(self, name)
    self.displayValue = True

