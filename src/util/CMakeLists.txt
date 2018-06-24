set(UTIL_HDRS
Font.h
Orthodromie.h
Settings.h
SylkFile.h
Util.h
zuFile.h
)

set(UTIL_SRCS
Font.cpp
Orthodromie.cpp
Settings.cpp
Util.cpp
zuFile.cpp
)

add_library(util ${UTIL_SRCS})

target_include_directories (util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../map)
