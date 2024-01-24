cmake_minimum_required(VERSION 3.27)
project(AbstractLab)

set(CMAKE_CXX_STANDARD 20)
include(FetchContent)

##################
####libraries#####
##################
find_package(magic_enum CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
FetchContent_Declare(hello_imgui GIT_REPOSITORY https://github.com/pthom/hello_imgui.git GIT_TAG master)
FetchContent_MakeAvailable(hello_imgui)

##################
#######src########
##################
add_library(${PROJECT_NAME}_src
        src/Lexer.hpp
        src/Lexer.cpp
        src/ast_builder.hpp
        src/DefaultLexer.hpp
        src/DefaultLexer.cpp
        src/ErrorLex.hpp)
target_link_libraries(${PROJECT_NAME}_src PRIVATE magic_enum::magic_enum)
target_include_directories(${PROJECT_NAME}_src PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
##################

##################
#######GUI########
##################

hello_imgui_add_app(${PROJECT_NAME}_gui main_gui.cpp)
target_link_libraries(${PROJECT_NAME}_gui PRIVATE ${PROJECT_NAME}_src)
target_include_directories(${PROJECT_NAME}_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

##################
#####console######
##################
add_executable(${PROJECT_NAME}_console main_console.cpp)
target_link_libraries(${PROJECT_NAME}_console PRIVATE ${PROJECT_NAME}_src)
target_link_libraries(${PROJECT_NAME}_console PRIVATE fmt::fmt)
##################