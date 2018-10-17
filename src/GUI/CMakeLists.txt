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
qt5_wrap_cpp(gui_mocs ${GUI_SRCS} ${GUI_HDRS} ${gui_gen_srcs})

add_library(gui STATIC ${GUI_SRCS} ${gui_gen_srcs} ${gui_mocs})

set(GUI_GENERATED_HEADERS "${CMAKE_CURRENT_BINARY_DIR}" PARENT_SCOPE)
# - AUTOUIC version: set(GUI_GENERATED_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/gui_autogen/include" PARENT_SCOPE)

