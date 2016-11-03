#--------------------------------------------------------------------------
# physics_manager project
#--------------------------------------------------------------------------

PROJECT(physics_manager)
FILE(GLOB physics_manager_headers *.h)
FILE(GLOB physics_manager_sources *.cpp)

SET(files_physics_manager
	${physics_manager_headers} 
	${physics_manager_sources})

SOURCE_GROUP("physics_manager" FILES ${files_physics_manager})

ADD_LIBRARY(physics_manager STATIC ${files_physics_manager})
TARGET_LINK_LIBRARIES(physics_manager mymathlib debug_draw object)
SET_TARGET_PROPERTIES(physics_manager PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(physics_manager PROPERTIES FOLDER "MyUtils")
TARGET_INCLUDE_DIRECTORIES(physics_manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})