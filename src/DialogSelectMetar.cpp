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

#include <QMessageBox>
#include <QScrollBar>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <time.h>

#include "DialogSelectMetar.h"
#include "Util.h"
#include "Settings.h"

//-------------------------------------------------------------------------------
DialogSelectMetar::DialogSelectMetar (QWidget *parent) : DialogBoxBase (parent)
{
    setWindowTitle (tr("METAR stations"));
    QFrame *ftmp;
    QLabel *label;

	frameGui = createFrameGui(this);
    layout = new QGridLayout(this);
    int lig=0;
    //-------------------------
    lig ++;
    QFont font;
    font.setBold(true);
    label = new QLabel(tr("METAR stations"), this);
    label->setFont(font);
    layout->addWidget( label,    lig,0, 1,-1, Qt::AlignCenter);
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); layout->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    layout->addWidget( frameGui,  lig,0,   1, 2);
    //-------------------------
    lig ++;
    ftmp = new QFrame(this); ftmp->setFrameShape(QFrame::HLine); layout->addWidget( ftmp, lig,0, 1, -1);
    //-------------------------
    lig ++;
    btOK     = new QPushButton(tr("Ok"), this);
    layout->addWidget( btOK,    lig,0);
    //----------------------------------------------------------
    connect(btOK, SIGNAL(clicked()), this, SLOT(slotBtOK()));
    //----------------------------------------------------------
	int treeoffset = Util::getSetting ("metar_tree_offset", 0).toInt();
	treeWidget->verticalScrollBar()->setSliderPosition (treeoffset);
	QDesktopWidget *wscr = QApplication::desktop ();
	QRect r = wscr->availableGeometry();
	this->setMinimumWidth  (qMin(400,r.width()));
	this->setMinimumHeight (qMin(800,r.height()));
}

//-------------------------------------------------------------------------------
void DialogSelectMetar::slotBtOK()
{
	QTreeWidgetItemIterator iter (treeWidget);
	QTreeWidgetItem *it;
	QString val;
	QStringList expandedlist, icaolist;
	// Write expanded items in settings
    while (*iter) {
		it = *iter;
		if (it->data(0, Qt::UserRole) == "country"
			|| it->data(0, Qt::UserRole) == "state") 
		{
			if (it->isExpanded()) {
				val = (it->data(0, Qt::UserRole+1)).toString();
				expandedlist << val;
			}
		}
		iter ++;
	}
	iter = QTreeWidgetItemIterator (treeWidget, QTreeWidgetItemIterator::Checked);
    while (*iter) {
		it = *iter;
		if (it->data(0, Qt::UserRole) == "icao") 
		{
			val = (it->data(0, Qt::UserRole+1)).toString();
			icaolist << val;
		}
		iter ++;
	}
	Util::setSetting ("metar_country_expanded", expandedlist);
	Util::setSetting ("metar_selected", icaolist);
	emit metarListChanged ();
	Util::setSetting ("metar_tree_offset", treeWidget->verticalScrollBar()->sliderPosition());
    accept();
}

//-------------------------------------------------------------------------------
void DialogSelectMetar::make_metar_tree ()
{
	MetarWidgetFactory factory;
	
	QSet <QString> allExpanded = 
		QSet <QString>::fromList
			(Util::getSetting("metar_country_expanded", QStringList()).toStringList() );
	QSet <QString> allSelected = 
		QSet <QString>::fromList
			(Util::getSetting("metar_selected", QStringList()).toStringList() );
			 		 
	allAirports = factory.mapAirports.values ();
	qSort (allAirports);	// sort by country/state/name
	
	treeWidget = new QTreeWidget ();
	treeWidget->setColumnCount (2);
	QStringList hdrs;
	hdrs << tr("METAR Stations") << tr("Name");
	treeWidget->setHeaderLabels (hdrs);

	QString curCountry = "";
	QString curState = "";
	QString name = "";
	QTreeWidgetItem *item;
	QTreeWidgetItem *itemCountry = NULL;
	QTreeWidgetItem *itemState = NULL;
	QTreeWidgetItem *itemRoot = NULL;
	for (int i=0; i < allAirports.size(); i++)
	{
		Airport a = allAirports.at(i);
		if (curCountry != a.country) {
			curCountry = a.country;
			itemCountry = new QTreeWidgetItem (treeWidget);
			assert (itemCountry);
			itemRoot = itemCountry;
			curState = "";
			treeWidget->setFirstItemColumnSpanned (itemCountry, true);
			name = factory.mapCountries [curCountry];
			if (name == "")
				DBGQS ("Country name not found : "+curCountry);
			itemCountry->setText (0, curCountry+" - "+name);
			itemCountry->setExpanded (allExpanded.contains(curCountry));
			itemCountry->setData (0, Qt::UserRole, "country");
			itemCountry->setData (0, Qt::UserRole+1, curCountry);
		}
		if (curCountry=="CA" || curCountry=="US") {
			if (curState != a.state) {
				curState = a.state;
				itemState = new QTreeWidgetItem (itemCountry);
				assert (itemState);
				itemRoot = itemState;
				treeWidget->setFirstItemColumnSpanned (itemState, true);
				name = factory.mapStates [QPair<QString,QString>(curCountry,curState)];
				if (name == "")
					DBGQS ("State name not found : "+curState);
				itemState->setText (0, curState+" - "+name);
				itemState->setExpanded (allExpanded.contains(curCountry+"/"+curState));
				itemState->setData (0, Qt::UserRole, "state");
				itemState->setData (0, Qt::UserRole+1, curCountry+"/"+curState);
			}
		}
		item = new QTreeWidgetItem (itemRoot);
		assert (item);
		item->setText (0, a.icao);
		item->setText (1, a.name);
		item->setFlags (Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
		if (allSelected.contains(a.icao))
			item->setCheckState (0, Qt::Checked);
		else
			item->setCheckState (0, Qt::Unchecked);
		item->setData (0, Qt::UserRole, "icao");
		item->setData (0, Qt::UserRole+1, a.icao);
	}
	
	treeWidget->header()->setSectionResizeMode (QHeaderView::ResizeToContents);
}
//=============================================================================
// GUI
//=============================================================================
QFrame *DialogSelectMetar::createFrameGui(QWidget *parent)
{
    QFrame * frm = new QFrame(parent);
	
    QGridLayout  *lay = new QGridLayout(frm);

	make_metar_tree ();
    lay->addWidget (treeWidget);
    return frm;
}







