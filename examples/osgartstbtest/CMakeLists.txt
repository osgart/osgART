set(exe_name osgartstbtest)

set(SRCS
	osgartstbtest.cpp
	)

add_executable(${exe_name} ${OSGART_EXECUTABLE_TYPE} ${SRCS})

set_target_properties(${exe_name}
		PROPERTIES		
		PROJECT_LABEL "Example ${exe_name}")

target_link_libraries(${exe_name} 
	osgART 
	${OSG_LIBRARY}
	${OSGDB_LIBRARY}
	${OSGGA_LIBRARY}	
	${OSGVIEWER_LIBRARY}
)
install_osgart_example(${exe_name})
