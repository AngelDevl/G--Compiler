#pragma once

#include <iostream>
#include <vector>

#include "Token.hpp"


class Tokenizer {

public:
    inline explicit Tokenizer(std::string src) 
        : m_src(std::move(src)) {}

    inline std::vector<Token> tokenize() {
        std::string buf;
        std::vector<Token> tokens;

        while(peek().has_value()) {
            if (std::isalpha(peek().value())) {
                buf.push_back(consume());
                while(peek().has_value() && std::isalnum(peek().value())) {
                    buf.push_back(consume());
                }

                if (buf == "exit") {
                    tokens.push_back({TokenType::exit});
                    buf.clear();
                } 
                else if (buf == "let") {
                    tokens.push_back({TokenType::let});
                    buf.clear();
                }
                else {
                    tokens.push_back({TokenType::ident, buf});
                    buf.clear();
                } 
            }
            else if (std::isdigit(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && std::isdigit(peek().value())) {
                    buf.push_back(consume());
                }

                tokens.push_back({TokenType::int_lit, buf});
                buf.clear();
            } 
            else if (peek().value() == '(') {
                tokens.push_back({TokenType::open_paren});
                consume();
            }
            else if (peek().value() == ')') {
                tokens.push_back({TokenType::close_paren});
                consume();
            }
            else if (peek().value() == ';') {
                tokens.push_back({TokenType::semi});
                consume();
            }
            else if (peek().value() == '=') {
                tokens.push_back({TokenType::eq});
                consume();
            }
            else if (peek().value() == '+') {
                tokens.push_back({TokenType::plus});
                consume();
            }
            else if (peek().value() == '*') {
                tokens.push_back({TokenType::star});
                consume();
            }
            else if (peek().value() == '/') {
                tokens.push_back({TokenType::div});
                consume();
            }
            else if (peek().value() == '-') {
                tokens.push_back({TokenType::sub});
                consume();
            }

            else if (std::isspace(peek().value())) {
                consume();
            } 
            else {
                std::cerr << "You messed up!" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        m_index = 0;
        return tokens;
    }


private:

    [[nodiscard]] inline std::optional<char> peek(int offset = 0) const 
    {
        if (m_index + offset >= m_src.length()) {
            return {};
        } else {
            return m_src.at(m_index + offset);
        }
    }

    inline char consume() {
        return m_src.at(m_index++);
    }


    const std::string m_src;
    size_t m_index = 0;
};