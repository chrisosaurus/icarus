#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void * ic_alloc(size_t size){
    void *v = 0;

    v = calloc(1, size);
    if( ! v ){
        puts("ic_alloc: call to calloc failed");
        exit(1);
    }

    return v;
}

/* pre-declarations */
typedef struct Void Void;
typedef struct Bool Bool;
typedef struct Sint Sint;
typedef struct Uint Uint;
typedef struct String String;

Bool * ic_bool_new(unsigned int boolean);
Sint * ic_sint_new(long long int integer);
Uint * ic_uint_new(long long unsigned int integer);
String * ic_string_new(char *str, unsigned int len);

/* builtin fn println() */
void i_println_a(void){
    puts("");
}

/* builtin type Void */
typedef struct Void{
} Void;


/* builtin type Bool */
typedef struct Bool{
    unsigned int boolean;
} Bool;

/* builtin fn print(a::Bool) */
void i_print_a_Bool(Bool *b){
    if( b->boolean ){
        fputs("True", stdout);
    } else {
        fputs("False", stdout);
    }
}
/* builtin fn println(a::Bool) */
void i_println_a_Bool(Bool *b){
    i_print_a_Bool(b);
    i_println_a();
}
Bool * ic_bool_new(unsigned int boolean){
    Bool *b = 0;
    b = ic_alloc(sizeof(Bool));
    b->boolean = boolean;
    return b;
}
/* builtin fn equal(a::Bool, b::Bool) -> Bool */
Bool * i_equal_a_Bool_Bool(Bool *a, Bool *b){
    if( a->boolean == 0 && b->boolean == 0 ){
        /* return truthy boolean */
        return ic_bool_new(1);
    }

    if( a->boolean && b->boolean ){
        /* return truthy boolean */
        return ic_bool_new(1);
    }

    /* return falsey boolean */
    return ic_bool_new(0);
}
/* builtin fn not(a::Bool) -> Bool */
Bool * i_not_a_Bool(Bool *a){
    if( a->boolean ){
        /* return falsey boolean */
        return ic_bool_new(0);
    } else {
        /* return truthy boolean */
        return ic_bool_new(1);
    }
}
/* builtin fn and(a::Bool, b::Bool) -> Bool */
Bool * i_and_a_Bool_Bool(Bool *a, Bool *b){
    if( a->boolean && b->boolean ){
        /* return truthy boolean */
        return ic_bool_new(1);
    }
    /* return falsey boolean */
    return ic_bool_new(0);
}
/* builtin fn or(a::Bool, b::Bool) -> Bool */
Bool * i_or_a_Bool_Bool(Bool *a, Bool *b){
    if( a->boolean ){
        /* return truthy boolean */
        return ic_bool_new(1);
    }

    if( b->boolean ){
        /* return truthy boolean */
        return ic_bool_new(1);
    }

    /* return falsey boolean */
    return ic_bool_new(0);
}


/* builtin type Signed int */
typedef struct Sint{
    long long int integer;
} Sint;

Sint * ic_sint_new(long long int integer){
    Sint *i = 0;
    i = ic_alloc(sizeof(Sint));
    i->integer = integer;
    return i;
}
/* builtin fn print(a::Sint) */
void i_print_a_Sint(Sint *i){
    printf("%lli", i->integer);
}
/* builtin fn println(a::Sint) */
void i_println_a_Sint(Sint *i){
    i_print_a_Sint(i);
    i_println_a();
}
/* builtin fn equal(a::Sint, b::Sint) -> Bool */
Bool * i_equal_a_Sint_Sint(Sint *a, Sint *b){
    if( a->integer == b->integer ){
        /* return truthy boolean */
        return ic_bool_new(1);
    }
    /* return falsey boolean */
    return ic_bool_new(0);
}
/* builtin fn plus(a::Sint, b::Sint) -> Sint */
Sint * i_plus_a_Sint_Sint(Sint *a, Sint *b){
    Sint *i = 0;
    i = ic_sint_new(a->integer + b->integer);
    return i;
}
/* builtin fn minus(a::Sint, b::Sint) -> Sint */
Sint * i_minus_a_Sint_Sint(Sint *a, Sint *b){
    Sint *i = 0;
    i = ic_sint_new(a->integer - b->integer);
    return i;
}
/* builtin fn multiply(a::Sint, b::Sint) -> Sint */
Sint * i_multiply_a_Sint_Sint(Sint *a, Sint *b){
    Sint *i = 0;
    i = ic_sint_new(a->integer * b->integer);
    return i;
}
/* builtin fn divide(a::Sint, b::Sint) -> Sint */
Sint * i_divide_a_Sint_Sint(Sint *a, Sint *b){
    Sint *i = 0;
    i = ic_sint_new(a->integer / b->integer);
    return i;
}
/* builtin fn modulo(a::Sint, b::Sint) -> Sint */
Sint * i_modulo_a_Sint_Sint(Sint *a, Sint *b){
    Sint *i = 0;
    i = ic_sint_new(a->integer % b->integer);
    return i;
}

/* builtin type Unsigned int */
typedef struct Uint{
    long long unsigned int integer;
} Uint;

Uint * ic_uint_new(long long unsigned int integer){
    Uint *i = 0;
    i = ic_alloc(sizeof(Uint));
    i->integer = integer;
    return i;
}
/* builtin fn print(a::Sint) */
void i_print_a_Uint(Uint *i){
    printf("%lli", i->integer);
}
/* builtin fn println(a::Sint) */
void i_println_a_Uint(Uint *i){
    i_print_a_Uint(i);
    i_println_a();
}
/* builtin fn equal(a::Sint, b::Sint) -> Bool */
Bool * i_equal_a_Uint_Uint(Uint *a, Uint *b){
    if( a->integer == b->integer ){
        /* return truthy boolean */
        return ic_bool_new(1);
    }
    /* return falsey boolean */
    return ic_bool_new(0);
}
/* builtin fn plus(a::Sint, b::Sint) -> Sint */
Uint * i_plus_a_Uint_Uint(Uint *a, Uint *b){
    Uint *i = 0;
    i = ic_uint_new(a->integer + b->integer);
    return i;
}
/* builtin fn minus(a::Sint, b::Sint) -> Sint */
Uint * i_minus_a_Uint_Uint(Uint *a, Uint *b){
    Uint *i = 0;
    i = ic_uint_new(a->integer - b->integer);
    return i;
}
/* builtin fn multiply(a::Sint, b::Sint) -> Sint */
Uint * i_multiply_a_Uint_Uint(Uint *a, Uint *b){
    Uint *i = 0;
    i = ic_uint_new(a->integer * b->integer);
    return i;
}
/* builtin fn divide(a::Sint, b::Sint) -> Sint */
Uint * i_divide_a_Uint_Uint(Uint *a, Uint *b){
    Uint *i = 0;
    i = ic_uint_new(a->integer / b->integer);
    return i;
}
/* builtin fn modulo(a::Sint, b::Sint) -> Sint */
Uint * i_modulo_a_Uint_Uint(Uint *a, Uint *b){
    Uint *i = 0;
    i = ic_uint_new(a->integer % b->integer);
    return i;
}

/* builtin type String */
typedef struct String{
    char *str;
    unsigned int len;
} String;

String * ic_string_new(char *str, unsigned int len){
    String *s = 0;
    s = ic_alloc(sizeof(String));
    /* FIXME sharing str */
    s->str = str;
    s->len = len;
    return s;
}
/* builtin fn print(a::String) */
void i_print_a_String(String *s){
    printf("%s", s->str);
}
/* builtin fn println(a::String) */
void i_println_a_String(String *s){
    i_print_a_String(s);
    i_println_a();
}
/* builtin fn equal(a::String, b::String) -> Bool */
Bool * i_equal_a_String_String(String *a, String *b){
    if( strcmp(a->str, b->str) ){
        /* return falsey boolean */
        return ic_bool_new(0);
    }
    /* return truthy boolean */
    return ic_bool_new(1);
}
/* builtin fn concat(a::String, b::String) -> String */
String * i_concat_a_String_String(String *a, String *b){
    String *s = 0;
    s = ic_alloc(sizeof(String));
    s->str = ic_alloc(a->len + b->len - 1);
    strcpy(s->str, a->str);
    strcpy(&(s->str[a->len]), b->str);
    return s;
}




