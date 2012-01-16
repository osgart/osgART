set(module_name osgart_tracker_stbnx)

if(OSGART_WITH_STBNX)

	find_package(StbNX)
	
	include_directories(${STBNX_INCLUDE_DIRS})

	add_definitions(-DSTBCORE_STATIC -DSTBMATH_STATIC -DSTBCV_STATIC -DSTBIO_STATIC -DSTBTRACKER_STATIC)
	
	set(stbnx_srcs)
	set(stbnx_libs)
	
	option(STBNX_USE_MARKERTRACKER "Use StbNX Markertracker" OFF)
	if(STBNX_USE_MARKERTRACKER)
		list(APPEND stbnx_srcs StbNX_MarkerTracker.cpp)
		list(APPEND stbnx_libs StbTracker)
	endif()
	
	option(STBNX_USE_NFT2 "Use StbNX NFT2 tracker" ON)
	if(STBNX_USE_NFT2)
		list(APPEND stbnx_srcs StbNX_NFT2.cpp)
		list(APPEND stbnx_libs NFT2)
	endif()
	
	add_library(${module_name} 
		MODULE 
		${stbnx_srcs}
		StbNX_Video.cpp
		)

	set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Tracker Plugin ${module_name}")

	target_link_libraries(${module_name}
		${STBNX_LIBRARIES}
		osgART
		)
	
	if(WIN32)
		target_link_libraries(${module_name} winmm)
	endif()

	install_osgart_plugin(${module_name})
endif()
