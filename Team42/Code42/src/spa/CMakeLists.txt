file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.h")
add_library(spa ${srcs} ${headers} "src/parser/Visitor.h" "src/parser/Visitor.cpp" "src/parser/AST.cpp" "src/PKB/Variable.cpp" "src/PKB/UtilityFunctions.hpp" "src/PKB/VarTable.h"  "src/PKB/VarTable.cpp" "src/PKB/Statement.h" "src/PKB/Statement.cpp" "src/PKB/StmtTable.h" "src/PKB/StmtTable.cpp" "src/PKB/Procedure.h" "src/PKB/ProcTable.h" "src/PKB/ProcTable.cpp" "src/PKB/FollowsFunctions.hpp" "src/PKB/Procedure.cpp" "src/PKB/TransitiveClosure.cpp" "src/PKB/ParentFunctions.hpp")
# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PKB)


