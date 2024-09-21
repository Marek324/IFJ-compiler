typedef struct dyn_str;

enum TokenType {
};

enum KeyWordType {
};

union TokenValue {
    int int_value;
    float float_value;
    dyn_str *string_value;
    KeyWordType keyword;
};
    

typedef struct {
    TokenType type;
    TokenValue value;
    int line;
} Token;