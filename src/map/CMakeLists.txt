set(MAP_HDRS
GisReader.h
GshhsRangsReader.h
GshhsReader.h
POI.h
POI_Editor.h
PositionEditor.h
Projection.h
)

set(MAP_SRCS
GisReader.cpp
GshhsRangsReader.cpp
GshhsReader.cpp
POI.cpp
POI_Editor.cpp
PositionEditor.cpp
Projection.cpp
Projection_libproj.cpp
)

add_library(map ${MAP_SRCS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/util ${CMAKE_CURRENT_SOURCE_DIR}/GUI ${GUI_GENERATED_HEADERS})
target_include_directories (map PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../util ${CMAKE_CURRENT_SOURCE_DIR}/../GUI ${GUI_GENERATED_HEADERS})

target_link_libraries(map gui)
add_dependencies(map gui)

set(MAP_GENERATED_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/map_autogen/include" PARENT_SCOPE)
