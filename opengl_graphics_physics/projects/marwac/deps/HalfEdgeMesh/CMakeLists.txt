#--------------------------------------------------------------------------
# halfedgemesh project
#--------------------------------------------------------------------------

PROJECT(halfedgemesh)
FILE(GLOB halfedgemesh_headers *.h)
FILE(GLOB halfedgemesh_sources *.cpp)

SET(files_halfedgemesh
	${halfedgemesh_headers} 
	${halfedgemesh_sources})

SOURCE_GROUP("halfedgemesh" FILES ${files_halfedgemesh})

ADD_LIBRARY(halfedgemesh STATIC ${files_halfedgemesh})
TARGET_LINK_LIBRARIES(halfedgemesh mesh mymathlib poolparty vector obj)
SET_TARGET_PROPERTIES(halfedgemesh PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(halfedgemesh PROPERTIES FOLDER "Libs")
TARGET_INCLUDE_DIRECTORIES(halfedgemesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})