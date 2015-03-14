#include <stdlib.h> /* calloc, realloc */
#include <stdio.h> /* puts, printf */
#include <string.h> /* strlen, memset */

#include "lexer.h"

/* expand supplied tokens to the specified new capacity
 * makes sure the new area is memset to 0
 *
 * if 0 is passed in as *tokens then it behaves like realloc
 * in that it will allocated *tokens and return it
 *
 * returns pointer to tokens on success (which may have a different value)
 * returns 0 on error
 */
static struct ic_tokens * ic_expand_tokens(struct ic_tokens *tokens, unsigned int new_cap);

/* consumes one word (alpha, numbers, -, and _)
 * adds this consumed unit as a token
 *
 * returns *tokens on success
 * returns 0 on error
 */
static struct ic_tokens * ic_consume_word(struct ic_tokens *tokens, char *source, unsigned int *i);

/* consume a number
 * adds this consumed unit as a token
 *
 * returns *tokens on success
 * returns 0 on failure
 */
static struct ic_tokens * ic_consume_number(struct ic_tokens *tokens, char *source, unsigned int *i);

/* consumes one symbol at current position
 * adds this consumed unit as a token
 *
 * returns *tokens on success
 * returns 0 on error
 */
static struct ic_tokens * ic_consume_single_symbol(struct ic_tokens *tokens, char *source, unsigned int *i);

/* consumes as many of the specified symbol `sym` in a row
 * adds this consumed unit as a token
 *
 * returns *tokens on success
 * returns 0 on error
 */
static struct ic_tokens * ic_consume_repeated_symbol(struct ic_tokens *tokens, char *source, unsigned int *i, char sym);

/* consume comment
 * this checks for a beginning # (comment character) and will raise an error if not found
 * this will consume everything from current character up to first \n or EOF
 *
 * NB: ic_strip_comment is one of the few ic_consume_* functions that will throw an error
 * when a null tokens is provided
 *
 * returns token passes in on success
 * returns 0 on failure
 */
static struct ic_tokens * ic_strip_comment(struct ic_tokens *tokens, char *source, unsigned int *i);

/* takes a character array of the source code as text
 * returns an struct ic_tokens * containing the output from lexing
 *
 * returns 0 on error
 */
struct ic_tokens * ic_lex(char *source){
    unsigned int i = 0;
    unsigned int len = 0;
    struct ic_tokens *tokens = 0;

    if( ! source ){
        puts("lex: provided source was null");
        return 0;
    }

    len = strlen(source);

    /* set initial capacity to strlen of source */
    tokens = ic_expand_tokens(tokens, len);
    if( ! tokens ){
        puts("lex icarus token expansion failed");
        return 0;
    }

    for( i=0; i<len; ){
        switch( source[i] ){
            case '\0':
                return tokens;
                break;

            /* whitespace is only significant in Icarus in terms
             * of terminating a comment, the lexer strips out comments
             * so at the parser level whitespace is insignificant
             */
            case '\t':
                puts("warning: tab found, continuing but spaces are preferred");
            case ' ':
            case '\n':
                ++i;
                break;

            /* (...)
             * a = b
             * a.b
             * "hello"
             */
            case '(':
            case ')':
            case '=':
            case '.':
            case '"':
                tokens = ic_consume_single_symbol(tokens, source, &i);
                break;

            /* # is only used to denote a comment
             * comments are stripped by the lexer
             * and so do not exist at the parse level
             */
            case '#':
                tokens = ic_strip_comment(tokens, source, &i);
                break;

            /* currently we only use : in field declarations
             *      a::Int
             */
            case ':':
                tokens = ic_consume_repeated_symbol(tokens, source, &i, source[i]);
                break;

            /* a number must begin with a digit */
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                tokens = ic_consume_number(tokens, source, &i);
                break;

            /* a word must start with a letter or _
             * but can then include any combination of
             *  a-z
             *  A-Z
             *  0-9
             *  -
             *  _
             */
            default:
                /* assume this is a word */
                tokens = ic_consume_word(tokens, source, &i);
                break;
        }

        if( ! tokens ){
            puts("lex switch consume failed, returns null tokens");
            return 0;
        }
    }

    return tokens;
}

/* expand supplied tokens to the specified new capacity
 * makes sure the new area is memset to 0
 *
 * if 0 is passed in as *tokens then it behaves like realloc
 * in that it will allocated *tokens and return it
 *
 * returns pointer to tokens on success (which may have a different value)
 * returns 0 on error
 */
static struct ic_tokens * ic_expand_tokens(struct ic_tokens *tokens, unsigned int new_cap){
    /* if null was provided we must behave like realloc
     * and correctly allocate the new struct ic_tokens
     */
    if( ! tokens ){
        tokens = calloc(1, sizeof(struct ic_tokens));
        if( ! tokens ){
            puts("ic_expand_tokens: alloc failed");
            return 0;
        }
    }

    if( new_cap <= tokens->cap ){
        /* no work to be done */
        return tokens;
    }

#ifdef DEBUG_LEXER
    printf("expanding tokens from '%d' to '%d'\n\n", tokens->cap, new_cap);
#endif

    tokens->cap = new_cap;

    /* resize tokens char[] */
    tokens->tokens = realloc(tokens->tokens, sizeof(char) * new_cap);
    if( ! tokens->tokens ){
        puts("ic_expand_tokens: realloc failed");
        return 0;
    }

    /* zero out new bytes */
    memset( &(tokens->tokens[tokens->len]), 0, (new_cap - tokens->len) );

    return tokens;
}

/* given the pointer to a start of a token and a len
 * will add to the end of struct ic_tokens
 *
 * will call ic_expand_tokens to ensure sufficient room
 *
 * returns the pointer to tokens (which may have changed)
 * behaves correctly when handed a null *tokens (will pass to ic_expand_tokens)
 *
 * return 0 on failure
 */
static struct ic_tokens * add_token(struct ic_tokens *tokens, char *start, unsigned int len){
    unsigned int required_len = len + 2;

    if( tokens ){
        required_len += tokens->len;
    }

    if( ! start ){
        puts("add_tokens: null start provided");
        return 0;
    }

#ifdef DEBUG_LEXER
    printf("add_token calling with '%d' (len '%d') \n", required_len, len);
#endif

    /* ensure tokens has sufficient room for:
     *  it's existing data
     *  + the length of our new token
     *  + 1 for space to separate tokens
     *  + 1 for the null string terminator
     */
    tokens = ic_expand_tokens(tokens, required_len);
    if( ! tokens ){
        puts("add_token: failed call to ic_expand_tokens");
        return 0;
    }

    if( ! tokens->tokens ){
        puts("add_token: tokens->tokens null");
        return 0;
    }

    strncpy( &(tokens->tokens[tokens->len]), start, len );
    tokens->len += len;

    /* space to separate tokens */
    tokens->tokens[ tokens->len ] = ' ';
    ++ tokens->len;
    /* make sure it looks like a string */
    tokens->tokens[ tokens->len ] = '\0';

    return tokens;
}

/* consumes one word (alpha, numbers, -, and _)
 * adds this consumed unit as a token
 *
 * returns *tokens on success
 * returns 0 on error
 */
static struct ic_tokens * ic_consume_word(struct ic_tokens *tokens, char *source, unsigned int *i){
    unsigned int len=0;
    char ch=0;

    if( ! i || ! source ){
        puts("ic_consume_word: null source or i provided");
        return 0;
    }

    /* a word must start with a letter or _
     * but can then include any combination of
     *  a-z
     *  A-Z
     *  0-9
     *  -
     *  _
     *
     * we do not currently enforce this constraint at
     * the lexer level
     */

    for( len=0;
         ;
         ++len ){

        ch = source[(*i) + len];

        if( ch >= 'a' && ch <= 'z' ){
            continue;
        }

        if( ch >= 'A' && ch <= 'Z' ){
            continue;
        }

        if( ch >= '0' && ch <= '9' ){
            continue;
        }

        if( ch == '-' ){
            continue;
        }

        if( ch == '_' ){
            continue;
        }

        /* invalid character */
        break;
    }

    if( len == 0 ){
        /* stuck on a non-word character
         * this most likely means the body in lex()
         * is missing a *_symbol case
         */
        printf("ic_consume_word: stuck on a non-word character '%c', raising error\n", source[*i]);
        return 0;
    }

#ifdef DEBUG_LEXER
    printf("ic_consume_word: calling add_token with len '%d'\n", len);
#endif

    tokens = add_token(tokens, &(source[*i]), len);

    *i += len;

    return tokens;
}

/* consume a number
 * adds this consumed unit as a token
 *
 * returns *tokens on success
 * returns 0 on failure
 */
static struct ic_tokens * ic_consume_number(struct ic_tokens *tokens, char *source, unsigned int *i){
    unsigned int len=0;

    if( ! i || ! source ){
        puts("ic_consume_number: null source or i provided");
        return 0;
    }

    /* a word must start with a letter or _
     * but can then include any combination of
     *  a-z
     *  A-Z
     *  0-9
     *  -
     *  _
     *
     * we do not currently enforce this constraint at
     * the lexer level
     */

    for( len=0;
         ;
         ++len ){

        switch( source[(*i) + len] ){
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                /* valid character, continue */
                continue;
                break;

            default:
                /* invalid character
                 * for now the lexer only supports integer numbers
                 */
                goto NUMBER_LOOP_EXIT;
                break;
        }
    }

NUMBER_LOOP_EXIT:

    if( len == 0 ){
        /* stuck on a non-number character
         * this most likely means the body in lex()
         * is missing a *_symbol case
         */
        printf("ic_consume_number: stuck on a non-number character '%c', raising error\n", source[*i]);
        return 0;
    }

#ifdef DEBUG_LEXER
    printf("ic_consume_number: calling add_token with len '%d'\n", len);
#endif

    tokens = add_token(tokens, &(source[*i]), len);

    *i += len;

    return tokens;
}

/* consumes one symbol at current position
 * adds this consumed unit as a token
 *
 * returns *tokens on success
 * returns 0 on error
 */
static struct ic_tokens * ic_consume_single_symbol(struct ic_tokens *tokens, char *source, unsigned int *i){
    if( ! i || ! source ){
        puts("ic_consume_single_symbol: null source or i provided");
        return 0;
    }

#ifdef DEBUG_LEXER
    printf("ic_consume_single_symbol: calling add_token with len '%d'\n", 1);
#endif

    tokens = add_token(tokens, &(source[*i]), 1);
    ++ *i;
    return tokens;
}

/* consumes as many of the specified symbol `sym` in a row
 * adds this consumed unit as a token
 *
 * returns *tokens on success
 * returns 0 on error
 */
static struct ic_tokens * ic_consume_repeated_symbol(struct ic_tokens *tokens, char *source, unsigned int *i, char sym){
    unsigned int len = 0;

    if( ! i || ! source ){
        puts("ic_consume_repeated_symbol: null source or i provided");
        return 0;
    }

    for( len=0;
         sym == source[(*i) + len];
         ++len ){
        /* skip over very repeat of the symbol sym */
    }

#ifdef DEBUG_LEXER
    printf("ic_consume_repeated_symbol: calling add_token with len '%d'\n", len);
#endif

    tokens =  add_token(tokens, &(source[*i]), len);
    *i += len;

    return tokens;
}

/* consume comment
 * this checks for a beginning # (comment character) and will raise an error if not found
 * this will consume everything from current character up to first \n or EOF
 *
 * NB: ic_strip_comment is one of the few ic_consume_* functions that will throw an error
 * when a null tokens is provided
 *
 * returns token passes in on success
 * returns 0 on failure
 */
static struct ic_tokens * ic_strip_comment(struct ic_tokens *tokens, char *source, unsigned int *i){
    /* we cannot return a 0 tokens as that is how we indicate error
     * for now we will raise an error
     * consider later on instead creating a new tokens when this happens and returning that
     */
    if( ! tokens ){
        puts("ic_strip_comment: no tokens supplied");
        return 0;
    }

    if( ! i || ! source ){
        puts("ic_strip_comment: null source or i provided");
        return 0;
    }

    if( source[*i] != '#' ){
        printf("ic_strip_comment: this does not look like a comment;\n expected '#'\n got '%c'\n", source[*i]);
        return 0;
    }

    for( ; ; ++*i ){
        switch( source[*i] ){
            case '\n':
            case '\0':
                /* end of comment */
                goto COMMENT_END;
                break;

            default:
                /* keep going */
                break;
        }
    }

COMMENT_END:

    /* we could instead leave the comments in adding them as a token
     * following by the lexer inserting another token to signify comment end
     */
    return tokens;
}


