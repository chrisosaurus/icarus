#ifndef ICARUS_SLOT_H
#define ICARUS_SLOT_H

#include <stdbool.h>

#include "../../data/symbol.h"
#include "../../parse/data/stmt.h"
#include "type.h"

enum ic_slot_type {
    /* this slot maps to a let */
    ic_slot_type_let,
    /* this slot maps to an arg */
    ic_slot_type_arg,
    /* this slot maps to the field on a struct - unsupported*/
    ic_slot_type_field,
    /* this slot maps to the value within a map/array - unsupported*/
    ic_slot_type_value
};

/* a slot represents the mapping from a name to one of:
 *  - an argument
 *  - a local variable (via let assignment)
 *  - a field on a struct (unsupported)
 *  - the value within an array/map (unsupported)
 * during the semantic analyse phase
 *
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

    /* type of this slot */
    enum ic_slot_type tag;
    /* original item that causes this slot to be created */
    union {
        struct ic_field *arg;
        struct ic_stmt_let *let;
    } source;
};

/* allocate and init a new slot
 *
 * returns 1 on success
 * returns 0 on failure
 */
struct ic_slot *ic_slot_new(struct ic_symbol *name, struct ic_type *type, unsigned int permissions, bool reference, enum ic_slot_type slot_type, void *source);

/* init an existing slot
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_slot_init(struct ic_slot *slot, struct ic_symbol *name, struct ic_type *type, unsigned int permissions, bool reference, enum ic_slot_type slot_type, void *source);

/* destroy slot
 * will only free slot if `free_slot` is true
 *
 * this will not free any of the members stored on this slot
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_slot_destroy(struct ic_slot *slot, unsigned int free_slot);

/* result of attempted assignment to this slot */
enum ic_slot_assign_result {
    ic_slot_assign_result_internal_error = 0,
    ic_slot_assign_result_success = 1,
    ic_slot_assign_result_permission_denied = 2
};

/* attempt to mark this slot as being the target of assignment
 *
 * returns ic_slot_assign result to indicate result
 */
enum ic_slot_assign_result ic_slot_assign(struct ic_slot *slot);

#endif
