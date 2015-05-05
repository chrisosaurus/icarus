Plans
=====

* arguments are passed as immutable reference by default, have to explicitly specify mutable reference - see `docs/reference.md`
* explicit function interface contracts around mutability of arguments - `example/mutability.ic`
* mutable values
* strong statically checked type system
* no 'holes' in the type system (null and void pointer type are all banished) - see `docs/noholes.md`
* no undefined behavior
* multiple dispatch (dispatch to functions is based on function name and type of arguments passed)
* operator overloading
* per-task managed memory (automatic garbage collection)
* concurrent message passing
* parametrized types (derived from C++, Julia, and Rust)
* union types (tagged enums, statically checked before being taken apart)
    * like Rust's enums
    * like haskell's ADTs (Algebraic Data Types)
* including both an interpreter and compiler(s) (currently considering both C and LLVM bitcode as compiler backend outputs) - see `docs/components.md`
* whitespace insensitive (except using \n to terminate a comment, but comments are stripped by the lexer for now)
* no semicolons
* type safety - no casting; explicit conversion functions between builtin types will be provided but only when they make sense, composite types can then build on these

