#ifndef IC_OPTS_H
#define IC_OPTS_H

struct ic_opts {
    bool check;
    bool transform;
    bool o2c;
    bool pancake;

    char *in_filename;
    char *out_filename;

    bool debug;

    bool version;

    bool help;
};

enum ic_arg_type {
    arg_type_bool,
    arg_type_string,
};

struct ic_arg {
    const char *l;
    const char s;
    enum ic_arg_type type;
    void *thing;
};

#define IC_ARG_END \
    { 0, 0, 0, 0 }

struct ic_command {
    const char *c;
    bool *thing;
};

#define IC_COMMAND_END \
    { 0, 0 }

/* returns 1 on success
 * returns 0 on failure
 */
unsigned int ic_arg_parse(struct ic_command commands[], struct ic_arg args[], char **argv, int argc, char **default_option);

#endif
