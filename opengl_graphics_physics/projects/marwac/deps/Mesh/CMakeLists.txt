#--------------------------------------------------------------------------
# mesh project
#--------------------------------------------------------------------------

PROJECT(mesh)
FILE(GLOB mesh_headers *.h)
FILE(GLOB mesh_sources *.cpp)

SET(files_mesh
	${mesh_headers} 
	${mesh_sources})

SOURCE_GROUP("mesh" FILES ${files_mesh})

ADD_LIBRARY(mesh STATIC ${files_mesh})
TARGET_LINK_LIBRARIES(mesh render)
SET_TARGET_PROPERTIES(mesh PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(mesh PROPERTIES FOLDER "MyLibs")
TARGET_INCLUDE_DIRECTORIES(mesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})