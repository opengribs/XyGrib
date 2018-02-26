#ifndef POSITIONEDITORWIDGET_H
#define POSITIONEDITORWIDGET_H

#include <QWidget>
// #include <QtDesigner/QDesignerExportWidget>

#include "ui_PositionEditorWidget.h"

class /*QDESIGNER_WIDGET_EXPORT*/ PositionEditorWidget
		: public  QWidget,
		  private Ui::PositionEditorWidget
{
    Q_OBJECT

public:
    PositionEditorWidget
     				( QWidget *parent = 0,
					  double lat = 0,
    				  double lon = 0,
					  QString userOrient_lat="Auto",  /* "Auto" "North+" "South+" */
					  QString userOrient_lon="Auto"   /* "Auto" "East+" "West+"   */
					);
	
	void setLatitude (double val);
	void setLongitude(double val);
	
	void setPosition (double lat, double lon)
				{ setLatitude  (lat);
				  setLongitude (lon); }

	double getLatitude();
    double getLongitude();

private:
	QString   orientLon, orientLat;

};

#endif
