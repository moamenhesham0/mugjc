//
// Created by moamen on 11/13/26.
//

#ifndef MUGJC_LEXER_RULES_HPP
#define MUGJC_LEXER_RULES_HPP
#include <string>
#include <string_view>

#include "../ast/ast.hpp"
#include "../types.hpp"

namespace mug::lexer::rule {

struct Rule {
    RuleType type;
    std::string tokenClass_;
    size_t priority_;
    mug::lexer::AST ast_;
};

mug::lexer::rule::Rule CreateRegexRule(const std::string_view token_class,
                                       std::string regex_body, size_t priority);

mug::lexer::rule::Rule CreatePunctuationRule(
    const mug::lexer::rule::Literal punctuation_symbol, size_t priority);

mug::lexer::rule::Rule CreateKeywordRule(const std::string_view keyword,
                                         size_t priority);
}  // namespace mug::lexer::rule

#endif  // MUGJC_LEXER_RULES_HPP
