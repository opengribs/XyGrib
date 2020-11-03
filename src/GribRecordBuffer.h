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

#ifndef GRIBRECORDBUFFER_H
#define GRIBRECORDBUFFER_H

#include <cstddef>
#include <cstdint>

class GribFile;

//----------------------------------------------
class GribRecordBuffer
{
    public:
        GribRecordBuffer () = default;
        GribRecordBuffer (GribFile* file, size_t iseek);
        ~GribRecordBuffer () = default;

        size_t file_offset() const {return file_offset_;}
        size_t record_start() const {return record_start_;}
        size_t record_length() const {return record_length_;}
        uint8_t version() const {return version_;}
    
        uint8_t get(size_t idx) const;
        bool copy(uint8_t* buffer, size_t idx, size_t count) const;
    
    protected:

        bool find_length();

    protected:
        
        size_t file_offset_;
        size_t record_start_;
        size_t record_length_{0};
        uint8_t version_{0};
        GribFile* file_;
    
};


#endif



