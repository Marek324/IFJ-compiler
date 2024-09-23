/**
 * @file token.h
 * 
 * @author Marek Hric
 * @author Adam Veselý
 * 
 * @brief Header file for tokens.
 */

typedef struct dyn_str;

/**
 * @brief Token type enumeration.
 * 
 * @param T_KW Keyword token.
 * @param T_ID Identifier token.
 * @param T_AT At token.
 * 
 * @param T_INT Integer token.
 * @param T_F64 Float token.
 * @param T_STR String token.
 * 
 * @param T_ASGN Assignment token.
 * @param T_EQ Equality token.
 * @param T_BANG Negation token.
 * @param T_NEQ Not equal token.
 * @param T_LESS Less than token.
 * @param T_LEQ Less or equal token.
 * @param T_MORE More than token.
 * @param T_MEQ More or equal token.
 * 
 * @param T_PLUS Addition token.
 * @param T_PASGN Plus assignment token.
 * @param T_MINUS Subtraction token.
 * @param T_MASGN Minus assignment token.
 * @param T_MUL Multiplication token.
 * @param T_MULASGN Multiplication assignment token.
 * @param T_DIV Division token.
 * @param T_DASGN Division assignment token.
 * 
 * @param T_LPAREN Left parenthesis token.
 * @param T_RPAREN Right parenthesis token.
 * @param T_LBRACE Left brace token.
 * @param T_RBRACE Right brace token.
 * @param T_LBRACKET Left bracket token.
 * @param T_RBRACKET Right bracket token.
 * 
 * @param T_COMMA Comma token.
 * @param T_DOT Dot token.
 * @param T_SEMICOL Semicolon token.
 * 
 * @param T_EOF End of file token.
 */
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

/**
 * @brief Keyword type enumeration.
 * 
 * @param KW_CONST Constant keyword.
 * @param KW_IF If keyword.
 * @param KW_ELSE Else keyword.
 * @param KW_FN Function keyword.
 * @param KW_I32 Integer 32 keyword.
 * @param KW_F64 Float 64 keyword.
 * @param KW_NULL Null keyword.
 * @param KW_PUB Public keyword.
 * @param KW_RETURN Return keyword.
 * @param KW_U8 Unsigned 8 keyword.
 * @param KW_VAR Variable keyword.
 * @param KW_VOID Void keyword.
 * @param KW_WHILE While keyword.
 * @param KW_IMPORT Import keyword.
 */
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