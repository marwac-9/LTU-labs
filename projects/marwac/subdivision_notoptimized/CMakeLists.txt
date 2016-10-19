#--------------------------------------------------------------------------
# notoptimizied project
#--------------------------------------------------------------------------

PROJECT(subdivision_notoptimizied)
FILE(GLOB subdivision_notoptimizied_headers code/*.h)
FILE(GLOB subdivision_notoptimizied_sources code/*.cpp)

SET(files_subdivision_notoptimizied 
	${subdivision_notoptimizied_headers} 
	${subdivision_notoptimizied_sources})

SOURCE_GROUP("SubdivisionNotOptimized" FILES ${files_subdivision_notoptimizied})

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
#FILE(COPY "code/Resources" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin")

ADD_EXECUTABLE(subdivision_notoptimizied ${files_subdivision_notoptimizied})
SET_TARGET_PROPERTIES(subdivision_notoptimizied PROPERTIES FOLDER "LABS")
TARGET_LINK_LIBRARIES(subdivision_notoptimizied mymathlib core render)
ADD_DEPENDENCIES(subdivision_notoptimizied core render)


#ADD_DEPENDENCIES(subdivision engine core render)

#create bin directory
ADD_CUSTOM_COMMAND(TARGET subdivision_notoptimizied PRE_BUILD
					COMMAND ${CMAKE_COMMAND} -E make_directory
						${CMAKE_CURRENT_SOURCE_DIR}/bin)

#copy resources to bin
ADD_CUSTOM_COMMAND(TARGET subdivision_notoptimizied POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
						${CMAKE_CURRENT_SOURCE_DIR}/code/Resources ${CMAKE_CURRENT_SOURCE_DIR}/bin/Resources)

if(MSVC)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_debug" RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RELWITHDEBINFO_POSTFIX "_relwithdebinfo" RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX "_release" RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES MINSIZEREL_POSTFIX "_minsizerel" RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif()