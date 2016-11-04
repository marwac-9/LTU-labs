#--------------------------------------------------------------------------
# gscept laboration environment
#--------------------------------------------------------------------------
PROJECT(mw_gscept_lab_env)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ENDIF()

IF(MSVC)
    SET(OPENGL_LIBS opengl32.lib)
ELSE()
    SET(OPENGL_LIBS GL GLU X11 Xxf86vm pthread Xrandr Xi Xinerama Xcursor)
ENDIF()

SET(GSCEPT_LAB_ENV_ROOT ${CMAKE_CURRENT_DIR})

SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS GLEW_STATIC)
ADD_SUBDIRECTORY(projects)

