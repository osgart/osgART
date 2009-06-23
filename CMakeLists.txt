cmake_minimum_required(VERSION 2.6.1)

project(osgART)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/etc/cmake)

if(APPLE)
	option(OSGART_BUILD_BUNDLES "Builds bundles and frameworks for the library and examples")
endif(APPLE)

#
# Check if somebody wants to build in-tree
#
if   (${CMAKE_BINARY_DIR} EQUAL ${CMAKE_SOURCE_DIR})
	MESSAGE("In-source builds are discouraged!")
endif(${CMAKE_BINARY_DIR} EQUAL ${CMAKE_SOURCE_DIR})

if(WIN32)
	set(CMAKE_DEBUG_POSTFIX  "_debug")
endif(WIN32)

set(OSGART_VERSION_MAJOR 2)
set(OSGART_VERSION_MINOR 0)
set(OSGART_VERSION_PATCH 0)

set(OSGART_VERSION_FULL ${OSGART_VERSION_MAJOR}.${OSGART_VERSION_PATCH}.${OSGART_VERSION_PATCH})

# some include
include(OSGARTUtils)

# Finding various packages
find_package(OSG REQUIRED)
find_package(OpenThreads REQUIRED)
# Find OpenGL
find_package(OpenGL REQUIRED)

# on *nix we need to make sure the local include
# directory is first
include_directories(${CMAKE_SOURCE_DIR}/include)

if(APPLE)
	# darn bug in CMake: http://www.cmake.org/Bug/view.php?id=7407
	include_directories("/${OSG_INCLUDE_DIR}")
else(APPLE)
	include_directories(${OSG_INCLUDE_DIR})
endif(APPLE)

# we might want to honor the actual OSG Version
include(parse_OSG_version)

# check the real version
check_osg_version(${OSG_INCLUDE_DIR}/osg/Version)

# Mac needs these for building the examples
if(APPLE)
    find_library(CARBON_LIBRARY Carbon REQUIRED)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(QUICKTIME_LIBRARY QuickTime REQUIRED)
endif(APPLE)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

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
		@ONLY
	)		
	include(${CMAKE_BINARY_DIR}/.cpack/OSGARTPackaging.cmake)			
endif (OSGART_BUILD_PACKAGE)


