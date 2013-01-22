Invoking code from the command-line
===================================

At the command-line, '`{`' will swap you into code.

        cd /media/usb_key/
        {
            var song = "gravy"
            song = song.replace('a', 'oo')
        }
        echo { song ~ ".mp3" }      # prints "groovy.mp3"

A '`{`' from the command-line does not invoke a new scope -- it just flips you
to wherever you last left off, like dipping into a parallel stream of
consciousness.  Once you're inside code-mode, only then will '`{`' invoke a new
scope, from which variables will "fall out" and expire when the block ends.

        {
            var yum = "melon"
            {
                var yuck = "grapefruit"
            }
            # yuck fell out of scope, and is gone forever.
            : echo { yum }          # prints "melon"
            : echo { yuck }         # error
        }
        echo {yum}                # prints "melon"
        { yum ~= " for lunch" }
        echo {yum}                # prints "melon for lunch"

There are two ways to swap to code from the command-line:

1. '`{`' is at the beginning of the line  (ignoring whitespace)

        lush: { var x = "foo" }

  In this form, the `{...}` denotes a block containing a list of statements to
  execute.  It does not return a value.

2. '`{`' is part of the arguments to a program:

        lush: echo { x }

  In this form, the `{...}` evalutes a single expression.  Its final value is
  converted to a string, shell-escaped, and then placed in your command-line.

        echo { 7 * 3 + 2 }                  # runs:  echo '23'
        { var x = "; '{:rm -rf *; !!}'" }   # this looks evil
        echo { x }                          # totally safe
        # It ran:   echo '; \'{:rm -rf *; !!}\'" }'
        # which is properly escaped to have no effect.

  List types are exceptional, in that each element of the list will be converted
  to str and shell-escaped into its own element of the command-line (a separate
  program argument):

        { var files = ["games" "music" "videos"] }
        cp {files} /media/usb_key/
        # runs:  cp 'games' 'music' 'videos' /media/usb_key/

Invoking commands from code
===========================

There are three ways to invoke commandlines from code:

1. Single command-lines with '`:`'

        {
            :echo "hello, world!"
        }

2. Multi-line command blocks with '`{:: /*commands*/ }`'

        {
            var pattern = "kitten"
            {::
                cd /media/usb_key
                grep {pattern} *
            }
        }

3. Drop to an interactive command shell with '`::user::`'

        {
            var numFiles = (/media/usb_key).count
            if numFiles == 0 {
                # someone took my files!  let's debug this
                var status = "code red"
                ::user::      # this instance of 'lush' is not a subshell,
                              # and has access to in-scope vars like "status"
                # when the shell instance ends, code flow resumes here
                print("done")
            }
        }

