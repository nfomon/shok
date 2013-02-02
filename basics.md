lush: language basics
=====================

lush is a an object-oriented language with a strongly-typed prototype-based
inheritance model.

Everything is an object.  New objects are made by copying and combining
existing "parent" objects.

The user interacts with an environment of "variables".  A variable is a name
that has a type, and an object.  The type is a restriction on the set of
objects that the variable is allowed to have.  You can copy different objects
to the variable but they must always match its type.

A variable also has "attributes" that control where and how it may be accessed
(const, private, etc.)

An object, in turn, is just a list of variables, called its "members".

If an object has a member called `op()`, it's called a "function object".
`op()` is a special member that has a "return type", and a set of code blocks
with "argument lists" that can be used to call them.

An object that only has an `op()` is a "function".  These are very common, so a
special syntax exists for defining them.

New variables are defined with 'new' statements:
  new x                   # x has type 'object' and has a copy of 'object'.
  new y = animal          # y has type 'animal' and has a copy of 'animal'.
  new z = animal = duck   # z has type 'animal' and has a copy of 'duck'.

'new' defines a variable name, and if a value is provided with '=' then it sets
both the type of the variable and its initial object.  Any future assignments
will replace the variable's object but not its type.  You can use '=' twice to
set its type separately from its default value.

When you assign ('=') an object to a variable, via 'new' or otherwise:
  1. A new object is made by copying the old object's members, types, and their
current values.
  2. The object's `clone()` function is called, if it has one.
  3. If we're in a 'new' statement: the variable's type is set to the name/type
of the new object.
  4. Otherwise, the variable already has an object.  Its `del()` function is
called if it has one.
  5. The variable is given the new object.

