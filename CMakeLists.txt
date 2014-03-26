# ==============================
# CMAKE Configuration for OSGART
# ==============================

# CMake version
cmake_minimum_required(VERSION 2.8.9)

# ==== Global Configuration ====


# Project definition
project(osgART)


# ==== Find Packages/Libs Dependencies ====

# Find OpenSceneGraph + NodeKits
#note: minimum version: 3.2.0
find_package(OpenSceneGraph 3.2.0 REQUIRED osg osgUtil osgDB osgGA osgText osgViewer osgParticle osgManipulator)

# Find OpenGL
find_package(OpenGL REQUIRED)

# OSGART Version
set(OSGART_VERSION_MAJOR 2)
set(OSGART_VERSION_MINOR 0)
set(OSGART_VERSION_PATCH 2)

set(OSGART_VERSION_FULL ${OSGART_VERSION_MAJOR}.${OSGART_VERSION_PATCH}.${OSGART_VERSION_PATCH})

# Patch in the osgART CMake module directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeModules)

if(WIN32)
	set(CMAKE_DEBUG_POSTFIX  "_debug")
endif(WIN32)

# make executable style configurable
set(OSGART_EXECUTABLE_TYPE)

# some include
include(OSGARTUtils)



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
# set(OSGART_LIB_PREFIX "lib")
# if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
# 	set(OSGART_LIB_PREFIX "lib64")
# endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE PATH "Executables go here")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH "Libraries go here")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH "Libraries go here")
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/staging CACHE PATH "Installation goes here")

install(FILES
	${CMAKE_SOURCE_DIR}/AUTHORS.txt
	${CMAKE_SOURCE_DIR}/ChangeLog.txt
	${CMAKE_SOURCE_DIR}/README.txt
	${CMAKE_SOURCE_DIR}/LICENSE.txt
	DESTINATION "share/osgART"
)


# ==== Find Optional Packages/Libs Dependencies ====

## Examples
option(OSGART_BUILD_EXAMPLES "Build all examples" OFF)
option(OSGART_BUILD_APPLICATIONS "Build all applications (viewer)" ON)

# Sensor Plugins

# ==== CMake  ====

#
# Dive into the subdirectories
#
add_subdirectory(src)

if(OSGART_BUILD_APPLICATIONS)
	add_subdirectory(applications)
endif()

if(OSGART_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

#
# Build API docs / but only if Doxygen is available
#
find_package(Doxygen)
if(DOXYGEN_FOUND)
	option(OSGART_BUILD_APIDOC "Build API Documentation" OFF)
	if   (OSGART_BUILD_APIDOC)

		configure_file(
			${CMAKE_SOURCE_DIR}/CMakeModules/osgART.dox.in
			${CMAKE_BINARY_DIR}/.doxygen/osgART.dox
			@ONLY
			)

		add_custom_target("docs"
			COMMENT "Building API docs from headers"
			COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/.doxygen/osgART.dox
			DEPENDS osgART
			VERBATIM
		)

		install(DIRECTORY ${CMAKE_BINARY_DIR}/doc
			DESTINATION "share/osgART"
			COMPONENT "API Documentation"
			)
	endif()
endif()

# ==== Uninstall ====

#
# Uninstall
#
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")


 # ==== Packaging ====

#
# Optional packaging for osgART 2.0 SDK
#
option(OSGART_BUILD_PACKAGE "Build distribution packages" OFF)
if(OSGART_BUILD_PACKAGE)
	configure_file(
		${CMAKE_SOURCE_DIR}/CMakeModules/Packaging.cmake.in
		${CMAKE_BINARY_DIR}/.cpack/OSGARTPackaging.cmake
		@ONLY IMMEDIATE
	)
	include(${CMAKE_BINARY_DIR}/.cpack/OSGARTPackaging.cmake)
endif()


