#
# Minimal FindOpenCV module for OpenCV2
#

FIND_PATH(OPENCV2_INCLUDE_DIR opencv/cxcore.h
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

FIND_LIBRARY(OpenCV2_cxcore_LIBRARY 
  NAMES cxcore cxcore200
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


FIND_LIBRARY(OpenCV2_cv_LIBRARY 
  NAMES cv cv200
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

FIND_LIBRARY(OpenCV2_highgui_LIBRARY 
  NAMES highgui highgui200
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

SET(OPENCV2_FOUND "NO")

IF(OpenCV2_cxcore_LIBRARY AND OPENCV2_INCLUDE_DIR)
  SET(OPENCV2_FOUND "YES")
  SET(OPENCV2_LIBRARIES 
	${OpenCV2_cxcore_LIBRARY}
	${OpenCV2_cv_LIBRARY}
	${OpenCV2_highgui_LIBRARY}
  )
ENDIF(OpenCV2_cxcore_LIBRARY AND OPENCV2_INCLUDE_DIR)
