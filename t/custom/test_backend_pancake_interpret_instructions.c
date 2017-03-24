#include <stdio.h>  /* puts, printf */
#include <stdlib.h> /* exit, free */
#include <string.h>

#include "../../src/backends/pancake/data/instructions.h"
#include "../../src/backends/pancake/data/runtime_data.h"
#include "../../src/backends/pancake/pancake.h"

int main(void) {
    struct ic_backend_pancake_instructions *instructions = 0;
    struct ic_backend_pancake_runtime_data *runtime_data = 0;

    instructions = ic_backend_pancake_instructions_load(stdin);
    if (!instructions) {
        puts("main: call to ic_backend_pancake_instructions_load failed");
        exit(1);
    }

    runtime_data = ic_backend_pancake_runtime_data_new(instructions);
    if (!runtime_data) {
        puts("main: call to ic_backend_pancake_runtime_data_new failed");
        exit(1);
    }

    if (!ic_backend_pancake_interpret(runtime_data)) {
        puts("main: call to ic_backend_pancake_interpret failed");
        exit(1);
    }

    if (!ic_backend_pancake_value_stack_print(stdout, runtime_data->value_stack)) {
        puts("main: call to ic_backend_pancake_value_stack_print failed");
        exit(1);
    }

    return 0;
}
