# Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
# directory of this distribution and at http://shok.io/code/copyright.html

# Tokens that come from the Lexer are either pairs or tuples:
#   colno:type
#   colno:type:value
class LexToken:
  colno = 0
  ttype = ''
  tvalue = ''
  def __init__(self, tokenstr):
    t = tokenstr.split(':')
    if len(t) < 2 or len(t) > 3:
      raise Exception("invalid token: %s" % t)
    self.colno = t[0]
    self.ttype = t[1]
    if len(t) == 3:
      self.tvalue = t[2]

  def __repr__(self):
    if '' == self.tvalue:
      return "<%s:%s>" % (self.colno, self.ttype)
    else:
      return "<%s:%s:%s>" % (self.colno, self.ttype, self.tvalue)

def NewlineToken():
  return LexToken('0:NEWL')
