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
 
#include <cmath>
#include <cassert>
#include <stdint.h>

#include <QMessageBox>
#include <QDir>

#include "GribAnimator.h"
#include "ImageWriter.h"
#include "Font.h"
#include "Util.h"


//=========================================================================================
AnimImage::AnimImage () 
{
	pixmap = NULL;
}
AnimImage::~AnimImage () 
{
	if (pixmap) delete pixmap;
}

//=========================================================================================
AnimCommand::AnimCommand(int nbImages, int speed, bool autoLoop, QWidget *parent)
	: QToolBar(parent)
{
	this->nbImages = nbImages;
	this->speed = speed;
    
    acExit = new QAction(this);
    acExit->setToolTip (tr("Close the window"));
	acExit->setIcon(QIcon(Util::pathImg("exit.png")));
    this->addAction(acExit);
	connect(acExit, SIGNAL(triggered()), this, SLOT(actionsCommonSlot()));
    
    acSaveFile = new QAction(this);
    acSaveFile->setToolTip (tr("Save all images"));
	acSaveFile->setIcon(QIcon(Util::pathImg("media-floppy.png")));
    this->addAction(acSaveFile);
	connect(acSaveFile, SIGNAL(triggered()), this, SLOT(actionsCommonSlot()));
    
    this->addSeparator();

    acRewind = new QAction(this);
    acRewind->setToolTip (tr("Restart animation"));
	acRewind->setIcon(QIcon(Util::pathImg("player_start.png")));
    this->addAction(acRewind);
	connect(acRewind, SIGNAL(triggered()), this, SLOT(actionsCommonSlot()));

    acStart = new QAction(this);
    acStart->setToolTip (tr("Start animation"));
	acStart->setIcon(QIcon(Util::pathImg("player_play.png")));
    this->addAction(acStart);
	connect(acStart, SIGNAL(triggered()), this, SLOT(actionsCommonSlot()));

    acPause = new QAction(this);
    acPause->setToolTip (tr("Stop animation"));
	acPause->setIcon(QIcon(Util::pathImg("player_pause.png")));
    this->addAction(acPause);
	connect(acPause, SIGNAL(triggered()), this, SLOT(actionsCommonSlot()));

    this->addSeparator();
    
    acAutoLoop = new QAction(this);
    acAutoLoop->setToolTip (tr("Play in loop"));
	acAutoLoop->setIcon(QIcon(Util::pathImg("reload.png")));
	acAutoLoop->setCheckable ( true );
	acAutoLoop->setChecked(autoLoop);
    this->addAction(acAutoLoop);
	connect(acAutoLoop, SIGNAL(triggered()), this, SLOT(actionsCommonSlot()));
    
    this->addSeparator();
    
	sliderSpeed = new QSlider(Qt::Horizontal, this);
	sliderSpeed->setRange(1, 16);
	sliderSpeed->setMaximumWidth(120);
	sliderSpeed->setTickPosition(QSlider::TicksAbove);
	sliderSpeed->setTickInterval(1);
    sliderSpeed->setToolTip (tr("Speed"));
    this->addWidget(sliderSpeed);
	connect(sliderSpeed, SIGNAL(valueChanged(int)), this, SLOT(actionsCommonSlot()));

	sliderSpeed->setValue(speedSlider_SpeedToValue(speed));
    
    this->addSeparator();

	sliderCurrentImage = new QSlider(Qt::Horizontal, this);
	sliderCurrentImage->setRange(1, nbImages);
	sliderCurrentImage->setTickPosition(QSlider::TicksAbove);
	sliderCurrentImage->setTickInterval(1);
    sliderCurrentImage->setToolTip (tr("Current image"));
    this->addWidget(sliderCurrentImage);
	connect(sliderCurrentImage, SIGNAL(valueChanged(int)), this, SLOT(actionsCommonSlot()));
}
//---------------------------------------------------
void AnimCommand::actionsCommonSlot()
{
	if (sender() == acExit) {
		emit exitAnim();
	}
	else if (sender() == acSaveFile) {
		emit saveAnimFile();
	}
	else if (sender() == acStart) {
		emit startAnim(speed);
	}
	else if (sender() == acPause) {
		emit pauseAnim();
	}
	else if (sender() == acRewind) {
		emit rewindAnim();
	}
	else if (sender() == sliderSpeed) {
		speed = speedSlider_ValueToSpeed();
		emit setSpeed(speed);
	}
	else if (sender() == sliderCurrentImage) {
		int v = sliderCurrentImage->value();
		emit setCurrentImage(v-1);
	}
	else if (sender() == acAutoLoop) {
		emit setAutoLoop(acAutoLoop->isChecked());
	}
}
//---------------------------------------------------
int AnimCommand::speedSlider_ValueToSpeed()
{
	int v = sliderSpeed->value();
	int max = sliderSpeed->maximum();
	speed =  (int)(10.0*pow(3.0*(max+1-v), 1.2) +0.5) ;
	return speed;
}
//---------------------------------------------------
int AnimCommand::speedSlider_SpeedToValue(int speed)
{
	int max = sliderSpeed->maximum();
	int v = (int)( max+1 - pow(speed/10.0, 1/1.2)/3.0 +0.5);
	return v;
}
//---------------------------------------------------
void AnimCommand::changeCurrentImage(int ind)
{
	sliderCurrentImage->setValue(ind+1);
}

//=========================================================================================
CreateAnimProgressBar::CreateAnimProgressBar(int nbImages, QWidget *parent)
	: QWidget(parent)
{
	this->nbImages = nbImages;
    
    QHBoxLayout *lay = new QHBoxLayout(this);
    assert(lay);
    progressBar = new QProgressBar();
    assert(progressBar);
	progressBar->setRange(1, nbImages-1);
	progressBar->setValue(1);

	lay->addWidget(progressBar);	
}
//---------------------------------------------------------
void CreateAnimProgressBar::setCurrentValue(int n)
{
	progressBar->setValue(n);
}

//=========================================================================================
//-------------------------------------------------------------------------------
void GribAnimator::showImage(int ind, bool showmsg)
{
	if (closestatus != 0) return;	// animation creation interrupted

	currentImage = ind;
	lbimage->setPixmap( *(vectorImages[currentImage]->pixmap) );
	if (showmsg) {
		lbmessage->setText(
			QString(tr("Image %1/%2 : %3"))
							.arg(currentImage+1, 3)
							.arg(nbImages, 3)
							.arg(Util::formatDateTimeLong(vectorImages[currentImage]->date))
				);
		emit changeCurrentImage(currentImage);
	}
}
//---------------------------------------
void GribAnimator::showNextImage()
{
	currentImage ++;
	
	if (!autoLoop)
	{
		if (currentImage >= vectorImages.size())
			timerLoop->stop();
		else
			showImage(currentImage);
	}
	else
	{
		// little pause before next loop
		if (currentImage == vectorImages.size()-1)
		{
			timerLoop->stop();
			timerPause->setSingleShot(true);
			timerPause->start(500);
		}
			
		currentImage = currentImage % vectorImages.size();
		showImage(currentImage);
	}

}
//---------------------------------------
void GribAnimator::setAutoLoop(bool auto_)
{
	Util::setSetting("animAutoLoop", auto_);
 	autoLoop = auto_;
}
//---------------------------------------
void GribAnimator::exitAnim()
{
closestatus=2;
	close();
}
//---------------------------------------
void GribAnimator::rewindAnim()
{
	showImage(0);
}
//---------------------------------------
void GribAnimator::startAnim(int speed)
{
	if (currentImage >= vectorImages.size()) {
		currentImage = 0;
		showImage(0);
	}
 	timerLoop->start(speed);
}
//---------------------------------------
void GribAnimator::setSpeed(int speed)
{
	Util::setSetting("animSpeed", speed);
 	timerLoop->setInterval(speed);
}
//---------------------------------------
void GribAnimator::pauseAnim()
{
 	timerLoop->stop();
}
//---------------------------------------
void GribAnimator::timerPauseOut()
{
	if (autoLoop) {
 		timerLoop->start();
	}
}


//===================================================================
void GribAnimator::createImages()
{
	QPainter pnt;
	std::set<time_t>::iterator iter;
	int num=0;

	stackWidgets->setCurrentWidget(createAnimProgressBar);
    lbmessage->setFont(Font::getFont(FONT_StatusBar));
	closestatus=0;
	bool isEarthMapValid = false;
	for (iter=gribplot->getListDates()->begin();
				iter!=gribplot->getListDates()->end();   iter++, num++)
	{
		time_t date = *iter;
		
//  		qApp->processEvents (); 
		if (closestatus != 0) break;
		
		currentImage = num;
		lbmessage->setText(
			QString(tr("Making animation : image %1/%2 : %3"))
								.arg(currentImage+1, 3)
								.arg(nbImages, 3)
								.arg(Util::formatDateTimeLong(date))
					);
		
		qApp->processEvents ();
		if (closestatus != 0) break;
		
		AnimImage *img = new AnimImage();
		assert(img);
		
		img->date = date;
		img->pixmap = drawer->createPixmap_GriddedData ( 
								date, isEarthMapValid, 
								gribplot,
								proj,
								lspois );
		isEarthMapValid = true;
		
 		if (img->pixmap == NULL) {
			QMessageBox::critical (NULL,
				tr("Error"),
                tr("Need more memory."));
			break;
		}
		 
		if (closestatus != 0) break;
		
		vectorImages.push_back (img);
		showImage (currentImage, false);
		createAnimProgressBar->setCurrentValue (num+1);
	}

	if (closestatus == 0)
		stackWidgets->setCurrentWidget(animCommand);
}
 
//=============================================================================
// GUI
//=============================================================================
QFrame *GribAnimator::createFrameGui(QWidget *parent)
{
    QFrame * frm = new QFrame(parent);
    QVBoxLayout *frameLayout = new QVBoxLayout(frm);
    
    lbimage = new QLabel();
    frameLayout->addWidget(lbimage);

	// Pile : progressBar when computing images, command widget after
	stackWidgets = new QStackedWidget(this);
		createAnimProgressBar = new CreateAnimProgressBar(nbImages, this);
		stackWidgets->addWidget(createAnimProgressBar);
		stackWidgets->addWidget(animCommand);
    frameLayout->addWidget(stackWidgets);
    
    lbmessage = new QLabel(tr("Making animation"));
    frameLayout->addWidget(lbmessage);
    
    return frm;
}

//=========================================================================================

GribAnimator::~GribAnimator()
{
	closestatus=1;
// 	DBG ("destructor GribAnimator");
	
	Util::cleanVectorPointers (vectorImages);
	
	if (proj)
		delete proj;	
	
	if (drawer)
		delete drawer;
}

//-------------------------------------------------------------------------------
GribAnimator::GribAnimator (Terrain *terre)
{
	setModal(false); 
    setWindowTitle(tr("Animation"));
	
	this->terre = terre;
	this->gribplot = terre->getGriddedPlotter();
	
	this->proj     = terre->getProjection()->clone();
	this->drawer   = new MapDrawer (* terre->getDrawer());
	this->lspois   = terre->getListPOIs();

    W = proj->getW();
    H = proj->getH();
	nbImages = gribplot->getListDates()->size();
	
	vectorImages.reserve (nbImages);
	currentImage = 0;
	
	speed = Util::getSetting("animSpeed", 200).toInt();
	autoLoop = Util::getSetting("animAutoLoop", false).toBool();

	animCommand  = new AnimCommand(nbImages, speed, autoLoop, this);
	assert(animCommand);

	timerLoop = new QTimer(this);
 	connect(timerLoop, SIGNAL(timeout()), this, SLOT(showNextImage()));
	timerPause = new QTimer(this);
 	connect(timerPause, SIGNAL(timeout()), this, SLOT(timerPauseOut()));

    QVBoxLayout *lay = new QVBoxLayout(this);
    frameGui = createFrameGui(this);
    lay->addWidget(frameGui);
	
 	connect(this, SIGNAL(changeCurrentImage(int)), animCommand, SLOT(changeCurrentImage(int)));
 	
 	connect(animCommand, SIGNAL(startAnim(int)), this, SLOT(startAnim(int)));
 	connect(animCommand, SIGNAL(exitAnim()), this, SLOT(exitAnim()));
 	connect(animCommand, SIGNAL(saveAnimFile()), this, SLOT(saveAnimFile()));
 	connect(animCommand, SIGNAL(rewindAnim()), this, SLOT(rewindAnim()));
 	connect(animCommand, SIGNAL(pauseAnim()), this, SLOT(pauseAnim()));
 	connect(animCommand, SIGNAL(setSpeed(int)), this, SLOT(setSpeed(int)));
 	connect(animCommand, SIGNAL(setCurrentImage(int)), this, SLOT(showImage(int)));
 	connect(animCommand, SIGNAL(setAutoLoop(bool)), this, SLOT(setAutoLoop(bool)));
	
	show();
	createImages();
	rewindAnim();
}

//---------------------------------------
void GribAnimator::saveAnimFile()
{
	ImageWriter writer (this, terre);
	writer.saveAllImages ();
}










