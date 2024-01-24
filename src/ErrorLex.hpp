#pragma once
#include "Lexer.hpp"

namespace parse{
    struct ErrorLexs{
        std::string_view before_error;
        std::string_view error;
        std::string_view error_char;
        std::string_view after_error;
    };

    inline ErrorLexs GetErrorLexs(parse::Lex lex, std::string_view source){
        auto before_error = source.substr(0, lex.start.pos);
        auto error = source.substr(lex.start.pos, lex.end.pos - lex.start.pos);
        auto error_char = source.substr(lex.end.pos, 1);
        std::string_view after_error;
        if (lex.end.pos + 1 < source.size()){
            after_error = source.substr(lex.end.pos + 1);
        }
        return ErrorLexs{before_error, error, error_char, after_error};
    }
}