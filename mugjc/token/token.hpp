//
// Created on 11/14/26.
//

#ifndef MUGJC_TOKEN_HPP
#define MUGJC_TOKEN_HPP
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

namespace mug::token {
// The token class whose lexemes are stored in the symbol table.
static constexpr std::string_view kIdentifier = "id";

struct Token {
    std::string type;    // token class: "id", "num", "if", ";", ...
    std::string lexeme;  // the matched text
    std::optional<size_t> symbol_index;  // symbol table index (identifiers)
};
}  // namespace mug::token

#endif  // MUGJC_TOKEN_HPP
