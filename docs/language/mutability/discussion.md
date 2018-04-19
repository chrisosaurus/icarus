mutability goals
================

Mutability is allowed, however:

* mutability is the special case
* immutability is the default case
* mutation must be clearly visible
* caller and callee must agree on a 'mutability contract'


consideration:

* storability
* storage contract

e.g.

    type Node
        &i::Signed
    end

    let &mine = 5
    let &n = new_node(&mine)

    foo(&n)

here the value of `mine` may mutate after the call to `new_node`;
if `new_node` stores the value of `&mine` on the `&n`,
then the call `foo(&n)` could mutate `&mine` is a less-than-obvious way


