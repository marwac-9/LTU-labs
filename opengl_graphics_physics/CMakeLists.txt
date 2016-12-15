#--------------------------------------------------------------------------
# opengl_graphics_physics
#--------------------------------------------------------------------------
PROJECT(opengl_graphics_physics)
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

ADD_SUBDIRECTORY(deps)
ADD_SUBDIRECTORY(shadows)
ADD_SUBDIRECTORY(deferred)
ADD_SUBDIRECTORY(physics)
ADD_SUBDIRECTORY(subdivision_optimizing)
ADD_SUBDIRECTORY(subdivision_notoptimized)
ADD_SUBDIRECTORY(subdivision_opengl)
ADD_SUBDIRECTORY(deferred_and_forward_particles)
ADD_SUBDIRECTORY(deferred_hdr_particles)
ADD_SUBDIRECTORY(deferred_hdr_bloom_particles)
ADD_SUBDIRECTORY(deferred_hdr_bloom_particles_scenegraph)
#ADD_SUBDIRECTORY(deferred_and_shadows)
