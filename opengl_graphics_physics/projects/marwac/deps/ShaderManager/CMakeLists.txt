#--------------------------------------------------------------------------
# shader_manager project
#--------------------------------------------------------------------------

PROJECT(shader_manager)
FILE(GLOB shader_manager_headers *.h)
FILE(GLOB shader_manager_sources *.cpp)

SET(files_shader_manager
	${shader_manager_headers} 
	${shader_manager_sources})

SOURCE_GROUP("shader_manager" FILES ${files_shader_manager})

ADD_LIBRARY(shader_manager STATIC ${files_shader_manager})
TARGET_LINK_LIBRARIES(shader_manager render)
SET_TARGET_PROPERTIES(shader_manager PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(shader_manager PROPERTIES FOLDER "MyUtils")
TARGET_INCLUDE_DIRECTORIES(shader_manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})