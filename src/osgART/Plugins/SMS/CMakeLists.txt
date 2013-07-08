set(module_name osgart_sms)

#
# using https://github.com/wouterverweirder/AIR-Sudden-Motion-Sensor-Extension
#

if(APPLE)

	option(OSGART_BUILD_PLUGIN_SMS "Build sudden motion sensor support (Mac OS X)" OFF)

	if(OSGART_BUILD_PLUGIN_SMS)

		add_library(${module_name} MODULE unimotion.c unimotion.h)
			set_target_properties(${module_name}
			PROPERTIES
			PREFIX ""
			PROJECT_LABEL "Plugin ${module_name}"
			)

		target_link_libraries(${module_name}
			osgART
			${OSG_LIBRARY}
			${OPENTHREADS_LIBRARY}
			)

		if(APPLE)
			target_link_libraries(${module_name}
				"-framework Cocoa"
				"-framework IOKit"
			)
		endif()

		install_osgart_plugin(${module_name})

	endif()

endif()
