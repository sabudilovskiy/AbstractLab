#pragma once
#include <string_view>
#include <functional>
#include <vector>
#include <unordered_map>
#include <optional>

namespace parse {

    enum struct LexType {
        Start, Ident,
        FirstEq,
        FirstL,
        FirstG,
        LE, EQ, GE, GR, LS, AS,
        Number, OpenBracket,
        CloseBracket,
        Space_m,
        Space, EndLine,
        Error,
        End
    };

    std::string_view ToStringView(LexType lexType);


    struct Position {
        std::size_t line{1};
        std::size_t column{1};
        std::size_t pos{};

        void NextLine();

        void NextColumn();

        void Consume(char ch);
    };

    struct Lex {
        LexType type;
        Position start;
        Position end;
        std::string_view text;
    };

    struct StateChange {
        std::optional<LexType> lex_type;
        LexType next_state;
    };


    struct Lexer {
        using Transition = std::function<std::optional<StateChange>(char)>;

        void AddTransition(LexType from, Transition transition);

        void AddTransitions(std::vector<LexType> from, const Transition &transition);

        void AddTransitions(LexType from, std::vector<Transition> transitions);

        void AddTransitions(const std::vector<LexType> &from, const std::vector<Transition> &transitions);

        std::vector<Lex> Parse(std::string_view source);

        Lexer(LexType start, LexType error) : start_state_(start), error_state_(error) {}

    protected:
        void ParseSymbol(char symbol);

        void ParseEnd();

        void init(std::string_view source);

        void EmplaceLex(LexType lex_type);

    private:
        std::string_view source_;
        LexType start_state_;
        LexType error_state_;
        Position start_lex_;
        Position end_lex_;
        std::unordered_map<LexType, std::vector<Transition>> transitions_;
        LexType current_state_;
        std::string_view::iterator it_;
        std::string_view::iterator end_;
        std::vector<Lex> parsed_;
        Position cur_position_;
    };

}
//=>
//==

//OP(=>)
//OP(==)
//
//if (text == "==" ) return EQ;
//else if (text == "<=") return LE;
//else if (text ==

//

//OP -> OP=>
//OP -> OP==