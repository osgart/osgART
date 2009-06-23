set(exe_name osgartpick)

set(SRCS
	osgartpick.cpp
	)


if   (NOT APPLE)
	add_executable(${exe_name} ${SRCS})
else (NOT APPLE)
	add_executable(${exe_name} MACOSX_BUNDLE ${SRCS})
endif(NOT APPLE)


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
