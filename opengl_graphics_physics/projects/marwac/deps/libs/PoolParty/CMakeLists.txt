#--------------------------------------------------------------------------
# poolparty lib project
#--------------------------------------------------------------------------

PROJECT(poolparty)
FILE(GLOB poolparty_headers *.h)
FILE(GLOB poolparty_sources *.cpp)

SET(files_poolparty
	${poolparty_headers} 
	${poolparty_sources})

SOURCE_GROUP("poolparty" FILES ${files_poolparty})

ADD_LIBRARY(poolparty STATIC ${files_poolparty})
SET_TARGET_PROPERTIES(poolparty PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(poolparty PROPERTIES FOLDER "MyLibs")
TARGET_INCLUDE_DIRECTORIES(poolparty PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})