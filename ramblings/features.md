lush
====
lush is a command shell.  It helps the user compose and run commands
(programs), manipulate the filesystem, manage system processes, and more.

A shell has to make a trade-off between simplicity for program invocation, vs.
the expressive power of a rich embedded toolset.  Most shells include a
scripting language with an awkward and error-prone syntax, designed as such so
that it does not interfere with the user's ability to write commands.
General-purpose scripting languages, conversely, tend to be very poor
environments for running programs interactively.

lush takes a novel approach in order to minimize the trade-off.  The key is to
provide a simple syntax with which to switch between the program invocation
shell and the embedded scripting language.  When the two modes are
complementary, an elegant user experience emerges.

haiku
=====
        all there is to lush
    { write code in curly braces }
       : colon runs commands


quick example
=============

        cd /usr/bin
        ls
        { new x = 'hello' }
        echo {x}

        {
          new playlist = []
          each song in ~/music/ {
            if song.name.startsWith('Raffi') {
              : mv {song} /media/usb/key/
            } else {
              playlist += song
            }
          }
        }
        mp3blaster {playlist}

command-mode
============
lush starts in command-mode.  Every line is a command, just like in familiar
shells like 'bash' or 'cmd.exe'.

        cd /usr/local/
        ls bin/

lush inherits many useful features from bash and other interactive shells:

 - command history
 - pipelines and redirection
 - job control
 - ...

code-mode
=========
The lush scripting language is a statically-typed, prototype-based
object-oriented programming language.  It is specifically designed for its role
as an interactive shell; a secure and friendly user-facing portal to the
underlying operating system in a platform-independent manner.  Some features:

 - Native path literals and other filesystem types
 - Static type safety
 - Familiar declarative style
 - Security-conscious transitions between language constructs and program
   invocation
 - Composable object prototypes with multiple inheritance

