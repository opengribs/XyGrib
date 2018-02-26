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

#ifndef SYLK_H
#define SYLK_H

#include <QFile>
#include <QTextStream>


class SylkFile
{
    public:
		SylkFile (QString filename, QString author) {
			file.setFileName (filename);
			ok = file.open (QIODevice::WriteOnly);
			if (ok) {
				out.setDevice (&file);
				out << "ID;P" << author <<"\n";
			}
		}
		~SylkFile () {
			if (ok)
				close ();
		}
		QFileInfo getFileInfo () {
			return QFileInfo (file);
		}
		QTextStream & getTextStream () {
			return out;
		}
		bool isOk () {
			return ok;
		}
		void close () {
			out << "E\n";
			file.close ();
		}
		void addCell (int row, int col, QString val) {
			out << "C;Y" << row << ";X" << col << ";K\"" << val << "\"\n";
		}
		void addCell (int row, int col, double val) {
			out << "C;Y" << row << ";X" << col << ";K" << val << "\n";
		}
		void addCell (int row, int col, int val) {
			out << "C;Y" << row << ";X" << col << ";K" << val << "\n";
		}
			
    private:
		QFile file;
		bool ok;
		QTextStream out;
};



#endif
