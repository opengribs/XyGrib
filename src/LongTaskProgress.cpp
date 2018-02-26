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

#include <cassert>

#include "Util.h" 
#include "LongTaskProgress.h" 


//------------------------------------------------------------
LongTaskProgress::LongTaskProgress (QWidget *parent)
{
    progress = new QProgressDialog 
					(QObject::tr("Loading file..."), 
					 QObject::tr("Cancel"), 
					 0, 100, 
					 parent);
	assert (progress);
	progress->setMinimumWidth (300);
	
	progress->setWindowModality(Qt::WindowModal);
 	progress->setAutoClose (false);
 	progress->setAutoReset (false);
	continueDownload = true; 
	
	setWindowTitle (tr("Open file"));
	setMessage (LTASK_OPEN_FILE);
	setVisible (false);
	
	connect (progress,  SIGNAL(canceled()), this, SLOT(downloadCanceled()));
}

//------------------------------------------------------------
LongTaskProgress::~LongTaskProgress ()
{
	if (progress)
		delete progress;
}

//-------------------------------------------
void LongTaskProgress::downloadCanceled ()
{ 
	continueDownload = false;
}

//------------------------------------------------------------
void LongTaskProgress::setWindowTitle (QString title)
{
	progress->setWindowTitle (title);
}

//------------------------------------------------------------
void LongTaskProgress::setValue (int value)
{
	progress->setValue (value);
	progress->open ();
	qApp->processEvents ();
}

//------------------------------------------------------------
void LongTaskProgress::setVisible (bool vis)
{
 	progress->setVisible (vis);
	qApp->processEvents ();
}

//------------------------------------------------------------
void LongTaskProgress::setMessage (LongTaskMessageType msgtype)
{
	switch (msgtype) {
		case LTASK_OPEN_FILE :
			progress->setLabelText (QObject::tr("Loading file..."));
			break;
		case LTASK_ANALYSE_DATA :
			progress->setLabelText (QObject::tr("Analyse data..."));
			break;
		case LTASK_PREPARE_MAPS :
			progress->setLabelText (QObject::tr("Prepare maps..."));
			break;
		case LTASK_UNCOMPRESS_FILE :
			progress->setLabelText (QObject::tr("Uncompress file..."));
			break;
	}
	
	qApp->processEvents ();
}






















