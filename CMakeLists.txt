cmake_minimum_required(VERSION 2.8.2)

project(osgART)

#
# Patch in the osgART CMake module directory
# 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/etc/cmake)

if(WIN32)
	set(CMAKE_DEBUG_POSTFIX  "_debug")
endif(WIN32)

set(OSGART_VERSION_MAJOR 2)
set(OSGART_VERSION_MINOR 0)
set(OSGART_VERSION_PATCH 2)

set(OSGART_VERSION_FULL ${OSGART_VERSION_MAJOR}.${OSGART_VERSION_PATCH}.${OSGART_VERSION_PATCH})


# make executable style configurable
set(OSGART_EXECUTABLE_TYPE)

# some include
include(OSGARTUtils)

# Finding various packages
find_package(OpenSceneGraph 2.8.0 REQUIRED osgUtil osgDB osgGA osgText osgViewer)

# Find OpenGL
find_package(OpenGL REQUIRED)

# on *nix we need to make sure the local include
# directory is first
include_directories(
		${CMAKE_SOURCE_DIR}/include
		${OPENSCENEGRAPH_INCLUDE_DIRS}
		)

# Mac needs these for building the examples
if(APPLE)
    find_library(CARBON_LIBRARY Carbon REQUIRED)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(QUICKTIME_LIBRARY QuickTime REQUIRED)
endif(APPLE)

#
# If we are on 64bit machines we use lib64 for installing
#
set(OSGART_LIB_PREFIX "lib")
if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
	set(OSGART_LIB_PREFIX "lib64")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE PATH "Executables go here")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH "Libraries go here")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH "Libraries go here")

install(FILES
	${CMAKE_SOURCE_DIR}/AUTHORS.txt
	${CMAKE_SOURCE_DIR}/ChangeLog.txt
	${CMAKE_SOURCE_DIR}/README.txt
	${CMAKE_SOURCE_DIR}/LICENSE.txt
	DESTINATION "share/osgART" 
)

#
# Dive into the subdirectories
#
add_subdirectory(src)
add_subdirectory(applications)
add_subdirectory(examples)

#
# Build API docs / but only if Doxygen is available
#
find_package(Doxygen)
if   (DOXYGEN_FOUND)
	option(OSGART_BUILD_APIDOC "Build API Documentation" OFF)
	if   (OSGART_BUILD_APIDOC)
		
		configure_file(
			${CMAKE_SOURCE_DIR}/etc/cmake/osgART.dox.in
			${CMAKE_BINARY_DIR}/.doxygen/osgART.dox
			@ONLY
			)
			
		add_custom_target("Build API Docs"
			COMMENT "Building API docs from headers"
			COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/.doxygen/osgART.dox
			DEPENDS osgART
			VERBATIM
		)
		
		install(DIRECTORY ${CMAKE_BINARY_DIR}/doc
			DESTINATION "share/osgART"
			# COMPONENT "API Documentation" reenable when moving up to 2.6
			)	
	endif(OSGART_BUILD_APIDOC)
endif(DOXYGEN_FOUND)


#
# Uninstall
#
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/etc/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
  
add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
  
 
#
# Optional packaging for osgART 2.0 SDK
#
option(OSGART_BUILD_PACKAGE "Build distribution packages" OFF)
if   (OSGART_BUILD_PACKAGE)
	configure_file(
		${CMAKE_SOURCE_DIR}/etc/cmake/Packaging.cmake.in
		${CMAKE_BINARY_DIR}/.cpack/OSGARTPackaging.cmake	
		@ONLY IMMEDIATE
	)		
	include(${CMAKE_BINARY_DIR}/.cpack/OSGARTPackaging.cmake)			
endif (OSGART_BUILD_PACKAGE)


