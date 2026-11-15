#ifndef MUGJC_LEXER_TYPES_HPP
#define MUGJC_LEXER_TYPES_HPP

#include <array>
#include <concepts>
#include <cstddef>
#include <limits>
#include <string>
#include <type_traits>
#include <unordered_set>

namespace mug::lexer {

using Literal = unsigned char;
using StateId = std::size_t;

inline constexpr StateId TrapState = std::numeric_limits<StateId>::max();
inline constexpr std::size_t AlphabetSize = 256;
inline constexpr unsigned char Lambda = 'L';

template <typename E>
concept CharEnum =
    std::is_enum_v<E> && std::is_convertible_v<std::underlying_type_t<E>, char>;

template <CharEnum E>
constexpr bool operator==(char c, E e) noexcept {
    return c == static_cast<char>(e);
}
template <CharEnum E>
constexpr bool operator==(E e, char c) noexcept {
    return static_cast<char>(e) == c;
}
template <CharEnum E>
constexpr bool operator!=(char c, E e) noexcept {
    return c != static_cast<char>(e);
}
template <CharEnum E>
constexpr bool operator!=(E e, char c) noexcept {
    return static_cast<char>(e) != c;
}

namespace rule {

using mug::lexer::CharEnum;
using mug::lexer::Literal;
using mug::lexer::StateId;
using mug::lexer::operator==;
using mug::lexer::operator!=;

static constexpr unsigned char LAMBDA = 'L';

enum class Symbol {
    kRegularExpression = ':',
    kRegularDefinition = '=',
    kKleeneStar = '*',
    kKleenePlus = '+',
    kAlternation = '|',
    kRange = '-',
    kLeftSquare = '[',
    kRightSquare = ']',
    kLeftCurly = '{',
    kRightCurly = '}',
    kLeftParen = '(',
    kRightParen = ')',
    kEscape = '\\',
};

enum class REOperator {
    kPlus = '+',
    kStar = '*',
    kRange = '-',
    kConcatenation = 0,
    kAlternate = '|'
};

enum REOpPrecedence {
    kPlus,
    kStar,
    kRange,
    kConcatenation,
    kAlternate,
};

inline const std::unordered_set<REOperator> REOperators = {
    REOperator::kPlus, REOperator::kStar, REOperator::kRange,
    REOperator::kAlternate};

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
