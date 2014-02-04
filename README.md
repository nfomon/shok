shok
====

http://shok.io

The shok command shell is a non-POSIX interactive command language interpreter with an expressive scripting language.  It intends to be a modern, discoverable shell intended for every-day command invocation, process management, and filesystem manipulation.

status
======

shok is in its early stages of initial development.  It has the framework for a shell, a lexer, a parser, and an "evaluator" (type-checking, AST execution, program invocation).  If it compiles, it may let you change directories and run commands but not write programs, or much else really.  Most core features have yet to be implemented.  All language attributes are suitable for discussion and replacement.  Nevertheless, it is progressing quickly and steadily.

Get involved!  See http://shok.io for details.  Description of the code layout and steps forward are coming soon.

todo
====

Immediate hacking priorities:

1. functions / methods

2. trivial implementations of a few basic objects

3. string literals

4. basic interactive niceties: left/right/home/end, ^L

5. comments
