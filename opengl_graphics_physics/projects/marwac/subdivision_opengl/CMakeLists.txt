#--------------------------------------------------------------------------
# subdivision_opengl project
#--------------------------------------------------------------------------

PROJECT(subdivision_opengl)
INCLUDE_DIRECTORIES("code/Managers") #helps access files without any ..\ when including files
INCLUDE_DIRECTORIES("code/Drawables")
INCLUDE_DIRECTORIES("code/HalfEdgeMesh")
INCLUDE_DIRECTORIES("code/")

FILE(GLOB SRC "code/*.h")
FILE(GLOB HEAD "code/*.cpp")	
LIST(APPEND SRCSBASE ${SRC} ${HEAD})
SOURCE_GROUP("SubdivisionOpenGL" FILES ${SRCSBASE})

FILE(GLOB SRC "code/Managers/*.cpp")
FILE(GLOB HEAD "code/Managers/*.h")
LIST(APPEND SRCSMANAGERS ${SRC} ${HEAD})
SOURCE_GROUP("SubdivisionOpenGL\\Managers" FILES ${SRCSMANAGERS})

FILE(GLOB SRC "code/Drawables/*.cpp")
FILE(GLOB HEAD "code/Drawables/*.h")
LIST(APPEND SRCDRAWABLES ${SRC} ${HEAD})
SOURCE_GROUP("SubdivisionOpenGL\\Drawables" FILES ${SRCDRAWABLES})

FILE(GLOB SRC "code/Resources/Shaders/*.glsl")
LIST(APPEND SRCSSHADERS ${SRC} )
SOURCE_GROUP("SubdivisionOpenGL\\Shaders" FILES ${SRCSSHADERS})

LIST(APPEND ALLSRCS ${SRCSBASE} ${SRCSMANAGERS} ${SRCSHALFEDGEMESH} ${SRCDRAWABLES} ${SRCSSHADERS})

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
#FILE(COPY "GLFW_lab6/Resources" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin")

ADD_EXECUTABLE(subdivision_opengl ${ALLSRCS})
SET_TARGET_PROPERTIES(subdivision_opengl PROPERTIES FOLDER "LABS")
TARGET_LINK_LIBRARIES(subdivision_opengl material node drawables camera frustum stb halfedgemesh core render)
#ADD_DEPENDENCIES(subdivision_opengl core render)


#ADD_DEPENDENCIES(subdivision engine core render)

#create bin directory
ADD_CUSTOM_COMMAND(TARGET subdivision_opengl PRE_BUILD
					COMMAND ${CMAKE_COMMAND} -E make_directory
						${CMAKE_CURRENT_SOURCE_DIR}/bin)

#copy resources to bin
ADD_CUSTOM_COMMAND(TARGET subdivision_opengl POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
						${CMAKE_CURRENT_SOURCE_DIR}/code/Resources ${CMAKE_CURRENT_SOURCE_DIR}/bin/Resources)

if(MSVC)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_debug" RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RELWITHDEBINFO_POSTFIX "_relwithdebinfo" RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RELEASE_POSTFIX "_release" RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES MINSIZEREL_POSTFIX "_minsizerel" RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif()