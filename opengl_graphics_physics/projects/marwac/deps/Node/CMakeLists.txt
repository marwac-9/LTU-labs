#--------------------------------------------------------------------------
# node project
#--------------------------------------------------------------------------

PROJECT(node)
FILE(GLOB node_headers *.h)
FILE(GLOB node_sources *.cpp)

SET(files_node
	${node_headers} 
	${node_sources})

SOURCE_GROUP("node" FILES ${files_node})

ADD_LIBRARY(node STATIC ${files_node})
TARGET_LINK_LIBRARIES(node mymathlib)
SET_TARGET_PROPERTIES(node PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(node PROPERTIES FOLDER "Libs")
TARGET_INCLUDE_DIRECTORIES(node PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})