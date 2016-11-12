#--------------------------------------------------------------------------
# shadows project
#--------------------------------------------------------------------------

PROJECT(shadows)
INCLUDE_DIRECTORIES("code/Managers") #helps access files without any ..\ when including files
INCLUDE_DIRECTORIES("code/")

FILE(GLOB SRC "code/*.h")
FILE(GLOB HEAD "code/*.cpp")	
LIST(APPEND SRCSBASE ${SRC} ${HEAD})
SOURCE_GROUP("Shadows" FILES ${SRCSBASE})

FILE(GLOB SRC "code/Managers/*.cpp")
FILE(GLOB HEAD "code/Managers/*.h")
LIST(APPEND SRCSMANAGERS ${SRC} ${HEAD})
SOURCE_GROUP("Shadows\\Managers" FILES ${SRCSMANAGERS})

FILE(GLOB SRC "code/Resources/Shaders/*.glsl")
LIST(APPEND SRCSSHADERS ${SRC} )
SOURCE_GROUP("Shadows\\Shaders" FILES ${SRCSSHADERS})

LIST(APPEND ALLSRCS ${SRCSBASE} ${SRCSMANAGERS} ${SRCSSHADERS})

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
#FILE(COPY "GLFW_lab6/Resources" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin")

ADD_EXECUTABLE(shadows ${ALLSRCS})
SET_TARGET_PROPERTIES(shadows PROPERTIES FOLDER "LABS")
TARGET_LINK_LIBRARIES(shadows camera frustum shader_manager graphics_manager scene gl_window app camera_manager)
#ADD_DEPENDENCIES(shadows core render)


#ADD_DEPENDENCIES(subdivision engine core render)

#create bin directory
ADD_CUSTOM_COMMAND(TARGET shadows PRE_BUILD
					COMMAND ${CMAKE_COMMAND} -E make_directory
						${CMAKE_CURRENT_SOURCE_DIR}/bin)

#copy resources to bin
ADD_CUSTOM_COMMAND(TARGET shadows POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
						${CMAKE_CURRENT_SOURCE_DIR}/code/Resources ${CMAKE_CURRENT_SOURCE_DIR}/bin/Resources)

if(MSVC)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_debug" RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RELWITHDEBINFO_POSTFIX "_relwithdebinfo" RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX "_release" RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES MINSIZEREL_POSTFIX "_minsizerel" RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif()