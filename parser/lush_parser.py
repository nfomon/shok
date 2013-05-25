#!/usr/bin/env python
import sys
import logging
from LexToken import LexToken, NewlineToken
from LushParser import LushParser
logging.basicConfig(filename="parse.log", level=logging.DEBUG)

# We may be writing to a pipe, so be careful not to print anything unless we're
# pretty sure it should be delivered.  Each line of input from stdin should
# produce 1 line on stdout.

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
    try:
      line = sys.stdin.readline()
    except KeyboardInterrupt:
      logging.info("Keyboard interrupt; done")
      break
    except:
      e = sys.exc_info()
      logging.error("Error reading from stdin: (%s,%s,%s)" % e)
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
        except:
          e = sys.exc_info()
          logging.error("Error writing output: (%s,%s,%s)" % e)
          print "Error writing output: (%s,%s,%s)" % e
          sys.stdout.flush()
          return
    except:
      e = sys.exc_info()
      logging.error("Parse error: (%s,%s,%s)" % e)
      print "Parse error: (%s,%s,%s)" % e
      sys.stdout.flush()
      parser = Restart()

if __name__ == "__main__":
  main()
