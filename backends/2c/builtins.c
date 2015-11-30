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
typedef struct Int Int;
typedef struct String String;

Bool * ic_bool_new(unsigned int boolean);
Int * ic_int_new(long long int integer);
String * ic_string_new(char *str, unsigned int len);

/* builtin fn println() */
void ic_println(void){
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
void ic_bool_print(Bool *b){
    if( b->boolean ){
        fputs("True", stdout);
    } else {
        fputs("False", stdout);
    }
}
/* builtin fn println(a::Bool) */
void ic_bool_println(Bool *b){
    ic_bool_print(b);
    ic_println();
}
Bool * ic_bool_new(unsigned int boolean){
    Bool *b = 0;
    b = ic_alloc(sizeof(Bool));
    b->boolean = boolean;
    return b;
}
/* builtin fn equal(a::Bool, b::Bool) -> Bool */
Bool * ic_bool_equal(Bool *a, Bool *b){
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
Bool * ic_bool_not(Bool *a){
    if( a->boolean ){
        /* return falsey boolean */
        return ic_bool_new(0);
    } else {
        /* return truthy boolean */
        return ic_bool_new(1);
    }
}
/* builtin fn and(a::Bool, b::Bool) -> Bool */
Bool * ic_bool_and(Bool *a, Bool *b){
    if( a->boolean && b->boolean ){
        /* return truthy boolean */
        return ic_bool_new(1);
    }
    /* return falsey boolean */
    return ic_bool_new(0);
}
/* builtin fn or(a::Bool, b::Bool) -> Bool */
Bool * ic_bool_or(Bool *a, Bool *b){
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


/* builtin type Int */
typedef struct Int{
    long long int integer;
} Int;

Int * ic_int_new(long long int integer){
    Int *i = 0;
    i = ic_alloc(sizeof(Int));
    i->integer = integer;
    return i;
}
/* builtin fn print(a::Int) */
void ic_int_print(Int *i){
    printf("%lli", i->integer);
}
/* builtin fn println(a::Int) */
void ic_int_println(Int *i){
    ic_int_print(i);
    ic_println();
}
/* builtin fn equal(a::Int, b::Int) -> Bool */
Bool * ic_int_equal(Int *a, Int *b){
    if( a->integer == b->integer ){
        /* return truthy boolean */
        return ic_bool_new(1);
    }
    /* return falsey boolean */
    return ic_bool_new(0);
}
/* builtin fn plus(a::Int, b::Int) -> Int */
Int * ic_int_plus(Int *a, Int *b){
    Int *i = 0;
    i = ic_int_new(a->integer + b->integer);
    return i;
}
/* builtin fn minus(a::Int, b::Int) -> Int */
Int * ic_int_minus(Int *a, Int *b){
    Int *i = 0;
    i = ic_int_new(a->integer - b->integer);
    return i;
}
/* builtin fn multiply(a::Int, b::Int) -> Int */
Int * ic_int_multiply(Int *a, Int *b){
    Int *i = 0;
    i = ic_int_new(a->integer * b->integer);
    return i;
}
/* builtin fn divide(a::Int, b::Int) -> Int */
Int * ic_int_divide(Int *a, Int *b){
    Int *i = 0;
    i = ic_int_new(a->integer / b->integer);
    return i;
}
/* builtin fn modulo(a::Int, b::Int) -> Int */
Int * ic_int_modulo(Int *a, Int *b){
    Int *i = 0;
    i = ic_int_new(a->integer % b->integer);
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
void ic_string_print(String *s){
    printf("%s", s->str);
}
/* builtin fn println(a::String) */
void ic_string_println(String *s){
    ic_string_print(s);
    ic_println();
}
/* builtin fn equal(a::String, b::String) -> Bool */
Bool * ic_string_equal(String *a, String *b){
    if( strcmp(a->str, b->str) ){
        /* return falsey boolean */
        return ic_bool_new(0);
    }
    /* return truthy boolean */
    return ic_bool_new(1);
}
/* builtin fn concat(a::String, b::String) -> String */
String * ic_string_concat(String *a, String *b){
    String *s = 0;
    s = ic_alloc(sizeof(String));
    s->str = ic_alloc(a->len + b->len - 1);
    strcpy(s->str, a->str);
    strcpy(&(s->str[a->len]), b->str);
    return s;
}




