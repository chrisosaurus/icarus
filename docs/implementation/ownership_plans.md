Ownership plans
===============
Plan: not yet implemented.

The Icarus implementation initially tried to capture ownership by using struct embedding to indicate ownership, and pointers to indicate reference.
This has become insufficient as Icarus has grown, as in many cases a pointer is more convenient (say when storing in a list or a map),
and ownership transfer is sometimes necessary.

To fix this the plan is to decouple all ownership semantics from struct vs pointer embedding, instead struct vs pointer is now purely
an optimisation concern.

There are two obvious options for the future direction.


Option 1: Ownership pointer
---------------------------

One option is for all shareable data types to have a `void *owner` field on them which is a pointer to the owner of this structure,
all deallocators on these types will take a `void *called_from` parameter which can be compared to this owner field to decide if deallocation is necessary or not.

This means it is safe for *all* holders of pointers to call the deallocators.
This will also support transfer of ownership by changing this owner pointer.

This owner field can be set to NULL to indicate that there isn't yet a data type expressing an ownership relation,
this can be useful for top level objects or local temporary objects (which will either then be deallocated by the creating function or have ownership transferred to something).


Option 2: Regions and copying
-----------------------------

Another option is to have every object allocated into a region based on the phase it was allocated in,
at the boundaries between two phases an explicit copy will occur for all the objects still needed,
then the 'from' region will be deallocated as one.

See:

 * [Samba talloc](https://talloc.samba.org/talloc/doc/html/index.html)
 * [mesa ralloc](https://github.com/anholt/mesa/blob/master/src/util/ralloc.h)


