# Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
# directory of this distribution and at http://lush-shell.org/copyright.html

import Parser

class Rule:
  def __init__(self,name,items,msg='%s',inds=[0]):
    self.name = name
    self.items = items
    self.msg = msg
    self.inds = inds
    self.bad = False
    self.done = False

  def display(self,displays):
    if not self.msg:
      return ''
    t = [displays[x] for x in self.inds]
    return self.msg % tuple(t)

  def MakeParser(self,parent):
    return Parser.Parser(self, parent)

