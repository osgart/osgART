set(lib_name "osgART")

#osgart_folder()

include_directories(${CMAKE_SOURCE_DIR}/include)

set(lib_srcs 
	Calibration.cpp
	Foundation.cpp
	ImageStreamCallback.cpp
	Marker.cpp
	MarkerCallback.cpp
	PluginManager.cpp
	Scene.cpp
	Tracker.cpp
	TransformFilterCallback.cpp
	Utils.cpp
	Video.cpp
	VideoConfig.cpp
	VideoGeode.cpp
	VideoImageStream.cpp
	VideoLayer.cpp
)

set(lib_hdrs 
	${CMAKE_SOURCE_DIR}/include/osgART/Calibration
	${CMAKE_SOURCE_DIR}/include/osgART/Export
	${CMAKE_SOURCE_DIR}/include/osgART/Field
	${CMAKE_SOURCE_DIR}/include/osgART/Foundation
	${CMAKE_SOURCE_DIR}/include/osgART/GeometryUtils
	${CMAKE_SOURCE_DIR}/include/osgART/ImageStreamCallback
	${CMAKE_SOURCE_DIR}/include/osgART/Marker
	${CMAKE_SOURCE_DIR}/include/osgART/MarkerCallback
	${CMAKE_SOURCE_DIR}/include/osgART/PluginManager
	${CMAKE_SOURCE_DIR}/include/osgART/Scene
	${CMAKE_SOURCE_DIR}/include/osgART/Tracker
	${CMAKE_SOURCE_DIR}/include/osgART/TransformFilterCallback
	${CMAKE_SOURCE_DIR}/include/osgART/Utils
	${CMAKE_SOURCE_DIR}/include/osgART/Video
	${CMAKE_SOURCE_DIR}/include/osgART/VideoConfig
	${CMAKE_SOURCE_DIR}/include/osgART/VideoGeode
	${CMAKE_SOURCE_DIR}/include/osgART/VideoImageStream
	${CMAKE_SOURCE_DIR}/include/osgART/VideoLayer
	${CMAKE_SOURCE_DIR}/include/osgART/VideoPlugin
)

# we are going to build a lib - tell the preprocessor
add_definitions(-DOSGART_LIBRARY)

source_group("Header Files" FILES ${lib_hdrs})

# create a shared library - always
add_library(${lib_name} SHARED ${lib_srcs} ${lib_hdrs})

# additional libs
set(addon_libs)

if (WIN32)
	# some target properties
	set_target_properties(${lib_name} 
		PROPERTIES	
		PREFIX "osg${OSG_VERSION_SO}-"
		IMPORT_PREFIX ""
		VERSION ${OSGART_VERSION_MAJOR}
		SOVERSION ${OSG_VERSION_SO}
	)
else(WIN32)
	# some target properties
	set_target_properties(${lib_name} 
		PROPERTIES	
		VERSION ${OSGART_VERSION_MAJOR}
		SOVERSION ${OSG_VERSION_SO}
	)
endif(WIN32)

# some finiky magic around Apple specifics
if   (APPLE)

	if (OSGART_USE_FRAMEWORKS)
		set_target_properties(${lib_name} PROPERTIES
		  FRAMEWORK TRUE
		  FRAMEWORK_VERSION ${OSGART_VERSION_FULL}
		  BUILD_WITH_INSTALL_RPATH 1
		  INSTALL_NAME_DIR "@executable_path/../Frameworks"
		)
	else(OSGART_USE_FRAMEWORKS)
		# do nothing
	endif(OSGART_USE_FRAMEWORKS)
	set(addon_libs ${CARBON_LIBRARY})
endif(APPLE)

# link external libs
target_link_libraries(${lib_name} 
	${OPENTHREADS_LIBRARY}
	${OSG_LIBRARY} 
	${OSGDB_LIBRARY} 
	${OPENGL_LIBRARY} 	
	${addon_libs}
	)	

add_subdirectory(Tracker)
add_subdirectory(Video)
	

set_source_files_properties(${lib_hdrs} 
	PROPERTIES
	HEADER_FILE_ONLY TRUE
	)

set(lib_libinstalldir ${OSGART_LIB_PREFIX})

if(WIN32)
	set(lib_libinstalldir bin)
endif(WIN32)

if (NOT OSGART_USE_FRAMEWORKS)
	install(TARGETS ${lib_name} 
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION ${lib_libinstalldir}
			ARCHIVE DESTINATION ${OSGART_LIB_PREFIX}
	#		FRAMEWORK DESTINATION /Library/Frameworks
			)
endif(NOT OSGART_USE_FRAMEWORKS)

install(FILES ${lib_hdrs} 
		DESTINATION include/${lib_name}
		)
		

#
# Install share/
#
install(DIRECTORY ${CMAKE_SOURCE_DIR}/share
	DESTINATION ${CMAKE_INSTALL_PREFIX}
	PATTERN ".svn" EXCLUDE
    )

#
# Optional SDK packaging (default is on)
#
if(WIN32)		
	option(OSGART_BUILD_SDK "Build as an SDK which will copy all necessary files into the osgART folder" ON)
	if(OSGART_BUILD_SDK)		
		install(
			DIRECTORY "${OSG_INCLUDE_DIR}/../"
			DESTINATION "."
		)
	endif()
endif()

