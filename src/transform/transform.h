#ifndef IC_TRANSFORM_H
#define IC_TRANSFORM_H

/* perform translation to TIR from kludge
 *
 * modifies kludge in place
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform(struct ic_kludge *kludge);

/* print out all transformed items within kludge
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_print(struct ic_kludge *kludge);

#endif /* ifndef IC_TRANSFORM_H */
