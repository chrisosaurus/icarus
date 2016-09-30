#include <stdio.h>  /* puts */
#include <stdlib.h> /* calloc */

#include "slot.h"

/* ignored unused parameters until implementation is complete */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* allocate and init a new slot
 *
 * returns 1 on success
 * returns 0 on failure
 */
struct ic_slot *ic_slot_new(struct ic_symbol *name, struct ic_type *type, unsigned int permissions, bool reference, enum ic_slot_type slot_type, void *source) {
    struct ic_slot *slot = 0;

    slot = calloc(1, sizeof(struct ic_slot));
    if (!slot) {
        puts("ic_slot_new: call to calloc failed");
        return 0;
    }

    if (!ic_slot_init(slot, name, type, permissions, reference, slot_type, source)) {
        puts("ic_slot_new: call to ic_slot_init failed");
        free(slot);
        return 0;
    }

    return slot;
}

/* init an existing slot
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_slot_init(struct ic_slot *slot, struct ic_symbol *name, struct ic_type *type, unsigned int permissions, bool reference, enum ic_slot_type slot_type, void *source) {
    if (!slot) {
        puts("ic_slot_init: slot was null");
        return 0;
    }

    if (!name) {
        puts("ic_slot_init: name was null");
        return 0;
    }

    if (!type) {
        puts("ic_slot_init: type was null");
        return 0;
    }

    if (!source) {
        puts("ic_slot_init: source was null");
        return 0;
    }

    slot->name = name;
    slot->type = type;
    slot->permissions = permissions;
    slot->reference = reference;

    slot->tag = slot_type;
    switch (slot_type) {
        case ic_slot_type_let:
            slot->source.let = source;
            break;

        case ic_slot_type_arg:
            slot->source.arg = source;
            break;

        case ic_slot_type_field:
            puts("ic_slot_init: ic_slot_type_field unsupported");
            return 0;
            break;

        case ic_slot_type_value:
            puts("ic_slot_init: ic_slot_type_value unsupported");
            return 0;
            break;

        default:
            puts("ic_slot_init: unknown ic_slot_type_field");
            return 0;
            break;
    }

    return 1;
}

/* destroy slot
 * will only free slot if `free_slot` is true
 *
 * this will not free any of the members stored on this slot
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_slot_destroy(struct ic_slot *slot, unsigned int free_slot) {
    if (!slot) {
        puts("ic_slot_destroy: slot was null");
        return 0;
    }

    slot->name = 0;
    slot->type = 0;

    if (free_slot) {
        free(slot);
    }

    return 1;
}

/* attempt to mark this slot as being the target of assignment
 *
 * returns ic_slot_assign result to indicate result
 */
enum ic_slot_assign_result ic_slot_assign(struct ic_slot *slot) {
    if (!slot) {
        puts("ic_slot_assign: slot was null");
        return ic_slot_assign_result_internal_error;
    }

    switch (slot->tag) {
        case ic_slot_type_let:
            if (!slot->source.let) {
                puts("ic_slot_assign: slot->source.let was null");
                return ic_slot_assign_result_internal_error;
            }

            /* FIXME TODO later on check for permission */
            slot->source.let->assigned_to = 1;

            return ic_slot_assign_result_success;
            break;

        case ic_slot_type_arg:
            if (!slot->source.arg) {
                puts("ic_slot_assign: slot->source.arg was null");
                return ic_slot_assign_result_internal_error;
            }

            /* FIXME TODO later on check for permission */
            slot->source.arg->assigned_to = 1;

            return ic_slot_assign_result_success;
            break;

        case ic_slot_type_field:
            puts("ic_slot_assign: ic_slot_type_field not yet supported");
            return ic_slot_assign_result_internal_error;
            break;

        case ic_slot_type_value:
            puts("ic_slot_assign: ic_slot_type_value not yet supported");
            return ic_slot_assign_result_internal_error;
            break;

        default:
            puts("ic_slot_assign: unknown ic_slot_type");
            return ic_slot_assign_result_internal_error;
            break;
    }
}
