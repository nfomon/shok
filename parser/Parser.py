import sys
import logging
logging.basicConfig(filename="Parser.log", level=logging.DEBUG)
#sys.setrecursionlimit(100)

class Parser(object):
  def __init__(self,rule,parent=None):
    self.rule = rule
    self.name = rule.name
    self.bad = self.rule.bad
    self.done = self.rule.done
    if parent:
      self.parent = parent
      self.top = parent.top
    else:
      self.parent = None
      self.top = self

class TerminalParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    if len(self.rule.items) != 1:
      raise Exception("TerminalParser's Terminal must have one rule.item")
    self.tname = self.rule.items[0]
    self.value = None

  def parse(self,token):
    logging.debug("TerminalParser '%s' parsing '%s'" % (self.name, token))
    if self.bad:
      raise Exception("Terminal '%s' already bad" % self.name)
    if self.done:
      self.done = False
      self.bad = True
      return
    if token.ttype == self.tname:
      self.done = True
      if token.tvalue != None and token.tvalue != '':
        self.value = token.tvalue
    else:
      self.done = False
      self.bad = True

  def display(self):
    if self.bad or not self.done:
      raise Exception("Terminal '%s' is unmatched" % self.name)
    if self.value == None:
      return self.tname
    return "%s(%s)" % (self.tname, self.value)

# The SeqParser's rule is a list of Rules that will be matched in
# sequence; advancing only when an individual rule is done and turns
# bad if given another token.  The SeqParser is done anytime that its
# last rule is done.
#
# If a not-the-last-rule's sub-parser moves into a Done state, we have two
# options: try feeding it another token to see if it turns bad before
# advancing, or move ahead immediately (which we might need to discover that we
# can complete the whole sequence).
#
# We used to go with plan b, but that was bad.  Now let's try plan a.
# Eventually we'll probably need to split into an OrParser.
#
# Bug: we move sub-parsers into "bad" state before we keep going.  When
# wasdone==True, store its display() in a list, so that we never ask something
# in bad state to display itself.
class SeqParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    self.parsers = []   # parsers constructed for each of rule.items
    self.active = None  # active parser (top of self.parsers)
    self.pos = 0        # position in self.rule.items
    self.tokens = []    # the tokens we've observed
    self.displays = []  # display() of any completed sub-parsers
    logging.info("Initialized SeqParser '%s' with rule: %s" % (self.name, self.rule))

  def parse(self,token):
    logging.debug("SeqParser '%s' parsing '%s'" % (self.name, token))
    self.tokens.append(token)
    if self.pos >= len(self.rule.items):
      self.bad = True
      self.done = False
    elif not self.active:
      logging.info("Making parser for %s" % self.rule.items[self.pos])
      self.parsers.append(MakeParser(self.rule.items[self.pos], self))
      self.active = self.parsers[-1]
      self.bad = self.active.bad
    if len(self.parsers) != self.pos+1:
      raise Exception("Bad SeqParser '%s': Parser vs. rule.items mismatch" % self.name)
    if self.bad:
      raise Exception("Bad SeqParser '%s' can't accept token" % self.name)
    logging.info("SeqParser '%s' parsing token '%s'; active=%s,%s" % (self.name, token, self.pos, self.active.name))

    tempdisp = ''
    wasdone = self.active.done    # was the current active parser already done
    if wasdone:
      logging.debug("- - - - %s wasdone; now displaying %s" % (self.name, self.active.name))
      tempdisp = self.active.display()
    self.active.parse(token)
    if self.pos < len(self.rule.items)-1 and wasdone and self.active.bad:
      logging.info("* SeqParser '%s' is force-advancing pos %s" % (self.name,self.pos))
      self.pos += 1
      self.active = None
      self.displays.append(tempdisp)
      return self.parse(token)
    else:
      logging.debug(" * * %s ELSE: %s %s %s %s" % (self.name, self.pos, len(self.rule.items)-1, wasdone, self.active.bad))
    self.bad = self.bad or self.active.bad

    logging.info("*** %s bad=%s active.done=%s" % (self.name, self.bad, self.active.done))
    if self.bad:
      logging.info("* SeqParser '%s' turned bad" % self.name)
      self.done = False
    elif self.pos < len(self.rule.items)-1 and self.active.done:
#      logging.info("* SeqParser '%s' advances (could be too eager!)" % self.name)
#      self.pos += 1
#      self.active = None
      logging.info("* SeqParser '%s' could advance but will stay in place (could be too lazy!)" % self.name)
    elif self.active.done:
      logging.info("* SeqParser '%s' is eagerdone" % self.name)
      self.done = True
      self.displays.append(self.active.display())

  def display(self):
    if self.bad or not self.done:
      raise Exception("SeqParser '%s' is unfinished" % self.name)
    return self.rule.display(self.displays)

# The OrParser's rule is a list of Rules that will be matched in
# parallel.  The OrParser is done when a single one of its rules is
# done.
class OrParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    self.parsers = []
    for item in rule.items:
      self.parsers.append(MakeParser(item, self))

  def parse(self,token):
    logging.debug("OrParser '%s' parsing '%s'" % (self.name, token))
    if len(self.parsers) == 0:
      logging.warning("can't parse; no productions")
      self.bad = True
    if self.bad:
      raise Exception("State '%s' is bad" % self.name)
    bad = []
    self.doneparsers = []
    for prod in self.parsers:
      logging.info("OrParser '%s' parsing prod %s" % (self.name, prod.name))
      if prod.bad:
        raise Exception("already bad prod '%s' in '%s'" % (prod.name, self.name))
      prod.parse(token)
      if prod.bad:
        logging.info("OrParser '%s' went bad at '%s'" % (self.name, prod.name))
        bad.append(prod)
      elif prod.done:
        logging.info("OrParser '%s' has doneparser '%s'" % (self.name, prod.name))
        self.doneparsers.append(prod)

    for prod in bad:
      self.parsers.remove(prod)

    if len(self.parsers) == 0:
      logging.info("OrParser '%s' turned bad" % self.name)
      self.bad = True
      self.done = False
    elif len(self.doneparsers) > 0:
      self.done = True
    else:
      self.done = False

  def display(self):
    logging.debug("I'm '%s' and bad: %s, done: %s, len: %s" % (self.name, self.bad, self.done, len(self.doneparsers)))
    # We used to check if len(self.parsers) == 1.  Now we allow display() to be
    # called early (just in case) so we must remove this check...
    # We also used to check if self.done.  But now we might call display() early and store it just in case...
    if self.bad or not self.done or len(self.doneparsers) == 0:
      logging.debug("MWA %s %s %s" % (self.bad, self.done, self.doneparsers))
      raise Exception("OrParser '%s' is unfinished" % self.name)
    logging.debug("(%s %s)" % (self.doneparsers[0].name, self.doneparsers[0].done))
    return self.doneparsers[0].display()

# PlusParser repeats its single parser at least once.  It's done anytime its
# sub-parser is done.  If it gets another token it re-creates itself with
# another instance, so we are memory- and stack-bounded.
# We only re-establish the sub-parser if it was done and it's bad if it were
# given another token.
class PlusParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    self.active = None
    self.disps = []

  def parse(self,token):
    logging.debug("PlusParser '%s' parsing '%s'" % (self.name, token))
    if not self.active:
      self.active = MakeParser(self.rule.items, self)
    if self.active.bad:
      self.bad = self.active.bad
    if self.bad:
      raise Exception("State '%s' is bad" % self.name)
    wasdone = False
    tempdisp = ''
    if self.active.done:
      wasdone = True
      tempdisp = self.active.display()
    self.active.parse(token)
    if wasdone and self.active.bad:
      # re-establish
      self.disps.append(tempdisp)
      self.active = MakeParser(self.rule.items, self)
      self.parse(token)   # this is not recursive in the way that would hurt
    self.bad = self.active.bad
    self.done = self.active.done

  def display(self):
    if self.bad or not self.done:
      raise Exception("PlusParser '%s' is unfinished" % self.name)
    disps = self.disps
    disps.append(self.active.display())
    s = ''
    for d in disps:
      s += d
    return s

# StarParser repeats its single rule 0 or more times.
class StarParser(PlusParser):
  def __init__(self,rule,parent):
    PlusParser.__init__(self, rule, parent)
    self.done = True
    self.anyparse = False
  def parse(self,token):
    logging.debug("StarParser '%s' parsing '%s'" % (self.name, token))
    PlusParser.parse(self, token)
    if not self.bad:
      self.anyparse = True
  def display(self):
    if not self.anyparse:
      return ''
    return PlusParser.display(self)

# ActionParser performs its Action-rule's associated function the first time
# its sub-parser is done.
#
# Why not do it every time?  Well, probably because of the way SeqParser works
# now (tries to repeat any rule until it fails)....  Not sure about that tho.
# We were getting many many repeats of the action...
class ActionParser(Parser):
  def __init__(self,rule,parent):
    if not isinstance(rule, Action):
      raise Exception("Cannot use an ActionParser on a non-Action rule")
    Parser.__init__(self, rule, parent)
    self.actioned = False
    self.active = MakeParser(rule.items, self)
    if self.active.done:
      raise Exception("ActionParser cannot start off with a done sub-parser")

  def parse(self,token):
    self.active.parse(token)
    self.bad = self.active.bad
    self.done = self.active.done
    if not self.actioned and not self.bad and self.done:
      self.rule.func(self)
      self.actioned = True

  def display(self):
    return self.active.display()

class Rule:
  def __init__(self,name,items,msg='%s',inds=[0]):
    self.name = name
    self.items = items
    self.msg = msg
    self.inds = inds
    self.bad = False
    self.done = False

  def display(self,displays):
    if self.msg:
      t = [displays[x] for x in self.inds]
      return self.msg % tuple(t)
    return ''

class Terminal(Rule):
  def __init__(self,name,items=None,msg='%s',inds=[0]):
    if not items:
      items = [name]
    Rule.__init__(self, name, items, msg, inds)

class Seq(Rule):
  pass

class Or(Rule):
  pass

class Plus(Rule):
  pass

class Star(Rule):
  pass

class Action(Rule):
  def __init__(self,items,func):
    Rule.__init__(self, "action(%s,%s)" % (items, func), items)
    self.func = func

def MakeRule(x):
  if isinstance(x, str):
    return Terminal(x)
  if isinstance(x, list):
    return Seq(str(x), x)
  raise Exception("Cannot make rule from unknown type of '%s'" % x)

def MakeParser(x, parent=None):
  if isinstance(x, Parser):
    return x
  if isinstance(x, Terminal):
    return TerminalParser(x, parent)
  if isinstance(x, Seq):
    return SeqParser(x, parent)
  if isinstance(x, Or):
    return OrParser(x, parent)
  if isinstance(x, Plus):
    return PlusParser(x, parent)
  if isinstance(x, Star):
    return StarParser(x, parent)
  if isinstance(x, Action):
    return ActionParser(x, parent)
  return MakeParser(MakeRule(x), parent)

