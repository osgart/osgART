set(module_name "osgdb_osgart")

set(srcs
	IO_Video.cpp
	IO_VideoLayer.cpp
	)

add_library(${module_name} MODULE ${srcs})
	
	
set_target_properties(${module_name}
	PROPERTIES
	PREFIX ""
	PROJECT_LABEL "Database Plugin ${module_name}"
	)
	
target_link_libraries(${module_name}
	osgART
	${OSG_LIBRARY}
	${OSGDB_LIBRARY}
	)

	
install_osgart_plugin(${module_name})
	
