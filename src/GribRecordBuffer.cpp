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

#include "GribRecordBuffer.h"
#include "GribFile.h"

GribRecordBuffer::GribRecordBuffer (GribFile* file, size_t iseek)
    : file_offset_(iseek), record_start_(0), file_(file)
{
    find_record_result_t res = file->find_record(iseek);
    if (res.record_length == 0)
    {
        version_ = 0;
        record_length_ = 0;
    } else {
        record_start_ = res.record_start;
        record_length_ = res.record_length;
        version_ = res.version;
    }
}

uint8_t GribRecordBuffer::get(size_t idx) const
{
    if (idx >= record_length_)
        return 0;
    return file_->get(idx + file_offset_);
}

bool GribRecordBuffer::copy(uint8_t* buffer, size_t idx, size_t count) const
{
    if (idx + count > record_length_)
        return false;
    return file_->copy(buffer, idx + file_offset_, count);
}

