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

known areas, types, and functions not conforming:

* parse
* analyse


allowed special cases:

* ic_pvector_append will return the index of the appended item, returns -1 on error
* ic_string_length will return the length, returns -1 on error
* ic_symbol_length will return the length, returns -1 on error


remaining cases:

    chris@Ox1b src(master)-> git grep -ni '1 on error' *
    analyse/analyse.c:96: * returns 1 on error
    analyse/analyse.c:138: * returns 1 on error
    analyse/analyse.h:21: * returns 1 on error
    analyse/analyse.h:30: * returns 1 on error
    data/set.c:34: * return 1 on error
    data/set.h:24: * return 1 on error
    data/string.c:143: * returns -1 on error
    data/string.h:81: * returns -1 on error
    data/symbol.c:106: * returns -1 on error
    data/symbol.h:53: * returns -1 on error
    parse/data/decl.c:49: * returns 1 on error
    parse/data/decl.c:181: * returns 1 on error
    parse/data/decl.c:210: * returns 1 on error
    parse/data/decl.c:247: * returns 1 on error
    parse/data/decl.c:461: * returns 1 on error
    parse/data/decl.c:552: * returns 1 on error
    parse/data/decl.c:660: * returns 1 on error
    parse/data/decl.h:46: * returns 1 on error
    parse/data/decl.h:65: * returns 1 on error
    parse/data/decl.h:74: * returns 1 on error
    parse/data/decl.h:81: * returns 1 on error
    parse/data/decl.h:133: * returns 1 on error
    parse/data/decl.h:152: * returns 1 on error
    parse/data/decl.h:195: * returns 1 on error
    parse/data/expression.c:132: * returns 1 on error
    parse/data/expression.c:434: * returns 1 on error
    parse/data/expression.h:51: * returns 1 on error
    parse/data/expression.h:137: * returns 1 on error
    parse/data/field.c:35: * returns 1 on error
    parse/data/field.h:30: * returns 1 on error
    parse/data/type.c:25: * returns 1 on error
    parse/data/type.h:43: * returns 1 on error
    parse/data/type_ref.c:121: * returns 1 on error
    parse/data/type_ref.c:170: * returns 1 on error
    parse/data/type_ref.c:228: * returns 1 on error
    parse/data/type_ref.h:93: * returns 1 on error
    parse/data/type_ref.h:106: * returns 1 on error
    parse/data/type_ref.h:117: * returns 1 on error


Error checking
==============

most functions in icarus return 0 on error, this should be checked as such

    if( ! ic_foo_bar(...) ){
        puts("ic_baz: call to ic_foo_bar failed");
        return 0;
    }


some functions return a value to be assigned, the preferred form for this is

    struct ic_foo *foo = 0;

    foo = ic_foo_new();
    if( ! foo ){
        puts("ic_baz: call to ic_foo_new failed");
        return 0;
    }


there are a few functions which use 0 as a valid response and so need `-1` to indicate error, the form
for checking this is

    if( -1 == ic_foo_append(...) ){
        ...
    }

this is so that a grep for 'if( ic_foo' will not flag this line, which allows us to more easily find functions
not conforming to the first error checking rule above.


