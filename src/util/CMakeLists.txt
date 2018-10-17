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

qt5_wrap_cpp(util_mocs ${UTIL_SRCS} ${UTIL_HDRS})

add_library(util STATIC ${UTIL_SRCS} ${util_mocs})

target_include_directories (util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../map)
