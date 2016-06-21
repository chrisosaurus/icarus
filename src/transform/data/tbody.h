#ifndef ICARUS_TRANSFORM_TBODY_H
#define ICARUS_TRANSFORM_TBODY_H

struct ic_transform_body {
  unsigned int literal_count;
  unsigned int temporary_count;
  unsigned int return_count;

  ??? variable_names;

  /* ordered vector of transform statements */
  struct ic_pvector *content;
};

struct ic_transform_body * ic_transform_body_new(void);
unsigned int ic_transform_body_init(struct ic_transform_body *tbody);
unsigned int ic_transform_body_destroy(struct ic_transform_body *tbody, unsigned int free_tbody);

??? ic_transform_body_register_literal(struct ic_transform_body *tbody, ???);
??? ic_transform_body_register_temporary(struct ic_transform_body *tbody, ???);
??? ic_transform_body_register_variable(struct ic_transform_body *tbody, ???);
??? ic_transform_body_register_return(struct ic_transform_body *tbody, ???);

#endif /* ifndef ICARUS_TRANSFORM_TBODY_H */
