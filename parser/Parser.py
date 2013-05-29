import logging
import sys
from copy import copy
logging.basicConfig(filename="Parser.log", level=logging.DEBUG)
#sys.setrecursionlimit(100)

class Parser(object):
  def __init__(self,rule,parent=None):
    self.rule = rule
    self.name = rule.name
    self.bad = self.rule.bad
    self.done = self.rule.done
    self.signalRestart = False
    if parent:
      self.parent = parent
      self.top = parent.top
    else:
      self.parent = None
      self.top = self
  def __repr__(self):
    return '%s (%s)' % (self.name, self.rule.name)
  def restart(self):
    logging.info("Restarting Parser %s" % self.name)
    self.bad = self.rule.bad
    self.done = self.rule.done

class TerminalParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    if len(self.rule.items) != 1:
      raise Exception("TerminalParser's Terminal must have one rule.item")
    self.tname = self.rule.items[0]
    self.value = None

  def restart(self):
    Parser.restart(self)
    self.tname = self.rule.items[0]
    self.value = None

  def parse(self,token):
    if self.signalRestart:
      self.restart()
      self.signalRestart = False
    logging.debug("%s TerminalParser parsing '%s'" % (self.name, token))
    if self.bad:
      raise Exception("%s TerminalParser already bad" % self.name)
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
class SeqParser(Parser):
  def __init__(self,rule,parent):
    Parser.__init__(self, rule, parent)
    self.parsers = []   # parsers constructed for each of rule.items
    self.active = None  # active parser (top of self.parsers)
    self.pos = 0        # position in self.rule.items
    self.displays = []  # display() of any completed sub-parsers
    logging.info("%s SeqParser initialized with rule: %s" % (self.name, self.rule))
    while len(self.displays) != len(self.rule.items):
      self.displays.append('')

  def restart(self):
    Parser.restart(self)
    self.parsers = []
    self.active = None
    self.pos = 0
    self.displays = []
    while len(self.displays) != len(self.rule.items):
      self.displays.append('')

  def parse(self,token):
    if self.signalRestart:
      self.restart()
      self.signalRestart = False
    logging.debug("%s SeqParser at pos=%s attempting to parse token '%s'" % (self.name, self.pos, token))

    if self.pos >= len(self.rule.items):
      logging.debug("%s SeqParser moved beyond rule items; now bad" % self.name)
      self.bad = True
      self.done = False
    elif not self.active:
      logging.info("%s SeqParser making parser for %s at pos=%s" % (self.name, self.rule.items[self.pos], self.pos))
      self.parsers.append(MakeParser(self.rule.items[self.pos], self))
      logging.info("%s SeqParser now parsers=%s, len=%s" % (self.name, self.parsers, len(self.parsers)))
      self.active = self.parsers[-1]
      self.bad = self.active.bad
    if self.pos != len(self.parsers)-1:
      raise Exception("%s SeqParser: parsers (%s) vs. self.pos=%s mismatch; len=%s, lenrule=%s" % (self.name, self.parsers, self.pos, len(self.parsers), len(self.rule.items)))
    if self.bad:
      self.done = False
      raise Exception("%s SeqParser is bad; can't accept token '%s'" % (self.name, token))
    logging.info("%s SeqParser parsing token '%s'; active=%s,%s" % (self.name, token, self.pos, self.active.name))

    tempdisp = ''
    wasdone = self.active.done    # was the current active parser already done
    if wasdone:
      logging.debug("%s SeqParser was done; get tempdisp '%s'" % (self.name, self.active.name))
      tempdisp = self.active.display()

    # Parse!
    self.active.parse(token)
    logging.debug("%s SeqParser parsed token '%s'; now evaluating" % (self.name, token))

    # if last
    if self.pos == len(self.rule.items)-1:
      self.bad = self.bad or self.active.bad
      if self.bad:
        self.done = False
      else:
        self.done = self.active.done
        if self.done:
          self.displays[self.pos] = self.active.display()
      logging.debug("%s SeqParser in last state; now bad=%s, done=%s" % (self.name, self.bad, self.done))
      return

    if self.active.bad:
      if wasdone:
        # accept that state as having never parsed this token yet, and go
        # forward
        logging.info("%s SeqParser at token '%s' is force-advancing pos %s to %s" % (self.name, token, self.pos, self.pos+1))
        self.displays[self.pos] = tempdisp
        self.pos += 1
        self.active = None
        self.parse(token)
      else:
        self.bad = True
        self.done = False
        logging.debug("%s SeqParser wasn't done, now bad" % self.name)
      return

    if self.active.done:
      # stay here until we turn bad
      # if all upcoming rule items are Stars, then mark the whole Seq as done.
      logging.info("%s SeqParser active pos=%s went done; waiting until it turns bad before advancing" % (self.name, self.pos))
      nonstars = False
      for item in self.rule.items[self.pos+1:]:
        if not isinstance(item, Star):
          nonstars = True
      if not nonstars:
        self.done = True
        self.displays[self.pos] = self.active.display()
        logging.debug("%s SeqParser is earlydone with displays %s" % (self.name, self.displays))
      return

    # we're ok, stay put for now
    logging.debug("%s SeqParser is staying put after '%s'" % (self.name, token))

  def display(self):
    if self.bad or not self.done:
      raise Exception("%s SeqParser is unfinished" % self.name)
    logging.debug("%s SeqParser is displaying; bad=%s done=%s displays=%s" % (self.name, self.bad, self.done, self.displays))
    logging.debug(" - will become: '%s'" % self.rule.display(self.displays))
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

  def display(self):
    # We used to check if len(self.parsers) == 1.  Now we allow display() to be
    # called early (just in case) so we must remove this check...
    # We also used to check if self.done.  But now we might call display() early and store it just in case...
    if self.bad or not self.done or len(self.doneparsers) == 0:
      raise Exception("%s OrParser is unfinished" % self.name)
    logging.debug("%s OrParser is displaying; bad=%s done=%s disp=%s" % (self.name, self.bad, self.done, self.doneparsers[0].display()))
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
    self.reps = 0

  def restart(self):
    Parser.restart(self)
    self.active = None
    self.disps = []

  def parse(self,token):
    if self.signalRestart:
      self.restart()
      self.signalRestart = False
    logging.debug("%s PlusParser parsing token '%s'" % (self.name, token))
    if not self.active:
      self.active = MakeParser(self.rule.items, self)
    self.bad = self.bad or self.active.bad
    if self.bad:
      raise Exception("%s PlusParser is bad" % self.name)
    wasdone = False
    tempdisp = ''
    if self.active.done:
      wasdone = True
      tempdisp = self.active.display()
    self.active.parse(token)
    if wasdone and self.active.bad:
      # re-establish
      self.active = MakeParser(self.rule.items, self)
      self.disps.append(tempdisp)
      self.reps += 1
      logging.debug("Re-establishing %s; reps=%s, disps=%s" % (self.name, self.reps, self.disps))
      self.parse(token)   # this is not recursive in the way that would hurt
    self.bad = self.active.bad
    self.done = self.active.done

  def display(self):
    if self.bad or not self.done:
      raise Exception("%s PlusParser is unfinished" % self.name)
    disps = copy(self.disps)
    disps.append(self.active.display())
    s = ''
    for m in range(0,len(disps)):
      s += self.rule.msg
    return s % tuple(disps)

# StarParser repeats its single rule 0 or more times.  That is, it always
# starts off 'done'.
class StarParser(PlusParser):
  def __init__(self,rule,parent):
    PlusParser.__init__(self, rule, parent)
    self.done = True
    self.anyparse = False
  def restart(self):
    PlusParser.restart(self)
    self.done = True
    self.anyparse = False
  def parse(self,token):
    logging.debug("%s StarParser parsing token '%s'" % (self.name, token))
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
#
# You should be careful about where you place Actions in your grammar :)
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
    if not self.msg:
      return ''
    t = [displays[x] for x in self.inds]
    return self.msg % tuple(t)

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

