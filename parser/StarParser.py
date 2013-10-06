# Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
# directory of this distribution and at http://lush-shell.org/copyright.html

from Parser import Parser, MakeParser
from PlusParser import PlusParser
from Rule import Rule
import logging

# StarParser repeats its single rule 0 or more times.  That is, it always
# starts off 'done'.
class StarParser(PlusParser):
  def __init__(self,rule,parent):
    PlusParser.__init__(self, rule, parent)
    self.done = True
    self.anyparse = False

  def parse(self,token):
    logging.debug("%s StarParser parsing token '%s'" % (self.name, token))
    disp = PlusParser.parse(self, token)
    if not self.bad:
      self.anyparse = True
      return disp
    return ''

class Star(Rule):
  def MakeParser(self,parent):
    return StarParser(self, parent)

