//
// Created by moamen on 11/4/26.
//

#ifndef MUGJC_LEXICAL_RULES_PARSER_HPP
#define MUGJC_LEXICAL_RULES_PARSER_HPP
#include <fstream>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../rules/lexer_rules.hpp"

namespace mug::lexer {
class LexicalRulesParser {
public:
    LexicalRulesParser(const std::string_view rules);

    ~LexicalRulesParser() = default;

    void set_rules(std::string_view rules);

    std::optional<mug::lexer::rule::Rule> NextRule();

private:
    mug::lexer::rule::RuleType RuleType(std::string_view rule);
    std::string_view rules_;
    size_t cursor_;
    size_t priority_;
    std::unordered_map<std::string, std::string> definitions_;
    std::queue<mug::lexer::rule::Rule> pending_rules_;

    mug::lexer::rule::Rule KeywordRule(std::string_view raw_rule);
    mug::lexer::rule::Rule PunctuationRule(std::string_view raw_rule);

    void AddDefinition(const std::string& token_class,
                       const std::string& token_body);
    std::string ReplaceDefinitionWithBody(std::string_view token_body);

    mug::lexer::ASTNode BuildRegexAST(std::string token_body,
                                        rule::RuleType rule_type);
    std::optional<mug::lexer::rule::Rule> RegularStatement(
        std::string_view raw_rule);
    mug::lexer::rule::Rule LiteralPunctuationRule(rule::Literal symbol);
    mug::lexer::rule::Rule EscapedPunctuationRule(std::string_view symbol);
    std::string TokenClass(std::string_view raw_rule);
    bool IsValidIdentifier(std::string_view name);
};
}  // namespace mug::lexer

#endif  // MUGJC_LEXICAL_RULES_PARSER_HPP
