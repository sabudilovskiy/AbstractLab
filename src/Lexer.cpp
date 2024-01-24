#include "Lexer.hpp"
#include <magic_enum.hpp>
namespace parse {
    std::vector<Lex> Lexer::Parse(std::string_view source) {
        init(source);
        start_lex_ = cur_position_;
        end_lex_ = cur_position_;
        ParseSymbol(*it_);
        ++it_;
        bool rollback = false;
        for (; current_state_ != error_state_ && it_ != end_; ++it_) {
            cur_position_.Consume(*it_);
            ParseSymbol(*it_);
        }
        if (it_ == end_) {
            cur_position_.NextColumn();
            ParseEnd();
        }
        if (current_state_ == error_state_) {
            EmplaceLex(error_state_);
        }
        return std::move(parsed_);
    }

    void Lexer::AddTransition(LexType from, Transition transition) {
        transitions_[from].emplace_back(std::move(transition));
    }

    void Lexer::init(std::string_view source) {
        source_ = source;
        current_state_ = start_state_;
        it_ = begin(source_);
        end_ = end(source_);
        parsed_ = {};
        cur_position_ = {};
    }

    void Lexer::EmplaceLex(LexType lex_type) {
        Lex lex{
                .type = lex_type,
                .start = start_lex_,
                .end = end_lex_,
                .text = source_.substr(start_lex_.pos, end_lex_.pos - start_lex_.pos + 1)
        };
        parsed_.emplace_back(std::move(lex));
    }

    void Lexer::ParseSymbol(char symbol) {
        auto found_trs = transitions_.find(current_state_);
        if (found_trs == transitions_.end()) {
            current_state_ = error_state_;
            end_lex_ = cur_position_;
            return;
        }
        for (auto &tr: found_trs->second) {
            if (!tr(symbol)) {
                continue;
            }
            auto state_change = *tr(symbol);
            current_state_ = state_change.next_state;
            if (state_change.lex_type) {
                EmplaceLex(*state_change.lex_type);
                start_lex_ = cur_position_;
                end_lex_ = cur_position_;
                return;
            } else {
                end_lex_ = cur_position_;
            }
            return;
        }
        current_state_ = error_state_;
        end_lex_ = cur_position_;
        return;
    }

    void Lexer::ParseEnd() {
        auto found_trs = transitions_.find(current_state_);
        if (found_trs == transitions_.end()) {
            current_state_ = error_state_;
            return;
        }
        for (auto &tr: found_trs->second) {
            if (!tr('\0')) {
                continue;
            }
            auto next_state = *tr('\0');
            current_state_ = next_state.next_state;
            if (next_state.lex_type) {
                EmplaceLex(*next_state.lex_type);
                return;
            } else {
                current_state_ = error_state_;
            }
            return;
        }
        current_state_ = error_state_;
    }

    void Lexer::AddTransitions(std::vector<LexType> from, const Lexer::Transition &transition) {
        for (auto &fr: from) {
            AddTransition(std::move(fr), transition);
        }
    }

    void Lexer::AddTransitions(LexType from, std::vector<Transition> transitions) {
        for (auto &tr: transitions) {
            AddTransition(from, std::move(tr));
        }
    }

    void Lexer::AddTransitions(const std::vector<LexType> &from, const std::vector<Transition> &transitions) {
        for (auto &fr: from) {
            for (auto &tr: transitions) {
                AddTransition(fr, tr);
            }
        }
    }

    void Position::NextLine() {
        pos++;
        line++;
        column = 0;
    }

    void Position::NextColumn() {
        pos++;
        column++;
    }

    void Position::Consume(char ch) {
        if (ch == '\n') {
            NextLine();
        } else {
            NextColumn();
        }
    }

    std::string_view ToStringView(LexType lexType) {
        return magic_enum::enum_name(lexType);
    }

}