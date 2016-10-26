#--------------------------------------------------------------------------
# material project
#--------------------------------------------------------------------------

PROJECT(material)
FILE(GLOB material_headers *.h)
FILE(GLOB material_sources *.cpp)

SET(files_material
	${material_headers} 
	${material_sources})

SOURCE_GROUP("material" FILES ${files_material})

ADD_LIBRARY(material STATIC ${files_material})
TARGET_LINK_LIBRARIES(material mymathlib)
SET_TARGET_PROPERTIES(material PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(material PROPERTIES FOLDER "Libs")
TARGET_INCLUDE_DIRECTORIES(material PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})