/**
 * \file token.h
 * 
 * \author Marek Hric
 * \author Adam Veselý
 * 
 * \brief Header file for tokens.
 */

typedef struct dyn_str;

enum TokenType {
};

enum KeyWordType {
};

/**
 * \brief Token value structure.
 * 
 * \param int_value Integer value of token value.
 * \param float_value Float value of token value.
 * \param string_value Dynamic string value of token value.
 * \param keyword Keyword type of token value.
 */
union TokenValue {
    int int_value;
    float float_value;
    dyn_str* string_value;
    KeyWordType keyword;
};

/**
 * \brief Token structure.
 * 
 * \param type Token type of the token.
 * \param value Token value of the token.
 * \param line Line where the token is located.
 */
typedef struct {
    TokenType type;
    TokenValue value;
    int line;
} Token;

/*** End of file token.h ***/