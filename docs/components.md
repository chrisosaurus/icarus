Icarus Components
=================


                    Read
                      |
                     Lex
                      |
                    Parse
                      |
                   Rewrite
                      |
                    Verify
                      |
               Select backend
                    / | \
                   /  |  \
        Interpreter   |   Compile to LLVM bitcode
                      |
                 Compile to C

Frontend
========
The job of the front-end is to build an abstract syntax tree (AST)

Read : filename -> source

Lex : source -> tokens

Parse : tokens -> AST

Middleware
==========
The role of the middleware is to perform any verification and modifications to this AST

Rewrite : AST -> AST

Verify : AST -> warnings and errors

Backends
========
The backend then takes this AST and does *something* with it

Icarus has plans to support multiple backends, some possible ones include:

* Interpreter : AST -> side effects
* Icarus -> C compiler : AST -> C source code
* Icarus -> LLVM compiler : AST -> LLVM bitcode

