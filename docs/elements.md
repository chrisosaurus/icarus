This document briefly discusses the elements that make up an Icarus program


**Type declaration**
A type declaration introduces a type into scope

see `docs/types.md` for a quick into the various types of types


**Function declaration**
A function declaration consists of a name,
a list of arguments and their types, and a body

    function name(args...)
        body
    end

Args... is a list of expressions

A body is a collection of statements

Some examples of statements:

    let identifier::Type = expr

    identifier = expr

    if expr
        body
    end

    if expr
        body
    else
        body
    end

    expr

Some examples of expressions:

    function_call(args...)
    expr == expr
    expr + expr
    (expr)
    identifier
    &identifier
    ifentifier.field
    &ifentifier.field
    literals ("string literal", 0, 1, 132)


