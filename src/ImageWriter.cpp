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

#include <QImageWriter>
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractEventDispatcher>

#include "ImageWriter.h"
#include "Util.h"


//==============================================================
ImageWriterDialog::ImageWriterDialog 
						(QWidget *parent, int W, int H, int quality, 
						 bool resizeAfter, 
						 bool showDateCursor, bool showColorScale,
						 int Winit, int Hinit)
		: DialogBoxColumn (parent, 1, 
							tr("Image parameters"), 
							tr("Image parameters"),
						   2)
{
	this->Winit = Winit;
	this->Hinit = Hinit;
	sbWidth = new QSpinBox (this);
	sbWidth->setRange (1, 999999);
	sbWidth->setValue (W);
	
	sbHeight = new QSpinBox (this);
	sbHeight->setRange (1, 999999);
	sbHeight->setValue (H);
	
	sbQuality = new QSpinBox (this);
	sbQuality->setRange (0, 100);
	sbQuality->setSuffix (" %");
	sbQuality->setValue (quality);
	
	cbResizeAfter = new QCheckBox (this);
	cbResizeAfter->setChecked (resizeAfter);
	
	cbShowDateCursor = new QCheckBox (this);
	cbShowDateCursor->setChecked (showDateCursor);
	cbShowColorScale = new QCheckBox (this);
	cbShowColorScale->setChecked (showColorScale);
	
	cbAsScreen = new QCheckBox (this);
	
	cbAsScreen->setChecked (W==Winit && H==Hinit);
	
	addLabeledWidget (0, tr("Current size"), cbAsScreen);
	addLabeledWidget (0, tr("Width"), sbWidth);
	addLabeledWidget (0, tr("Height"), sbHeight);
	addLabeledWidget (0, tr("Quality"), sbQuality);
	addLabeledWidget (0, tr("Resize after"), cbResizeAfter);
	addLabeledWidget (0, tr("Display date cursor"), cbShowDateCursor);
	addLabeledWidget (0, tr("Display color scale"), cbShowColorScale);

	connect(sbWidth,  SIGNAL(valueChanged(int)), this, SLOT(slotUpdateWidgets()));
	connect(sbHeight,  SIGNAL(valueChanged(int)), this, SLOT(slotUpdateWidgets()));
	connect(cbAsScreen,  SIGNAL(stateChanged(int)), this, SLOT(slotUpdateWidgets()));
	connect(cbResizeAfter,  SIGNAL(stateChanged(int)), this, SLOT(slotUpdateWidgets()));
	connect(cbShowDateCursor,  SIGNAL(stateChanged(int)), this, SLOT(slotUpdateWidgets()));
	connect(cbShowColorScale,  SIGNAL(stateChanged(int)), this, SLOT(slotUpdateWidgets()));
}
//---------------------------------------------------------
void ImageWriterDialog::slotUpdateWidgets ()
{
	if (sender() == cbAsScreen) {
		if (cbAsScreen->isChecked()) {
			sbWidth->setValue (Winit);
			sbHeight->setValue (Hinit);
		}
	}
	else {
		 cbAsScreen->setChecked (sbWidth->value()==Winit && sbHeight->value()==Hinit);
	}
}

//==============================================================
ImageWriter::ImageWriter (QWidget *parent, Terrain *terre)
{
	this->parent = parent;
	this->terre = terre;
}

//-------------------------------------------------
ImageWriter::~ImageWriter ()
{
}

//-------------------------------------------------
QImage * ImageWriter::createImage (
			time_t date, ImageWriterDialog &dial, int Winit, int Hinit)
{
	QPixmap *pixmap = NULL;
	QImage *image = NULL;
	int W = dial.getW ();
	int H = dial.getH ();
	
	bool showDateCursor = dial.getShowDateCursor ();
	bool showColorScale = dial.getShowColorScale ();
	
	int HDateChooser = showDateCursor ? 16 : 0;
	int WColorScale = showColorScale ? 32 : 0;
	
	if (dial.getResizeAfter() && (W!=Winit || H!=Hinit) )
	{
		QPixmap *pixmap2 =  terre->createPixmap (date, Winit, Hinit);
		if (pixmap2) {
			pixmap = new QPixmap (pixmap2->scaled (
									W-WColorScale, H-HDateChooser,
									Qt::KeepAspectRatioByExpanding,
									Qt::SmoothTransformation));
			delete pixmap2;
		}
	}
	else {
		pixmap =  terre->createPixmap (date, W-WColorScale, H-HDateChooser);
	}
	
	if (pixmap) {
		image = new QImage (W, H, QImage::Format_RGB32);
		if (image) {
			QPainter pntimage (image);
			pntimage.drawPixmap (0, 0, *pixmap);
			if (showDateCursor) {
				DateChooser datechooser;
				datechooser.setGriddedPlotter (terre->getGriddedPlotter());
				datechooser.setMarkToday (false);
				datechooser.setDate (date);
				datechooser.resize (W, HDateChooser);
				datechooser.render (&pntimage, QPoint(0,H-HDateChooser));
			}
			if (showColorScale) {
				ColorScaleGraph colorScale;
				colorScale.setColorScale (
						terre->getGriddedPlotter(),
						terre->getColorMapData ());
				colorScale.resize (WColorScale, H-HDateChooser);
				colorScale.render (&pntimage, QPoint(W-WColorScale,0));
			}
		}
		delete pixmap;
	}
	return image; 
}
//-------------------------------------------------
void ImageWriter::saveCurrentImage ()
{
	saveImage (terre->getCurrentDate());
}
//-------------------------------------------------
void ImageWriter::saveImage (time_t date)
{
	int W = Util::getSetting("imageSaveWidth",  640).toInt();
	int H = Util::getSetting("imageSaveHeight", 480).toInt();
	int Q = Util::getSetting("imageSaveQuality", 90).toInt();
	int Winit = terre->getProjection()->getW();
	int Hinit = terre->getProjection()->getH();
	bool rsz  = Util::getSetting("imageSaveResizeAfter", false).toBool();
	bool showDateCursor  = Util::getSetting("imageShowDateCursor", false).toBool();
	bool showColorScale  = Util::getSetting("imageShowColorScale", true).toBool();
	
	ImageWriterDialog dial (parent, W,H,Q,
							rsz, showDateCursor, showColorScale, Winit,Hinit);
	dial.exec ();

	if (dial.isAccepted()) 
	{
		QImage *image = createImage (date, dial, Winit,Hinit);
		if (image) 
		{
			if (! image->isNull())
			{
				QString filename = Util::getSetting("imageSaveFilename", "").toString();
				filename = Util::getSaveFileName (terre,
							tr("Save JPEG image"),
							filename, 
							tr("Images (*.jpg *.jpeg)") );
							
				if (filename != "") {
					saveSettings (dial, filename);
					qApp->setOverrideCursor (Qt::WaitCursor);
					if (   ! filename.endsWith(".jpg", Qt::CaseInsensitive) 
						&& ! filename.endsWith(".jpeg", Qt::CaseInsensitive) )
								filename += ".jpg";
					image->save (filename, "JPEG", Q);
					
					qApp->restoreOverrideCursor ();
				}
			}
			delete image;
		}
	}
}
//-------------------------------------------------
void ImageWriter::saveSettings (ImageWriterDialog &dial, QString filename)
{
	Util::setSetting("imageSaveWidth", dial.getW());
	Util::setSetting("imageSaveHeight", dial.getH());
	Util::setSetting("imageSaveQuality", dial.getQ());
	Util::setSetting("imageSaveFilename", filename);
	Util::setSetting("imageSaveResizeAfter", dial.getResizeAfter());
	Util::setSetting("imageShowDateCursor", dial.getShowDateCursor());
	Util::setSetting("imageShowColorScale", dial.getShowColorScale());
}
//-------------------------------------------------
void ImageWriter::saveAllImages ()
{
	GriddedPlotter  *plotter = terre->getGriddedPlotter ();
	if (!plotter || !plotter->isReaderOk()) {
		return;
	}
	
	int W = Util::getSetting("imageSaveWidth",  640).toInt();
	int H = Util::getSetting("imageSaveHeight", 480).toInt();
	int Q = Util::getSetting("imageSaveQuality", 90).toInt();
	int Winit = terre->getProjection()->getW();
	int Hinit = terre->getProjection()->getH();
	bool rsz  = Util::getSetting("imageSaveResizeAfter", false).toBool();
	bool showDateCursor  = Util::getSetting("imageShowDateCursor", false).toBool();
	bool showColorScale  = Util::getSetting("imageShowColorScale", true).toBool();
	
	ImageWriterDialog dial (parent, W,H,Q,
							rsz, showDateCursor, showColorScale, Winit,Hinit);
	dial.exec ();
	
	if (dial.isAccepted()) 
	{
		QString filename = Util::getSetting("imageSaveFilename", "").toString();
		filename = Util::getSaveFileName (terre,
					tr("Save all images (JPEG)"),
					filename, 
					tr("Images (*.jpg *.jpeg)") );
		if (filename != "") 
		{
			QRegExp reg ("(.jpeg$|.jpg$)");
			reg.setCaseSensitivity(Qt::CaseInsensitive);
			QString prefix = filename.replace(reg, "");
			reg.setPattern ("(_$|_\\d\\d\\d$)");
			prefix = filename.replace(reg, "");
			
			int nb = plotter->getNumberOfDates ();
			QString nom1 = createAnimImageFilename (prefix, 1);
			QString nom2 = createAnimImageFilename (prefix, nb);
			
			if ( QMessageBox::question (terre,
					tr("Save all images"),
					tr("This operation will create %1 files :").arg(nb)
						+ "\n\n"
						+ QString("%1\n...\n%2").arg(nom1).arg(nom2)
						+ "\n\n"
						+ tr("Warning: existing files will be deleted !")
					,
					QMessageBox::Ok|QMessageBox::Cancel
				)
				== QMessageBox::Ok )
			{	
				saveSettings (dial, filename);
				qApp->setOverrideCursor (Qt::WaitCursor);
				std::set<time_t>::iterator iter;
				int num=1;
				for (iter=plotter->getListDates()->begin();
							iter!=plotter->getListDates()->end();   iter++, num++)
				{
					qApp->processEvents ();
					time_t date = *iter;
					QImage * image = createImage (date, dial, Winit,Hinit);
					if (image)
					{						
						QString filename = createAnimImageFilename (prefix, num);
						image->save (filename, "JPEG", Q);
						delete image;
					}
				}
				qApp->restoreOverrideCursor ();
			}
		}
	}
}
//-------------------------------------------------------
QString ImageWriter::createAnimImageFilename (QString prefix, int n)
{
	return (prefix+"_%1.jpg").arg(n, 3, 10, QChar('0'));
}


