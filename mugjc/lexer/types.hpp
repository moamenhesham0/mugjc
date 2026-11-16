#ifndef MUGJC_LEXER_TYPES_HPP
#define MUGJC_LEXER_TYPES_HPP

#include <array>
#include <concepts>
#include <cstddef>
#include <limits>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace mug::lexer {

using Literal = unsigned char;
using StateId = std::size_t;

inline constexpr StateId TrapState = std::numeric_limits<StateId>::max();
inline constexpr std::size_t AlphabetSize =
    std::numeric_limits<Literal>::max() + 1;
inline constexpr Literal Lambda = 'L';

namespace rule {

using mug::lexer::Literal;
using mug::lexer::StateId;

static constexpr unsigned char LAMBDA = 'L';

enum class Symbol {
    kRegularExpression = ':',
    kRegularDefinition = '=',
    kLeftSquare = '[',
    kRightSquare = ']',
    kLeftCurly = '{',
    kRightCurly = '}',
    kEscape = '\\',
    kUnderscore = '_',
};

enum class REOperator {
    kLeftParenthesis = '(',
    kRightParenthesis = ')',
    kPlus = '+',
    kStar = '*',
    kRange = '-',
    kConcatenation = 0,
    kAlternate = '|'
};

inline const std::unordered_map<REOperator, int> REOperators = {
    {REOperator::kLeftParenthesis, 0},
    {REOperator::kRightParenthesis, 0},
    {REOperator::kAlternate, 1},
    {REOperator::kConcatenation, 2},
    {REOperator::kStar, 3},
    {REOperator::kPlus, 3},
    {REOperator::kRange, 4}};

inline bool IsREOperator(Literal literal) {
    return REOperators.contains(static_cast<REOperator>(literal));
}

inline bool HasPrecedence(const REOperator top, const REOperator incoming) {
    return REOperators.at(top) >= REOperators.at(incoming);
}

enum class RuleType {
    kRegularStatement,
    kRegularExpression,
    kRegularDefinition,
    kPunctuation,
    kKeyword,
    kUnknown,
};

}  // namespace rule
}  // namespace mug::lexer

#endif  // MUGJC_LEXER_TYPES_HPP
