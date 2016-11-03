#--------------------------------------------------------------------------
# drawables project
#--------------------------------------------------------------------------

PROJECT(drawables)
FILE(GLOB drawables_headers *.h)
FILE(GLOB drawables_sources *.cpp)

SET(files_drawables
	${drawables_headers} 
	${drawables_sources})

SOURCE_GROUP("drawables" FILES ${files_drawables})

ADD_LIBRARY(drawables STATIC ${files_drawables})
TARGET_LINK_LIBRARIES(drawables mymathlib material mesh render)
SET_TARGET_PROPERTIES(drawables PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(drawables PROPERTIES FOLDER "MyLibs")
TARGET_INCLUDE_DIRECTORIES(drawables PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})