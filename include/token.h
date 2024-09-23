/**
 * @file token.h
 * 
 * @author Marek Hric
 * @author Adam Veselý
 * 
 * @brief Header file for tokens.
 */

typedef struct dyn_str;

typedef enum {
    T_KW,
    T_ID,
    T_AT,
    
    T_INT,
    T_F64,
    T_STR,

    T_ASGN, T_EQ,
    T_BANG, T_NEQ,
    T_LESS, T_LEQ,
    T_MORE, T_MEQ,


    T_PLUS, T_PASGN,
    T_MINUS, T_MASGN,
    T_MUL, T_MULASGN,
    T_DIV, T_DASGN,

    T_LPAREN, T_RPAREN,
    T_LBRACE, T_RBRACE,
    T_LBRACKET, T_RBRACKET,

    T_COMMA,
    T_DOT,
    T_SEMICOL,

    T_EOF
} TokenType;

typedef enum {
    KW_CONST,
    KW_IF,
    KW_ELSE,
    KW_FN,
    KW_I32,
    KW_F64,
    KW_NULL,
    KW_PUB,
    KW_RETURN,
    KW_U8,
    KW_VAR,
    KW_VOID,
    KW_WHILE,
    KW_IMPORT
} KeyWordType;

/**
 * @brief Token value structure.
 * 
 * @param int_value Integer value of token value.
 * @param float_value Float value of token value.
 * @param string_value Dynamic string value of token value.
 * @param keyword Keyword type of token value.
 */

typedef union {
    int int_value;
    float float_value;
    dyn_str* string_value;
    KeyWordType keyword;
} TokenValue;

/**
 * @brief Token structure.
 * 
 * @param type Token type of the token.
 * @param value Token value of the token.
 * @param line Line where the token is located.
 */
typedef struct {
    TokenType type;
    TokenValue value;
    int line;
} Token;

/*** End of file token.h ***/