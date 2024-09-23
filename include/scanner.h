/**
 * @file scanner.h
 * 
 * @author Marek Hric
 * 
 * @brief Header file for the scanner.
 */

enum State {
    S_START,

    //OPERATORS
    S_EQ, 
    S_BANG,
    S_LESS,
    S_MORE,

    //KEYWORDS
    S_POS_KW_I, S_POS_KW_IF,
                S_POS_KW_IN, S_POS_KW_INC, S_POS_KW_INCL, S_POS_KW_INCLU, S_POS_KW_INCLUD, S_POS_KW_INCLUDE,
    S_POS_KW_E, S_POS_KW_EL, S_POS_KW_ELS, S_POS_KW_ELSE,
    S_POS_KW_W, S_POS_KW_WH, S_POS_KW_WHI, S_POS_KW_WHIL, S_POS_KW_WHILE,

    //IDENTIFIERS
    S_IDENTIFIER,

    //LITERALS
    S_STR_LITERAL, S_STR_LITERAL_BSLASH,
    S_CHAR_LITERAL, S_CHAR_LITERAL_BSLASH,

    //COMMENTS
    S_POS_COMM,
    S_COMM,
    S_POS_MULTILINE_COMM,    
};

typedef struct Token;

// Function prototypes
Token* get_token();
void free_token(Token* token);

/*** End of file scanner.h ***/