#--------------------------------------------------------------------------
# vector project
#--------------------------------------------------------------------------

PROJECT(vector)
FILE(GLOB vector_headers *.h)
FILE(GLOB vector_sources *.cpp)
FILE(GLOB vector_headershpp *.hpp)

SET(files_vector
	${vector_headers} 
	${vector_sources}
	${vector_headershpp})

SOURCE_GROUP("vector" FILES ${files_vector})

ADD_LIBRARY(vector STATIC ${files_vector})
SET_TARGET_PROPERTIES(vector PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(vector PROPERTIES FOLDER "Libs")
TARGET_INCLUDE_DIRECTORIES(vector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})