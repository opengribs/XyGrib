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
    : file_offset_(iseek), record_start_(0), record_length_(0),
      version_(0), file_(file)
{
    if (!find_length())
    {
        version_ = 0;
        record_length_ = 0;
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
    if (idx + count >= record_length_)
        return false;
    return file_->copy(buffer, idx + file_offset_, count);
}

enum parse_state_t {
    START, G, R, I, B, LEN1, LEN2, LEN3, V, V2LEN1, V2LEN2, V2LEN3, V2LEN4, SEEKEND, FINISHED
};

bool GribRecordBuffer::find_length()
{
    bool finished = true;
    uint32_t len = 0;
    size_t skip = 0;
    size_t i = 0;
    enum parse_state_t parse_state = START;
    while (parse_state != FINISHED && i <= file_->num_bytes() - file_offset_) {
        uint8_t ch = file_->get(i + file_offset_);
        switch (parse_state) {
        case START:
            if (ch == 'G') {
                parse_state = G;
                record_start_ = i;
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
            version_ = ch;
            parse_state = V;
            break;
        case V:
            if (version_ == 1) {
                parse_state = SEEKEND;
            }
            else {
                if (version_ == 2) {
                    skip = 0;
                    parse_state = V2LEN1;
                }
                else
                    parse_state = START;
            }
            break;
        case V2LEN1:
            if (++skip == 5) {
                len = ch << 24;
                parse_state = V2LEN2;
            }
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
            if (!file_->copy(eor, file_offset_ + record_start_ + len - 4, 4)
                || eor[0] != '7' || eor[1] != '7' || eor[2] != '7' || eor[3] != '7')
                finished = false;
            break;
        default:
            finished = false;
            break;
        } // end switch(parse_state)
        i++;
    }
    if (parse_state != FINISHED || i >= file_->num_bytes() - file_offset_)
        finished = false;
    record_length_ = len;
    return finished;
}
