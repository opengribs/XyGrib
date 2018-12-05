#add_subdirectory(openjpeg-2.3.0)
add_subdirectory(g2clib-1.6.0)
add_subdirectory(GUI)
add_subdirectory(util)
add_subdirectory(map)

set(XYGRIB_HDRS
Astro.h
BoardPanel.h
ColorScale.h
ColorScaleWidget.h
DataColors.h
DataDefines.h
DataMeteoAbstract.h
DataPointInfo.h
DataQString.h
DateChooser.h
DialogBoxBase.h
DialogBoxColumn.h
DialogFonts.h
DialogGraphicsParams.h
DialogLoadGRIB.h
DialogProxy.h
DialogSelectMetar.h
DialogServerStatus.h
DialogUnits.h
FileLoader.h
FileLoaderGRIB.h
Grib2Plot.h
Grib2Reader.h
Grib2Record.h
GribAnimator.h
GribPlot.h
GribReader.h
GribRecord.h
GriddedPlotter.h
GriddedReader.h
GriddedRecord.h
ImageWriter.h
IrregularGridded.h
IsoLine.h
LonLatGrid.h
LongTaskMessage.h
LongTaskProgress.h
MainWindow.h
MapDrawer.h
MenuBar.h
Metar.h
MeteoTable.h
MeteoTableWidget.h
MeteotableOptionsDialog.h
RegularGridded.h
RegularGriddedPlot.h
SkewT.h
Stylesheet.h
Terrain.h
Therm.h
#Version.h
)

set(XYGRIB_SRCS
Astro.cpp
BoardPanel.cpp
ColorScale.cpp
ColorScaleWidget.cpp
DataColors.cpp
DataMeteoAbstract.cpp
DataPointInfo.cpp
DataQString.cpp
DateChooser.cpp
DialogBoxColumn.cpp
DialogFonts.cpp
DialogGraphicsParams.cpp
DialogLoadGRIB.cpp
DialogProxy.cpp
DialogSelectMetar.cpp
DialogServerStatus.cpp
DialogUnits.cpp
FileLoaderGRIB.cpp
Grib2Plot.cpp
Grib2Reader.cpp
Grib2Record.cpp
GribAnimator.cpp
GribPlot.cpp
GribReader.cpp
GribRecord.cpp
GriddedPlotter.cpp
GriddedReader.cpp
GriddedRecord.cpp
ImageWriter.cpp
IrregularGridded.cpp
IsoLine.cpp
LonLatGrid.cpp
LongTaskMessage.cpp
LongTaskProgress.cpp
MainWindow.cpp
MapDrawer.cpp
MenuBar.cpp
Metar.cpp
MeteoTable.cpp
MeteoTableWidget.cpp
MeteotableOptionsDialog.cpp
SkewT.cpp
SkewTWindow.cpp
Terrain.cpp
Therm.cpp
main.cpp
)
IF(WIN32)
  SET(XYGRIB_SRCS ${XYGRIB_SRCS} ../data/img/resource.rc)
ENDIF()

qt5_wrap_cpp(xygrib_mocs ${XYGRIB_SRCS} ${XYGRIB_HDRS})

add_executable(${CMAKE_PROJECT_NAME} WIN32 MACOSX_BUNDLE ${XYGRIB_SRCS} ${xygrib_mocs})

install(TARGETS
    ${CMAKE_PROJECT_NAME}
    BUNDLE DESTINATION ${PREFIX_BIN} RUNTIME DESTINATION ${PREFIX_BIN} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/util)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/map ${MAP_GENERATED_HEADERS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GUI ${GUI_GENERATED_HEADERS})

target_link_libraries(${CMAKE_PROJECT_NAME} g2clib gui util map ${LIBNOVA_LIBRARY} ${OPENJPEG_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5PrintSupport_LIBRARIES} ${BZIP2_LIBRARIES} ${ZLIB_LIBRARIES} ${PROJ4_LIBRARIES} ${PNG_LIBRARIES})

# Sanitizers, part 2/2
if ( CMAKE_VERSION VERSION_GREATER 3.4 )
  if (NOT "${ENABLE_SANITIZER}" MATCHES "none")
     target_link_libraries(${CMAKE_PROJECT_NAME} -fsanitize=${ENABLE_SANITIZER})
  endif()
endif()
