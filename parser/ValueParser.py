# Copyright (C) 2013 Michael Biggs.  See the COPYRIGHT file at the top-level
# directory of this distribution and at http://lush-shell.org/copyright.html

from TerminalParser import TerminalParser, Terminal
import logging

# A Terminal whose display() function just drops the token name (type)
# and returns the original text (value) verbatim

class ValueParser(TerminalParser):
  def display(self):
    if self.bad or not self.done:
      raise Exception("Value '%s' is unmatched" % self.name)
    if self.value == None:
      if self.rule.msg.find('%s') == -1:
        return self.rule.msg
      raise Exception("ValueParser received token with no value")
    return self.rule.msg % self.value[1:-1]

class ValueTerminal(Terminal):
  def MakeParser(self,parent):
    return ValueParser(self, parent)

