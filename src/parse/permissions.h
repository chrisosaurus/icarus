#ifndef ICARUS_PERMISSIONS_H
#define ICARUS_PERMISSIONS_H

#include "../lex/data/token.h"

#define IC_UNIQUE 0x08 /* 1000 */
#define IC_STORE  0x04 /* 0100 */
#define IC_WRITE  0x02 /* 0010 */
#define IC_READ   0x01 /* 0001 */

/* value is our default permission */
#define IC_VALUE (1 + IC_ASTERISK)

/* map of 'token' to permission */
int ic_parse_perm_map[] = {
    /*               UNIQUE      READ      WRITE      STORE    */
    [IC_VALUE]     = IC_UNIQUE | IC_READ | 0        | IC_STORE  ,
    [IC_DOLLAR]    = 0         | IC_READ | 0        | 0         ,
    [IC_PERCENT]   = 0         | IC_READ | 0        | IC_STORE  ,
    [IC_AMPERSAND] = 0         | IC_READ | IC_WRITE | 0         ,
    [IC_AT]        = 0         | IC_READ | IC_WRITE | IC_STORE  ,
    [IC_ASTERISK]  = 0         | 0       | 0        | 0         ,
};

#endif
