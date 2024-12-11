#pragma once

#include "TokenType.hpp"
#include <optional>
#include <string>


struct Token {
    TokenType type;
    std::optional<std::string> value {};
};