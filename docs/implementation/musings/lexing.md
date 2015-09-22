# lexer musings


## present

the current lexer code outputs a char array of space-separated string tokens

this forces both parse and analyse to do string/symbol comparisons


## future

ideally instead we would have an enum of possible syntactical elements

    enum token_id {
        IC_NEWLINE,
        IC_IF,
        IC_ELSE,
        IC_END,
        IC_RETURN,
        ...
    };

and then a more powerful token type

    struct token {
        enum token_id id;
        char * line;
        // offset of this token into line
        unsigned int line_location;
        // offset of this line into file
        unsigned int line_number;
        char * file;
    };

of course we will still need some additional data:

    * strings and symbols for thing such as identifiers (function, type, argument and variables names).
    * literal values for strings
    * literal values for integers and floats

this yields:

    struct token {
        enum token_id id;
        char * line;
        // offset of this token into line
        unsigned int line_location;
        // offset of this line into file
        unsigned int line_number;
        char * file;

        union {
            char * string;
            int integer;
            float floating;
        } u;
    };


