//
// Created by moamen on 11/13/26.
//

#ifndef MUGJC_THOMPSON_HPP
#define MUGJC_THOMPSON_HPP
#include "nfa.hpp"

namespace mug::lexer::thompson {

mug::lexer::FA::NFA Concatenation(mug::lexer::FA::NFA a, mug::lexer::FA::NFA b);

mug::lexer::FA::NFA Star(mug::lexer::FA::NFA a);

mug::lexer::FA::NFA Plus(mug::lexer::FA::NFA a);

mug::lexer::FA::NFA Union(mug::lexer::FA::NFA a, mug::lexer::FA::NFA b);

mug::lexer::FA::NFA Lambda(mug::lexer::FA::NFA a, mug::lexer::FA::NFA b);

};  // namespace mug::lexer::thompson

#endif  // MUGJC_THOMPSON_HPP
