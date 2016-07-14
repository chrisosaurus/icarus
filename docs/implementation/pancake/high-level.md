pancake layout
==============

Pancake is a bytecode stack vm

pancake will take TIR and convert to an internal bytecode

it will have

Data
====

instruction area
----------------
static fixed-sized area storing all the instructions for our program
all 'instruction/function addresses' refer to offsets within this area

heap
----
allocation area
all 'memory addresses' refer to items within this heap

return address stack
--------------------
shadow stack storing return addresses only

value stack
-----------
stack for passing arguments and returning values
all computation is done on the stack

scope system
------------
a set of dictionary mapping names to memory addresses

function name system
--------------------
a dictionary mapping function names to function addresses
this isn't strictly necessary and could be compiled out, but having
run-time information about fname->address is useful


Computation
===========

computation will progress by starting at address 0 of the instruction area,
this will correspond to our main function
it will progress reading the instructions one-by-one in order

the bytecode language supports jumps, calls, and returns
a jump will move execution to another address
a call will put the current address on the return stack, and then jump to the address
a return will pop the top return address and jump to it


