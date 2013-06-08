from Parser import Parser, MakeParser
from Rule import Rule
import logging

# The OrParser's rule is a list of Rules that will be matched in
# parallel.  The OrParser is done when a single one of its rules is
# done.
class OrParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    self.parsers = []
    for item in rule.items:
      self.parsers.append(MakeParser(item, self))

  def restart(self):
    Parser.restart(self)
    self.parsers = []
    for item in rule.items:
      self.parsers.append(MakeParser(item, self))

  def parse(self,token):
    if self.signalRestart:
      self.restart()
      self.signalRestart = False
    logging.debug("%s OrParser parsing '%s'" % (self.name, token))
    if len(self.parsers) == 0:
      logging.warning("%s OrParser can't parse; no productions" % self.name)
      self.bad = True
    if self.bad:
      raise Exception("State '%s' is bad" % self.name)
    bad = []
    self.doneparsers = []
    for prod in self.parsers:
      logging.info("%s OrParser parsing prod %s" % (self.name, prod.name))
      if prod.bad:
        raise Exception("%s OrParser already has bad prod '%s'" % (self.name, prod.name))
      prod.parse(token)
      if prod.bad:
        logging.info("%s OrParser went bad at '%s'" % (self.name, prod.name))
        bad.append(prod)
      elif prod.done:
        logging.info("%s OrParser has doneparser '%s'" % (self.name, prod.name))
        self.doneparsers.append(prod)

    for prod in bad:
      self.parsers.remove(prod)

    if len(self.parsers) == 0:
      logging.info("%s OrParser turned bad" % self.name)
      self.bad = True
      self.done = False
    elif len(self.doneparsers) > 0:
      self.done = True
    else:
      self.done = False
    self.neverGoBadCheck(token)

  def display(self):
    # We used to check if len(self.parsers) == 1.  Now we allow display() to be
    # called early (just in case) so we must remove this check...
    # We also used to check if self.done.  But now we might call display()
    # early and store it just in case...
    if self.bad or not self.done or len(self.doneparsers) == 0:
      raise Exception("%s OrParser is unfinished" % self.name)
    logging.debug("%s OrParser is displaying; bad=%s done=%s disp=%s" % (self.name, self.bad, self.done, self.doneparsers[0].display()))
    return self.rule.msg % self.doneparsers[0].display()

class Or(Rule):
  def MakeParser(self,parent):
    return OrParser(self, parent)

