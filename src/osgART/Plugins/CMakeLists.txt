
#dummy plugins
add_subdirectory(DummyVideo)
add_subdirectory(DummyTracker)

#multiplatforms video plugins
add_subdirectory(OpenCV)

#multiplatforms tracker plugins

#multiplatforms sensor plugins

#multiplatforms video/tracker plugins
add_subdirectory(ARToolKit)

#multiplatforms sensor/tracker plugins

#platform specifics plugins
if(APPLE)
	add_subdirectory(SMS)
#	add_subdirectory(QTKit)
#	add_subdirectory(AVFoundation)
endif()

if (WIN32)
#	add_subdirectory(DSVL)
#	add_subdirectory(videoInput)
endif()
