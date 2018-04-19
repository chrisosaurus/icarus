operators
=========

defintion
---------

In icarus all operators map to a function,
currently only binary operators are supported

    + add()
    - minus()
    * times()
    / divide()
    % modulo()
    == equal()
    and and
    or or


eventually unary operators will be supported

    ! not

this mapping is managed in `src/stdlib/core.ic`


Any type can implement the operators for any type,
if you wanted you could define

    fn add(b::Bool, i::Signed) -> SomeType ... end

and then you may use

    True + 1


precedence
----------


icarus allows mixing of operators to create arbitrary expressions

    let x = 1 + 2 + 3 + 4 + 5
    let y = x * 7 * 8
    if 1 == 2 ... end

however you cannot mix different operators in the same expression

    # ILLEGAL
    let z = 1 + 2 * 3

instead you must specify precedence

    let z = 1 + (2 * 3)

this is likewise required for complex if expressions

    if ((a == b) and (c == d)) or (e == f)
        ...
    end


