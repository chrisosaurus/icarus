#include <stdio.h>

#include "../analyse/data/kludge.h"
#include "transform.h"

/* perform translation to TIR from kludge
 *
 * modifies kludge in place
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_transform(struct ic_kludge *kludge){
  if( ! kludge ){
    puts("ic_transform: kludge was null");
    return 0;
  }

  puts("ic_transform: transform implementation pending");
  return 1;
}

