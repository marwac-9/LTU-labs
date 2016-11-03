#--------------------------------------------------------------------------
# graphics_storage project
#--------------------------------------------------------------------------

PROJECT(graphics_storage)
FILE(GLOB graphics_storage_headers *.h)
FILE(GLOB graphics_storage_sources *.cpp)

SET(files_graphics_storage
	${graphics_storage_headers} 
	${graphics_storage_sources})

SOURCE_GROUP("graphics_storage" FILES ${files_graphics_storage})

ADD_LIBRARY(graphics_storage STATIC ${files_graphics_storage})
TARGET_LINK_LIBRARIES(graphics_storage material mesh)
SET_TARGET_PROPERTIES(graphics_storage PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(graphics_storage PROPERTIES FOLDER "MyUtils")
TARGET_INCLUDE_DIRECTORIES(graphics_storage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})