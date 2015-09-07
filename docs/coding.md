Implementation coding standards guide
=====================================

Error handling:
---------------

On error all functions should print their function name and then a description of the error,
all such printouts should be traceable to a single line number, so all error strings should be globally unique.

Every function should return 0 in the case of error.

Example conforming function:

    unsigned int ic_foo_bar(struct ic_foo *foo){
        if( ! foo ){
            puts("ic_foo: called with null foo");
            return 0;
        }

        if( ... ){
            puts("ic_foo: an error description goes here");
            return 0;
        }

        return 1;
    }

known areas, types and functions not conforming:

* carray
* parray
* pvector
* lex
* read
* parse
* analyse
* symbol
* string

