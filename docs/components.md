Icarus Components
=================


                    Read
                      |
                     Lex
                      |
                    Parse
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

Read : filename -> source []

Lex : source[] -> tokens[]

Parse : tokens[] -> AST

Middleware
==========
The role of the middleware is to perform any verification and modifications to this AST

Verify : AST -> errors[]

Backends
========
The backend then takes this AST and does *something* with it

Icarus then plans to support multiple backends:

* Interpreter : AST -> side effects
* Icarus -> C compiler : AST -> C source code
* Icarus -> LLVM compiler : AST -> LLVM bitcode

