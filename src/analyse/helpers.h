#ifndef ICARUS_ANALYSE_HELPERS_H
#define ICARUS_ANALYSE_HELPERS_H

#include "../data/pvector.h"
#include "data/kludge.h"

/* iterate through the field list checking:
 *  a) all field's names are unique within this list
 *  b) all field's types exist in this kludge
 *  c) (if provided) that no field's types eq forbidden_type
 *
 * `unit` and `unit_name` are used for error printing
 * it is always printed as '%s for %s error goes here'
 * e.g. unit of 'function declaration', name of 'Foo'
 *
 * returns 0 on success (all fields are valid as per the 3 rules)
 * returns 1 on failure
 */
unsigned int ic_analyse_field_list(char *unit, char *unit_name, struct ic_kludge *kludge, struct ic_pvector *fields, char *forbidden_type);

/* perform analysis on body
 * this will iterate through each statement and perform analysis
 *
 * `unit` and `unit_name` are used for error printing
 * it is always printed as '%s for %s error goes here'
 * e.g. unit of 'function declaration', name of 'Foo'
 * returns 0 on success (pass)
 * returns 1 on failure
 */
unsigned int ic_analyse_body(char *unit, char *unit_name, struct ic_kludge *kludge, struct ic_body *body);

#endif // ifndef ICARUS_ANALYSE_HELPERS_H

