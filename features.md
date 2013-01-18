lush
====

lush is a command language interpreter.  It helps the user compose and run commands, manage system processes, manipulate the filesystem, and more.

A shell has to make a trade-off between simplicity for command invocation, and the expressive power of a rich embedded toolset.  Most shells include a scripting language with an awkward and error-prone syntax, so that it does not interfere with the user''s ability to write commands.  General-purpose scripting languages, alternatively, tend to be very poor environments for running programs interactively.

lush takes a different approach in order to minimize the trade-off.  The key is to provide a simple syntax with which to switch between the command invocation shell and the embedded scripting language.  When the two modes are complementary, an elegant user experience emerges.

haiku
=====

all there is to lush
{ write code in curly braces }
: colon runs command


quick example
=============

lush: cd /home/mike
lush: ls animals/
Bear  Cow  Duck
lush: { var animal = 'Duck' }
lush: echo {animal}
Duck
lush: cat animals/{animal}
quack!
lush: {
$>      each file f in ./ {
$$>       print(f.name ~~ 'says,')
$$>       : cat {f}
$>      }
        : echo "That's all, folks"
      }
bear says,
roar!
cow says,
moo!
Duck says,
quack!
That''s all, folks
lush: exit

command-mode
============

lush starts in command-mode.  Every line is a command, just like 'bash' or
'cmd.exe'.
lush: cd /home/mike
lush: ls animals/
Bear  Cow  Duck

lush inherits many useful features from bash and other interactive shells:
 - command history
 - pipelines and redirection
 - job control
 - ...

code-mode
=========

The lush scripting language is a statically-typed, prototype-based object-oriented programming language.  It tries to be simple and idiomatic, but with small amounts of magic to assist in its role as an interactive shell language.  Some features:
 - Typist-friendly syntax
 - Native path literals and other filesystem types
 - Native job control types
 - Static type safety
 - Familiar declarative style
 - Concurrency (coming soon)
 - Shell security-aware string escapes
 - Composable prototypes with multiple inheritance


language basics
===============

Native types:
 - str
 - int (integer, unbounded size)
 - fixed (fixed-point number)
 - float (floating-point number of a specific IEEE_754-2008 binary64 format)
 - [...] (list)
 - <...> (tuple)
 - table (data table with typed and indexable columns for efficient lookups)
    -- e.g. a dictionary is a 2-column table
 - path
    -- subtypes: file, dir, link, pipe, dev (subtypes: cdev, bdev)
 - process

Overloadable operators:
 - !
 - +
 - -
 - *
 - /
 - %
 - ^
 - <
 - >

Non-overloadable operators:
 - ~ - str concatenation
 - ~~ - winged-bat operator; syntactically equivalent to  (~' '~)
  -- aka two-worms-looking-at-each-other operator
 - & - append code chunks (type intersection)
 - | - conditional object parents (type union)
 - ? - conditional type acceptance: { var x = int?(value>12)|null }
 - . - property accessor
 - () - run code


language details
================
