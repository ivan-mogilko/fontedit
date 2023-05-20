/*
 * Copyright (C) 2004-2023 Pieter Simoons <psimoons@crystalshard.net>
 * 
 * This file is part of Radiant FontEdit.
 * 
 * Radiant FontEdit is free software: you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * Radiant FontEdit is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with
 * Radiant FontEdit. If not, see <https://www.gnu.org/licenses/lgpl-3.0.html>.
 */

#ifndef __ALPHABET_H
#define __ALPHABET_H

#undef max
#include <map>

typedef unsigned short uint16_t;

struct CLetter {
    int height,
        width,
        data   [64][64];

    CLetter();
};

class CAlphabet {
public:
    static const int AsciiSize = 0x80;
    static const int MaxSize = 0xFFFF; // UINT16_MAX

    CAlphabet (int prealloc = AsciiSize);
    bool    bmp_read  (const char*);
    bool    bmp_write (const char*);
    bool    sci_read  (const char*);
    bool    sci_write (const char*);
    bool    ttf_read  (const char*);
    bool    ttf_write (const char*);
    bool    wfn_read  (const char*);
    bool    wfn_write (const char*);

    int     line_h;
    std::map<uint16_t, CLetter> letters;
};

extern CAlphabet alphabet;

#endif
