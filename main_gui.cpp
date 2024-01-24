#include "hello_imgui/hello_imgui.h"
#include "ErrorLex.hpp"

#include <imgui.h>
#include <Lexer.hpp>
#include <DefaultLexer.hpp>
#include <span>

namespace im_hui = ImGui;
namespace hello_im_hui = HelloImGui;

void LoadCirilyc(){
    auto& io = im_hui::GetIO();
    io.FontDefault = io.Fonts->AddFontFromFileTTF("tmr.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());

}

std::string_view GetSource(const std::array<char, 256>& buf){
    std::size_t len = std::find(buf.begin(), buf.end(), '\0') - buf.begin();
    return {
        buf.data(),
        len
    };
}

auto address_elem(auto it){
    return &*it;
}

constexpr ImVec4 kRed = {255, 0, 0, 100};
constexpr ImVec4 kYellow = {255, 255, 0, 100};

//precondition: buf.size() > 0 && buf.back()
void ErrorLexsWindow(parse::Lex lex, std::string_view source){
    im_hui::Begin("Возникла ошибка во время разбора");
    std::size_t cur_line_n = {1};
    std::size_t cur_column{1};
    auto it = source.begin();
    auto end = source.end();
    auto in_error = [&](){
        return cur_line_n == lex.start.line || cur_line_n == lex.end.line;
    };
    auto before_error = [&](){
        return lex.start.line == cur_line_n && lex.start.column > cur_column;
    };
    auto char_error = [&](){
        return lex.end.line == cur_line_n && lex.end.column == cur_column;
    };
    auto after_error = [&](){
        return lex.end.line > cur_line_n || lex.end.line == cur_line_n && lex.end.column < cur_column;
    };
    while (it != end){
        auto cur_text = address_elem(it);
        auto end_line = std::find(it, end, '\n');
        if (!in_error()){
            int len = end_line - cur_text;
            im_hui::Text("%.*s", len, cur_text);
            if (end_line == end) {
                it = end_line;
            }
            else {
                it = end_line + 1;
            }
            cur_line_n++;
        }
        else{
            im_hui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, {0, 4});
            for (;it != end; it++){
                cur_text = address_elem(it);
                if (before_error() && *cur_text != '\n'){
                    im_hui::Text("%.*s", 1, cur_text);
                    im_hui::SameLine();
                    ++cur_column;
                }
                else if (before_error() && *cur_text == '\n'){
                    im_hui::NewLine();
                    cur_column = 1;
                    ++cur_line_n;
                }
                else if (char_error()){
                    im_hui::TextColored(kRed, "%.*s", 1, cur_text);
                    im_hui::SameLine();
                    ++cur_column;
                }
                else if (after_error()){
                    int len = std::find(cur_text, end, '\n') - cur_text;
                    im_hui::Text("%.*s", len, cur_text);
                    ++cur_line_n;
                    ++it;
                    break;
                }
                else if (*cur_text != '\n'){
                    im_hui::TextColored(kYellow, "%.*s", 1, cur_text);
                    im_hui::SameLine();
                    ++cur_column;
                }
                else {
                    im_hui::NewLine();
                    ++cur_line_n;
                    ++cur_column;
                }
            }
            im_hui::PopStyleVar();
        }
    }
    im_hui::End();
}

int main(int , char *[])
{
    std::array<char, 256> buf{};
    std::string_view source = GetSource(buf);
    std::vector<parse::Lex> lexs;
    auto lexer = parse::CreateLexer();
    auto guiFunction = [&]() {
        im_hui::ShowDemoWindow();
        im_hui::Begin("Ввод текста");
        im_hui::Text("Здесь можно ввести конфиг, который будет распаршен");
        im_hui::InputTextMultiline("Конфиг", buf.data(), buf.size());
        bool need_parse = im_hui::Button("Распарсить");
        if (need_parse){
            source = GetSource(buf);
            lexs = lexer.Parse(source);
        }
        im_hui::End();
        if (lexs.empty()){
            return;
        }
        if (lexs.back().type == parse::LexType::Error){
            ErrorLexsWindow(lexs.back(), source);
        }
    };
    hello_im_hui::RunnerParams params;
    params.callbacks.ShowGui = guiFunction;
    params.appWindowParams.windowGeometry.size = {1366, 768};
    params.appWindowParams.windowTitle = "Лексер";
    params.callbacks.LoadAdditionalFonts = LoadCirilyc;
    hello_im_hui::Run(params);
    return 0;
}