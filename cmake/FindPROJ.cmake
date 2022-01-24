# - Find PROJ
# Find the PROJ includes and library
#
#  PROJ_INCLUDE_DIR - Where to find PROJ includes
#  PROJ_LIBRARIES   - List of libraries when using PROJ
#  PROJ_FOUND       - True if PROJ was found

IF(PROJ_INCLUDE_DIR)
  SET(PROJ_FIND_QUIETLY TRUE)
ENDIF(PROJ_INCLUDE_DIR)

FIND_PATH(PROJ_INCLUDE_DIR "proj.h"
  PATHS
  $ENV{EXTERNLIBS}/include
  $ENV{EXTERNLIBS}/PROJ/include
  ~/Library/Frameworks/include
  /Library/Frameworks/include
  /usr/local/include
  /usr/include
  /sw/include # Fink
  /opt/local/include # DarwinPorts
  /opt/csw/include # Blastwave
  /opt/include
  DOC "PROJ - Headers"
)

IF(PROJ_INCLUDE_DIR)
    SET(PROJ_NAMES PROJ proj proj_8_0)
    SET(PROJ_DBG_NAMES PROJD projD proj_8_0_1)
ELSE(PROJ_INCLUDE_DIR)
    FIND_PATH(PROJ_INCLUDE_DIR "proj_api.h"
      PATHS
      $ENV{EXTERNLIBS}/include
      $ENV{EXTERNLIBS}/PROJ/include
      ~/Library/Frameworks/include
      /Library/Frameworks/include
      /usr/local/include
      /usr/include
      /sw/include # Fink
      /opt/local/include # DarwinPorts
      /opt/csw/include # Blastwave
      /opt/include
      DOC "PROJ - Headers"
    )
    ADD_DEFINITIONS(-DACCEPT_USE_OF_DEPRECATED_PROJ_API_H=1)
    SET(PROJ_NAMES PROJ proj proj_4_9)
    SET(PROJ_DBG_NAMES PROJD projD proj_4_9_D)
ENDIF(PROJ_INCLUDE_DIR)

FIND_LIBRARY(PROJ_LIBRARY NAMES ${PROJ_NAMES}
  PATHS
  $ENV{EXTERNLIBS}
  $ENV{EXTERNLIBS}/PROJ
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  PATH_SUFFIXES lib lib64
  DOC "PROJ - Library"
)

INCLUDE(FindPackageHandleStandardArgs)

IF(MSVC)
  # VisualStudio needs a debug version
  FIND_LIBRARY(PROJ_LIBRARY_DEBUG NAMES ${PROJ_DBG_NAMES}
    PATHS
    $ENV{EXTERNLIBS}/PROJ/lib
    DOC "PROJ - Library (Debug)"
  )
  
  IF(PROJ_LIBRARY_DEBUG AND PROJ_LIBRARY)
    SET(PROJ_LIBRARIES optimized ${PROJ_LIBRARY} debug ${PROJ_LIBRARY_DEBUG})
  ENDIF(PROJ_LIBRARY_DEBUG AND PROJ_LIBRARY)

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(PROJ DEFAULT_MSG PROJ_LIBRARY PROJ_LIBRARY_DEBUG PROJ_INCLUDE_DIR)

  MARK_AS_ADVANCED(PROJ_LIBRARY PROJ_LIBRARY_DEBUG PROJ_INCLUDE_DIR)
  
ELSE(MSVC)
  # rest of the world
  SET(PROJ_LIBRARIES ${PROJ_LIBRARY})

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(PROJ DEFAULT_MSG PROJ_LIBRARY PROJ_INCLUDE_DIR)
  
  MARK_AS_ADVANCED(PROJ_LIBRARY PROJ_INCLUDE_DIR)
  
ENDIF(MSVC)

IF(PROJ_FOUND)
  SET(PROJ_INCLUDE_DIRS ${PROJ_INCLUDE_DIR})
ENDIF(PROJ_FOUND)
