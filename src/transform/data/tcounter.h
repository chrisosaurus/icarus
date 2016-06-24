#ifndef ICARUS_TRANSFORM_TCOUNTER_H
#define ICARUS_TRANSFORM_TCOUNTER_H

/* counter to keep track of literal and temporary names
 * within a function
 *
 * _l<N>
 * _t<N>
 *
 * where Ns are unique within a function
 */
struct ic_transform_counter {
  unsigned int literal_count;
  unsigned int temporary_count;
};

/* allocate and initialise a new transform_counter
 *
 * returns * on success
 * returns 0 on failure
 */
struct ic_transform_counter * ic_transform_counter_new(void);

/* initialise an existing transform_counter
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_counter_init(struct ic_transform_counter *tcounter);

/* destroy a transform_counter
 *
 * will only free if `free_tcounter` is truthy
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform_counter_destroy(struct ic_transform_counter *tcounter, unsigned int free_tcounter);

/* get a unique literal number within this counter
 *
 * returns count > 0 on success
 * returns 0 on failure
 */
unsigned int ic_transform_counter_register_literal(struct ic_transform_counter *tcounter);

/* get a unique temporary number within this counter
 *
 * returns count > 0 on success
 * returns 0 on failure
 */
unsigned int ic_transform_counter_register_temporary(struct ic_transform_counter *tcounter);

#endif /* ifndef ICARUS_TRANSFORM_TCOUNTER_H */
