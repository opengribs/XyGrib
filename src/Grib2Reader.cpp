/**********************************************************************
XyGrib: meteorological GRIB file viewer
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

#include "Grib2Reader.h"

//-----------------------------------------------------
Grib2Reader::Grib2Reader ()
	: GribReader ()
{
}
//-----------------------------------------------------
Grib2Reader::~Grib2Reader ()
{
}
//---------------------------------------------------------------------------------
void Grib2Reader::readGribFileContent (int nbrecs)
{
    fileSize = zu_filesize(file);
	
    unsigned char *cgrib;
    g2int  listsec0[3],listsec1[13],numlocal,numfields;
    g2int  n, lskip=0,lgrib=0,iseek=0;
    int    unpack=1, ierr=0;
    gribfield  *gfld;
    g2int expand=1;
	int idrec=0;
	allUnknownRecords.clear();
    while (ierr==0 && continueDownload) {
		//qApp->processEvents ();
		seekgb_zu (file, iseek, 64*1024, &lskip, &lgrib);
		if (idrec%4 == 1)
			emit valueChanged ((int)(100.0*idrec/nbrecs));

        //DBG("READ FIELD : idrec=%d lskip=%ld lgrib=%ld", idrec, lskip, lgrib);
		if (lgrib == 0)
			break;    // end loop at EOF or problem
		iseek = lskip + lgrib;

		cgrib = (unsigned char *) malloc (lgrib);
        if (cgrib == nullptr)
			break;

		if (zu_seek (file, lskip, SEEK_SET) == 0 && zu_read(file, cgrib, lgrib) == lgrib)
		{
			numfields = 0;
			numlocal = 0;
			ierr = g2_info (cgrib,listsec0,listsec1,&numfields,&numlocal);
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
				for (n=0; n<numfields; n++) {
                    gfld = nullptr;
					ierr = g2_getfld (cgrib, n+1, unpack, expand, &gfld);
					if (ierr == 0) {
						idrec++;
                        //DBG("LOAD FIELD idrec=%d/%d field=%ld/%ld numlocal=%ld",idrec,nbrecs, n+1,numfields, numlocal);
                        Grib2Record *rec = new Grib2Record (gfld, idrec, idCenter, refDate, discipline);
						if (rec->isOk()) {
                            //DBG("storeRecordInMap %d", rec->getId());
							storeRecordInMap (rec);
						}
						else {
							Grib2RecordMarker mark = rec->getGrib2RecordMarker();
							if (!allUnknownRecords.contains(mark)) {
								allUnknownRecords << mark;
								mark.dbgRec();
							}
						}
					}
					if (gfld)
						g2_free(gfld);
				}
			}
		}
		free(cgrib);
    }
}
