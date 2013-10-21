set(exe_name osgartviewer)


set(MISC
#	${CMAKE_SOURCE_DIR}/share/osgART/data/artoolkit/osgartviewer.cfg
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
	${OSG_LIBRARY}
	${OSGDB_LIBRARY}
	${OSGGA_LIBRARY}	
	${OSGVIEWER_LIBRARY}
)

install_osgart_example(${exe_name})
