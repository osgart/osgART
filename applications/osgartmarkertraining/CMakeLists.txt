set(exe_name osgartmarkertraining)

set(SRCS
	osgartmarkertraining.cpp
	)

osgart_get_data()

add_executable(${exe_name} ${OSGART_EXECUTABLE_TYPE} ${SRCS})
		
set_target_properties(${exe_name}
		PROPERTIES		
		PROJECT_LABEL "Application ${exe_name}")

target_link_libraries(${exe_name} 
	osgART 
	${OSG_LIBRARY}
	${OSGDB_LIBRARY}
	${OSGGA_LIBRARY}
	${OSGTEXT_LIBRARY}	
	${OSGVIEWER_LIBRARY}
)

install_osgart_example(${exe_name})
