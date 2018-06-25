set(GUI_UI
AngleConverterWidget.ui
ColorEditorWidget.ui
FindCityDialog.ui
GraphicsParamsDialog.ui
GribLoadDialog.ui
LineEditorWidget.ui
MeteotableOptionsDialog.ui
PoiEditorDialog.ui
PositionEditorWidget.ui
TextStyleEditorWidget.ui
)

set(GUI_HDRS
AngleConverterDialog.h
AngleConverterWidget.h
ColorEditorWidget.h
LineEditorWidget.h
PositionEditorWidget.h
TextStyleEditorWidget.h
)

set(GUI_SRCS
AngleConverterDialog.cpp
AngleConverterWidget.cpp
ColorEditorWidget.cpp
LineEditorWidget.cpp
PositionEditorWidget.cpp
TextStyleEditorWidget.cpp
)

qt5_wrap_ui(gui_gen_srcs ${GUI_UI})

add_library(gui ${GUI_SRCS} ${gui_gen_srcs})

set(GUI_GENERATED_HEADERS "${CMAKE_CURRENT_BINARY_DIR}" PARENT_SCOPE)
# - AUTOUIC version: set(GUI_GENERATED_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/gui_autogen/include" PARENT_SCOPE)

