#include <stdio.h>

#include "pancake.h"
#include "../../analyse/data/kludge.h"

/* pancake - stack based vm backend
 *
 * returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_backend_pancake(struct ic_kludge *kludge){
    if (!kludge) {
		puts("ic_backend_pancake: kludge was null");
		return 0;
		}

		puts("ic_backend_pancake: unimplemented");
		return 0;
}

