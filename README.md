shok
====

The shok command shell is a non-POSIX interactive command language interpreter with an expressive scripting language.  It is a modern, discoverable environment intended for every-day command invocation, process management, and filesystem manipulation.

status
======

shok is in its early stages of initial development.  It has the framework for a shell, a lexer, a parser, and an "evaluator" (type-checking, AST execution, program invocation).  If it compiles, it may let you change directories and run commands but not write programs, or much else really.  Most core features have yet to be implemented.  All language attributes are suitable for discussion and replacement.  Nevertheless, it is progressing quickly and steadily.

todo
====

1. Interpreter directives (pragmas)

2. Some basic objects and literals

3. Basic expressions and statements

4. Basic shell features: I/O redirection, pipes, simple history

5. Unicode

6. Programming language

7. Concurrency model

8. ...
