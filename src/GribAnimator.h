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

#ifndef GRIBANIMATOR_H
#define GRIBANIMATOR_H

#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QStackedWidget>
#include <QAction>
#include <QSlider>
#include <vector>

#include "DialogBoxColumn.h"
#include "Terrain.h"
#include "Projection.h"
#include "GribPlot.h"
#include "POI.h"

//=====================================================================================
class AnimImage 
{
    public:
		AnimImage ();
		~AnimImage ();
		
    	QPixmap *pixmap;
    	time_t  date;
};

//=====================================================================================
class AnimCommand : public QToolBar
{ Q_OBJECT
    public:
		AnimCommand(int nbImages, int speed, bool autoLoop, QWidget *parent);
	
	signals:
		void exitAnim();
		void saveAnimFile();
		void startAnim(int);
		void rewindAnim();
		void pauseAnim();
		void setSpeed(int);
		void setCurrentImage(int);
		void setAutoLoop(bool);
	
	private:
		int nbImages;
		int currentImage;
		int speed;
		
	    QAction *acExit;
	    QAction *acSaveFile;
	    QAction *acStart;
	    QAction *acPause;
	    QAction *acRewind;
	    QAction *acAutoLoop;

		QSlider *sliderSpeed;
		QSlider *sliderCurrentImage;
		
		int speedSlider_ValueToSpeed();
		int speedSlider_SpeedToValue(int speed);
		
	private slots :
		void actionsCommonSlot();
	
		void changeCurrentImage(int);
};

//=====================================================================================
class CreateAnimProgressBar : public QWidget
{ Q_OBJECT
    public:
		CreateAnimProgressBar(int nbImages, QWidget *parent);
		void setCurrentValue(int n);

	private:
		int nbImages;
		QProgressBar *progressBar;
		
};

//=====================================================================================
class GribAnimator : public QDialog
{ Q_OBJECT
    public:
        GribAnimator (Terrain *terre);
        ~GribAnimator();
    
    signals:
    	void changeCurrentImage(int);

	private slots:
		void showImage(int ind, bool showmsg=true);
		void showNextImage();
		void setSpeed(int speed);
		void startAnim(int speed);
		void pauseAnim();
		void exitAnim();
		void saveAnimFile();
		void rewindAnim();
		void setAutoLoop(bool);
		void timerPauseOut();
		    
    private:
		int 		W, H;
        MapDrawer 	*drawer;
        GriddedPlotter 	*gribplot;
        Projection 	*proj;
	    QList<POI*> lspois;
		Terrain 	*terre;
		
		volatile int 	closestatus;		
        std::vector <AnimImage *> vectorImages;
		void	createImages();
        unsigned int		currentImage;
        int 	nbImages;
        int		speed;
        bool	autoLoop;
        
        QFrame 			*frameGui;
        QVBoxLayout 	*frameLayout;
        QStackedWidget 	*stackWidgets;
        QTimer *timerLoop;
        QTimer *timerPause;
        QLabel *lbimage, *lbmessage;
        CreateAnimProgressBar *createAnimProgressBar;
        AnimCommand			  *animCommand;
        
        QGridLayout *layout;
        QFrame * createFrameGui(QWidget *parent);
		void closeEvent(QCloseEvent *) {delete this;};
};


#endif
