# Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
# directory of this distribution and at http://lush-shell.org/copyright.html

from PlusParser import Plus
import logging

class Star(Plus):
  def __init__(self,name,items):
    Plus.__init__(self, name, items)
    self.done = True

