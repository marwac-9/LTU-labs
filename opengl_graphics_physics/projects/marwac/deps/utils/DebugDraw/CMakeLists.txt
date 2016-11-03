#--------------------------------------------------------------------------
# debug_draw project
#--------------------------------------------------------------------------

PROJECT(debug_draw)
FILE(GLOB debug_draw_headers *.h)
FILE(GLOB debug_draw_sources *.cpp)

SET(files_debug_draw
	${debug_draw_headers} 
	${debug_draw_sources})

SOURCE_GROUP("debug_draw" FILES ${files_debug_draw})

ADD_LIBRARY(debug_draw STATIC ${files_debug_draw})
TARGET_LINK_LIBRARIES(debug_draw mymathlib drawables graphics_storage node frustum shader_manager object)
SET_TARGET_PROPERTIES(debug_draw PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(debug_draw PROPERTIES FOLDER "MyUtils")
TARGET_INCLUDE_DIRECTORIES(debug_draw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})