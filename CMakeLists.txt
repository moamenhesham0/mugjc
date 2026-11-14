cmake_minimum_required(VERSION 3.16)
project(mugjc CXX)

add_executable(new_target mugjc/mugjc.cpp
        mugjc/io/io.cpp
        mugjc/io/io.hpp
        mugjc/grammar/parser.cpp
        mugjc/grammar/parser.hpp
        mugjc/lexer/generator/lexical_analyzer_generator.cpp
        mugjc/lexer/generator/lexical_analyzer_generator.hpp
        mugjc/lexer/generator/lexical_rules_parser.cpp
        mugjc/lexer/generator/lexical_rules_parser.hpp
        mugjc/token/token.cpp
        mugjc/token/token.hpp
        mugjc/lexer/finite_automata/dfa.cpp
        mugjc/lexer/finite_automata/dfa.hpp
        mugjc/lexer/finite_automata/nfa.cpp
        mugjc/lexer/finite_automata/nfa.hpp
        mugjc/lexer/generator/lexical_analyzer.cpp
        mugjc/lexer/generator/lexical_analyzer.hpp
        mugjc/lexer/rules/lexer_rules.cpp
        mugjc/lexer/rules/lexer_rules.hpp
        mugjc/lexer/finite_automata/thompson.cpp
        mugjc/lexer/finite_automata/thompson.hpp
)
target_compile_features(new_target PRIVATE cxx_std_20)
