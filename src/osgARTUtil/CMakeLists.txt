set(lib_name "osgARTUtils")

#osgart_folder()

include_directories(${CMAKE_SOURCE_DIR}/include)

set(lib_srcs
	Tracker3DView.cpp
)

set(lib_hdrs
	${CMAKE_SOURCE_DIR}/include/osgARTUtils/Tracker3DView
)

# we are going to build a lib - tell the preprocessor
add_definitions(-DOSGARTUTIL_LIBRARY)

source_group("Header Files" FILES ${lib_hdrs})

# create a shared library - always
add_library(${lib_name} SHARED ${lib_srcs} ${lib_hdrs})

# additional libs
set(addon_libs)


if (WIN32)
	# some target properties
	set_target_properties(${lib_name}
		PROPERTIES
#		PREFIX "osg${OSG_VERSION_SO}-"
		IMPORT_PREFIX ""
#		VERSION ${OSGART_VERSION_MAJOR}
#		SOVERSION ${OSG_VERSION_SO}
	)
else(WIN32)
	# some target properties
#	set_target_properties(${lib_name}
#		PROPERTIES
#		VERSION ${OSGART_VERSION_MAJOR}
#		SOVERSION ${OSG_VERSION_SO}
#	)
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
	$(OSGART_LIBRARY}
	${addon_libs}
	)


if(UNIX AND "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
        message(STATUS "x86-64 detected")
        set_property(TARGET ${lib_name}
            PROPERTY
            COMPILE_FLAGS "-fPIC"
        )
endif()

set_source_files_properties(${lib_hdrs}
	PROPERTIES
	HEADER_FILE_ONLY TRUE
	)

set(lib_libinstalldir ${OSGART_LIB_PREFIX})

if(WIN32)
	set(lib_libinstalldir bin)
endif(WIN32)

if(APPLE)
	set(lib_libinstalldir ${CMAKE_INSTALL_PREFIX}/lib/)	
		install(TARGETS ${lib_name} 
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION ${lib_libinstalldir}
			# LIBRARY DESTINATION ${lib_libinstalldir}
			ARCHIVE DESTINATION lib
			)
endif(APPLE)

# if (NOT OSGART_USE_FRAMEWORKS)
# 	install(TARGETS ${lib_name}
# 			RUNTIME DESTINATION bin
# 			LIBRARY DESTINATION ${lib_libinstalldir}
# 			ARCHIVE DESTINATION ${OSGART_LIB_PREFIX}
# 	#		FRAMEWORK DESTINATION /Library/Frameworks
# 			)
# endif(NOT OSGART_USE_FRAMEWORKS)

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

