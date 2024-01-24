#pragma once
#include <memory>
#include "Lexer.hpp"
#include <unordered_map>
#include <typeindex>

#define FWD(x) std::forward<decltype(x)>(x)

struct Visiter{
public:
    Visiter(){}
    template <typename T, typename Callable>
    void Add(Callable&& callable){
        functions_.emplace(typeid(T), FWD(callable));
    }
    template <typename T>
    void Visit(T& t){
        std::type_index index{typeid(t)};
        auto it = functions_.find(index);
        if (it != functions_.end()){

        }
    }

private:
    std::unordered_map<std::type_index, std::function<void()>> functions_;

};

struct Node{
    virtual void Visit(Visiter& visiter){
        visiter.Visit(*this);
    }
};

struct BinaryExpression: Node{
    std::unique_ptr<Node> node;
};

struct AstBuilder{
public:
    AstBuilder(){}

private:
};

/*

 int foo(){
    for (int i = 0; i < 10; i++){
        std::cout << i << '\n';
    }
 }

a = 5
b = "dsdsds"
matrix = {
0 1 0
1 0 1
0 1 a
}
////////////
auto a = storage.Get("a").AsNumber();
auto b = storage.Get("b").AsString();
auto matrix = storage.Get("matrix).AsMatrix();
 */