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

add_library(gui ${GUI_SRCS})

set(GUI_GENERATED_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/gui_autogen/include" PARENT_SCOPE)
