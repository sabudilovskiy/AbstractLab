#include <DefaultLexer.hpp>

namespace parse{
    template <LexType next_state, std::same_as<char> auto... ch>
    auto matcher = [](char value) -> std::optional<StateChange> {
        auto res = ((value == ch) || ...);
        if (res){
            return StateChange{
                    .next_state = next_state,
            };
        }
        return std::nullopt;
    };
    template <LexType lex_type, LexType next_state, std::same_as<char> auto... ch>
    auto f_matcher = [](char value) -> std::optional<StateChange> {
        auto res = ((value == ch) || ...);
        if (res){
            return StateChange{
                    .lex_type = lex_type,
                    .next_state = next_state,
            };
        }
        return std::nullopt;
    };

    template <LexType lex_type, LexType next_state, std::same_as<char> auto... ch>
    auto ender = [](char value) -> std::optional<StateChange> {
        auto res = ((value == ch) || ...);
        if (!res) {
            return StateChange{
                    .lex_type = lex_type,
                    .next_state = next_state,
            };
        }
        else return std::nullopt;
    };

    auto IsDigit(char ch){
        return '0' <= ch && ch <= '9';
    }

    auto is_start_ident(char ch){
        return 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z';
    }

    auto is_continue_ident(char ch){
        return is_start_ident(ch) || ch == '_' || IsDigit(ch);
    }

    template <LexType lex_type>
    auto f_start_ident = [](char ch) -> std::optional<StateChange> {
        if (is_start_ident(ch)) {
            return StateChange{
                    .lex_type = lex_type,
                    .next_state = LexType::Ident
            };
        }
        return std::nullopt;
    };

    auto start_ident = [](char ch) -> std::optional<StateChange> {
        if (is_start_ident(ch)) {
            return StateChange{
                    .next_state = LexType::Ident
            };
        }
        else return std::nullopt;
    };

    auto continue_ident = [](char ch) -> std::optional<StateChange> {
        if (is_continue_ident(ch)) {
            return StateChange{
                    .next_state = LexType::Ident
            };
        }
        else return std::nullopt;
    };


    template <LexType lex_type>
    auto f_start_number = [](char ch) -> std::optional<StateChange> {
        if (IsDigit(ch)){
            return StateChange{
                    .lex_type = lex_type,
                    .next_state = LexType::Number
            };
        }
        return std::nullopt;
    };

    auto continue_number = [](char ch) -> std::optional<StateChange> {
        if (IsDigit(ch)){
            return StateChange{
                    .next_state = LexType::Number
            };
        }
        return std::nullopt;
    };

    Lexer CreateLexer() {
        using enum LexType;
        Lexer lexer(Start, Error);
        lexer.AddTransition(Start, matcher<FirstEq, '='>);
        lexer.AddTransition(Start, start_ident);

        lexer.AddTransition(Ident, f_matcher<Ident, Space, ' '>);
        lexer.AddTransition(Ident, f_matcher<Ident, EndLine, '\n'>);
        lexer.AddTransition(Ident, continue_ident);

        lexer.AddTransition(EndLine, f_start_ident<EndLine>);
        lexer.AddTransition(EndLine, f_start_number<EndLine>);
        lexer.AddTransition(EndLine, f_matcher<EndLine, Space, ' '>);

        lexer.AddTransition(Space, f_matcher<Space, FirstEq, '='>);
        lexer.AddTransition(Space, f_matcher<Space, FirstG, '>'>);
        lexer.AddTransition(Space, f_matcher<Space, FirstL, '<'>);
        lexer.AddTransition(Space, f_matcher<Space, EndLine, '\n'>);
        lexer.AddTransition(Space, matcher<Space, ' '>);
        lexer.AddTransition(Space, f_start_number<Space>);
        lexer.AddTransition(Space, f_start_ident<Space>);

        lexer.AddTransition(Number, continue_number);
        lexer.AddTransition(Number, f_matcher<Number, Space, ' '>);
        lexer.AddTransition(Number, f_matcher<Number, EndLine, '\n'>);

        lexer.AddTransition(FirstEq, f_matcher<AS, Space, ' '>);
        lexer.AddTransition(FirstEq, f_matcher<AS, EndLine, '\n'>);
        lexer.AddTransition(FirstEq, f_matcher<AS, End, '\0'>);
        return lexer;
    }
}