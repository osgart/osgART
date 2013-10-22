
include_directories(${OSG_INCLUDE_DIR} ${OPENTHREADS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(testbasic)
add_subdirectory(testplugins)

add_subdirectory(advancedAPI)
add_subdirectory(intermAPI)
add_subdirectory(simpleAPI)
