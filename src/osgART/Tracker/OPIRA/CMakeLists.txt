set(module_name osgart_tracker_opira)

find_package(OpenCV2)

set(SRCS 
	OPIRATracker.cpp
)

set(HDRS
	${CMAKE_CURRENT_SOURCE_DIR}/OPIRATracker
	${OPIRA_INCLUDE_DIR}
	${OPIRA_REGISTRATION_INCLUDE_DIR}
	${OPENCV2_INCLUDE_DIR}
	)
	
set_source_files_properties(${HDRS}
	PROPERTIES
	HEADER_FILE_ONLY TRUE
	)
	
source_group("Header Files" FILES
	${HDRS})

source_group("Source Files" FILES
	${SRCS})	

include_directories(
	"." 
	${OPIRA_INCLUDE_DIR}
	${OPIRA_REGISTRATION_INCLUDE_DIR}
	${OPIRA_MT_INCLUDE_DIR}
	${OPIRA_GESTURE_INCLUDE_DIR}
	${OPIRA_CAPTURE_INCLUDE_DIR}
	${OPENCV2_INCLUDE_DIR}/opencv
)
	
add_library(${module_name} MODULE ${SRCS})

set_target_properties(${module_name}
	PROPERTIES
	PREFIX ""
	PROJECT_LABEL "Tracker Plugin ${module_name}")

target_link_libraries(${module_name} 
	${OSG_LIBRARY}
	${OSGDB_LIBRARY} 
	${OPENTHREADS_LIBRARY} 
	${OPIRA_LIBRARIES}
	${OPENCV2_LIBRARIES}
	osgART
	)

install_osgart_plugin(${module_name})