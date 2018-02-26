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

#include <set>

#include "MbzFile.h"

//==================================================================
bool MButil::readInt8   (ZUFILE *f, int *val)
{
	unsigned char a;
	if (zu_read (f, &a, 1) != 1) return false;
	*val = a;
	return true;
}
//----------------------------------------------------------
bool MButil::readInt16  (ZUFILE *f, int *val)
{
	unsigned char a,b;
	if (zu_read (f, &a, 1) != 1) return false;
	if (zu_read (f, &b, 1) != 1) return false;
	*val = (a<<8) + b;
	return true;
}
//----------------------------------------------------------
bool MButil::readInt32  (ZUFILE *f, int *val)
{
	unsigned char a,b,c,d;
	if (zu_read (f, &a, 1) != 1) return false;
	if (zu_read (f, &b, 1) != 1) return false;
	if (zu_read (f, &c, 1) != 1) return false;
	if (zu_read (f, &d, 1) != 1) return false;
	*val = (a<<24) + (b<<16) + (c<<8) + d;
	return true;
}
//----------------------------------------------------------
bool MButil::readFloat32  (ZUFILE *f, float *val)
{
	if (zu_read (f, val, 4) != 4)
		return false;
	return true;
}

//==================================================================
bool MButil::getDateFromName (const char *fname,
			int*year, int*month, int*day, int*href, int*hour)
{
	// fname = 20090619_00_006.dat.....
	// fname = yyyymmdd_HH_hhh.dat.....  HH=ref hour, hhh=current hour
	int base = strlen(fname)-1;
	while (base>0 && fname[base]!='/' && fname[base]!='\\') {
		base --;
	}
	if (fname[base]=='/' || fname[base]=='\\') {
		base ++;
	}
	*year=0; *month=0; *day=0; *href=0; *hour=0;
	if (    MButil::substring2int (year, fname, base,   4)
	     && MButil::substring2int (month,fname, base+4, 2)
	     && MButil::substring2int (day,  fname, base+6, 2)
	     && MButil::substring2int (href, fname, base+9, 2)
	     && MButil::substring2int (hour, fname, base+12,3)
	) {
		return true;
	}
	else {
		return false;
	}
}
//-----------------------------------------------------------------------
bool MButil::substring2int (int *val, const char *str,int start,int size)
{
	bool res = true;
	int p = start;
	int v = 0;
	int s = 1;
	if (str[p] == '+') {
		p ++;
		size --;
	}
	else if (str[p] == '-') {
		s = -1;
		p ++;
		size --;
	}
	for (int nb=0; nb<size; nb++,p++) {
		if (str[p]>='0' && str[p]<='9')
			v = v*10 + str[p]-'0';
		else
			res = false;
	}
	if (res)
		*val = v*s;
	return res;
}
//-------------------------------------------------------------
bool MButil::readPosition (char *line, float *x, float *y)
{
	// format 03473E4599N
	int i=0;
	while (isspace(line[i]))
			i++;
	int tlon=0;
	while (isdigit(line[i])) {
		tlon = tlon*10 + (line[i]-'0');
		i++;
	}
	*x = tlon/100.0;
	if (line[i] == 'W')
		*x = -*x;
	else if (line[i] != 'E')
		return false;
	i++;

	int tlat=0;
	while (isdigit(line[i])) {
		tlat = tlat*10 + (line[i]-'0');
		i++;
	}
	*y = tlat/100.0;
	if (line[i] == 'S')
		*y = -*y;
	else if (line[i] != 'N')
		return false;

	if (*y<-90.0 || *y>90.0 || *x<-360.0 || *x>360.0)
		return false;

	return true;
}

//---------------------------------------------------
// MbzFile
//---------------------------------------------------
MbzFile::MbzFile (const char *fname, LongTaskProgress *taskProgress)
{
	read_MbzFile (fname, taskProgress);
}
//---------------------------------------------------
MbzFile::~MbzFile ()
{
// 	DBGS("Destroy MbzFile");
	Util::cleanVectorPointers (vlines);
}
//---------------------------------------------------
void MbzFile::read_MbzFile (const char *fname, LongTaskProgress *taskProgress)
{
	ok = true;
	vlines.clear ();
	vcodes.clear ();
	
	ZUFILE *fin = zu_open (fname, "rb");
	if (!fin) {
		DBG ("Error: Can't open file %s", fname);
		ok = false;
		return;
	}
	read_header (fin);
	if (!ok) {
		//DBG ("Error: Can't read header from %s", fname);
		zu_close (fin);
		return;
	}
	read_data_codes (fin);
	if (!ok) {
		DBG ("Error: Can't read data codes from %s", fname);
		zu_close (fin);
		return;
	}
	
// DBG("version mbz %d", version);	

	read_data_lines (fin, taskProgress);
	if (!ok) {
		DBG ("Error: Can't read data lines from %s", fname);
		zu_close (fin);
		return;
	}


	zu_close (fin);
}
//---------------------------------------------------
int MbzFile::getDataCodeIndex (uint32_t code) {
	int ind = -1;
	for (unsigned int i=0; ind<0 && i<vcodes.size(); i++) {
		if (vcodes[i] == code)
			ind = i;
	}
	return ind;
}
//---------------------------------------------------
void MbzFile::read_data_codes  (ZUFILE *f)
{
	DataCode dtc;
	uint32_t v;
	for (int i=0; i<nbData; i++) {
		if (! MButil::readInt32 (f, (int*)(&v)))    {ok=false; return;}
		vcodes.push_back (v);
	}
}

//---------------------------------------------------
void MbzFile::read_data_lines  (ZUFILE *f, LongTaskProgress *taskProgress)
{
	float v;
	MbzLine *line;
	vlines.reserve (nbLines);
	for (int j=0; taskProgress->continueDownload && j<nbLines; j++) {
		taskProgress->setValue ((int)(100*j/nbLines));
		line = new MbzLine ();
		assert (line);
		if (! MButil::readInt16   (f, &(line->hour))) {ok=false; return;}
		if (! MButil::readFloat32 (f, &(line->x)))    {ok=false; return;}
		if (! MButil::readFloat32 (f, &(line->y)))    {ok=false; return;}
		line->data.reserve (nbData);
		for (int i=0; i<nbData; i++) {
			if (! MButil::readFloat32 (f,&v))    {ok=false; return;}
			line->data.push_back (v);
		}
		vlines.push_back (line);
	}
	
	if (!taskProgress->continueDownload) {
		Util::cleanVectorPointers (vlines);
		ok = false;
	}
}

//---------------------------------------------------
void MbzFile::read_header (ZUFILE *f)
{
	char buf[128];
	int  pad;
	if (zu_read (f, buf, 8) != 8)     {ok=false; return;}
	buf [8] = 0;
	if (strcmp(buf, "MBZYGRIB") != 0)  {ok=false; return;}
	if (! MButil::readInt8  (f, &version))    {ok=false; return;}
	if (! MButil::readInt16 (f, &year))   {ok=false; return;}
	if (! MButil::readInt8  (f, &month))  {ok=false; return;}
	if (! MButil::readInt8  (f, &day))    {ok=false; return;}
	if (! MButil::readInt8  (f, &href))   {ok=false; return;}
	if (! MButil::readInt32 (f, &nbData))    {ok=false; return;}
	if (! MButil::readInt32 (f, &nbLines))   {ok=false; return;}
	if (! MButil::readFloat32 (f, &xmin))    {ok=false; return;}
	if (! MButil::readFloat32 (f, &xmax))    {ok=false; return;}
	if (! MButil::readFloat32 (f, &ymin))    {ok=false; return;}
	if (! MButil::readFloat32 (f, &ymax))    {ok=false; return;}	
	if (! MButil::readInt16 (f, &pad))   {ok=false; return;}
	if (! MButil::readInt32 (f, &pad))   {ok=false; return;}
	if (! MButil::readInt32 (f, &pad))   {ok=false; return;}
}
//---------------------------------------------------
void MbzFile::debugmbz () const
{
	DBGS ("--------------------");
	if (ok) {
		for (unsigned int i=0; i<vcodes.size(); i++) {
			DataCode dtc (vcodes[i]);
			DBG ("dtc %2d: %3d %3d %5d", i,dtc.dataType,dtc.levelType,dtc.levelValue);
		}
		DBG ("%d %d %d %d", year,month,day,href);
		DBG ("Data:%d   Lines:%d", nbData, nbLines);
		DBG ("X: %f %f    Y: %f %f", xmin,xmax, ymin,ymax);
		std::set<int> alldates;
		for (uint32_t i=0; i<vlines.size(); i++) 
		{
			alldates.insert (vlines[i]->hour);
		}
		int nbdates = alldates.size();
		alldates.clear ();
		if (nbdates>0 && xmax!=xmin && ymax!=ymin) {
			
			double dens = nbLines /nbdates / (fabs(xmax-xmin)*fabs(ymax-ymin));
			double ec = 0;
			if (dens > 0) {
				ec = 1/sqrt(dens);
			}
			DBG ("nb dates: %d    density: %.1f %.5f", nbdates, dens, ec);
			DBG ("vlines.size()=%d  %d/hour", (int)vlines.size(), (int)(vlines.size()/nbdates));
		}
		if (vlines.size()>0)  vlines[0]->print();
		if (vlines.size()>1)  vlines[1]->print();
		if (vlines.size()>0)  vlines[vlines.size()-1]->print();
	}
	else {
		DBG ("Error in file");
	}
} 
//---------------------------------------------------
void MbzLine::print () const
{
	fprintf (stderr, "hr=%3d (%5g %5g) :", hour, x, y);
	for (unsigned int i=0; i<data.size(); i++) {
		fprintf (stderr, " %g", data[i]);
	}
	fprintf (stderr, "\n");
}

