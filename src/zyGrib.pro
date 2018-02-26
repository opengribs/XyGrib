CONFIG += qt release c++11
# CONFIG += qt debug c++11

QT += widgets xml
QT += printsupport

TEMPLATE = app
TARGET   = zyGrib

DEPENDPATH  += . util map GUI
INCLUDEPATH += . util map GUI curvedrawer qwt-6.1.3/src g2clib

# ----------------------------------------------------
# platform specific
# ----------------------------------------------------
win32 {
	INCLUDEPATH += C:/libs/include/
	INCLUDEPATH += C:/mingw/include/
	LIBS += -LC:/libs/lib/ -LC:/mingw/lib/
	DESTDIR = release
	RC_FILE += resource.rc
}
else {
macx {
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8
	INCLUDEPATH += /opt/local/include/
	DESTDIR = ..
	LIBS += -L/opt/local/lib
	CONFIG += i386
	ICON = ../data/img/zyGrib.icns
}
else {
# linux
	DESTDIR = .
}
}

LIBS += -lbz2 -lz -lproj -lnova -Lqwt-6.1.3/lib/ -lqwt
LIBS +=  -Lg2clib -lg2c -lpng -ljasper  # respect order!

# ----------------------------------------------------
CFLAGS += -std=c99 

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
	
QMAKE_LFLAGS_RELEASE -= -O1
QMAKE_LFLAGS_RELEASE += -O3

OBJECTS_DIR = objs
MOC_DIR = objs
UI_DIR  = GUI

QT += network xml

CODECFORTR = UTF-8
CODECFORSRC = UTF-8
TRANSLATIONS =  ../data/tr/zyGrib_fr.ts \
				../data/tr/zyGrib_cz.ts \
				../data/tr/zyGrib_de.ts \
				../data/tr/zyGrib_fi.ts \
				../data/tr/zyGrib_it.ts \
				../data/tr/zyGrib_nl.ts \
				../data/tr/zyGrib_pt.ts \
				../data/tr/zyGrib_ru.ts \
				../data/tr/zyGrib_ar.ts \
				../data/tr/zyGrib_gr.ts \
				../data/tr/zyGrib_es.ts

!win32 {
	# conditional, because uic dont work in my cross compilator environment.
	# ui_*.h files are made under linux before windows compilation.
	
	FORMS += GUI/PositionEditorWidget.ui \
		 	 GUI/PoiEditorDialog.ui \
			 GUI/ColorEditorWidget.ui \
			 GUI/LineEditorWidget.ui \
			 GUI/TextStyleEditorWidget.ui \
			 GUI/GraphicsParamsDialog.ui \
			 GUI/MeteotableOptionsDialog.ui \
		 	 GUI/AngleConverterWidget.ui 
}

HEADERS += \
			GUI/PositionEditorWidget.h \
			GUI/ColorEditorWidget.h \
			GUI/LineEditorWidget.h \
			GUI/TextStyleEditorWidget.h \
		 	GUI/AngleConverterWidget.h \
		 	GUI/AngleConverterDialog.h \
			curvedrawer/BoatSpeed.h \
			curvedrawer/CurveDrawer.h \
			curvedrawer/CustomQwtClasses.h \
		   Astro.h \
           BoardPanel.h \
		   ColorScale.h \
		   ColorScaleWidget.h \
		   DataDefines.h \
		   DataColors.h \
		   DataQString.h \
		   DataMeteoAbstract.h \
           DataPointInfo.h \
           DateChooser.h \
		   DialogBoxBase.h \
		   DialogBoxColumn.h \
           DialogFonts.h \
           DialogGraphicsParams.h \
           DialogLoadGRIB.h \
           DialogLoadIAC.h \
           DialogLoadMBLUE.h \
           DialogProxy.h \
           DialogSelectMetar.h \
           DialogServerStatus.h \
           DialogUnits.h \
           FileLoader.h \
           FileLoaderGRIB.h \
           FileLoaderIAC.h \
           FileLoaderMBLUE.h \
           util/Font.h \
           map/GshhsRangsReader.h \
           map/GshhsReader.h \
           map/GisReader.h \
           GribAnimator.h \
           GribPlot.h \
           Grib2Plot.h \
           GribReader.h \
           Grib2Reader.h \
           GribRecord.h \
           Grib2Record.h \
		   GriddedPlotter.h \
		   GriddedRecord.h \
		   GriddedReader.h \
           IacPlot.h \
           IacReader.h \
           ImageWriter.h \
		   IrregularGridded.h \
           IsoLine.h \
		   LongTaskProgress.h \
           LonLatGrid.h \
           MbzFile.h \
           MbluePlot.h \
           MblueReader.h \
           MblueRecord.h \
           Metar.h \
           MeteoTable.h \
           MeteoTableWidget.h \
           MeteotableOptionsDialog.h \
           MainWindow.h \
           MapDrawer.h \
           MenuBar.h \
           util/Orthodromie.h \
           map/POI.h \
           map/POI_Editor.h \
           map/PositionEditor.h \
           map/Projection.h \
           util/Settings.h \
           SkewT.h \
           util/SylkFile.h \
           Terrain.h \
           Therm.h \
           util/Util.h \
           Version.h \
           util/zuFile.h

SOURCES += \
			 GUI/PositionEditorWidget.cpp \
			 GUI/ColorEditorWidget.cpp \
			 GUI/LineEditorWidget.cpp \
			 GUI/TextStyleEditorWidget.cpp \
		 	 GUI/AngleConverterWidget.cpp \
		 	 GUI/AngleConverterDialog.cpp \
				curvedrawer/BoatSpeed.cpp \
				curvedrawer/CurveDrawer.cpp \
				curvedrawer/CustomQwtClasses.cpp \
		Astro.cpp \
        MbzFile.cpp \
		MblueRecord.cpp \
		MblueReader.cpp \
		MbluePlot.cpp \
           BoardPanel.cpp \
		   ColorScale.cpp \
		   ColorScaleWidget.cpp \
		   DataColors.cpp \
		   DataQString.cpp \
		   DataMeteoAbstract.cpp \
           DataPointInfo.cpp \
           DateChooser.cpp \
		   DialogBoxColumn.cpp \
           DialogFonts.cpp \
           DialogGraphicsParams.cpp \
           DialogLoadGRIB.cpp \
           DialogLoadIAC.cpp \
           DialogLoadMBLUE.cpp \
           DialogProxy.cpp \
           DialogSelectMetar.cpp \
           DialogServerStatus.cpp \
           DialogUnits.cpp \
           FileLoaderGRIB.cpp \
           FileLoaderIAC.cpp \
           FileLoaderMBLUE.cpp \
           util/Font.cpp \
		   GriddedPlotter.cpp \
		   GriddedReader.cpp \
		   GriddedRecord.cpp \
           map/GshhsRangsReader.cpp \
           map/GshhsReader.cpp \
           GribAnimator.cpp \
           GribPlot.cpp \
           Grib2Plot.cpp \
           map/GisReader.cpp \
           GribReader.cpp \
           Grib2Reader.cpp \
           GribRecord.cpp \
           Grib2Record.cpp \
           IacPlot.cpp \
           IacReader.cpp \
           IacReaderUtil.cpp \
           ImageWriter.cpp \
		   IrregularGridded.cpp \
           IsoLine.cpp \
		   LongTaskProgress.cpp \
           LonLatGrid.cpp \
           main.cpp \
           MainWindow.cpp \
           MapDrawer.cpp \
           MenuBar.cpp \
           Metar.cpp \
           MeteoTable.cpp \
           MeteoTableWidget.cpp \
           MeteotableOptionsDialog.cpp \
           util/Orthodromie.cpp \
           map/POI.cpp \
           map/POI_Editor.cpp \
           map/PositionEditor.cpp \
           map/Projection.cpp \
           map/Projection_libproj.cpp \
           util/Settings.cpp \
           SkewT.cpp \
           SkewTWindow.cpp \
           Terrain.cpp \
           Therm.cpp \
           util/Util.cpp \
           util/zuFile.cpp


