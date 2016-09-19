#ifndef IC_TRANSFORM_BODY_H
#define IC_TRANSFORM_BODY_H

#include "../../data/labeller.h"
#include "../../data/pvector.h"
#include "tir.h"

struct ic_transform_body {
    struct ic_pvector tstmts;
    struct ic_labeller *labeller_tmp;
    struct ic_labeller *labeller_lit;
};

/* allocate and initialise a new tbody with no parent
 *
 * returns 1 on success
 * returns 0 on failure
 */
struct ic_transform_body *ic_transform_body_new(void);

/* allocate and initialise a new tbody with a parent
 *
 * returns 1 on success
 * returns 0 on failure
 */
struct ic_transform_body *ic_transform_body_new_child(struct ic_transform_body *parent);

/* initialise existing tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_body_init(struct ic_transform_body *tbody, struct ic_labeller *labeller_tmp, struct ic_labeller *labeller_lit);

/* destroy tbody
 *
 * will only free tbody if `free_tbody` is truthy
 *
 * will always call ic_pvector_destroy on tstmts
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_body_destroy(struct ic_transform_body *tbody, unsigned int free_tbody);

/* iterate through each tir_stmt within tbody and call print
 *
 * returns 1 on success
 * returns 0 on failures
 */
unsigned int ic_transform_body_print(FILE *fd, struct ic_transform_body *tbody, unsigned int *indent);

/* append tstmt to tbody
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_body_append(struct ic_transform_body *tbody, struct ic_transform_ir_stmt *tstmt);

/* get length of this tbody
 *
 * FIXME no real failure mode
 *
 * returns count on success
 * returns 0 on failure
 */
unsigned int ic_transform_body_length(struct ic_transform_body *tbody);

/* get tstmt at index `index`
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_ir_stmt *ic_transform_body_get(struct ic_transform_body *tbody, unsigned int index);

/* get a unique label for a temporary within this counter
 *
 * returns count > 0 on success
 * returns 0 on failure
 */
struct ic_symbol *ic_transform_body_register_temporary(struct ic_transform_body *tbody);

/* get a unique label for a literal within this counter
 *
 * returns count > 0 on success
 * returns 0 on failure
 */
struct ic_symbol *ic_transform_body_register_literal(struct ic_transform_body *tbody);

#endif /* ifndef IC_TRANSFORM_BODY_H */
