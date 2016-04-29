Name Mangling
=============

Justification
-------------
Name mangling is important for multiple reasons

One obvious reason is to allow function overloading when the destination language
(e.g. C) doesn't support it

  fn print(s::String)
  fn print(i::Int)

so therefore the mangled name must include information about the argument types


It is also important that user-defined symbols in Icarus don't clash with
symbols used by the destination language, again in C

  fn main()

so therefore the mangled name must include something to set it apart


Proposed solution
-----------------

1) prefix an `i_`, i for Icarus - this prevents icarus clashing with host language


2) attach `_a`, a for Arguments

  this prevents the following from clashing

  fn bar(a::String, b::String)
  fn bar_String(a::String)


3) follow with a list of argument types, each type is prefixed by '_'

  this prevents the following from clashing

  fn bar(a::String, b::String)
  fn bar(a::StringString)


full proposal:

  i_<fname>_a[_<ArgType>*]


some examples


  fn main() => i_main_a
  fn bar(a::String, b::String) => i_bar_a_String_String
  fn bar_String(a::Sring) => i_bar_String_a_String_String
  fn bar_a_String(a::String) => i_bar_a_String_a_String_String


Problems:

the above solution is naive, but serves as a 'good enough' starting point

the following 2 names will currently clash

  fn bar(a::String, b::String) => i_bar_a_String_String
  fn bar(a::String_String) => i_bar_a_String_String

something as simple as adding a prefix to each type is insufficient.

