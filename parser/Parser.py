import MakeRule
import Rule
import logging

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

def MakeParser(x, parent=None):
  if isinstance(x, Parser):
    return x
  elif isinstance(x, Rule.Rule):
    return x.MakeParser(parent)
  return MakeParser(MakeRule.MakeRule(x), parent)

