#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void panic(char *ch) {
    puts("Panic!");
    if (ch) {
        puts(ch);
    }
    exit(1);
}

void *ic_alloc(size_t size) {
    void *v = 0;

    v = calloc(1, size);
    if (!v) {
        puts("ic_alloc: call to calloc failed");
        exit(1);
    }

    return v;
}

/* pre-declarations
 * types should be opaque
 */
typedef void Void;
typedef uint8_t Bool;
typedef int32_t Sint;
typedef uint32_t Uint;
typedef struct String *String;

Bool ic_bool_new(uint8_t boolean);
Sint ic_sint_new(int32_t integer);
Uint ic_uint_new(uint32_t integer);
String ic_string_new(char *str, unsigned int len);

/* builtin fn println() */
void i_println_a(void) {
    puts("");
}

/* builtin fn print(a::Bool) */
void i_print_a_Bool(Bool b) {
    if (b) {
        fputs("True", stdout);
    } else {
        fputs("False", stdout);
    }
}
/* builtin fn println(a::Bool) */
void i_println_a_Bool(Bool b) {
    i_print_a_Bool(b);
    i_println_a();
}
Bool ic_bool_new(uint8_t boolean) {
    Bool b = boolean;
    return b;
}
/* builtin fn equal(a::Bool, b::Bool) -> Bool */
Bool i_equal_a_Bool_Bool(Bool a, Bool b) {
    if (a == 0 && b == 0) {
        /* return truthy boolean */
        return ic_bool_new(1);
    }

    if (a && b) {
        /* return truthy boolean */
        return ic_bool_new(1);
    }

    /* return falsey boolean */
    return ic_bool_new(0);
}
/* builtin fn not(a::Bool) -> Bool */
Bool i_not_a_Bool(Bool a) {
    if (a) {
        /* return falsey boolean */
        return ic_bool_new(0);
    } else {
        /* return truthy boolean */
        return ic_bool_new(1);
    }
}
/* builtin fn and(a::Bool, b::Bool) -> Bool */
Bool i_and_a_Bool_Bool(Bool a, Bool b) {
    if (a && b) {
        /* return truthy boolean */
        return ic_bool_new(1);
    }
    /* return falsey boolean */
    return ic_bool_new(0);
}
/* builtin fn or(a::Bool, b::Bool) -> Bool */
Bool i_or_a_Bool_Bool(Bool a, Bool b) {
    if (a) {
        /* return truthy boolean */
        return ic_bool_new(1);
    }

    if (b) {
        /* return truthy boolean */
        return ic_bool_new(1);
    }

    /* return falsey boolean */
    return ic_bool_new(0);
}
/* builtin fn to_str(a::Bool) -> String */
String i_to_str_a_Bool(Bool a) {
    /* FIXME TODO should re-use constants */
    if (a) {
        return ic_string_new("True", 5);
    } else {
        return ic_string_new("False", 6);
    }
}

Sint ic_sint_new(int32_t integer) {
    Sint i = integer;
    return i;
}
/* builtin fn print(a::Sint) */
void i_print_a_Sint(Sint i) {
    printf("%" PRId32, i);
}
/* builtin fn println(a::Sint) */
void i_println_a_Sint(Sint i) {
    i_print_a_Sint(i);
    i_println_a();
}
/* builtin fn equal(a::Sint, b::Sint) -> Bool */
Bool i_equal_a_Sint_Sint(Sint a, Sint b) {
    if (a == b) {
        /* return truthy boolean */
        return ic_bool_new(1);
    }
    /* return falsey boolean */
    return ic_bool_new(0);
}
/* builtin fn plus(a::Sint, b::Sint) -> Sint */
Sint i_plus_a_Sint_Sint(Sint a, Sint b) {
    Sint i = 0;
    i = ic_sint_new(a + b);
    return i;
}
/* builtin fn minus(a::Sint, b::Sint) -> Sint */
Sint i_minus_a_Sint_Sint(Sint a, Sint b) {
    Sint i = 0;
    i = ic_sint_new(a - b);
    return i;
}
/* builtin fn multiply(a::Sint, b::Sint) -> Sint */
Sint i_multiply_a_Sint_Sint(Sint a, Sint b) {
    Sint i = 0;
    i = ic_sint_new(a * b);
    return i;
}
/* builtin fn divide(a::Sint, b::Sint) -> Sint */
Sint i_divide_a_Sint_Sint(Sint a, Sint b) {
    Sint i = 0;
    i = ic_sint_new(a / b);
    return i;
}
/* builtin fn modulo(a::Sint, b::Sint) -> Sint */
Sint i_modulo_a_Sint_Sint(Sint a, Sint b) {
    Sint i = 0;
    i = ic_sint_new(a % b);
    return i;
}
/* builtin fn lessthan(a::Sint, b::Sint) -> Bool */
Bool i_lessthan_a_Sint_Sint(Sint a, Sint b) {
    Bool r = 0;
    r = ic_bool_new(a < b);
    return r;
}
/* builtin fn greaterthan(a::Sint, b::Sint) -> Bool */
Bool i_greaterthan_a_Sint_Sint(Sint a, Sint b) {
    Bool r = 0;
    r = ic_bool_new(a > b);
    return r;
}
/* builtin fn lessthan_equal(a::Sint, b::Sint) -> Bool */
Bool i_lessthan_equal_a_Sint_Sint(Sint a, Sint b) {
    Bool r = 0;
    r = ic_bool_new(a <= b);
    return r;
}
/* builtin fn greaterthan_equal(a::Sint, b::Sint) -> Bool */
Bool i_greaterthan_equal_a_Sint_Sint(Sint a, Sint b) {
    Bool r = 0;
    r = ic_bool_new(a >= b);
    return r;
}

Uint ic_uint_new(uint32_t integer) {
    Uint i = integer;
    return i;
}
/* builtin fn Uint(a::Sint) */
Uint i_Uint_a_Sint(Sint a) {
    if (a < 0) {
        panic("i_Uint_a_Sint panic! Sint was <0");
    }
    return ic_uint_new(a);
}
/* builtin fn print(a::Sint) */
void i_print_a_Uint(Uint i) {
    printf("%" PRId32, i);
}
/* builtin fn println(a::Sint) */
void i_println_a_Uint(Uint i) {
    i_print_a_Uint(i);
    i_println_a();
}
/* builtin fn equal(a::Sint, b::Sint) -> Bool */
Bool i_equal_a_Uint_Uint(Uint a, Uint b) {
    if (a == b) {
        /* return truthy boolean */
        return ic_bool_new(1);
    }
    /* return falsey boolean */
    return ic_bool_new(0);
}
/* builtin fn plus(a::Sint, b::Sint) -> Sint */
Uint i_plus_a_Uint_Uint(Uint a, Uint b) {
    Uint i = 0;
    i = ic_uint_new(a + b);
    return i;
}
/* builtin fn minus(a::Sint, b::Sint) -> Sint */
Uint i_minus_a_Uint_Uint(Uint a, Uint b) {
    Uint i = 0;
    i = ic_uint_new(a - b);
    return i;
}
/* builtin fn multiply(a::Sint, b::Sint) -> Sint */
Uint i_multiply_a_Uint_Uint(Uint a, Uint b) {
    Uint i = 0;
    i = ic_uint_new(a * b);
    return i;
}
/* builtin fn divide(a::Sint, b::Sint) -> Sint */
Uint i_divide_a_Uint_Uint(Uint a, Uint b) {
    Uint i = 0;
    i = ic_uint_new(a / b);
    return i;
}
/* builtin fn modulo(a::Sint, b::Sint) -> Sint */
Uint i_modulo_a_Uint_Uint(Uint a, Uint b) {
    Uint i = 0;
    i = ic_uint_new(a % b);
    return i;
}
/* builtin fn lessthan(a::Uint, b::Uint) -> Bool */
Bool i_lessthan_a_Uint_Uint(Uint a, Uint b) {
    Bool r = 0;
    r = ic_bool_new(a < b);
    return r;
}
/* builtin fn greaterthan(a::Uint, b::Uint) -> Bool */
Bool i_greaterthan_a_Uint_Uint(Uint a, Uint b) {
    Bool r = 0;
    r = ic_bool_new(a > b);
    return r;
}
/* builtin fn lessthan_equal(a::Uint, b::Uint) -> Bool */
Bool i_lessthan_equal_a_Uint_Uint(Uint a, Uint b) {
    Bool r = 0;
    r = ic_bool_new(a <= b);
    return r;
}
/* builtin fn greaterthan_equal(a::Uint, b::Uint) -> Bool */
Bool i_greaterthan_equal_a_Uint_Uint(Uint a, Uint b) {
    Bool r = 0;
    r = ic_bool_new(a >= b);
    return r;
}

/* builtin type String */
struct String {
    char *str;
    unsigned int len;
};

String ic_string_new(char *str, unsigned int len) {
    String s = 0;
    s = ic_alloc(sizeof(String));
    /* FIXME sharing str */
    s->str = str;
    s->len = len;
    return s;
}
/* builtin fn print(a::String) */
void i_print_a_String(String s) {
    printf("%s", s->str);
}
/* builtin fn println(a::String) */
void i_println_a_String(String s) {
    i_print_a_String(s);
    i_println_a();
}
/* builtin fn equal(a::String, b::String) -> Bool */
Bool i_equal_a_String_String(String a, String b) {
    if (strcmp(a->str, b->str)) {
        /* return falsey boolean */
        return ic_bool_new(0);
    }
    /* return truthy boolean */
    return ic_bool_new(1);
}
/* builtin fn concat(a::String, b::String) -> String */
String i_concat_a_String_String(String a, String b) {
    String s = 0;
    s = ic_alloc(sizeof(String));
    s->len = a->len + b->len;
    s->str = ic_alloc(s->len);
    strncpy(s->str, a->str, a->len);
    strncpy(&(s->str[a->len]), b->str, b->len);
    return s;
}
/* builtin fn length(a::String) -> Uint */
Uint i_length_a_String(String a) {
    return a->len;
}

/* builtin fn equal(a::Sint, b::Uint) -> Bool */
Bool i_equal_a_Sint_Uint(Sint a, Uint b) {
    Sint tmp = 0;
    if (b < 0) {
        return 0;
    }
    tmp = b;
    if (a == b) {
        return 1;
    }
    return 0;
}
/* builtin fn equal(a::Uint, b::Sint) -> Bool */
Bool i_equal_a_Uint_Sint(Uint a, Sint b) {
    Sint tmp = 0;
    if (a < 0) {
        return 0;
    }
    tmp = a;
    if (a == b) {
        return 1;
    }
    return 0;
}
