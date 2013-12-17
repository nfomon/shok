# Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
# directory of this distribution and at http://shok.io/code/copyright.html

from PlusParser import Plus
import logging

class Star(Plus):
  def __init__(self,name,items):
    Plus.__init__(self, name, items)
    self.done = True

