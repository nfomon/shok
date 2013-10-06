# Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
# directory of this distribution and at http://lush-shell.org/copyright.html

import MakeRule
import Rule
import logging

class Parser(object):
  def __init__(self,rule,parent=None):
    self.rule = rule
    self.name = rule.name
    self.bad = self.rule.bad
    self.done = self.rule.done
    if parent:
      self.parent = parent
      self.top = parent.top
    else:
      self.parent = None
      self.top = self

  def __repr__(self):
    return '%s (%s)' % (self.name, self.rule.name)

  def goDone(self):
    self.done = True
    self.parse = self.goBad

  def goBad(self,token):
    if self.bad:
      raise Exception("%s Parser is already bad" % self.name)
    self.bad = True
    self.done = False

def MakeParser(x, parent=None):
  if isinstance(x, Parser):
    return x
  elif isinstance(x, Rule.Rule):
    return x.MakeParser(parent)
  return MakeParser(MakeRule.MakeRule(x), parent)
