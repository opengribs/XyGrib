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

#include "GribRecordTask.h"

extern "C" {
    #include "grib2.h"
}
    
GribRecordTask::GribRecordTask ()
    : id_(0), buffer_(nullptr), v1record_(nullptr), finished_(false), stop_(true)
{
}

GribRecordTask::GribRecordTask (int id, GribRecordBuffer *buffer)
    : id_(id), buffer_(buffer), v1record_(nullptr), finished_(false), stop_(false)
{
    // does nothing else
}

GribRecordTask::~GribRecordTask()
{
    delete buffer_;
    v1record_.reset();
    for (size_t i=0; i<v2records_.size(); i++)
        v2records_[i].reset();
}

void GribRecordTask::run()
{
    if (!stop_ && buffer_ && buffer_->version() == 1) {
        v1record_ = std::shared_ptr<GribRecord>(new GribRecord(buffer_, id_));
        if (!v1record_->isOk()) {
            v1record_.reset();
        }
    } else if (!stop_ && buffer_ && buffer_->version() == 2) {
        readGrib2Record();
    }
    finished_ = true;
}

//---------------------------------------------------------------------------------
bool GribRecordTask::readGrib2Record()
{
	int idrec=0;

	uint8_t *cgrib = new uint8_t[buffer_->record_length()];
    if (cgrib == nullptr || !buffer_->copy(cgrib, 0, buffer_->record_length())) {
        delete [] cgrib;
		return false;
    }

    g2int numfields = 0;
	g2int numlocal = 0;
    g2int listsec0[3],listsec1[13];
    int ierr = g2_info (cgrib,listsec0,listsec1,&numfields,&numlocal);
    if (ierr == 0) {
        // analyse values returned by g2_info
        // added by david to handle discipling
        int discipline = listsec0[0];
        
        int idCenter = listsec1[0];
        int refyear  = listsec1[5];
        int refmonth = listsec1[6];
        int refday   = listsec1[7];
        int refhour  = listsec1[8];
        int refminute= listsec1[9];
        int refsecond= listsec1[10];
        time_t refDate = DataRecordAbstract::UTC_mktime
            (refyear,refmonth,refday,refhour,refminute,refsecond);
        // 				idModel
        // 				idGrid
        // extract fields
        for (g2int n=0; n<numfields; n++) {
            gribfield  *gfld = nullptr;
            int unpack = 1;
            g2int expand = 1;
            ierr = g2_getfld (cgrib, n+1, unpack, expand, &gfld);
            if (ierr == 0) {
                idrec++;
                //DBG("LOAD FIELD idrec=%d/%d field=%ld/%ld numlocal=%ld",idrec,nbrecs, n+1,numfields, numlocal);
                auto rec = std::shared_ptr<Grib2Record>(
                    new Grib2Record (gfld, idrec, idCenter, refDate, discipline));
                if (!rec->isOk())
                    rec.reset();
                else
                    v2records_.push_back(rec);
            }
            if (gfld)
                g2_free(gfld);
        }
    }
	delete [] cgrib;
    return true;
}
