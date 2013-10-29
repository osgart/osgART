
#dummy plugins
add_subdirectory(DummySensor)
add_subdirectory(DummyGPSSensor)
add_subdirectory(DummyInertialSensor)

add_subdirectory(DummyVideo)
add_subdirectory(DummyRGBDVideo)

add_subdirectory(DummyTracker)
add_subdirectory(DummyGPSInertialTracker)
add_subdirectory(DummySLAMTracker)

#multiplatforms video plugins
add_subdirectory(OSGVideo)
add_subdirectory(OpenCV)
add_subdirectory(OpenNI)
add_subdirectory(PCL)

#multiplatforms tracker plugins
add_subdirectory(ARToolKitPlus)
#add_subdirectory(ARma)
#add_subdirectory(aruco)

#multiplatforms sensor plugins

#multiplatforms video/tracker plugins
add_subdirectory(ARToolKit)

#multiplatforms sensor/tracker plugins

#platform specifics plugins
IF(APPLE)
	add_subdirectory(SMS)
	add_subdirectory(QTKit)
	add_subdirectory(AVFoundation)
ENDIF()

IF(WIN32)
	add_subdirectory(DSVideoLib)
	add_subdirectory(videoInput)
ENDIF()

IF(ANDROID)

ENDIF()

IF(IOS)

ENDIF()


