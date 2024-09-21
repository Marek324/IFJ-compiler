enum TokenType {
};

enum KeyWordType {
};

typedef char* dyn_string; // TODO: implement dynamic string

union TokenValue {
    int int_value;
    float float_value;
    dyn_string string_value;
    KeyWordType keyword;
};
    

typedef struct {
    TokenType type;
    TokenValue value;
    int line;
} Token;