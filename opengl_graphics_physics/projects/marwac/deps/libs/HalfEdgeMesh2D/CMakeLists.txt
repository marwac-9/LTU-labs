#--------------------------------------------------------------------------
# halfedgemesh2d project
#--------------------------------------------------------------------------

PROJECT(halfedgemesh2d)
FILE(GLOB halfedgemesh2d_headers *.h)
FILE(GLOB halfedgemesh2d_sources *.cpp)

SET(files_halfedgemesh2d
	${halfedgemesh2d_headers} 
	${halfedgemesh2d_sources})

SOURCE_GROUP("halfedgemesh2d" FILES ${files_halfedgemesh2d})

ADD_LIBRARY(halfedgemesh2d STATIC ${files_halfedgemesh2d})
TARGET_LINK_LIBRARIES(halfedgemesh2d mesh poolparty vector obj)
SET_TARGET_PROPERTIES(halfedgemesh2d PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(halfedgemesh2d PROPERTIES FOLDER "MyLibs")
TARGET_INCLUDE_DIRECTORIES(halfedgemesh2d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})