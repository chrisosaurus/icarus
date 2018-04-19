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

    /* malloc/calloc allow a size of 0 but state
     * "if size is 0, then malloc() returns either NULL, or a unique pointer
     * value that can later be successfully passed to free()."
     *
     * we want to guarantee the second behavior
     * so we default to a minimum size of 1
     */
    if (size == 0) {
        size = 1;
    }

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
typedef char Unit;
typedef uint8_t Bool;
typedef int32_t Signed;
typedef uint32_t Unsigned;
typedef struct String *String;

Unit ic_unit_new();
Bool ic_bool_new(uint8_t boolean);
Signed ic_signed_new(int32_t integer);
Unsigned ic_unsigned_new(uint32_t integer);
String ic_string_new(char *str, unsigned int len);

Unit ic_unit_new(void) {
    return 0;
}

Unit Unit_a_b(void) {
  return ic_unit_new();
}

/* builtin fn println() */
Unit println_a_b(void) {
    puts("");
    return 0;
}

/* builtin fn print(a::Unit) */
Unit print_a_Unit_b(Unit a) {
    fputs("Unit()", stdout);
    return 0;
}

/* builtin fn println(a::Unit) */
Unit println_a_Unit_b(Unit a) {
    print_a_Unit_b(a);
    println_a_b();
    return 0;
}

/* builtin fn equal(a::Unit, b::Unit) -> Bool */
Bool equal_a_Unit_Unit_b(Unit a, Unit b) {
    return ic_bool_new(1);
}

/* builtin fn to_str(a::Unit) -> String */
String to_str_a_Unit_b(Unit a) {
    return ic_string_new("()", 2);
}

/* builtin fn print(a::Bool) */
Unit print_a_Bool_b(Bool b) {
    if (b) {
        fputs("True", stdout);
    } else {
        fputs("False", stdout);
    }
    return 0;
}
/* builtin fn println(a::Bool) */
Unit println_a_Bool_b(Bool b) {
    print_a_Bool_b(b);
    println_a_b();
    return 0;
}
Bool ic_bool_new(uint8_t boolean) {
    Bool b = boolean;
    return b;
}
/* builtin fn equal(a::Bool, b::Bool) -> Bool */
Bool equal_a_Bool_Bool_b(Bool a, Bool b) {
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
Bool not_a_Bool_b(Bool a) {
    if (a) {
        /* return falsey boolean */
        return ic_bool_new(0);
    } else {
        /* return truthy boolean */
        return ic_bool_new(1);
    }
}
/* builtin fn and(a::Bool, b::Bool) -> Bool */
Bool and_a_Bool_Bool_b(Bool a, Bool b) {
    if (a && b) {
        /* return truthy boolean */
        return ic_bool_new(1);
    }
    /* return falsey boolean */
    return ic_bool_new(0);
}
/* builtin fn or(a::Bool, b::Bool) -> Bool */
Bool or_a_Bool_Bool_b(Bool a, Bool b) {
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
String to_str_a_Bool_b(Bool a) {
    /* FIXME TODO should re-use constants */
    if (a) {
        return ic_string_new("True", 5);
    } else {
        return ic_string_new("False", 6);
    }
}
/* builtin fn assert(a::Bool) -> Unit */
Unit assert_a_Bool_b(Bool a) {
    if (!a) {
        panic("assertion failed");
    }
    return 0;
}

Signed ic_signed_new(int32_t integer) {
    Signed i = integer;
    return i;
}
/* builtin fn print(a::Signed) */
Unit print_a_Signed_b(Signed i) {
    printf("%" PRId32, i);
    return 0;
}
/* builtin fn println(a::Signed) */
Unit println_a_Signed_b(Signed i) {
    print_a_Signed_b(i);
    println_a_b();
    return 0;
}
/* builtin fn equal(a::Signed, b::Signed) -> Bool */
Bool equal_a_Signed_Signed_b(Signed a, Signed b) {
    if (a == b) {
        /* return truthy boolean */
        return ic_bool_new(1);
    }
    /* return falsey boolean */
    return ic_bool_new(0);
}
/* builtin fn plus(a::Signed, b::Signed) -> Signed */
Signed plus_a_Signed_Signed_b(Signed a, Signed b) {
    Signed i = 0;
    i = ic_signed_new(a + b);
    return i;
}
/* builtin fn minus(a::Signed, b::Signed) -> Signed */
Signed minus_a_Signed_Signed_b(Signed a, Signed b) {
    Signed i = 0;
    i = ic_signed_new(a - b);
    return i;
}
/* builtin fn multiply(a::Signed, b::Signed) -> Signed */
Signed multiply_a_Signed_Signed_b(Signed a, Signed b) {
    Signed i = 0;
    i = ic_signed_new(a * b);
    return i;
}
/* builtin fn divide(a::Signed, b::Signed) -> Signed */
Signed divide_a_Signed_Signed_b(Signed a, Signed b) {
    Signed i = 0;
    i = ic_signed_new(a / b);
    return i;
}
/* builtin fn modulo(a::Signed, b::Signed) -> Signed */
Signed modulo_a_Signed_Signed_b(Signed a, Signed b) {
    Signed i = 0;
    i = ic_signed_new(a % b);
    return i;
}
/* builtin fn lessthan(a::Signed, b::Signed) -> Bool */
Bool lessthan_a_Signed_Signed_b(Signed a, Signed b) {
    Bool r = 0;
    r = ic_bool_new(a < b);
    return r;
}
/* builtin fn greaterthan(a::Signed, b::Signed) -> Bool */
Bool greaterthan_a_Signed_Signed_b(Signed a, Signed b) {
    Bool r = 0;
    r = ic_bool_new(a > b);
    return r;
}
/* builtin fn lessthan_equal(a::Signed, b::Signed) -> Bool */
Bool lessthan_equal_a_Signed_Signed_b(Signed a, Signed b) {
    Bool r = 0;
    r = ic_bool_new(a <= b);
    return r;
}
/* builtin fn greaterthan_equal(a::Signed, b::Signed) -> Bool */
Bool greaterthan_equal_a_Signed_Signed_b(Signed a, Signed b) {
    Bool r = 0;
    r = ic_bool_new(a >= b);
    return r;
}

Unsigned ic_unsigned_new(uint32_t integer) {
    Unsigned i = integer;
    return i;
}
/* builtin fn print(a::Signed) */
Unit print_a_Unsigned_b(Unsigned i) {
    printf("%" PRId32, i);
    return 0;
}
/* builtin fn println(a::Signed) */
Unit println_a_Unsigned_b(Unsigned i) {
    print_a_Unsigned_b(i);
    println_a_b();
    return 0;
}
/* builtin fn equal(a::Signed, b::Signed) -> Bool */
Bool equal_a_Unsigned_Unsigned_b(Unsigned a, Unsigned b) {
    if (a == b) {
        /* return truthy boolean */
        return ic_bool_new(1);
    }
    /* return falsey boolean */
    return ic_bool_new(0);
}
/* builtin fn plus(a::Signed, b::Signed) -> Signed */
Unsigned plus_a_Unsigned_Unsigned_b(Unsigned a, Unsigned b) {
    Unsigned i = 0;
    i = ic_unsigned_new(a + b);
    return i;
}
/* builtin fn minus(a::Signed, b::Signed) -> Signed */
Unsigned minus_a_Unsigned_Unsigned_b(Unsigned a, Unsigned b) {
    Unsigned i = 0;
    i = ic_unsigned_new(a - b);
    return i;
}
/* builtin fn multiply(a::Signed, b::Signed) -> Signed */
Unsigned multiply_a_Unsigned_Unsigned_b(Unsigned a, Unsigned b) {
    Unsigned i = 0;
    i = ic_unsigned_new(a * b);
    return i;
}
/* builtin fn divide(a::Signed, b::Signed) -> Signed */
Unsigned divide_a_Unsigned_Unsigned_b(Unsigned a, Unsigned b) {
    Unsigned i = 0;
    i = ic_unsigned_new(a / b);
    return i;
}
/* builtin fn modulo(a::Signed, b::Signed) -> Signed */
Unsigned modulo_a_Unsigned_Unsigned_b(Unsigned a, Unsigned b) {
    Unsigned i = 0;
    i = ic_unsigned_new(a % b);
    return i;
}
/* builtin fn lessthan(a::Unsigned, b::Unsigned) -> Bool */
Bool lessthan_a_Unsigned_Unsigned_b(Unsigned a, Unsigned b) {
    Bool r = 0;
    r = ic_bool_new(a < b);
    return r;
}
/* builtin fn greaterthan(a::Unsigned, b::Unsigned) -> Bool */
Bool greaterthan_a_Unsigned_Unsigned_b(Unsigned a, Unsigned b) {
    Bool r = 0;
    r = ic_bool_new(a > b);
    return r;
}
/* builtin fn lessthan_equal(a::Unsigned, b::Unsigned) -> Bool */
Bool lessthan_equal_a_Unsigned_Unsigned_b(Unsigned a, Unsigned b) {
    Bool r = 0;
    r = ic_bool_new(a <= b);
    return r;
}
/* builtin fn greaterthan_equal(a::Unsigned, b::Unsigned) -> Bool */
Bool greaterthan_equal_a_Unsigned_Unsigned_b(Unsigned a, Unsigned b) {
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
Unit print_a_String_b(String s) {
    printf("%s", s->str);
    return 0;
}
/* builtin fn println(a::String) */
Unit println_a_String_b(String s) {
    print_a_String_b(s);
    println_a_b();
    return 0;
}
/* builtin fn equal(a::String, b::String) -> Bool */
Bool equal_a_String_String_b(String a, String b) {
    if (strcmp(a->str, b->str)) {
        /* return falsey boolean */
        return ic_bool_new(0);
    }
    /* return truthy boolean */
    return ic_bool_new(1);
}
/* builtin fn concat(a::String, b::String) -> String */
String concat_a_String_String_b(String a, String b) {
    String s = 0;
    s = ic_alloc(sizeof(String));
    s->len = a->len + b->len;
    s->str = ic_alloc(s->len);
    strncpy(s->str, a->str, a->len);
    strncpy(&(s->str[a->len]), b->str, b->len);
    return s;
}
/* builtin fn length(a::String) -> Unsigned */
Unsigned length_a_String_b(String a) {
    return a->len;
}

/* builtin fn equal(a::Signed, b::Unsigned) -> Bool */
Bool equal_a_Signed_Unsigned_b(Signed a, Unsigned b) {
    Signed tmp = 0;
    if (b < 0) {
        return 0;
    }
    tmp = b;
    if (a == b) {
        return 1;
    }
    return 0;
}
/* builtin fn equal(a::Unsigned, b::Signed) -> Bool */
Bool equal_a_Unsigned_Signed_b(Unsigned a, Signed b) {
    Signed tmp = 0;
    if (a < 0) {
        return 0;
    }
    tmp = a;
    if (a == b) {
        return 1;
    }
    return 0;
}
