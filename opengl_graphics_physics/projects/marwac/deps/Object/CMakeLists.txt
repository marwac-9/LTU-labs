#--------------------------------------------------------------------------
# object project
#--------------------------------------------------------------------------

PROJECT(object)
FILE(GLOB object_headers *.h)
FILE(GLOB object_sources *.cpp)

SET(files_object
	${object_headers} 
	${object_sources})

SOURCE_GROUP("object" FILES ${files_object})

ADD_LIBRARY(object STATIC ${files_object})
TARGET_LINK_LIBRARIES(object mymathlib drawables shader_manager node material obj render)
SET_TARGET_PROPERTIES(object PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(object PROPERTIES FOLDER "MyLibs")
TARGET_INCLUDE_DIRECTORIES(object PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})