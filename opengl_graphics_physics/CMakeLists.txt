#--------------------------------------------------------------------------
# opengl_graphics_physics
#--------------------------------------------------------------------------
PROJECT(opengl_graphics_physics)
CMAKE_MINIMUM_REQUIRED(VERSION 3.11.1)

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

SET(BUILD_STATIC OFF CACHE BOOL "Build static")
if (BUILD_STATIC)
#IF(WIN32)
#    SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
#ELSE(WIN32)
#    SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
#ENDIF(WIN32)
#SET(BUILD_SHARED_LIBRARIES OFF)

set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
set(CMAKE_MFC_FLAG 1)
endif()

ADD_SUBDIRECTORY(deps)
ADD_SUBDIRECTORY(shadows)
ADD_SUBDIRECTORY(deferred)
ADD_SUBDIRECTORY(physics)
ADD_SUBDIRECTORY(subdivision_optimizing)
ADD_SUBDIRECTORY(subdivision_notoptimized)
ADD_SUBDIRECTORY(subdivision_opengl)
ADD_SUBDIRECTORY(deferred_and_forward_particles)
ADD_SUBDIRECTORY(deferred_hdr_bloom_particles_scenegraph)
ADD_SUBDIRECTORY(simpleWater)
