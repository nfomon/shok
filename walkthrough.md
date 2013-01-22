lush: language walkthrough
==========================

lush is a an object-oriented language with a strongly-typed prototype-based
inheritance model.  Everything is an object.  An object's name is just the name
of a variable to which it's assigned.  An object knows its prototype (a
permanent set of members: variables and functions, and their types), as well as
the current values of its members which can change over time.  You make new
objects by copying or combining existing object prototypes, adding new
functionality and putting deeper restrictions on its members.

Comments
--------
        # this is a single-line comment

        /* this is a
            /* multi-line comment */
           that allows /*nesting*/
        */

Help operator: `?`
------------------
        ? date    # describes an object
        ? :grep   # man/info/--help integration
        ? ->      # describes language symbols and operators

Variables
---------
        var color = "orange"                  # Declare variable
        var color = "green"                   # Error! 'color' already defined.
        revar color = 3                       # Re-declare (change type)
        del color                             # Delete variable
        if isvar color { print("Color!") }    # Check if variable exists

Each variable knows its `type` (set of all possible values it could have) as
well as its current value (a specific object in memory).  The `var` and `revar`
statements used the return type of the right-hand side of the `=` to determine
the type of the variable.  It's harder to see with literals like `"orange"`,
but it's equivalent to `str.new("orange")` which has return type `str`.  Thus
`color` has type `str` and current value `"orange"`.

If we want to be more specific about the type, we can either split the statements:

        var color = str?($length==6)    # a type restriction; more on this later
        color = "orange"
        color = "blue"                  # Error; does not match $length==6

Or we could use the comma form of `var` to set a default value:

        var color = str?($length==6), "orange"

You can't just use comma to bunch `var` statements together:

        var x = str, y = int, z = "blue"      # Error

Instead you have to use multiple `var` statements with '`;`'.  Sorry.

        var x = str; var y = int; var z = "blue"

Expressions
-----------
        var wantPapaya = (isTasty and isJuicy) or (amIStarving() and !isRotten)
        var x = (14 + 12) * 3 / (9 `mod` 4)

`` ` ``backticks`` ` `` let you make your own infix operators.

int defines `` `mod` `` because `%` is already taken, as we shall see.

Operators and precedence
------------------------

Standard types
--------------
lush includes these standard types:

        anything    Parent of both 'object' and '@'.  Rarely used directly.
        @           Function
        object      Parent of all objects; automatic default.
        null        Stubborn object that refuses to do anything.
        bool        = true | false
        str         Text string
        num         Abstract parent of numeric types: int, fixed, float
        int         Any integer (unbounded).  Use int32 or int64 if you need to.
        fixed       Fixed-precision decimal-point number.  Literals like 014.3600
        float       IEEE 754-2008 binar64 floating-point number
        <"hi", 12>  Tuple: Collection with fixed length and types
        [4, 1, 17]  List: Ordered collection of any length and one specific type
                    (though the type could be "anything")
        table       Data table with typed columns
        path        Filesystem path, with children: file, dir, link, pipe, bdev, cdev
        %           Process,  e.g. %123 refers to process #123
        cmd         A command invocation; its I/O streams, filehandles, exit code...
                    Returned by command-line invocations:  var x = {: wget www.com}

Good programs will use lush's object-composition system to hand-craft subsets
and combinations of these to describe the exact domain required for any
particular task.

String concatenation
--------------------
`~` is string concatenation

`~~` is sugar for `~' '~`  i.e. "string concat with a space in-between"

        var h = "hello"
        var w = "world"
        print(h ~ w)      # helloworld
        print(h ~~ w)     # hello world

These are the only operators that do an implicit cast:  `->str`

Every object has a `->str` cast.

        print(4 ~~ "is four, and" ~~ someFunction ~~ "is a function")

`format()`: another way to mix variables into strings

        print("one {1} two {2} three {3}".format("cat", 42, someFunction))

Regular expressions
-------------------

Numeric types
-------------
No implicit casts

        fixed x = 1.0 + 5         # error
        fixed x = 1.0 + 5->fixed  # ok

Tuples
------

Lists
-----

Tables
------
Consider: a dictionary is a table with 2 columns, the first of which is
indexed.  Our `table` standard object helps you make tables with typed columns,
and you can declare which ones should be indexed so that you can use them as
keys with which to select rows.

A multi-set is a table with one column that is set 'sorted'.
A set is a table with a single column that is set 'sorted' and 'unique'.
A map is a table with two columns, the first of which is sorted and unique.
A multi-map is a table with two columns, the first of which is sorted.

Paths
-----
The filesystem path is a native object in lush, and the root of an object tree:

        path
        file, dir, link, pipe, bdev, cdev

paths can be constructed from strings, in which case they'll interpret the
'`/`' character to mean a platform-independent path separator.

        var bindir = path.new('/usr/bin/')

In the statement above, the path has not been evaluated, so it needn't exist on
the target system for the variable "bindir" to get assigned.

The previous statement is equivalent to:

        var bindir = /usr/bin/

In this case, we used a "path literal" -- a string directly in the sourcecode
that gets parsed to be a path.  A path literal must either:

                             example           description
        start with a /       /vmlinuz          absolute path
        end in a /           Desktop/          directory relative to working dir
        start with a ./      ./recipe.txt      relative to working dir
        start with a ../     ../recipe.txt     relative to parent dir
        start with a ~/      ~/music/omg.mp3   relative to home dir

A `path` object gives you easy access to all its file attributes and helps you
build command invocations or manipulate the filesystem.

Processes
---------
`%123` is a literal that means "process #123", if it exists.

        print(%123.runtime)     # how long has this process been running?
        %123.signal($SIGKILL)   # kill it!

We just used the  `$` scope operator.  It let us conveniently refer to
`'SIGKILL'` which is a symbol defined in the `process.signal()` function
itself.  What we wrote was equivalent to:

        %123.signal(%123.signal.SIGKILL)

Control flow
------------
`if`, `elif`, `else`

        if x > 500 {
            print("big")
        } elif x > 50 {
            print("medium")
        } else {
            print("small")
        }

One-line `if`.  The `','` is only required for `if` and `elif`.

        if x > 500, print("big")
        elif x > 50, print("medium")
        else print("small")

`switch` statement.  The object must have type 'comparable' which gives it `==`

        switch snack {
            case "celery" {
                print("veggie")
            }
            case "cookie" | "pie" {
                print("dessert")
            }
            # "case is" lets you select based on type rather than operator==
            # This is useful for specific types of fall-through
            case is food {
                print("Well, at least it's some kind of food")
            }
            case else {
                print("Don't put that in your mouth!")
            }
        }

Alternatively you can provide your own comparison function

        switch food, food.foodGroupEquals {
            # ...
        }

Easy loops

        var x = int
        loop {
            x += 1
            if x > 7, break
        }

        repeat x times {
            print("Woo!")
        }

        while x < 7 {
            x += 1
        }

`each` loops iterate over a list or tuple

        each x in [5, 10, 15] {
            print(x)
        }

        # Type filters: you only get objects that match the type:
        each int x in [5, "hello", -12] {
            print("Got int:" ~~ x)          # will only get 5 and -12
        }

        # The where clause lets you be very picky:
        each int x in [5, "hello", -12] where x > 0 {
            print("I could have just used a better type filter")
            print(whatis x)   # int
        }

        # You could have used a fancy type filter instead:
        # Another use of $, the scope operator, here referring to the object being
        # tested after the ?
        each int?($>0) in [5, "hello", -12] {
            print("This puts the positivity constraint right in the type!")
            print(whatis x)   # int?($ > 0)
        }

        # If you don't specify a loop variable, the automagic var '_' is used
        # instead:
        each in [5, 10, 15] {
            print(typeof _)     # int
            print("My number is: ~~ _)
        }

Every object has a magic member `i` that you're not allowed to change.  It gets
set by the `each` loop, and tells you how far along you are:

        each int n in [5, 10, 15] {
            print(n.i ~ ":" ~~ n)
        }
        # Prints:
        1: 5
        2: 10
        3: 15

        each food f in ["pie", "cookie", "cake"] {
            each size s in ["small", "large"] {
                print(size.i ~'.'~ item.i ~~ ": a" ~~ s ~~ f)
            }
        }

        # Prints:
        1.1: a small pie
        1.2: a large pie
        2.1: a small cookie
        # and so on.


Other control-flow statements include `continue`, `break`, and `return`.
Blocks can be given labels, which the `continue` and `break` can refer to.

        *first* each x in [1 2 3] {
            *second* each y in [4, 6, 8] {
                if x*y >= 12, continue *first*
                print("y:"~~y)
            }
            print("x:"~~x)
        }

Modules
-------
A module is just a script that defines some objects.

Importing a module (or specific members of a module) runs the code in a
sandbox, then brings the symbols into your local namespace, while giving
everything the attribute "const".  Import fails if there are any name
collisions for objects that aren't identical.

        import my_date_lib.date     # import a single object from a module
        import my_date_lib          # ok, just grabs the rest

Using objects
-------------
Make a copy of `date`:

        var today = date

Note that we didn't use `()`'s; no code or constructor was called.  This
'today' is just a prototype, a child object of `'date'` that didn't add any new
functionality.  Not really useful, yet.  You can ask 'today' meta-information
about its members and their types, but you didn't call `new()`, so you can't
really use it.

        print(typeof date)      # object
        print(attribs date)     # const proto     # const because it was imported,
        print(typeof today)     # date
        print(attribs today)    # proto
        today.addDays(1)        # error: 'adate' is just a proto.

What we did above was give the variable `'today'` its type -- it's allowed to
be any date.  Every object has a `'new()'` that returns a newly-constructed
object of its specific type.

        today = date.new(2014, 1, 18)
        print(typeof today)     # date
        print(attribs today)    # var   (just a variable)
        var tomorrow = today.addDays(1)
        print(typeof tomorrow)  # date  (the return type of date.addDays(int))

The `new` call is important because it assigns initial values to the object's
members, rather than just set their types.

Functions
---------
        # No args, no return type
        var func1 = @() {
            print("hello")
        }
        func1()     # prints "hello"

        # No args, returns an int
        var func2 = @()->int {
            return 14 * 3
        }

        # takes a str and an int, and returns a 'fixed' (a decimal-point number)
        var func3 = @(str s, int n)->fixed {
            print("s:" ~~ s ~~ "and n:" ~~ n)
            return n->fixed + 0.123
        }
        var x = func3("hello", 5)   # s: hello and n: 5
        print(x)                    # 5.123

When you assign a function to a variable, you actually have a size-1 set of
functions that take different sets of arguments, but must all return the same
type.  You can append more functions into the set:

        func3 = func3 & @(str s, fixed n)->fixed {
            print("Now you've given me a fixed-pt number!");
            return func3(s, n->floor->int)      # can call its other form
        }

        func3 &= @(str s, num n, str msg) {
            print(msg);
            return func3(s, n)
        }

Functions can call themselves (recursion) and expect the tail-call
optimization.

If the type of an argument is not specified, it is assumed to be '`object`':

        var f1 = @(int n, thing) {
            print(typeof thing)     # object
            print(whatis thing)     # who knows!? depends on the caller!
            print("What is this thing:" ~~ thing)   # object still has ->str  :)

            print(typeof args)      # [|n=int|, |
        }

Inside a function, the special variable 'args' is set to a tuple or list of the
arguments that were passed in.  An ellipsis (`...`) can be used to allow a
function to accept any number of arguments, and it may be attached to a type
(default: object)

        var f2 = @(int, ...) {          # Takes an int and any number of objects
            print(typeof args)            # [int, object...]    # lists can use ... too
        }
        var f3 = @(int, str...) {}      # Takes an int and any number of strings

You can use the `$` scope operator to set values for named parameters:

        func3("ok", $msg="muffins", $n=42)

Specify optional arguments with a prefix `?`:

        func4 = @(str s, num n, ?str msg, ?str another)

Defining new objects
--------------------
        var date = {
            # Define some members, and their types and default values
            var year = 1970     # type 'int', current value 'int.new(1970)'
            var month = 1
            var day = 1

            # A function with no return value
            var print = @() {
                stdout.print(self->str)    # just use our ->str cast
            }

            # We already have a self->str (inherited from 'object'), so we don't
            # declare with 'var'.  We'll just replace the parent version with '='.
            self->str = @() {
                # yyyy-MM-dd
                return year->str.pad('0',4) ~'-'~       # beware of bats!  ~'-'~
                       month->str.pad('0',2) ~'-'~
                       day->str.pad('0',2)
            }
            # Append another ->str cast function.  This one takes an argument.
            # format is passed in by-value (copy-on-write) so it's ok to modify.
            self->str &= @(str format) {
                format.replace('%YEAR%', year)
                format.replace('%MONTH%', month)
                format.replace('%DAY%', day)
                return format
            }

            # "new" is the special constructor function.  Some magic happens
            # behind-the-scenes to make new memory-space for the new object, which also
            # gets automatically returned at the end of the function.
            # Here we're using '=' to replace any inherited forms.  So the default
            # new() that takes no arguments, will not be permitted.
            new = @(int year, int month, int day)->self {
                self.year = year; self.month = month; self.day = day
            }
        }

Object inheritance and composition
----------------------------------
Assigning a block (`{...}`) implicitly means it inherits from `'object'`

        var date = {          # equivalent:  var date = object & { ... }
            var year = int
            var month = int
            var day = int
        }

`&` is the "append code" operator.  We extend `date` by making a child that
appends some more code:

        var datetime = date & {
            var hour = int
            var minute = int
            var second = int
        }

We can just merge objects together:

        var cool = {
            var wow = @() { print("Woooo!") }
        }
        var cooldate = date & cool

If any members collide (have the same name but different types or values), then
the child can't access them directly.  It must cast itself to a specific parent
in order to access that parent's version of it.  An inherited method would only
access the version of the variable appropriate for the method.

We can use `|` to say an object could be one thing or another.

    var nullableInt = int|null

    var something = cool|date|datetime
    print(typeof something)     # cool|date|datetime
    print(whatis something)     # cool|date|datetime
    something = datetime.new(2012, 01, 20)
    print(typeof something)     # cool|date|datetime
    print(whatis something)     # datetime

`typeof` tells you the type, which is always the full range of possible values.

`whatis` tells you the most specific type that the object is known to be.

We use `is` to discover what functionality we're allowed to access

    var foo = @(cool|date|datetime thing) {
        if thing is cool {
            thing.wow()
        } else {
            print(whatis thing)     # date|datetime
            print(thing.month)      # date and datetime can both do this
            print(thing.minute)     # error!  don't know it's a datetime
        }
    }

    # x is either a positive int, or a null
    var x = positive & int | null

`?` lets you quickly declare an object with some restrictions on its members

    var positive = int?($ > 0)
    var dateIn2013 = date?($year==2013)
    var dateInQ4of2013 = date?($year==2013 and $month>=10)

Scope and self
--------------
`self` is an object member that always automatically refers to the object itself.

If you're defining a method (member function of an object), then `self` is
still in scope from the owning object.

Alternatively, `$` is the "scope operator".  It has type `anything` meaning it
can be either `object` or `@`.  On its own it's a reference to the current
scope in which you're operating:

    var x = "outside"
    {
        var w = 42
        print(w)        # as a local variable
        print($.w)      # as a member of $
        print($w)       # you don't need to write the '.'
        print($x)       # Error: does not exist
    }

In a type restriction (`?`), within the ()'s, the $ refers to the object in
question:

    var x = int?($ > 12)
    var y = str?($length > 4 and $length < 12)

`$` lets you use a "filesystem-like" syntax to refer to variables that were
defined in a parent scope:

    var x = "toast"
    {
        var x = "soup"
        var y = "salad"
        if y.length > 2 {
            var x = "burrito"
            print($x)           # burrito
            print($.x)          # burrito
            print($./x)         # burrito
            print($../x)        # soup
            print($../y)        # salad
            print($../../x)     # toast
            print($/x)          # toast  (via "absolute variable path")
        }
    }

The "variable path hierarchy" root is at the nearest object definition, or the
base command-line scope that started this code block.  Its use is mainly for
interactive discovery.  Within a function, the $/ and $../ forms can only be
used with member variables that are explicitly closed over (see the next
section).

Closure
-------
lush allows explicit closure over variables from a parent scope, by value but
not by reference.  This is really just a way to parameterize a function with
data present at the time of definition, as opposed to arguments that are passed
in when the function is called.

Here is our version of wikipedia's Closure (Programming) first Python3 example:

    var counter = @()->@() {
        var x = 0
        var increment = @(int) with(x) {     # explicitly closes over x
            x += y
            print(x)
        }
        return increment
    }
    counter1_increment = counter()
    counter2_increment = counter()
    counter1_increment(1)   # prints 1
    counter1_increment(7)   # prints 8
    counter2_increment(1)   # prints 1    # counter2 has its own copy of x
    counter1_increment(1)   # prints 9

Since interactively it's easy to forget (or not know) upfront what parent-scope
variables we might need, the `with()` clause can go at the end of the function:

    var increment = @(int) {
        x += y; print(x)
    } with(x)

In this form, the `with()` must be on the same line as the `}`.

If a function is a method (member of an object), it should not declare
`with(self)`.  The method can only be run in the context of an owning object,
so that `self` and its members are implied.  (I'm not sure if this is true /
actually works....)

Exceptions
----------

Object attributes and permissions
---------------------------------
proto, etc.

const, etc.

Full grammar
------------

