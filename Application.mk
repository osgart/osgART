APP_BUILD_SCRIPT := $(call my-dir)/Android.mk
APP_OPTIM := release

APP_PLATFORM 	:= android-10
#APP_STL 		:= gnustl_static
APP_CPPFLAGS 	:= -fexceptions -frtti
#APP_ABI 		:= armeabi armeabi-v7a
APP_ABI 		:= armeabi-v7a
APP_MODULES     := osgART osgart_tracker_vuforia osgart_video_vuforia