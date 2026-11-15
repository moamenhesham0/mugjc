cmake_minimum_required(VERSION 3.20)
project(mugjc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(new_target
    mugjc/mugjc.cpp
    mugjc/io/io.cpp
    mugjc/io/io.hpp
    mugjc/lexer/ast/ast.cpp
    mugjc/lexer/ast/ast.hpp
    mugjc/lexer/finite_automata/dfa.cpp
    mugjc/lexer/finite_automata/dfa.hpp
    mugjc/lexer/finite_automata/nfa.cpp
    mugjc/lexer/finite_automata/nfa.hpp
    mugjc/lexer/finite_automata/thompson.cpp
    mugjc/lexer/finite_automata/thompson.hpp
    mugjc/lexer/generator/lexical_analyzer.cpp
    mugjc/lexer/generator/lexical_analyzer.hpp
    mugjc/lexer/generator/lexical_analyzer_generator.cpp
    mugjc/lexer/generator/lexical_analyzer_generator.hpp
    mugjc/lexer/generator/lexical_rules_parser.cpp
    mugjc/lexer/generator/lexical_rules_parser.hpp
    mugjc/lexer/logger/lexical_logger.cpp
    mugjc/lexer/logger/lexical_logger.hpp
    mugjc/lexer/rules/lexer_rules.cpp
    mugjc/lexer/rules/lexer_rules.hpp
    mugjc/lexer/types.hpp
)

target_include_directories(new_target PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
