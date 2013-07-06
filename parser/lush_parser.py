#!/usr/bin/env python
import logging
logging.basicConfig(filename='parser.log',filemode='w',level=logging.WARNING)
import sys
import traceback as tb
from LexToken import LexToken, NewlineToken
from LushParser import LushParser


# We may be writing to a pipe, so be careful not to print anything unless we're
# pretty sure it should be delivered.  Each line of input from stdin should
# produce 1 line on stdout.

def main():
  if len(sys.argv) == 2:
    lev=sys.argv[1].upper()
    logging.getLogger().setLevel(lev)
    parse()
  elif len(sys.argv) != 1 and len(sys.argv) != 2:
    print "usage: %s [log level]" % sys.argv[0]
  else:
    parse()

def Restart():
  return LushParser()

def parse():
  parser = Restart()
  while 1:
    try:
      line = sys.stdin.readline()
    except KeyboardInterrupt:
      logging.info("Keyboard interrupt; done")
      break
    except:
      logging.error("Error reading from stdin: %s" % tb.format_exc())
      return
    if not line:
      logging.info("End of input; done")
      return
    try:
      ast = ''    # AST snippet for this line
      line = line.strip()
      logging.info("line: '%s'" % line)

      if "" != line:
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

      if parser.bad:
        raise Exception("Top parser went bad: %s" % parser)
      else:
        logging.info(" - sending token '%s'" % NewlineToken())
        parser.parse(NewlineToken())
        if parser.ast:
          ast += parser.ast
          parser.ast = ''
        try:
          print ast
          sys.stdout.flush()
        except Exception as e:
          logging.error("Error writing output: %s" % tb.format_exc())
          print "Error writing output: %s" % e
          sys.stdout.flush()
          return
    except Exception as e:
      logging.error("Parse error: %s" % tb.format_exc())
      print "::Parse error: %s" % e
      sys.stdout.flush()
      parser = Restart()

if __name__ == "__main__":
  main()
