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
struct ic_slot * ic_slot_new(void){
    struct ic_slot *slot = 0;

    slot = calloc(1, sizeof(struct ic_slot));
    if( ! slot ){
        puts("ic_slot_new: call to calloc failed");
        return 0;
    }

    if( ! ic_slot_init(slot) ){
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
unsigned int ic_slot_init(struct ic_slot *slot){
    if( ! slot ){
        puts("ic_slot_init: slot was null");
        return 0;
    }

    puts("ic_slot_init: implementation pending");
    return 0;
}

/* destroy slot
 * will only free slot if `free_slot` is true
 *
 * returns 1 on success
 * returns 0 on error
 */
unsigned int ic_slot_destroy(struct ic_slot *slot, unsigned int free_slot){
    if( ! slot ){
        puts("ic_slot_destroy: slot was null");
        return 0;
    }

    puts("ic_slot_destroy: implementation pending");
    return 0;
}

