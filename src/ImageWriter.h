/**********************************************************************
zyGrib: meteorological GRIB file viewer
Copyright (C) 2008-2012 - Jacques Zaninetti - http://www.zygrib.org

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#ifndef IMAGEWRITER_H
#define IMAGEWRITER_H

#include <QSpinBox>
#include <QCheckBox>

#include "DialogBoxColumn.h"
#include "Projection.h"
#include "Terrain.h"
#include "DateChooser.h"
#include "ColorScaleWidget.h"

//-------------------------------------------
class ImageWriterDialog : public DialogBoxColumn 
{ Q_OBJECT
	public:
		ImageWriterDialog (QWidget *parent, int W, int H, int quality, 
						   bool resizeAfter, 
						   bool showDateCursor, bool showColorScale,
						   int Winit, int Hinit);
		
		int getW () {return sbWidth->value ();}
		int getH () {return sbHeight->value ();}
		int getQ () {return sbQuality->value ();}
		bool getResizeAfter () {return cbResizeAfter->isChecked ();}
		bool getShowDateCursor () {return cbShowDateCursor->isChecked ();}
		bool getShowColorScale () {return cbShowColorScale->isChecked ();}
		
		QSpinBox *sbWidth;
		QSpinBox *sbHeight;
		QSpinBox *sbQuality;
		QCheckBox *cbAsScreen;
		QCheckBox *cbResizeAfter;
		QCheckBox *cbShowDateCursor;
		QCheckBox *cbShowColorScale;
		
    public slots:
        void slotUpdateWidgets ();
		
	private:
		int Winit, Hinit;
};

//-------------------------------------------
class ImageWriter : public QObject
{ Q_OBJECT
	public:
		ImageWriter (QWidget *parent, Terrain *terre);
		~ImageWriter ();

		void saveImage (time_t date);
		void saveCurrentImage ();
		void saveAllImages ();

	private:
		Terrain  *terre;
		QWidget  *parent;
		
		void saveSettings (ImageWriterDialog &dial, QString filename);
		QImage * createImage (
					time_t date, ImageWriterDialog &dial, 
					int Winit, int Hinit);
		QString createAnimImageFilename (QString prefix, int n);
};




#endif
