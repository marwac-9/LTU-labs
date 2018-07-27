#--------------------------------------------------------------------------
# subdivision_optimizing project
#--------------------------------------------------------------------------

PROJECT(subdivision_optimized)
FILE(GLOB subdivision_optimized_headers code/*.h)
FILE(GLOB subdivision_optimized_sources code/*.cpp)

SET(files_subdivision_optimized 
	${subdivision_optimized_headers} 
	${subdivision_optimized_sources})

SOURCE_GROUP("SubdivisionOptimized" FILES ${files_subdivision_optimized})

#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
#FILE(COPY "code/Resources" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)

ADD_EXECUTABLE(${PROJECT_NAME} ${files_subdivision_optimized})

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES FOLDER "LABS")
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} object halfedgemesh graphics_storage graphics_manager)

#create bin directory
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} PRE_BUILD
					COMMAND ${CMAKE_COMMAND} -E make_directory
						${CMAKE_CURRENT_SOURCE_DIR}/bin)

#copy resources to bin
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
						${CMAKE_CURRENT_SOURCE_DIR}/code/Resources ${CMAKE_CURRENT_SOURCE_DIR}/bin/Resources)

if(MSVC)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_debug" RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RELWITHDEBINFO_POSTFIX "_relwithdebinfo" RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX "_release" RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES MINSIZEREL_POSTFIX "_minsizerel" RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif()