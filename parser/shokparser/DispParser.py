# Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
# directory of this distribution and at http://shok.io/code/copyright.html

from Parser import Parser, MakeParser
from Rule import Rule
from copy import copy
import logging

class DispParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    self.firstparse = True
    msg = self.rule.msg.split('%s')
    self.msg_start = msg[0]
    self.msg_middle = self.rule.msg.count('%s') > 0
    self.msg_end = ''
    if len(msg) == 2:
      self.msg_end = msg[1]
    elif len(msg) > 2:
      raise Exception("DispParser %s found inappropriate msg '%s'" % (self.name, msg))
    if len(self.rule.items) != 1:
      raise Exception("DispParser %s must have 1 item" % self.name)
    self.parser = MakeParser(self.rule.items[0], self)
    self.bad = self.parser.bad
    self.done = self.parser.done
    self.name = self.name % self.parser.name
    self.debug = []

  def parse(self,token):
    if self.parser.name in self.debug:
      print "%s parsing %s" % (self.name, token)
    if self.bad:
      raise Exception("%s DispParser: cannot parse '%s'; already bad" % (self.name, token))
    if not self.parser:
      self.parser = MakeParser(self.rule.items[0], self)
    disp = self.parser.parse(token)
    self.bad = self.parser.bad
    self.evil = self.parser.evil
    self.done = self.parser.done
    if self.bad:
      if self.parser.name in self.debug:
        print "%s went bad" % self.name
      if self.firstparse:
        if self.parser.name in self.debug:
          print "%s bad and firstparse" % self.name
        return ''   # deny a bad first parse
      if self.parser.name in self.debug:
        print "%s bad and wasdone" % self.name
      if self.msg_middle:
        disp += copy(self.msg_end)
      else:
        disp = copy(self.msg_end)
      self.msg_end = ''
    if self.firstparse:
      if self.msg_middle:
        if self.parser.name in self.debug:
          print "%s firstparse yields '%s':'%s'" % (self.name, self.msg_start, disp)
        disp = copy(self.msg_start) + disp
      else:
        if self.parser.name in self.debug:
          print "%s firstparse yields '%s'" % (self.name, disp)
        disp = copy(self.msg_start)
      self.msg_start = ''
      self.firstparse = False
    if self.parser.name in self.debug:
      print "%s boring yields '%s'" % (self.name, disp)
    return disp

  def fakeEnd(self):
    if self.parser:
      return self.parser.fakeEnd() + self.msg_end
    return self.msg_end

class Disp(Rule):
  def __init__(self,items,msg='%s'):
    Rule.__init__(self, '<disp:%s>', [items])
    self.msg = msg

  def MakeParser(self,parent):
    return DispParser(self, parent)

