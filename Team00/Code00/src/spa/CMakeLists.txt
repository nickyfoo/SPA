file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_library(spa ${srcs} ${headers} src/SelectClauseParser.h src/SelectClauseParser.cpp src/QueryObject.cpp src/QueryObject.h src/QueryEvaluator.cpp src/QueryEvaluator.h src/QueryReceiver.cpp src/Relationship.cpp src/Relationship.h)
# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


