# Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
# directory of this distribution and at http://shok.io/code/copyright.html

import Parser

class Rule:
  def __init__(self,name,items):
    self.name = name
    self.items = items
    self.bad = False
    self.done = False
    self.evil = False

  def MakeParser(self,parent):
    return Parser.Parser(self, parent)

  def __repr__(self):
    return "{rule %s}" % self.name

def Opt(rule):
  rule.done = True
  return rule

def IsRuleDone(x):
  if hasattr(x, 'done'):
    return x.done
  if isinstance(x, str):
    return False
  if isinstance(x, tuple):
    return IsRuleDone(x[0])
  if isinstance(x, list):
    return all([IsRuleDone(i) for i in x])
  raise Exception("Cannot check IsRuleDone from unknown rule type of '%s'" % x)

