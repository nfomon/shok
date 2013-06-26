lush interpreter design
=======================
The lush interpreter has been designed with a UNIX philosophy in mind:
orchestrate a complicated system (language interpreter) using
inter-process communication (IPC) between discrete single-minded
processes.  The lush lexer, parser, and evaluator are all independent
programs.  The shell is another program that simply depends on these
other programs in order to function.

This design is questionable, but it has a few nice properties:
 - easy to test
 - easy to extend
 - different parts can be written in different programming languages
 - some of the pieces may be useful on their own
 - motivates reasoning about IPC, which the lush language will work to
   improve

There's an obvious, enormous loss from this design: all the code and
runtime efficiency of serializing and deserializing the I/O between
each of the processes.  However this will be mitigated by a (planned)
generic platform improvement to IPC, similar to Cap'n Proto but
specifically aimed at stdin/stdout text streams that have both
binary-serialized representations as well as human-interpretable ascii
representations.  Ping the author for details.

Other problems with the design include the increased complexity of
supporting the IPC mechanism across different operating environments,
the runtime penalty (and UI awkwardness) of multi-processing, possible
security risks with all this I/O.  Perhaps the most egregious problem
is the complexity of getting the user's commands to run as children of
the parent (shell) process, while providing information from these jobs
back to the evaluator when/where/how appropriate.

Ultimately, this is an experimental, reference compiler/interpreter
that prioritizes flexibility over performance or elegance.  Help
ranging from complete alternate implementations to
suggestions/discussions about the interpreter design are welcome!
