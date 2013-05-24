#!/usr/bin/env python
import sys
import logging
from LexToken import LexToken, NewlineToken
from LushParser import LushParser
logging.basicConfig(filename="parse.log", level=logging.DEBUG)

def main():
  if len(sys.argv) > 1 and sys.argv[1].lower() == "debug":
    parse(debug=1)
  elif len(sys.argv) != 1:
    print "usage: %s [debug]" % sys.argv[0]
  else:
    parse(debug=0)

def Restart():
  return LushParser()

def parse(debug=0):
  parser = Restart()
  while 1:
    line = sys.stdin.readline()
    if not line:
      break
    ast = ''    # AST snippet for this line
    line = line.strip()
    logging.info("line: '%s'" % line)

    if "" != line:
      try:
        tokens = line.split(' ')
        lineno = int(tokens[0])
        tokens = tokens[1:]
        for token in tokens:
          t = LexToken(token)
          logging.info(" - sending token '%s'" % t)
          parser.parse(t)
          if parser.ast:
            ast += parser.ast
            parser.ast = ''

      except:
        sys.stderr.write("Invalid token found in line '%s'\n" % line)
        raise

    if parser.bad:
      print "bad parser: '%s'" % parser
      parser = Restart()
    else:
      logging.info(" - sending token '%s'" % NewlineToken())
      parser.parse(NewlineToken())
      if parser.ast:
        ast += parser.ast
        parser.ast = ''
      if ast:
        print "AST:" + ast
      else:
        print "..."

if __name__ == "__main__":
  main()
