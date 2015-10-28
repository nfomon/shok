statik
======

http://statik.rocks

`statik` is a C++ library for incremental compilation.  You give it a grammar, and it gives you an incremental parser.  "Incremental" means that the input are changes to the characters of the input source code, and the output are changes to the resulting compiled bytecode.

Statik allows you to write a compiler as a sequence of "phases" of parsing.  Each phase incrementally translates an input list to an output list.  For example, your compiler might be constructed as the pipeline: lexer -> parser -> code-generator.  The lexer incrementally translates source-code characters to tokens.  The parser translates incremental changes to the token list, to an incrementally-updating parse tree.  The code-generator responds to the changing parse tree by generating the bytecode for those changes, and emits the bytecode changes incrementally.

The parser author can provide as many phases as they want; for example, you could toss in an incremental pre-processor, or other phases of compilation.

status
======

Statik still has some major bugs in some of its rules, especially when grammars are more than a few layers deep.  So it's not yet practical for usage, but it's getting pretty close.  It can also output graphs (in GraphViz format) to help understand how it runs an incremental parse.

code
====

Statik is embedded within the codebase for [shok](http://shok.io); they will be separated soon.

There are three pieces:
- statik/  The library itself, compile it down to libstatik.a
  - statik/test/  Test suite
- exstatik/  Example parsers
- istatik/  An interactive ncurses GUI for running/visualizing statik

Publications
===========

- [Poster at SPLASH 2015](http://2015.splashcon.org/event/splash2015-posters-statik-an-incremental-compiler-generator)
