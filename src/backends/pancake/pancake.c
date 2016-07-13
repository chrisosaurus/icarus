#include <stdio.h>

#include "../../analyse/data/kludge.h"
#include "pancake.h"

/* pancake - stack based interpreter backend
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake(struct ic_kludge *kludge) {
    if (!kludge) {
        puts("ic_backend_pancake: kludge was null");
        return 0;
    }

    puts("ic_backend_pancake: unimplemented");
    return 0;
}
