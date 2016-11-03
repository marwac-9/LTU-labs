#--------------------------------------------------------------------------
# scene project
#--------------------------------------------------------------------------

PROJECT(scene)
FILE(GLOB scene_headers *.h)
FILE(GLOB scene_sources *.cpp)

SET(files_scene
	${scene_headers} 
	${scene_sources})

SOURCE_GROUP("scene" FILES ${files_scene})

ADD_LIBRARY(scene STATIC ${files_scene})
TARGET_LINK_LIBRARIES(scene graphics_storage physics_manager)
SET_TARGET_PROPERTIES(scene PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(scene PROPERTIES FOLDER "MyUtils")
TARGET_INCLUDE_DIRECTORIES(scene PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})