#--------------------------------------------------------------------------
# dirent project
#--------------------------------------------------------------------------

PROJECT(dirent)
FILE(GLOB dirent_headers "*.h")
FILE(GLOB dirent_sources "*.cpp")

SET(files_dirent
	${dirent_headers} 
	${dirent_sources})

SOURCE_GROUP("dirent" FILES ${files_dirent})

ADD_LIBRARY(dirent STATIC ${files_dirent})
SET_TARGET_PROPERTIES(dirent PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(dirent PROPERTIES FOLDER "Libs")
TARGET_INCLUDE_DIRECTORIES(dirent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})