//
// Created by moamen on 11/4/26.
//

#include "lexical_rules_parser.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "../finite_automata/thompson.hpp"

namespace mug::lexer {
namespace {
constexpr std::string_view kBlank = " \t\r\n";

std::string_view Trim(std::string_view text) {
    const size_t first = text.find_first_not_of(kBlank);
    if (first == std::string_view::npos) {
        return {};
    }
    const size_t last = text.find_last_not_of(kBlank);
    return text.substr(first, last - first + 1);
}

// Position of the first unescaped '=' or ':' (the name / regex separator).
size_t FindSeparator(std::string_view line) {
    for (size_t i = 0; i < line.size(); ++i) {
        if (line[i] == static_cast<char>(rule::Symbol::kEscape)) {
            ++i;  // skip the escaped character
        } else if (line[i] ==
                       static_cast<char>(rule::Symbol::kRegularDefinition) ||
                   line[i] ==
                       static_cast<char>(rule::Symbol::kRegularExpression)) {
            return i;
        }
    }
    return std::string_view::npos;
}

std::vector<std::string> SplitWords(std::string_view text) {
    std::istringstream stream{std::string(text)};
    std::vector<std::string> words;
    for (std::string word; stream >> word;) {
        words.push_back(std::move(word));
    }
    return words;
}
}  // namespace

// ---------------------------------------------------------------------------
// LexicalRulesParser
// ---------------------------------------------------------------------------

LexicalRulesParser::LexicalRulesParser(const std::string_view rules)
    : rules_(rules), cursor_(0) {}

void LexicalRulesParser::set_rules(std::string_view rules) {
    this->rules_ = rules;
    this->cursor_ = 0;
}

bool LexicalRulesParser::HasNext() const {
    return rules_.find_first_not_of(kBlank, cursor_) != std::string_view::npos;
}

std::string_view LexicalRulesParser::NextLine() {
    while (cursor_ < rules_.size()) {
        size_t end = rules_.find('\n', cursor_);
        if (end == std::string_view::npos) {
            end = rules_.size();  // last line without a trailing newline
        }
        const std::string_view line =
            Trim(rules_.substr(cursor_, end - cursor_));
        cursor_ = std::min(end + 1, rules_.size());
        if (!line.empty()) {
            return line;
        }
    }
    return {};
}

rule::RuleType LexicalRulesParser::RuleTypeOf(std::string_view line) {
    if (line.front() == '{' && line.back() == '}') {
        return rule::RuleType::kKeywords;
    }
    if (line.front() == '[' && line.back() == ']') {
        return rule::RuleType::kPunctuation;
    }
    const size_t sep = FindSeparator(line);
    if (sep == std::string_view::npos) {
        return rule::RuleType::kRegularStatement;
    }
    return line[sep] == static_cast<char>(rule::Symbol::kRegularDefinition)
               ? rule::RuleType::kRegularDefinition
               : rule::RuleType::kRegularExpression;
}

std::vector<rule::Rule> LexicalRulesParser::NextRule() {
    std::vector<rule::Rule> parsed_rule;

    const std::string_view line = NextLine();
    if (line.empty()) {
        return parsed_rule;
    }

    const rule::RuleType type = RuleTypeOf(line);
    switch (type) {
        case rule::RuleType::kKeywords:
            for (std::string& word :
                 SplitWords(line.substr(1, line.size() - 2))) {
                parsed_rule.push_back({type, std::move(word), ""});
            }
            break;
        case rule::RuleType::kPunctuation:
            for (const std::string& word :
                 SplitWords(line.substr(1, line.size() - 2))) {
                // "\;" -> ';'. Any other word contributes its first character.
                const char c =
                    (word[0] == static_cast<char>(rule::Symbol::kEscape) &&
                     word.size() > 1)
                        ? word[1]
                        : word[0];
                parsed_rule.push_back({type, std::string(1, c), ""});
            }
            break;
        case rule::RuleType::kRegularDefinition:
        case rule::RuleType::kRegularExpression: {
            const size_t sep = FindSeparator(line);
            parsed_rule.push_back({type, std::string(Trim(line.substr(0, sep))),
                                   std::string(Trim(line.substr(sep + 1)))});
            break;
        }
        case rule::RuleType::kRegularStatement:
            throw std::invalid_argument("bad rule line: " + std::string(line));
    }
    return parsed_rule;
}

// ---------------------------------------------------------------------------
// RegexParser
// ---------------------------------------------------------------------------

RegexParser::RegexParser(std::string_view regex, const Definitions& definitions)
    : regex_(regex), cursor_(0), definitions_(definitions) {}

FA::NFA RegexParser::Parse() {
    FA::NFA nfa = Alternation();
    if (More()) {
        throw std::invalid_argument("unmatched ')' in regex: " +
                                    std::string(regex_));
    }
    return nfa;
}

void RegexParser::SkipWhitespace() {
    while (cursor_ < regex_.size() &&
           std::isspace(static_cast<unsigned char>(regex_[cursor_]))) {
        ++cursor_;
    }
}

bool RegexParser::More() {
    SkipWhitespace();
    return cursor_ < regex_.size();
}

FA::NFA RegexParser::Alternation() {
    FA::NFA a = Concatenation();
    while (More() && regex_[cursor_] == '|') {
        ++cursor_;
        a = thompson::Union(std::move(a), Concatenation());
    }
    return a;
}

FA::NFA RegexParser::Concatenation() {
    bool empty = true;
    FA::NFA a;
    // Stop at '|' or ')': they belong to the caller.
    while (More() && regex_[cursor_] != '|' && regex_[cursor_] != ')') {
        FA::NFA b = Unary();
        a = empty ? std::move(b)
                  : thompson::Concatenation(std::move(a), std::move(b));
        empty = false;
    }
    return empty ? thompson::Lambda() : a;
}

FA::NFA RegexParser::Unary() {
    FA::NFA a = Atom();
    while (More() && (regex_[cursor_] == '*' || regex_[cursor_] == '+')) {
        a = regex_[cursor_] == '*' ? thompson::Star(std::move(a))
                                   : thompson::Plus(std::move(a));
        ++cursor_;
    }
    return a;
}

// `c` has already been consumed. If a '-' follows, build the range c-<next>.
FA::NFA RegexParser::RangeOrLiteral(char c) {
    if (More() && regex_[cursor_] == '-') {
        const size_t save = cursor_;
        ++cursor_;
        SkipWhitespace();
        if (cursor_ < regex_.size()) {
            char end = regex_[cursor_];
            if (end == '\\' && cursor_ + 1 < regex_.size()) {
                end = regex_[++cursor_];
            }
            ++cursor_;
            return thompson::Range(static_cast<FA::Terminal>(c),
                                   static_cast<FA::Terminal>(end));
        }
        cursor_ = save;  // a trailing '-' is just a literal
    }
    return thompson::Literal(static_cast<FA::Terminal>(c));
}

FA::NFA RegexParser::Atom() {
    SkipWhitespace();
    if (cursor_ >= regex_.size()) {
        throw std::invalid_argument("unexpected end of regex: " +
                                    std::string(regex_));
    }
    const char c = regex_[cursor_];

    // ( group )
    if (c == '(') {
        ++cursor_;
        FA::NFA a = Alternation();
        if (!More() || regex_[cursor_] != ')') {
            throw std::invalid_argument("missing ')' in regex: " +
                                        std::string(regex_));
        }
        ++cursor_;
        return a;
    }

    // \x : escaped character, or \L for lambda
    if (c == '\\') {
        ++cursor_;
        if (cursor_ >= regex_.size()) {
            throw std::invalid_argument("dangling '\\' in regex: " +
                                        std::string(regex_));
        }
        const char escaped = regex_[cursor_++];
        if (escaped == 'L') {
            return thompson::Lambda();
        }
        return RangeOrLiteral(escaped);
    }

    // word: a defined name, or plain literal characters
    if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
        size_t end = cursor_;
        while (end < regex_.size() &&
               (std::isalnum(static_cast<unsigned char>(regex_[end])) ||
                regex_[end] == '_')) {
            ++end;
        }
        const std::string word(regex_.substr(cursor_, end - cursor_));
        if (auto it = definitions_.find(word); it != definitions_.end()) {
            cursor_ = end;
            return it->second;  // copy: every use gets its own states
        }
        // All but the last character are plain literals ...
        FA::NFA a;
        bool empty = true;
        for (size_t i = 0; i + 1 < word.size(); ++i) {
            FA::NFA b = thompson::Literal(static_cast<FA::Terminal>(word[i]));
            a = empty ? std::move(b)
                      : thompson::Concatenation(std::move(a), std::move(b));
            empty = false;
        }
        // ... the last one may start a range (a-z).
        cursor_ = end;
        FA::NFA last = RangeOrLiteral(word.back());
        return empty ? last : thompson::Concatenation(std::move(a), last);
    }

    // any other symbol is a literal (e.g. '.', '!', '<')
    ++cursor_;
    return thompson::Literal(static_cast<FA::Terminal>(c));
}
}  // namespace mug::lexer
