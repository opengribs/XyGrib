# 
# Try with pkg-config first
if( AEC_INCLUDE_DIR )
    SET( AEC_FIND_QUIETLY TRUE )
endif()

find_path(AEC_INCLUDE_DIR "libaec.h"
     PATHS
     $ENV{EXTERNLIBS}/include
     ~/Library/Frameworks/include
     /Library/Frameworks/include
     /usr/local/include
     /usr/include
     /sw/include # Fink
     /opt/local/include # DarwinPorts
     /opt/csw/include # Blastwave
     /opt/include
  DOC "AEC - Headers"
)

find_library(AEC_LIBRARY
     NAMES "aec" 
     PATHS
     $ENV{EXTERNLIBS}/lib
     ~/Library/Frameworks
     /Library/Frameworks
     /usr/local
     /usr
     /sw
     /opt/local
     /opt/csw
     /opt
     PATH_SUFFIXES lib lib64
     DOC "AEC - Library"
)

INCLUDE( "FindPackageHandleStandardArgs" )

MARK_AS_ADVANCED( AEC_LIBRARY AEC_INCLUDE_DIR  )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( AEC DEFAULT_MSG AEC_LIBRARY AEC_INCLUDE_DIR)

IF(AEC_FOUND)
    set(AEC_LIBRARIES ${AEC_LIBRARY} )
  SET(AEC_INCLUDE_DIRS ${AEC_INCLUDE_DIR})
ENDIF()
