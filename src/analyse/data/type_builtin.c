#include <stdio.h> /* puts */
#include <stdlib.h> /* calloc, free */

#include "type_builtin.h"

/* allocate and initialise a new builtin
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_builtin_new(struct ic_symbol *name){
    struct ic_type_builtin *builtin = 0;

    if( ! name ){
        puts("ic_type_builtin_new: name was null");
        return 0;
    }

    builtin = calloc(1, sizeof(struct ic_type_builtin));
    if( ! builtin ){
        puts("ic_type_builtin_new: call to calloc failed");
        return 0;
    }

    if( ! ic_type_builtin_init(builtin, name) ){
        puts("ic_type_builtin_new: call to ic_type_builtin_init failed");
        return 0;
    }

    return 1;
}

/* initialise an existing builtin
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_builtin_init(struct ic_type_builtin *builtin, struct ic_symbol *name){
    if( ! builtin ){
        puts("ic_type_builtin_init: builtin was null");
        return 0;
    }

    if( ! name ){
        puts("ic_type_builtin_init: name was null");
        return 0;
    }

    /* FIXME leaking builtin->name */

    builtin->name = name;

    return 1;
}

/* destroy builtin
 *
 * will only free builtin is free_builtin is truthy
 *
 * FIXME does not free name, need to decide on ownership
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_type_builtin_destroy(struct ic_type_builtin *builtin, unsigned int free_builtin){
    if( ! builtin ){
        puts("ic_type_builtin_destroy: builtin was null");
        return 0;
    }

    /* FIXME leaking builtin->name */

    if( free_builtin ){
        free(builtin);
    }

    return 1;
}


