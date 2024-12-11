#pragma once

enum class TokenType {
    exit,
    int_lit,
    semi,
    open_paren,
    close_paren,
    ident,
    let,
    eq,
    plus,
    star,
    div,
    sub
};

std::optional<int> bin_prec(TokenType type) {
    switch (type) {
        case TokenType::sub:
        case TokenType::plus:
            return 0;
            
        case TokenType::div:
        case TokenType::star:
            return 1;

        default: 
            return {};
    }
}