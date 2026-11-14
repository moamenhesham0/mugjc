//
// Created by moamen on 11/13/26.
//

#ifndef MUGJC_THOMPSON_HPP
#define MUGJC_THOMPSON_HPP
#include "nfa.hpp"

namespace mug::lexer::thompson {
// Every function returns an NFA fragment with one start and one final state.

// Matches the empty string.
mug::lexer::FA::NFA Lambda();

// Matches exactly one character.
mug::lexer::FA::NFA Literal(mug::lexer::FA::Terminal on);

// Matches exactly one character in [lo, hi].
mug::lexer::FA::NFA Range(mug::lexer::FA::Terminal lo,
                          mug::lexer::FA::Terminal hi);

mug::lexer::FA::NFA Concatenation(mug::lexer::FA::NFA a, mug::lexer::FA::NFA b);

mug::lexer::FA::NFA Union(mug::lexer::FA::NFA a, mug::lexer::FA::NFA b);

mug::lexer::FA::NFA Star(mug::lexer::FA::NFA a);

mug::lexer::FA::NFA Plus(mug::lexer::FA::NFA a);
}  // namespace mug::lexer::thompson

#endif  // MUGJC_THOMPSON_HPP
