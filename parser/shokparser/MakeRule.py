# Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
# directory of this distribution and at http://shok.io/code/copyright.html

import DispParser
import SeqParser
import TerminalParser

def MakeRule(x):
  if isinstance(x, str):
    return TerminalParser.Terminal(x)
  if isinstance(x, tuple):
    return DispParser.Disp(x[0], x[1])
  if isinstance(x, list):
    return SeqParser.Seq(str(x), x)
  raise Exception("Cannot make rule from unknown type of '%s'" % x)

