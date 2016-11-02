#--------------------------------------------------------------------------
# physics project
#--------------------------------------------------------------------------

PROJECT(physics)
INCLUDE_DIRECTORIES("code/Managers") #helps access files without any ..\ when including files
INCLUDE_DIRECTORIES("code/")

FILE(GLOB SRC "code/*.h")
FILE(GLOB HEAD "code/*.cpp")	
LIST(APPEND SRCSBASE ${SRC} ${HEAD})
SOURCE_GROUP("Physics" FILES ${SRCSBASE})

FILE(GLOB SRC "code/Managers/*.cpp")
FILE(GLOB HEAD "code/Managers/*.h")
LIST(APPEND SRCSMANAGERS ${SRC} ${HEAD})
SOURCE_GROUP("Physics\\Managers" FILES ${SRCSMANAGERS})

FILE(GLOB SRC "code/Resources/Shaders/*.glsl")
LIST(APPEND SRCSSHADERS ${SRC} )
SOURCE_GROUP("Physics\\Shaders" FILES ${SRCSSHADERS})

LIST(APPEND ALLSRCS ${SRCSBASE} ${SRCSMANAGERS} ${SRCSSHADERS})

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
#FILE(COPY "GLFW_lab6/Resources" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin")

ADD_EXECUTABLE(physics ${ALLSRCS})
SET_TARGET_PROPERTIES(physics PROPERTIES FOLDER "LABS")
TARGET_LINK_LIBRARIES(physics camera frustum graphics_storage shader_manager graphics_manager object physics_manager scene core)
#ADD_DEPENDENCIES(physics mymathlib stb core render)


#ADD_DEPENDENCIES(subdivision engine core render)

#create bin directory
ADD_CUSTOM_COMMAND(TARGET physics PRE_BUILD
					COMMAND ${CMAKE_COMMAND} -E make_directory
						${CMAKE_CURRENT_SOURCE_DIR}/bin)

#copy resources to bin
ADD_CUSTOM_COMMAND(TARGET physics POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
						${CMAKE_CURRENT_SOURCE_DIR}/code/Resources ${CMAKE_CURRENT_SOURCE_DIR}/bin/Resources)

if(MSVC)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_debug" RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RELWITHDEBINFO_POSTFIX "_relwithdebinfo" RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX "_release" RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES MINSIZEREL_POSTFIX "_minsizerel" RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif()