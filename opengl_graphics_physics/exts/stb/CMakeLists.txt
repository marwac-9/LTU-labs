#--------------------------------------------------------------------------
# stb project
#--------------------------------------------------------------------------

PROJECT(stb)
FILE(GLOB stb_headers "*.h")
FILE(GLOB stb_sources "*.cpp")

SET(files_stb
	${stb_headers} 
	${stb_sources})

SOURCE_GROUP("stb" FILES ${files_stb})

ADD_LIBRARY(stb STATIC ${files_stb})
SET_TARGET_PROPERTIES(stb PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(stb PROPERTIES FOLDER "Libs")
TARGET_INCLUDE_DIRECTORIES(stb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})