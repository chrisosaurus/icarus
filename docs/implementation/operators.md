# Operators

An operator is an infix alias for a function

Icarus currently plans to support both unary and binary operators,
some examples of operators and their matching functions

    ! True => not(Bool) -> Bool => not(True) => false

    True and False => and(Bool, Bool) -> Bool => and(True, False) => False

    1 + 2 => plus(Int, Int) -> Int => plus(1, 2) => 3

    a == b => equal<X, Y>(X, Y) -> Bool => equal(a, b)

    a != b => not(equal<X, Y>(X, Y)) -> Bool = not(equal(a, b))

the functions implemented operators are looked up through the same means as
all variable lookups (`scope`), and thus the user may define their own operators
for their own types in order to achieve operator overloading.

`kludge` will contain a table mapping `symbol *operator` to `symbol *function`
examples of entries in that table:

    + => plus
    - => minus
    ! => not
    && => and
    and => and
    == => equal

for operators that map to more than one function special thought will have to be given

    != => not(equal(...))

