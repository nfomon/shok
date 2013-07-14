# Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
# directory of this distribution and at http://lush-shell.org/copyright.html

import SeqParser
import TerminalParser

def MakeRule(x):
  if isinstance(x, str):
    return TerminalParser.Terminal(x)
  if isinstance(x, list):
    return SeqParser.Seq(str(x), x)
  raise Exception("Cannot make rule from unknown type of '%s'" % x)

