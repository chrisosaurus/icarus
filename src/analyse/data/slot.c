#include <stdlib.h> /* calloc */
#include <stdio.h> /* puts */

#include "slot.h"

/* ignored unused parameters until implementation is complete */
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* allocate and init a new slot
 *
 * returns 1 on success
 * returns 0 on error
 */
struct ic_slot * ic_slot_new(struct ic_symbol *name, struct ic_type_ref *type, bool mutable, bool reference){
    struct ic_slot *slot = 0;

    slot = calloc(1, sizeof(struct ic_slot));
    if( ! slot ){
        puts("ic_slot_new: call to calloc failed");
        return 0;
    }

    if( ! ic_slot_init(slot, name, type, mutable, reference) ){
        puts("ic_slot_new: call to ic_slot_init failed");
        return 0;
    }

    return slot;
}

/* init an existing slot
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ic_slot_init(struct ic_slot *slot, struct ic_symbol *name, struct ic_type_ref *type, bool mutable, bool reference){
    if( ! slot ){
        puts("ic_slot_init: slot was null");
        return 0;
    }

    if( ! name ){
        puts("ic_slot_init: name was null");
        return 0;
    }

    if( ! type ){
        puts("ic_slot_init: type was null");
        return 0;
    }

    slot->name = name;
    slot->type = type;
    slot->mutable = mutable;
    slot->reference = reference;

    return 1;
}

/* destroy slot
 * will only free slot if `free_slot` is true
 *
 * this will not free any of the members stored on this slot
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ic_slot_destroy(struct ic_slot *slot, unsigned int free_slot){
    if( ! slot ){
        puts("ic_slot_destroy: slot was null");
        return 0;
    }

    slot->name = 0;
    slot->type = 0;

    if( free_slot ){
        free(slot);
    }

    return 1;
}

