#ifndef ICARUS_SLOT_H
#define ICARUS_SLOT_H

#include <stdbool.h>

#include "../../data/symbol.h"
#include "type.h"

/* a slot represents an argument or a variable at the semantic analyse phase
 * examples:
 *
 *      fn foo(a::Int) ... end
 *
 * in the scope of foo we have a slot `a`:
 *   with type `Int`
 *   mutable 0 (as it is an argument)
 *   and reference 0
 *
 *
 *      fn bar(&b::String) ... end
 *
 * in the scope of bar we have a slot `b`
 *  with type String
 *  mutable 0 (as it is an argument)
 *  reference 1 (due to & usage)
 *
 *
 *      let &c = 5
 * in this scope we have a slot `c`
 *  with type Int
 *  mutable 1 (as it is a local variable)
 *  reference 1 (due to the & usage)
 */
struct ic_slot {
    /* the name of this variable or argument */
    struct ic_symbol *name;

    /* the type of this slot
     * this is either declared by the user
     *  let a::Int
     * or inferred via usage
     *  let a = 5
     */
    struct ic_type *type;

    unsigned int permissions;

    /* is this slot a reference?
     * was this slot declared with a &
     */
    bool reference;
};

/* allocate and init a new slot
 *
 * returns 1 on success
 * returns 0 on failure
 */
struct ic_slot *ic_slot_new(struct ic_symbol *name, struct ic_type *type, unsigned int permissions, bool reference);

/* init an existing slot
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_slot_init(struct ic_slot *slot, struct ic_symbol *name, struct ic_type *type, unsigned int permissions, bool reference);

/* destroy slot
 * will only free slot if `free_slot` is true
 *
 * this will not free any of the members stored on this slot
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_slot_destroy(struct ic_slot *slot, unsigned int free_slot);

#endif
