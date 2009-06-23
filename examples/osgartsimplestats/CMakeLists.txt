set(exe_name osgartsimplestats)

set(SRCS
	osgartsimplestats.cpp
	)



osgart_get_data()

if   (APPLE)
	add_executable(${exe_name} MACOSX_BUNDLE ${SRCS} ${OSGART_MISC_DATA})
else (APPLE)
	add_executable(${exe_name} ${SRCS})
endif(APPLE)

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
