set(module_name osgart_video_videoinput)

include_directories("D:\\Development\\videoInput0.1995\\compiledLib\\compiledByVS2008\\include")

add_library(${module_name} MODULE VideoInput.cpp)	

	set_target_properties(${module_name}
	PROPERTIES
	PREFIX ""
	PROJECT_LABEL "Video Plugin ${module_name}"
	)

set(addon_libs)


target_link_libraries(${module_name} 
	osgART 
	${OSG_LIBRARY} 
	${OPENTHREADS_LIBRARY} 
	${addon_libs}
	"D:\\Development\\videoInput0.1995\\compiledLib\\compiledByVS2008\\videoInput.lib"
	)
	
install_osgart_plugin(${module_name})