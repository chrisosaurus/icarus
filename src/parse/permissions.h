#ifndef ICARUS_PERMISSIONS_H
#define ICARUS_PERMISSIONS_H

#include "../lex/data/token.h"

/* get permissions for this token_id */
unsigned int ic_parse_perm(enum ic_token_id);

/* get default permissions */
unsigned int ic_parse_perm_default(void);

/* return a string representation of this permission */
char *ic_parse_perm_str(unsigned int permissions);

/* check if the provided downgrade is valid
 *
 * a valid downgrade is one where every bit set in `to` is also
 * set in `from`
 *
 * returns 1 if this is a valid downgrade
 * returns 0 otherwise
 */
unsigned int ic_parse_perm_is_valid_downgrade(unsigned int from, unsigned int to);

/* check if given permissions is default
 *
 * returns 1 if is default
 * returns 0 otherwise
 */
unsigned int ic_parse_perm_is_default(unsigned int permissions);

/* check if the given permissions has read
 *
 * returns 1 if can read
 * returns 0 otherwise
 */
unsigned int ic_parse_perm_has_read(unsigned int permissions);

/* check if the given permissions has write
 *
 * returns 1 if can write
 * returns 0 otherwise
 */
unsigned int ic_parse_perm_has_write(unsigned int permissions);

/* check if the given permissions has store
 *
 * returns 1 if can store
 * returns 0 otherwise
 */
unsigned int ic_parse_perm_has_store(unsigned int permissions);

/* check if the given permissions has immut
 *
 * returns 1 if can unique
 * returns 0 otherwise
 */
unsigned int ic_parse_perm_has_immut(unsigned int permissions);

#endif
