LOCAL_PATH 				:= $(call my-dir)
STL_PATH 				:= c:/users/julian/documents/development/android/android-ndk-r8e/sources/cxx-stl/gnu-libstdc++/4.6
LIBDIR 					:= $(OSG_ANDROID_DIR)/obj/local/$(TARGET_ARCH_ABI)

# ---------------------------------------------------
# osgART
# ---------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE    		:= osgART
LOCAL_ARM_NEON 			:= true
LOCAL_C_INCLUDES		:= $(OSG_ANDROID_DIR)/include
LOCAL_C_INCLUDES 		+= $(LOCAL_PATH)/include
LOCAL_C_INCLUDES 		+= $(STL_PATH)/include 
LOCAL_C_INCLUDES 		+= $(STL_PATH)/libs/armeabi-v7a/include
LOCAL_CFLAGS    		:= -Werror -fno-short-enums
LOCAL_CPPFLAGS  		:= -DOSG_LIBRARY_STATIC 
LOCAL_LDLIBS    		:= -llog -lGLESv2 -lz $(STL_PATH)/libs/armeabi-v7a/libgnustl_static.a
MY_FILES 				:= $(wildcard $(LOCAL_PATH)/src/osgART/*.cpp)
LOCAL_SRC_FILES 		:= $(MY_FILES:$(LOCAL_PATH)/%=%) 

LOCAL_LDFLAGS   := -L $(LIBDIR) \
-losgdb_dds \
-losgdb_openflight \
-losgdb_tga \
-losgdb_rgb \
-losgdb_osgterrain \
-losgdb_osg \
-losgdb_ive \
-losgdb_bmp \
-losgdb_obj \
-losgdb_deprecated_osgviewer \
-losgdb_deprecated_osgvolume \
-losgdb_deprecated_osgtext \
-losgdb_deprecated_osgterrain \
-losgdb_deprecated_osgsim \
-losgdb_deprecated_osgshadow \
-losgdb_deprecated_osgparticle \
-losgdb_deprecated_osgfx \
-losgdb_deprecated_osganimation \
-losgdb_deprecated_osg \
-losgdb_serializers_osgvolume \
-losgdb_serializers_osgtext \
-losgdb_serializers_osgterrain \
-losgdb_serializers_osgsim \
-losgdb_serializers_osgshadow \
-losgdb_serializers_osgparticle \
-losgdb_serializers_osgmanipulator \
-losgdb_serializers_osgfx \
-losgdb_serializers_osganimation \
-losgdb_serializers_osg \
-losgViewer \
-losgVolume \
-losgTerrain \
-losgText \
-losgShadow \
-losgSim \
-losgParticle \
-losgManipulator \
-losgGA \
-losgFX \
-losgDB \
-losgAnimation \
-losgUtil \
-losg \
-lOpenThreads \

include $(BUILD_STATIC_LIBRARY)

# ---------------------------------------------------
# osgart_tracker_null
# ---------------------------------------------------
#include $(CLEAR_VARS)
#LOCAL_MODULE    		:= osgart_tracker_null
#LOCAL_STATIC_LIBRARIES 	:= osgART
#LOCAL_ARM_NEON 			:= true
#LOCAL_C_INCLUDES		:= $(OSG_ANDROID_DIR)/include
#LOCAL_C_INCLUDES 		+= $(LOCAL_PATH)/include
#LOCAL_C_INCLUDES 		+= $(STL_PATH)/include 
#LOCAL_C_INCLUDES 		+= $(STL_PATH)/libs/armeabi-v7a/include
#LOCAL_CFLAGS    		:= -Werror -fno-short-enums
#LOCAL_CPPFLAGS  		:= -DOSG_LIBRARY_STATIC 
#LOCAL_LDLIBS    		:= -llog -lGLESv2 -lz $(STL_PATH)/libs/armeabi-v7a/libgnustl_static.a
#MY_FILES 				:= $(wildcard $(LOCAL_PATH)/src/osgART/Tracker/Null/*.cpp)
#LOCAL_SRC_FILES 		:= $(MY_FILES:$(LOCAL_PATH)/%=%) 
#LOCAL_LDFLAGS   		:= -L $(LIBDIR) -losgDB -losgUtil -losg -lOpenThreads
#include $(BUILD_STATIC_LIBRARY)

# ---------------------------------------------------
# Pull in the QCAR shared library
# ---------------------------------------------------
include $(CLEAR_VARS)
#QCAR_ROOT_DIR 			:= ../vuforia-sdk-android-1-5-9
QCAR_ROOT_DIR 			:= ../vuforia-sdk-android-2-5-7
LOCAL_MODULE 			:= QCAR-prebuilt
LOCAL_SRC_FILES 		:= $(QCAR_ROOT_DIR)/build/lib/$(TARGET_ARCH_ABI)/libQCAR.so
LOCAL_EXPORT_C_INCLUDES := $(QCAR_ROOT_DIR)/build/include
include $(PREBUILT_SHARED_LIBRARY)

# ---------------------------------------------------
# osgart_tracker_vuforia
# ---------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE    		:= osgart_tracker_vuforia
LOCAL_STATIC_LIBRARIES 	:= osgART
LOCAL_SHARED_LIBRARIES 	:= QCAR-prebuilt
LOCAL_ARM_NEON 			:= true
LOCAL_C_INCLUDES		:= $(OSG_ANDROID_DIR)/include
LOCAL_C_INCLUDES 		+= $(LOCAL_PATH)/include
LOCAL_C_INCLUDES 		+= $(LOCAL_PATH)/src/osgART/Plugins/Vuforia
LOCAL_C_INCLUDES 		+= $(STL_PATH)/include 
LOCAL_C_INCLUDES 		+= $(STL_PATH)/libs/armeabi-v7a/include
LOCAL_CFLAGS    		:= -Werror -fno-short-enums
LOCAL_CPPFLAGS  		:= -DOSG_LIBRARY_STATIC 
LOCAL_LDLIBS    		:= -llog -lGLESv2 -lz $(STL_PATH)/libs/armeabi-v7a/libgnustl_static.a
MY_FILES 				:= $(LOCAL_PATH)/src/osgART/Plugins/Vuforia/VuforiaTracker.cpp $(LOCAL_PATH)/src/osgART/Plugins/Vuforia/VuforiaTarget.cpp 
LOCAL_SRC_FILES 		:= $(MY_FILES:$(LOCAL_PATH)/%=%) 
LOCAL_LDFLAGS   		:= -L $(LIBDIR) -losgDB -losgUtil -losg -lOpenThreads
include $(BUILD_STATIC_LIBRARY)

# ---------------------------------------------------
# osgart_video_vuforia
# ---------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE    		:= osgart_video_vuforia
LOCAL_STATIC_LIBRARIES 	:= osgART
LOCAL_SHARED_LIBRARIES 	:= QCAR-prebuilt
LOCAL_ARM_NEON 			:= true
LOCAL_C_INCLUDES		:= $(OSG_ANDROID_DIR)/include
LOCAL_C_INCLUDES 		+= $(LOCAL_PATH)/include
LOCAL_C_INCLUDES 		+= $(LOCAL_PATH)/src/osgART/Plugins/Vuforia
LOCAL_C_INCLUDES 		+= $(STL_PATH)/include 
LOCAL_C_INCLUDES 		+= $(STL_PATH)/libs/armeabi-v7a/include
LOCAL_CFLAGS    		:= -Werror -fno-short-enums
LOCAL_CPPFLAGS  		:= -DOSG_LIBRARY_STATIC 
LOCAL_LDLIBS    		:= -llog -lGLESv2 -lz $(STL_PATH)/libs/armeabi-v7a/libgnustl_static.a
MY_FILES 				:= $(LOCAL_PATH)/src/osgART/Plugins/Vuforia/VuforiaVideo.cpp
LOCAL_SRC_FILES 		:= $(MY_FILES:$(LOCAL_PATH)/%=%) 
LOCAL_LDFLAGS   		:= -L $(LIBDIR) -losgDB -losgUtil -losg -lOpenThreads
include $(BUILD_STATIC_LIBRARY)


# ---------------------------------------------------
# osgart_video_dummyimage
# ---------------------------------------------------
#include $(CLEAR_VARS)
#LOCAL_MODULE    		:= osgart_video_dummyimage
#LOCAL_STATIC_LIBRARIES 	:= osgART
#LOCAL_ARM_NEON 			:= true
#LOCAL_C_INCLUDES		:= $(OSG_ANDROID_DIR)/include
#LOCAL_C_INCLUDES 		+= $(LOCAL_PATH)/include
#LOCAL_C_INCLUDES 		+= $(STL_PATH)/include 
#LOCAL_C_INCLUDES 		+= $(STL_PATH)/libs/armeabi-v7a/include
#LOCAL_C_INCLUDES 		+= $(LOCAL_PATH)/src/osgART/Video/DummyImage
#LOCAL_CFLAGS    		:= -Werror -fno-short-enums
#LOCAL_CPPFLAGS  		:= -DOSG_LIBRARY_STATIC 
#LOCAL_LDLIBS    		:= -llog -lGLESv2 -lz $(STL_PATH)/libs/armeabi-v7a/libgnustl_static.a
#MY_FILES 				:= $(wildcard $(LOCAL_PATH)/src/osgART/Video/DummyImage/*.cpp)
#LOCAL_SRC_FILES 		:= $(MY_FILES:$(LOCAL_PATH)/%=%) 
#LOCAL_LDFLAGS   		:= -L $(LIBDIR) -losgDB -losgUtil -losg -lOpenThreads
#include $(BUILD_STATIC_LIBRARY)
