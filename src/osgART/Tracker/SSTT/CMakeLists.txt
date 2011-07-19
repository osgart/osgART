#
# This module requires at least SSTT 2010.04 with respective export system
#

set(module_name osgart_tracker_sstt)

add_library(${module_name} MODULE SSTT.cpp)

find_package(SSTT)

set_target_properties(${module_name}
	PROPERTIES
	PREFIX ""
	PROJECT_LABEL "Tracker Plugin ${module_name}")


target_link_libraries(${module_name}
	${SSTT_LIBRARIES}
	osgART
)

install_osgart_plugin(${module_name})
