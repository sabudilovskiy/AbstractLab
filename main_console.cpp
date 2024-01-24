#include <iostream>

#include "src/Lexer.hpp"
#include "src/DefaultLexer.hpp"
#include "src/ErrorLex.hpp"
#include <fmt/format.h>
#include <fmt/color.h>

void Print(const std::vector<parse::Lex>& lexs){
    for (auto& lex : lexs){
        fmt::print("lex_type: {}, start: {}#{}:{}, end: {}#{}:{} , text: \"{}\"\n",
                   ToStringView(lex.type),
                   lex.start.pos, lex.start.line, lex.start.column,
                   lex.end.pos, lex.end.line, lex.end.column,
                   lex.text);
    }
}

void print_lex_error(std::string_view source, parse::Lex lex){
    std::cout << "\n";
    auto er = parse::GetErrorLexs(lex, source);
    fmt::print(er.before_error);
    fmt::print(fmt::bg(fmt::color::yellow), er.error);
    fmt::print(fmt::bg(fmt::color::red), er.error_char);
    fmt::print(er.after_error);
}

int main() {
    std::string_view source = "matrix = \n0 1\n0 1a";
    auto lexer = parse::CreateLexer();
    auto res = lexer.Parse(source);
    Print(res);
    if (res.back().type == parse::LexType::Error){
        print_lex_error(source, res.back());
    }
}
