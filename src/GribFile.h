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

#ifndef GRIBFILE_H
#define GRIBFILE_H

#include <cstdint>

#include "zuFile.h"

//----------------------------------------------
class GribFile
{
    public:
        GribFile () = default;
        GribFile (ZUFILE* file);
        ~GribFile ();

        size_t num_bytes() const {return num_bytes_;}
    
        uint8_t get(size_t idx) const;
        bool copy(uint8_t* buffer, size_t idx, size_t count) const;
    
    protected:

        bool retrieve(ZUFILE *f);

    protected:
        
        size_t num_bytes_{0};
        uint8_t* grib_data_;
};


#endif



