#set(lib_name osgwrapper_osgART)


#find_package(osgIntrospection)

#option(OSGART_BUILD_WRAPPERS "Build the osgART wrappers" OFF)
#if(OSGINTROSPECTION_FOUND AND BUILD_OSGART_WRAPPERS)

#	include_directories(${CMAKE_SOURCE_DIR}/include ${OSGINTROSPECTION_INCLUDE_DIR})

#	set(srcs
#		CameraConfiguration.cpp
#		Field.cpp
#		ImageStreamCallback.cpp
#		MarkerCallback.cpp
#		Marker.cpp
#		PluginManager.cpp
#		Tracker.cpp
#		TransformFilterCallback.cpp
#		VideoConfig.cpp
#		Video.cpp
#		VideoGeode.cpp
#		VideoImageStream.cpp
#		VideoLayer.cpp
#		Scene.cpp
#		)

#	add_library(${lib_name} MODULE ${srcs})

#	target_link_libraries(${lib_name}
#		osgART
#		${OSG_LIBRARIES}
#		${OSGINTROSPECTION_LIBRARY}
#		)
	
#	set_target_properties(${lib_name} PROPERTIES
#		TARGET_NAME "osgART Introspection"
#		PREFIX ""
#		)
	
#	install_osgart_plugin(${lib_name})
#endif()
