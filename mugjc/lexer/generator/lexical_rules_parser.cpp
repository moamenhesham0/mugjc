//
// Created by moamen on 11/4/26.
//
#include "lexical_rules_parser.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <optional>
#include <sstream>
#include <utility>

mug::lexer::LexicalRulesParser::LexicalRulesParser(const std::string_view rules)
    : rules_(rules), cursor_(0), priority_(0) {}

void mug::lexer::LexicalRulesParser::set_rules(const std::string_view rules) {
    this->rules_ = rules;
    this->cursor_ = 0;
}

static mug::lexer::rule::RuleType DefineRule(const std::string_view raw_rule) {
    if (raw_rule.empty()) {
        return mug::lexer::rule::RuleType::kUnknown;
    }

    if (raw_rule.front() ==
        std::to_underlying(mug::lexer::rule::Symbol::kLeftCurly)) {
        return mug::lexer::rule::RuleType::kKeyword;
    }

    if (raw_rule.front() ==
        std::to_underlying(mug::lexer::rule::Symbol::kLeftSquare)) {
        return mug::lexer::rule::RuleType::kPunctuation;
    }

    if (isalpha(raw_rule.front())) {
        return mug::lexer::rule::RuleType::kRegularStatement;
    }

    return mug::lexer::rule::RuleType::kUnknown;
}

mug::lexer::rule::Rule mug::lexer::LexicalRulesParser::KeywordRule(
    const std::string_view raw_rule) {
    if (raw_rule.back() !=
        std::to_underlying(mug::lexer::rule::Symbol::kRightCurly)) {
        // TODO Throw ERROR
    }

    auto content = raw_rule.substr(1, raw_rule.size() - 2);
    std::istringstream iss{std::string(content)};
    std::vector<std::string> keywords{std::istream_iterator<std::string>{iss},
                                      {}};

    for (auto& keyword : keywords) {
        if (!IsValidIdentifier(keyword)) {
            // TODO ERROR
        }
        auto rule =
            mug::lexer::rule::CreateKeywordRule(keyword, this->priority_++);
        this->pending_rules_.push(std::move(rule));
    }

    auto popped_rule = std::move(this->pending_rules_.front());
    this->pending_rules_.pop();
    return popped_rule;
}

mug::lexer::rule::Rule mug::lexer::LexicalRulesParser::LiteralPunctuationRule(
    const rule::Literal symbol) {
    if (isalnum(symbol)) {
        // TODO ERROR
    }

    if (rule::IsREOperator(symbol) || symbol ==
        std::to_underlying(mug::lexer::rule::Symbol::kEscape)) {
        // TODO ERROR
    }

    return mug::lexer::rule::CreatePunctuationRule(symbol, this->priority_++);
}

mug::lexer::rule::Rule mug::lexer::LexicalRulesParser::EscapedPunctuationRule(
    const std::string_view symbol) {
    if (symbol.front() !=
        std::to_underlying(mug::lexer::rule::Symbol::kEscape)) {
        // TODO ERROR
    }

    if (!(rule::IsREOperator(symbol.back()) ||
        symbol.back() == std::to_underlying(rule::Symbol::kEscape))) {
        // TODO ERROR
    }

    return rule::CreatePunctuationRule(symbol.back(), this->priority_++);
}

mug::lexer::rule::Rule mug::lexer::LexicalRulesParser::PunctuationRule(
    const std::string_view raw_rule) {
    if (raw_rule.back() !=
        std::to_underlying(mug::lexer::rule::Symbol::kRightSquare)) {
        // TODO Throw ERROR
    }

    const auto content = raw_rule.substr(1, raw_rule.size() - 2);
    std::istringstream iss{std::string(content)};
    const std::vector<std::string> symbols{
        std::istream_iterator<std::string>{iss}, {}};

    for (auto& symbol : symbols) {
        switch (symbol.size()) {
            case 1:
                this->pending_rules_.push(
                    LiteralPunctuationRule(symbol.front()));
                break;
            case 2:
                this->pending_rules_.push(EscapedPunctuationRule(symbol));
                break;
            default:
                // TODO ERROR
                break;
        }
    }

    auto popped_rule = std::move(this->pending_rules_.front());
    this->pending_rules_.pop();
    return popped_rule;
}

bool mug::lexer::LexicalRulesParser::IsValidIdentifier(std::string_view name) {
    if (name.empty()) {
        return false;
    }

    if (!std::isalpha(static_cast<Literal>(name.front())) &&
        name.front() != '_') {
        return false;
    }

    return std::ranges::all_of(name, [](char c) {
        return std::isalnum(static_cast<Literal>(c)) || c == '_';
    });
}

std::string mug::lexer::LexicalRulesParser::TokenClass(
    const std::string_view raw_rule) {
    const size_t token_class_size = raw_rule.find_first_of(":=");
    if (token_class_size == std::string_view::npos) {
        // TODO ERROR
        return "";
    }

    std::string_view token_class = raw_rule.substr(0, token_class_size);

    // Trim whitespace around the name
    while (!token_class.empty() &&
           std::isspace(static_cast<Literal>(token_class.front()))) {
        token_class.remove_prefix(1);
    }
    while (!token_class.empty() &&
           std::isspace(static_cast<Literal>(token_class.back()))) {
        token_class.remove_suffix(1);
    }

    if (!IsValidIdentifier(token_class)) {
        // TODO ERROR
    }

    return std::string(token_class);
}

static std::string RemoveWhitespace(std::string token_body) {
    std::erase_if(token_body, [](unsigned char c) { return std::isspace(c); });
    return token_body;
}

static mug::lexer::rule::RuleType RegularStatementType(
    std::string_view raw_rule) {
    size_t symbol_index = raw_rule.find_first_of("=:");

    if (symbol_index == std::string_view::npos) {
        return mug::lexer::rule::RuleType::kUnknown;
    }

    if (raw_rule[symbol_index] ==
        std::to_underlying(mug::lexer::rule::Symbol::kRegularExpression)) {
        return mug::lexer::rule::RuleType::kRegularExpression;
    }

    return mug::lexer::rule::RuleType::kRegularDefinition;
}

void mug::lexer::LexicalRulesParser::AddDefinition(
    const std::string& token_class, const std::string& token_body) {
    if (this->definitions_.contains(token_class)) {
        // TODO ERROR
    }

    this->definitions_[token_class] = token_body;
}

std::string mug::lexer::LexicalRulesParser::ReplaceDefinitionWithBody(
    const std::string_view token_body) {
    std::string new_token_body;
    std::string definition_class;

    for (auto& c : token_body) {
        if (std::isalnum(c) ||
            c == std::to_underlying(rule::Symbol::kUnderscore)) {
            definition_class += c;
            continue;
        }

        auto definition_body = this->definitions_[definition_class];

        if (definition_body.empty()) {
            new_token_body += definition_class + c;
        } else {
            new_token_body += "(" + definition_body + ")" + c;
        }

        definition_class = "";
    }

    auto definition_body = this->definitions_[definition_class];

    if (definition_body.empty()) {
        new_token_body += definition_class;
    } else {
        new_token_body += "(" + definition_body + ")";
    }

    return new_token_body;
}

mug::lexer::ASTNode mug::lexer::LexicalRulesParser::BuildRegexAST(
    std::string token_body, rule::RuleType type) {
    std::vector<mug::lexer::rule::REOperator> reop_stack;
    std::vector<mug::lexer::ASTNode> node_stack;

    token_body = RemoveWhitespace(token_body);

    if (type == rule::RuleType::kRegularExpression) {
        token_body = ReplaceDefinitionWithBody(token_body);
    }

    bool escaped = false;
    for (auto& c : token_body) {
        if (c == std::to_underlying(rule::Symbol::kEscape)) {
            escaped = true;
            continue;
        }

        if (escaped && (rule::IsREOperator(c) ||
                        c == std::to_underlying(rule::Symbol::kEscape))) {
        }
    }
}

std::optional<mug::lexer::rule::Rule>
mug::lexer::LexicalRulesParser::RegularStatement(std::string_view raw_rule) {
    std::string token_class = TokenClass(raw_rule);
    rule::RuleType type = RegularStatementType(raw_rule);

    if (type == rule::RuleType::kUnknown) {
        // TODO ERROR
    }

    std::string token_body =
        std::string(raw_rule.substr(raw_rule.find_first_of("=:")));

    auto ast = BuildRegexAST(token_body, type);

    if (type == rule::RuleType::kRegularDefinition) {
        AddDefinition(token_class, token_body);
        return std::nullopt;
    }

    return rule::CreateRegexRule(token_class, std::move(ast),
                                 this->priority_++);
}

std::optional<mug::lexer::rule::Rule>
mug::lexer::LexicalRulesParser::NextRule() {
    auto start = cursor_;
    auto end = rules_.find('\n', cursor_);
    cursor_ = end + 1;
    const std::string_view raw_rule = rules_.substr(start, end);

    // Define type
    switch (DefineRule(raw_rule)) {
        case mug::lexer::rule::RuleType::kPunctuation:
            return PunctuationRule(raw_rule);

        case mug::lexer::rule::RuleType::kKeyword:
            return KeywordRule(raw_rule);

        case mug::lexer::rule::RuleType::kRegularStatement:
            return RegularStatement(raw_rule).value();

        default:
            // TODO THROW ERROR
            break;
    }

    return std::nullopt;
}
