# Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
# directory of this distribution and at http://lush-shell.org/copyright.html

from TerminalParser import TerminalParser, Terminal
import logging

# A Terminal that displays the original text (value) verbatim, and not the
# terminal name.

class ValueTerminal(Terminal):
  def __init__(self,name):
    Terminal.__init__(self, name)
    self.displayValue = True
