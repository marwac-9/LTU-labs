#--------------------------------------------------------------------------
# exts
#--------------------------------------------------------------------------
ADD_LIBRARY(exts INTERFACE)

ADD_SUBDIRECTORY(glew)
SET_TARGET_PROPERTIES(glew PROPERTIES FOLDER "exts/glew")
TARGET_INCLUDE_DIRECTORIES(exts INTERFACE glew/include)

SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
SET(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
SET(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")		  

ADD_SUBDIRECTORY(glfw-3.1.1)
SET_TARGET_PROPERTIES(glfw PROPERTIES FOLDER "exts/glfw")
TARGET_INCLUDE_DIRECTORIES(exts INTERFACE glfw-3.1.1/include)

ADD_SUBDIRECTORY(dirent)
SET_TARGET_PROPERTIES(dirent PROPERTIES FOLDER "exts/dirent")

ADD_SUBDIRECTORY(stb)
SET_TARGET_PROPERTIES(stb PROPERTIES FOLDER "exts/stb")

ADD_SUBDIRECTORY(vector)
SET_TARGET_PROPERTIES(vector PROPERTIES FOLDER "exts/vector")