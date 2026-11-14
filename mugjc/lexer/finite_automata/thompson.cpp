//
// Created by moamen on 11/13/26.
//

#include "thompson.hpp"

#include <stdexcept>
#include <utility>

namespace mug::lexer::thompson {
using FA::NFA;
using FA::StateId;

NFA Lambda() {
    NFA nfa;
    const StateId s = nfa.AddState();
    const StateId f = nfa.AddState();
    nfa.AddLambda(s, f);
    nfa.set_start_state(s);
    nfa.set_final_state(f);
    return nfa;
}

NFA Literal(FA::Terminal on) {
    return Range(on, on);
}

NFA Range(FA::Terminal lo, FA::Terminal hi) {
    if (lo > hi) {
        throw std::invalid_argument("thompson: empty character range");
    }
    NFA nfa;
    const StateId s = nfa.AddState();
    const StateId f = nfa.AddState();
    for (int c = lo; c <= hi; ++c) {
        nfa.AddTransition(s, static_cast<FA::Terminal>(c), f);
    }
    nfa.set_start_state(s);
    nfa.set_final_state(f);
    return nfa;
}

// a.start ... a.final -λ-> b.start ... b.final
NFA Concatenation(NFA a, NFA b) {
    const StateId a_final = a.final_state();
    const StateId offset = a.Absorb(b);
    a.AddLambda(a_final, b.start_state() + offset);
    a.set_final_state(b.final_state() + offset);
    return a;
}

// new start branches to both, both join into a new final
NFA Union(NFA a, NFA b) {
    NFA nfa;
    const StateId s = nfa.AddState();
    const StateId oa = nfa.Absorb(a);
    const StateId ob = nfa.Absorb(b);
    const StateId f = nfa.AddState();
    nfa.AddLambda(s, a.start_state() + oa);
    nfa.AddLambda(s, b.start_state() + ob);
    nfa.AddLambda(a.final_state() + oa, f);
    nfa.AddLambda(b.final_state() + ob, f);
    nfa.set_start_state(s);
    nfa.set_final_state(f);
    return nfa;
}

// loop back from a.final to a.start, with a bypass for zero repetitions
NFA Star(NFA a) {
    NFA nfa;
    const StateId s = nfa.AddState();
    const StateId oa = nfa.Absorb(a);
    const StateId f = nfa.AddState();
    nfa.AddLambda(s, a.start_state() + oa);
    nfa.AddLambda(s, f);
    nfa.AddLambda(a.final_state() + oa, a.start_state() + oa);
    nfa.AddLambda(a.final_state() + oa, f);
    nfa.set_start_state(s);
    nfa.set_final_state(f);
    return nfa;
}

// same as Star but without the bypass: at least one pass is required
NFA Plus(NFA a) {
    NFA nfa;
    const StateId s = nfa.AddState();
    const StateId oa = nfa.Absorb(a);
    const StateId f = nfa.AddState();
    nfa.AddLambda(s, a.start_state() + oa);
    nfa.AddLambda(a.final_state() + oa, a.start_state() + oa);
    nfa.AddLambda(a.final_state() + oa, f);
    nfa.set_start_state(s);
    nfa.set_final_state(f);
    return nfa;
}
}  // namespace mug::lexer::thompson
