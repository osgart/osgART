#
# Minimal FindOpenCV module
#

FIND_PATH(OPENCV_INCLUDE_DIR opencv/cxcore.h
  HINTS
  $ENV{OPENCV_DIR}
  $ENV{OPENCVDIR}
  #$ENV{PROGRAMFILES}/OpenCV
  ${OPENCV_DIR}
  PATH_SUFFIXES include
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw # Fink
    /opt/local # DarwinPorts
    /opt/csw # Blastwave
    /opt
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OPENCV_ROOT]
    [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;CV_ROOT]
)

FIND_LIBRARY(OpenCV_cxcore_LIBRARY 
  NAMES cxcore cxcore110
  HINTS
  $ENV{OPENCV_DIR}
  $ENV{OPENCVDIR}
  #$ENV{PROGRAMFILES}/OpenCV
  ${OPENCV_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)


FIND_LIBRARY(OpenCV_cv_LIBRARY 
  NAMES cv cv110
  HINTS
  $ENV{OPENCV_DIR}
  $ENV{OPENCVDIR}
  #$ENV{PROGRAMFILES}/OpenCV
  ${OPENCV_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

FIND_LIBRARY(OpenCV_highgui_LIBRARY 
  NAMES highgui highgui110
  HINTS
  $ENV{OPENCV_DIR}
  $ENV{OPENCVDIR}
  #$ENV{PROGRAMFILES}/OpenCV
  ${OPENCV_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

SET(OPENCV_FOUND "NO")

IF(OpenCV_cxcore_LIBRARY AND OPENCV_INCLUDE_DIR)
  SET(OPENCV_FOUND "YES")
  SET(OPENCV_LIBRARIES 
	${OpenCV_cxcore_LIBRARY}
	${OpenCV_cv_LIBRARY}
	${OpenCV_highgui_LIBRARY}
  )
ENDIF(OpenCV_cxcore_LIBRARY AND OPENCV_INCLUDE_DIR)
