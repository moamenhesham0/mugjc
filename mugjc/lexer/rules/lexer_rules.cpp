//
// Created by moamen on 11/13/26.
//

#include "lexer_rules.hpp"

namespace mug::lexer::rule {

mug::lexer::rule::Rule CreateRegexRule(const std::string_view token_class,
                                       std::string regex_body,
                                       size_t priority) {
    return Rule{
        .type = RuleType::kRegularExpression,
        .tokenClass_ = std::string(token_class),
        .priority_ = priority,
        .ast_ = mug::lexer::AST(regex_body),
    };
}

mug::lexer::rule::Rule CreatePunctuationRule(
    const mug::lexer::rule::Literal punctuation_symbol, size_t priority) {
    std::string symbol_str(1, static_cast<char>(punctuation_symbol));
    return Rule{
        .type = RuleType::kPunctuation,
        .tokenClass_ = symbol_str,
        .priority_ = priority,
        .ast_ = mug::lexer::AST(symbol_str),
    };
}

mug::lexer::rule::Rule CreateKeywordRule(const std::string_view keyword,
                                         size_t priority) {
    return Rule{
        .type = RuleType::kKeyword,
        .tokenClass_ = std::string(keyword),
        .priority_ = priority,
        .ast_ = mug::lexer::AST(keyword),
    };
}

}  // namespace mug::lexer::rule
