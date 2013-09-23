PROJECT(OSGART_PLUGINS_MASTER)

IF(NOT DYNAMIC_OSGART)
    ADD_DEFINITIONS(-DOSGART_LIBRARY_STATIC)
ENDIF()

#IF(NOT MSVC)
#    SET(LIBRARY_OUTPUT_PATH "${LIBRARY_OUTPUT_PATH}/${OSG_PLUGINS}")
#    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${OSG_PLUGINS}")
#ENDIF()

#SET(CMAKE_SHARED_MODULE_PREFIX ${OSG_PLUGIN_PREFIX})

#IF(MSVC80 OR MSVC90)
#    IF(NOT OSG_MSVC_GENERATE_PLUGINS_AND_WRAPPERS_MANIFESTS)
#        SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
#    ENDIF()
#ENDIF()

#SET(TARGET_DEFAULT_PREFIX "osgdb_")
#SET(TARGET_DEFAULT_LABEL_PREFIX "Plugins")
#SET(TARGET_COMMON_LIBRARIES
#    OpenThreads
#    osg
#    osgDB
#    osgUtil
#)

############################################################
#
#  Video plugins
#
#ADD_SUBDIRECTORY(osga)

############################################################
#
#  Sensor plugins
#
#ADD_SUBDIRECTORY(osga)

############################################################
#
#  Tracker plugins
#
#ADD_SUBDIRECTORY(osga)

