file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*.h")
add_library(spa ${srcs} ${headers})
# this makes the headers accessible for other projects which uses spa lib
target_include_directories(
        spa
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/ast
        ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/lexer
        ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parse
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pkb
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pkb/entities
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pkb/tables
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PQL)
