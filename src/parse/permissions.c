#include <stdio.h>

#include "permissions.h"

#define IC_UNIQUE 0x08 /* 1000 */
#define IC_STORE  0x04 /* 0100 */
#define IC_WRITE  0x02 /* 0010 */
#define IC_READ   0x01 /* 0001 */

/* value is a permission but NOT a token
 *    * so doesn't exist in token.h
 *       */
#define IC_VALUE (1 + IC_ASTERISK)
/* our default permission is IC_VALUE */
#define IC_PERM_DEFAULT IC_VALUE

/* map of token_id to permission */
unsigned int ic_parse_perm_map[] = {
    /*               UNIQUE      READ      WRITE      STORE    */
    [IC_VALUE]     = IC_UNIQUE | IC_READ | 0        | IC_STORE  ,
    [IC_DOLLAR]    = 0         | IC_READ | 0        | 0         ,
    [IC_PERCENT]   = 0         | IC_READ | 0        | IC_STORE  ,
    [IC_AMPERSAND] = 0         | IC_READ | IC_WRITE | 0         ,
    [IC_AT]        = 0         | IC_READ | IC_WRITE | IC_STORE  ,
    [IC_ASTERISK]  = 0         | 0       | 0        | 0         ,
};

/* get permissions for this token_id */
unsigned int ic_parse_perm(enum ic_token_id id){
    if( id > IC_VALUE ){
        puts("ic_parse_perm: invalid id");
        return 0;
    }

    return ic_parse_perm_map[id];
}

/* get default permissions */
unsigned int ic_parse_perm_default(void){
    return ic_parse_perm(IC_PERM_DEFAULT);
}

/* check if the provided decay is valid
 *
 * a valid decay is one where every bit set in `to` is also
 * set in `from`
 *
 * returns 1 if this is a valid decay
 * returns 0 otherwise
 */
unsigned int ic_parse_perm_is_valid_decay(unsigned int from, unsigned int to){
    /* for a decay to be valid every bit set in `to` must also
     * be set in `from`
     */
    if( (from & to) == to ){
        return 1;
    }
    return 0;
}

/* return a string representation of this permission */
char * ic_parse_perm_str(unsigned int permissions){
    /* current index */
    unsigned int i = 0;
    /* length */
    unsigned int len = IC_VALUE + 1;

    /* our found token_id */
    enum ic_token_id id = 0;

    /* our resulting str */
    char *str = 0;

    /* to avoid hard coding the perm to str
     * we instead search first for our token
     * by using the reverse of our map
     */
    for( i=0; i<len; ++i ){
        if( permissions == ic_parse_perm_map[i] ){
            id = i;
            break;
        }
    }

    if( i == len ){
        /* failed to find */
        printf("ic_parse_perm_str: failed to find token_id for permissions '%d'\n", permissions);
        return 0;
    }

    /* otherwise we found a token id */
    switch( id ){
        case IC_VALUE:
            str = "";
            break;

        case IC_DOLLAR:
            str = "$";
            break;

        case IC_PERCENT:
            str = "%";
            break;

        case IC_AMPERSAND:
            str = "&";
            break;

        case IC_AT:
            str = "@";
            break;

        case IC_ASTERISK:
            str = "*";
            break;

        default:
            printf("ic_parse_perm_str: failed to match to expected token_id for '%d'\n", id);
            return 0;
            break;
    }

    if( ! str ){
        printf("ic_parse_perm_str: failed to match to expected token_id for '%d'\n", id);
        return 0;
    }

    return str;
}

/* check if given permissions is default
 *
 * returns 1 if is default
 * returns 0 otherwise
 */
unsigned int ic_parse_perm_is_default(unsigned int permissions){
    return permissions == ic_parse_perm_map[IC_PERM_DEFAULT];
}

/* check if the given permissions has read
 *
 * returns 1 if can read
 * returns 0 otherwise
 */
unsigned int ic_parse_perm_has_read(unsigned int permissions){
    return 0 != (permissions & IC_READ);
}

/* check if the given permissions has write
 *
 * returns 1 if can write
 * returns 0 otherwise
 */
unsigned int ic_parse_perm_has_write(unsigned int permissions){
    return 0 != (permissions & IC_WRITE);
}

/* check if the given permissions has store
 *
 * returns 1 if can store
 * returns 0 otherwise
 */
unsigned int ic_parse_perm_has_store(unsigned int permissions){
    return 0 != (permissions & IC_STORE);
}

/* check if the given permissions has unique
 *
 * returns 1 if can unique
 * returns 0 otherwise
 */
unsigned int ic_parse_perm_has_unique(unsigned int permissions){
    return 0 != (permissions & IC_UNIQUE);
}

