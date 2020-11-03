/**********************************************************************
XyGrib: meteorological GRIB file viewer

Copyright (C) 2020 - opengrib - http://www.xygrib.org

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

#ifndef GRIBRECORDTASK_H
#define GRIBRECORDTASK_H

#include "GribRecordBuffer.h"
#include "GribRecord.h"
#include "Grib2Record.h"

#include <QRunnable>
#include <memory>
#include <vector>

//----------------------------------------------
class GribRecordTask : public QRunnable
{
    public:
        GribRecordTask ();
        GribRecordTask (int id, GribRecordBuffer* buffer);
        virtual ~GribRecordTask ();

        int id() const   {return id_;}
        bool isOk() const   {return v1record_ || v2records_.size() > 0;}
        bool isFinished() const   {return finished_;}
        void cancel()   {stop_=true;}
        std::shared_ptr<GribRecord> record()   {return v1record_;}
        std::vector<std::shared_ptr<Grib2Record> >& grib2records()  {return v2records_;}
        
        void run() override;
        
    protected:

        bool readGrib2Record();
        
        int id_;
        GribRecordBuffer *buffer_;
        std::shared_ptr<GribRecord> v1record_;
        std::vector<std::shared_ptr<Grib2Record> > v2records_;
        bool finished_;
        bool stop_;
};


#endif



