file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_library(spa ${srcs} ${headers} src/PQL/select_clause_parser.h src/PQL/select_clause_parser.cpp src/PQL/query_preprocessor.cpp src/PQL/query_preprocessor.h src/PQL/query_receiver.cpp src/PQL/such_that_clause.cpp src/PQL/such_that_clause.h src/PQL/pattern_clause.cpp src/PQL/pattern_clause.h src/PQL/pql_query.cpp src/PQL/pql_query.h src/PQL/such_that_ref.cpp src/PQL/such_that_ref.h src/PQL/stmt_ref.cpp src/PQL/stmt_ref.h src/PQL/ent_ref.cpp src/PQL/ent_ref.h src/PQL/expression_spec.cpp src/PQL/expression_spec.h src/PQL/pattern_query_manager.cpp src/PQL/pattern_query_manager.h)
# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


