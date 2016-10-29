#--------------------------------------------------------------------------
# frustum project
#--------------------------------------------------------------------------

PROJECT(frustum)
FILE(GLOB frustum_headers *.h)
FILE(GLOB frustum_sources *.cpp)

SET(files_frustum
	${frustum_headers} 
	${frustum_sources})

SOURCE_GROUP("frustum" FILES ${files_frustum})

ADD_LIBRARY(frustum STATIC ${files_frustum})
TARGET_LINK_LIBRARIES(frustum mymathlib)
SET_TARGET_PROPERTIES(frustum PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(frustum PROPERTIES FOLDER "MyLibs")
TARGET_INCLUDE_DIRECTORIES(frustum PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})