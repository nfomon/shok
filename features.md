lush
====
lush is a command language interpreter.  It helps the user compose and run
commands, manage system processes, manipulate the filesystem, and more.

A shell has to make a trade-off between simplicity for command invocation, and
the expressive power of a rich embedded toolset.  Most shells include a
scripting language with an awkward and error-prone syntax, designed as such so
that it does not interfere with the user's ability to write commands.
General-purpose scripting languages, conversely, tend to be very poor
environments for running programs interactively.

lush takes a different approach in order to minimize the trade-off.  The key is
to provide a simple syntax with which to switch between the command invocation
shell and the embedded scripting language.  When the two modes are
complementary, an elegant user experience emerges.

haiku
=====
        all there is to lush
    { write code in curly braces }
       : colon runs commands


quick example
=============
Assume `lush:` is the lush prompt.

        lush: cd /home/mike
        lush: ls animals/
        Bear  Cow  Duck

        lush: echo "roar!" > Bear
        lush: echo "moo!" > Cow
        lush: echo "quack!" > Duck

        lush: { new animal = 'Duck' }
        lush: echo {animal}
        Duck

        lush: cat animals/{animal}
        quack!

        lush: {
                  each file f in ./ {
                      print(f.name ~~ 'says,')
                      : cat {f}
                  }
                  : echo "That's all, folks"
              }
        Bear says,
        roar!
        Cow says,
        moo!
        Duck says,
        quack!
        That's all, folks

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
The lush scripting language is a statically-typed, prototype-based
object-oriented programming language.  It tries to be simple and idiomatic, but
with small amounts of magic to assist in its role as an interactive shell
language.  Some features:

 - Native path literals and other filesystem types
 - Native job control types
 - Static type safety
 - Familiar declarative style
 - Typist-friendly syntax
 - Concurrency (coming soon)
 - Shell security-aware string escapes
 - Composable prototypes with multiple inheritance

