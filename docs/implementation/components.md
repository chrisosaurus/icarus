Icarus Components
=================


                    Read
                      |
                     Lex
                      |
                    Parse
                      |
                   Analyse
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
The role of the middleware is to perform any verification and modifications to the AST

Analyze : AST -> Kludge

Backends
========
The backend then takes this AST and does *something* with it

Icarus has plans to support multiple backends, some possible ones include:

* Interpreter : Kludge -> side effects
* Icarus -> C compiler : Kludge -> C source code
* Icarus -> LLVM compiler : Kludge -> LLVM bitcode

