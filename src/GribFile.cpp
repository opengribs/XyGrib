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

#include "GribFile.h"
#include <cstring>

// start with 1mb chunks
const int BUFFER_SIZE = 1 << 20;

GribFile::GribFile (ZUFILE* file)
    : num_bytes_(0)
{
    if (!retrieve(file))
    {
        delete [] grib_data_;
        num_bytes_ = 0;
    }
}

GribFile::~GribFile()
{
    delete [] grib_data_;
}

uint8_t GribFile::get(size_t idx) const
{
    if (idx >= num_bytes_)
        return 0;
    return grib_data_[idx];
}

bool GribFile::copy(uint8_t* buffer, size_t idx, size_t count) const
{
    if (idx + count >= num_bytes_)
        return false;
    std::memcpy(buffer, &grib_data_[idx], count);
    return true;
}

bool GribFile::retrieve(ZUFILE *f)
{
    if (zu_seek(f, 0, SEEK_SET) != 0) {
         return false;
    }
    grib_data_ = new uint8_t[BUFFER_SIZE];
    int bytes_read = 0;
    int count = 1;
    do {
        size_t offset = BUFFER_SIZE*(count-1);
        bytes_read = zu_read(f, grib_data_+offset, BUFFER_SIZE);
        num_bytes_ += bytes_read;
        if (bytes_read == BUFFER_SIZE) {
            uint8_t* newbuf = new uint8_t[BUFFER_SIZE * ++count];
            std::memcpy(newbuf, grib_data_, BUFFER_SIZE*(count-1));
            delete [] grib_data_;
            grib_data_ = newbuf;
        } else if (bytes_read < BUFFER_SIZE)
            break;
    }
    while (bytes_read != 0);
    return num_bytes_ > 0;
}

