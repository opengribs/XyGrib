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
    : num_bytes_(0), num_records_(0)
{
    if (!retrieve(file))
    {
        delete [] grib_data_;
        num_bytes_ = 0;
    }
    // count number of records
    size_t iseek = 0;
    find_record_result_t res;
    do {
        res = find_record(iseek);
        if (res.record_length > 0)
            num_records_++;
        iseek += res.record_length;
    } while (iseek < num_bytes_);
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
    if (idx + count > num_bytes_)
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

enum parse_state_t {
    START, G, R, I, B, LEN1, LEN2, LEN3, V, V2LEN1, V2LEN2, V2LEN3, V2LEN4, SEEKEND, FINISHED
};

find_record_result_t GribFile::find_record(size_t file_offset) const
{
    find_record_result_t res = {file_offset,0,0,0};

    if (file_offset >= num_bytes_)
        return res;
    
    bool finished = true;
    uint32_t len = 0;
    size_t i = 0;
    enum parse_state_t parse_state = START;
    while (parse_state != FINISHED && i <= num_bytes_ - file_offset) {
        uint8_t ch = get(i + file_offset);
        switch (parse_state) {
        case START:
            if (ch == 'G') {
                parse_state = G;
                res.record_start = i;
            }
            break;
        case G:
            parse_state = ch == 'R' ? R : START;
            break;
        case R:
            parse_state = ch == 'I' ? I : START;
            break;
        case I:
            parse_state = ch == 'B' ? B : START;
            break;
        case B:
            len = ch << 16;
            parse_state = LEN1;
            break;
        case LEN1:
            len += ch << 8;
            parse_state = LEN2;
            break;
        case LEN2:
            len += ch;
            parse_state = LEN3;
            break;
        case LEN3:
            res.version = ch;
            parse_state = V;
            break;
        case V:
            if (res.version == 1) {
                parse_state = SEEKEND;
            }
            else {
                if (res.version == 2) {
                    i = 11;
                    parse_state = V2LEN1;
                }
                else
                    parse_state = START;
            }
            break;
        case V2LEN1:
            len = ch << 24;
            parse_state = V2LEN2;
            break;
        case V2LEN2:
            len += ch << 16;
            parse_state = V2LEN3;
            break;
        case V2LEN3:
            len += ch << 8;
            parse_state = V2LEN4;
            break;
        case V2LEN4:
            len += ch;
            parse_state = SEEKEND;
            break;
        case SEEKEND:
            uint8_t eor[4];
            parse_state = FINISHED;
            if (!copy(eor, file_offset + res.record_start + len - 4, 4)
                || eor[0] != '7' || eor[1] != '7' || eor[2] != '7' || eor[3] != '7')
                finished = false;
            break;
        default:
            finished = false;
            break;
        } // end switch(parse_state)
        i++;
    }
    if (parse_state != FINISHED || i >= num_bytes_ - file_offset)
        finished = false;
    if (finished) {
        res.record_length = len;
    }
    // if record_length is 0, no record found
    return res;
}

