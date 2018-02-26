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

#ifndef METEOTABLEWIDGET_H
#define METEOTABLEWIDGET_H

#include <QGridLayout>
#include <QLabel>

#include "GriddedPlotter.h"
#include "DataPointInfo.h"
#include "SkewT.h"

#define MTABLE_WIND_CELL    1
#define MTABLE_CLOUD_CELL   2
#define MTABLE_CURRENT_CELL 3
#define MTABLE_SKEWT_CELL   4


//===================================================================
class TableCell : public QWidget
{ Q_OBJECT
    public:
        TableCell (QWidget *parent, QString txt="", bool bold=false,
					QColor bgcolor=QColor(200,200,255),
					Qt::Alignment alignment=Qt::AlignHCenter|Qt::AlignBottom
			);
		
		void setBorders (int lig, int col);
	    QLabel  *label;

    protected:
	    void  	paintEvent(QPaintEvent *event);
	    void    setContrastedTextColor(QColor bgColor);
		
		static const uint none  = 0;
		static const uint north = 1;
		static const uint west  = 2;
		static const uint south = 4;
		static const uint east  = 8;
		static const uint all   = 1+2+4+8;
		
		void setBorders (int num) { borders=num; }
		
	    QColor  bgcolor, bordercolor;
	    int     borders;
};

//===================================================================
class TableCell_SkewT : public TableCell
{ Q_OBJECT
    public:
        TableCell_SkewT (QWidget *parent,
						 double lon, double lat, QString locationName,
						 time_t date,
						 GriddedReader *reader);
    protected:
		QPixmap pixmap;
		double lon, lat;
		QString locationName;
		time_t date;
		GriddedReader *reader;
	    void  paintEvent (QPaintEvent *event);
		void  enterEvent (QEvent *);
		void  leaveEvent (QEvent *);
		void  mouseReleaseEvent (QMouseEvent *);
};

//===================================================================
class TableCell_Wind : public TableCell
{ Q_OBJECT
    public:
        TableCell_Wind(
        			double vx, double vy, bool south,
        			GriddedPlotter  *plotter,
        			QWidget *parent, QString txt, bool bold,
        			QColor bgcolor
				);
    protected:
    	QColor    windArrowsColor;
    	double 	  vx, vy;
    	bool	  south;
    	bool	  showWindArrows;
    	GriddedPlotter *plotter;
	    void  	paintEvent(QPaintEvent *event);
};

//===================================================================
class TableCell_Current : public TableCell
{ Q_OBJECT
    public:
        TableCell_Current(
        			double cx, double cy, bool south,
        			GriddedPlotter  *plotter,
        			QWidget *parent, QString txt, bool bold,
        			QColor bgcolor
				);
    protected:
    	QColor    windArrowsColor;
    	QColor    currentArrowsColor;
    	double 	  cx, cy;
    	bool	  south;
    	bool	  showCurrentArrows;
    	GriddedPlotter *plotter;
	    void  	paintEvent(QPaintEvent *event);
};

//===================================================================
class TableCell_Clouds : public TableCell
{ Q_OBJECT
    public:
        TableCell_Clouds (
        			double val,
        			GriddedPlotter *plotter,
        			QWidget *parent, QString txt, bool bold,
        			QColor bgcolor
				);
    protected:
    	GriddedPlotter *plotter;
    	double   val;
	    void  	 paintEvent(QPaintEvent *event);
};

//===================================================================
// 
class MTGribData {
	public :
		MTGribData (QString codeStr, int pos) {
			this->codeStr = codeStr;
			this->pos = pos;
			this->dtc.fromInt32 (codeStr.toUInt(0, 16));
		}
		MTGribData (uint code, int pos) {
			this->codeStr = QString("%1").arg(code,8,16,QLatin1Char('0'));
			this->pos = pos;
			this->dtc.fromInt32 (codeStr.toUInt(0, 16));
		}
		QString  codeStr;
		DataCode dtc;
		int  pos;
};

//===================================================================
class MeteoTableWidget : public QWidget
{ Q_OBJECT
    public:
        MeteoTableWidget (GriddedPlotter *plotter, 
						  double lon, double lat, 
						  QString locationName, QWidget *parent);
        ~MeteoTableWidget();
		QList <MTGribData *> & getListVisibleData() 
									{ return listVisibleData; }
		
		std::vector <DataPointInfo *> & getListPinfos() 
									{ return lspinfos; }

		QWidget *getDataHeaders (); 
		
	private:
        double          lon, lat;
		QString         locationName;
        GriddedPlotter  *plotter;
        GriddedReader   *reader;
		QGridLayout     *layout;
		
		QWidget         *parent;
		QWidget         *headerWidget;
		QGridLayout     *headerLayout;
		
		std::vector <time_t> lsdates;
		std::vector <DataPointInfo *> lspinfos;
		
		QList <MTGribData *> listVisibleData;
		
		void  createTable();
		void  createListVisibleGribData();
		
		void  addLine_Wind        (const Altitude &alt, int lig);
		void  addLine_Current     (const Altitude &alt, int lig);
		void  addLine_Temperature (const Altitude &alt, uchar type, int lig);
		void  addLine_HumidRel    (const Altitude &alt, int lig);
		void  addLine_CloudCover  (int lig);
		void  addLine_Rain        (int lig);
		void  addLine_DewPoint    (const Altitude &alt, int lig);
		void  addLine_Pressure    (const Altitude &alt, int lig);
		void  addLine_SnowDepth        (int lig);
		void  addLine_DeltaTemperature (const Altitude &alt, uchar type, int lig);
		void  addLine_Categorical 	   (uchar type, int lig);
		void  addLine_CAPEsfc    (int lig);
		void  addLine_CINsfc     (int lig);
		void  addLine_SkewT      (int lig);
		void  addLine_GUSTsfc    (int lig);
		void  addLine_Isotherm0Height  (int lig);
		void  addLine_GeopotentialAltitude  (const Altitude &alt, int lig);

		void  addLine_WaveHeight (int type, int lig);
		void  addLine_WaveWhitecap (int type, int lig);
		void  addLine_WaveCompleteCell (int prvtype, int lig);
		
		void addCell_content (QString txt, 
				QGridLayout *layout,int lig,int col,
				int    rowspan=1,
				int    colspan=1,
				QColor bgcolor=Qt::white,
				int    cellType = 0,
				double  vx=0,
				double  vy=0
				);
				
		void addCell_title (QString txt, bool bold,
				QGridLayout *layout,int lig,int col,
				int rowspan=1, int colspan=1,
				bool isNowDate=false);
		
		void addCell_SunMoonAlmanac (time_t t, double lat, double lon,
				QGridLayout *layout,int lig,int col, int rowspan,int colspan);
		
		void addCell_SkewT (QGridLayout *layout,int lig,int col, time_t date);
		
		void addCell_title_dataline (QString txt, bool bold,
				int lig,int col);
		
		void showEvent (QShowEvent *event);		
};




#endif
