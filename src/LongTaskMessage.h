/**********************************************************************
XyGrib: meteorological GRIB file viewer

Copyright (C) 2018 - opengrib - http://www.xygrib.org

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

#ifndef LONGTASKMESSAGE_H
#define LONGTASKMESSAGE_H

#include <QObject>

//-----------------------------------------
class LongTaskMessage :public QObject
{
Q_OBJECT
public:

    enum LongTaskMessageType
	{
		LTASK_OPEN_FILE,
		LTASK_ANALYSE_DATA,
		LTASK_PREPARE_MAPS,
		LTASK_UNCOMPRESS_FILE
	};

    LongTaskMessage() = default;
    ~LongTaskMessage() = default;

    bool continueDownload{true};

public slots:
    void cancel();

signals:
    void newMessage(LongTaskMessage::LongTaskMessageType type);
    // gauge 1 -- 100 %
    void valueChanged(int newValue);
};

#endif
