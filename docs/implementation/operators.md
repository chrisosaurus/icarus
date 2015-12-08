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


## field access

field access shares some similarities with a binary operator, so parsing is handled the same,
however the difference is that field access is syntactic:

    a.b

`a` will be evaluated as an expr
`b` is a literal identifier

for example

    type Foo
        i::Int
    end

    fn bar() -> Foo ... end

    bar().i


## mixed operator expressions

an expression containing the same operator is as expected

    let a = 1 * 2
    let b = 1 + 2
    let c = a / b

if you decide to mix these expressions together, the expression must be bracketed
this means that icarus does not enforce BEDMAS, instead requiring the user to specify the order

    let c = (1 * 2) / (1 + 2)


## operator parsing

operators (and field accesses) are parsed left to right

so the expression

    1 + 2 + 3 + 4 + 5

is parsed as though it were

    (((1 + 2) + 3) +4) + 5

this builds a tree structure like

               Expr:
                +
              +   5
           +    4
         +   3
       1  2

with each `+` being an operator having a `left` and `right` subexpr


More power is needed
====================

We need more power in our current operator implementation

the current implementation handles this fine

    builtin op + plus

which then maps

    1 + 1

to

    plus(1, 1)



However there are other cases we care about

  !1 # should map to not(1)

  collection[index]         # access(index)
  collection[index, index]  # slice(index, index)

