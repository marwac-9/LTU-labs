#--------------------------------------------------------------------------
# my math lib project
#--------------------------------------------------------------------------

PROJECT(mymathlib)
FILE(GLOB mymathlib_headers *.h)
FILE(GLOB mymathlib_sources *.cpp)

SET(files_mymathlib 
	${mymathlib_headers} 
	${mymathlib_sources})

SOURCE_GROUP("mymathlib" FILES ${files_mymathlib})

ADD_LIBRARY(mymathlib STATIC ${files_mymathlib})
SET_TARGET_PROPERTIES(mymathlib PROPERTIES FOLDER "MyLibs")
TARGET_INCLUDE_DIRECTORIES(mymathlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})