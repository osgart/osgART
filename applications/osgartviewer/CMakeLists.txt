set(exe_name osgartviewer)

set(MISC
	${CMAKE_SOURCE_DIR}/share/osgART/data/artoolkit2/osgartviewer.cfg
	)

set(SRCS
	osgartviewer.cpp
	)

include_directories(${CMAKE_SOURCE_DIR}/include)

osgart_get_data()

add_executable(${exe_name} ${OSGART_EXECUTABLE_TYPE} ${SRCS} ${MISC})

set_target_properties(${exe_name}
		PROPERTIES		
		PROJECT_LABEL "Application ${exe_name}")

target_link_libraries(${exe_name} 
	osgART 
	${OPENSCENEGRAPH_LIBRARIES}
	)

install_osgart_example(${exe_name})
