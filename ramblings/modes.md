Invoking code from the command-line
===================================
At the command-line, '`{`' will swap you into code.

        cd /media/usb_key/
        ls
        {
            new song = "gravy"
            song = song.replace('a', 'oo')
        }
        mp3blaster { song ~ ".mp3" }    # plays "groovy.mp3"

A '`{`' from the command-line does not invoke a new scope -- it just flips you
to wherever you last left off, like a parallel world of code alongside your
command shell.  You can access whatever variables you left there.  Once you're
inside code-mode, only then will '`{`' invoke a new scope, from which variables
can "fall out" and expire when the block ends.

        {
            new yum = "melon"
            { new yuck = "grapefruit" }
            print(yuck)       # error! yuck fell out of scope, gone forever.
            print(yum)        # prints "melon"
            : mkdir {yum}     # runs command:  mkdir 'melon'
        }
        cd {yum}              # runs: cd 'melon'

A code block can be either a single expression, or any number of statements.
If it's a single expression, the result will drop onto the command-line, to be
invoked as a command:

        {   # A statement list
            new program = /usr/bin/climateModel
            new lognum = 1
        }
        {program} waterloo.dat > output{lognum}.log    # Expressions; runs the command
        { lognum += 1 }                                # A statement; no command is invoked
        {program} waterloo.dat > output{lognum}.log

        # An expression: its result becomes part of the command
        echo "The next logfile will be: " {lognum + 1}

Code can also "return" a value to the command-line:

        cp cat.jpg {if (~/pics/).isdir(), return ~/pics/; return /tmp/}

When a value comes from code, it is converted to a string and then shell-escaped.
This is for security:

        echo { 7 * 3 + 2 }                  # runs:  echo '23'
        { new x = "; '{:rm -rf *; !!}'" }   # this looks evil
        echo { x }                          # totally safe
        # It ran:   echo '; \'{:rm -rf *; !!}\'" }'
        # which is properly escaped to have no effect.

When a list type goes from code to the command-line, each element is escaped as
a separate entity so that they become different command-line arguments:

        { new files = ["games", "music", "videos"] }
        cp {files} /media/usb_key/
        # runs:  cp 'games' 'music' 'videos' /media/usb_key/

Invoking commands from code
===========================
There are three ways to invoke commandlines from code:

1. Single command-lines with '`:`'

        {
            :echo "hello, world!"
        }

2. Multi-line command blocks with '`{:: (*commands*) }`'

        {
            new pattern = "kitten"
            {::
                cd /media/usb_key
                grep {pattern} *
            }
        }

3. Drop to an interactive command shell with `shell`

        {
            new numFiles = (/media/usb_key).count
            if numFiles == 0 {
                # someone took my files!  let's debug this
                new status = "code red"
                shell       # this instance of 'lush' is not a subshell,
                            # and has access to in-scope vars like "status"
                # when the shell instance ends, code flow resumes here
                print("done")
            }
        }

