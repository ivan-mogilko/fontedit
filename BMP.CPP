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

#include "FontEdit.h"
#include "Alphabet.h"

bool CAlphabet::bmp_read (const char *filename) {
    int   i, j, k, l, x, y, size, offs;
    short s;
    char  *buf;
    FILE  *file;
    file = fopen (filename, "rb");
    if (file == NULL) return false;

    if (fgetc (file) != 'B') return false;
    if (fgetc (file) != 'M') return false;

    fread (&size,  4, 1, file);
    fseek (file,   4, SEEK_CUR);
    fread (&offs,  4, 1, file);
    fseek (file,   4, SEEK_CUR);
    fread (&x,     4, 1, file);
    fread (&y,     4, 1, file);
    fread (&s,     2, 1, file);
    fread (&s,     2, 1, file);
    if (s != 24) {
        AfxMessageBox ("Only 24-bit bitmaps are supported.");
        fclose (file);
        return true;
    }
    fseek (file, offs, SEEK_SET);
    size -= offs;
    buf = new char [size];
    if (buf == NULL) { fclose (file); return false; }
    fread (buf, size, 1, file);
    if (ferror (file)) { fclose (file); return false; }
    fclose (file);
    *this = CAlphabet();

    k = x / 0x10;
    l = y / 0x08;

    for (i=0; i<0x10; i++) {
        for (j=0; j<0x8; j++) {
            CLetter letter;
            for (x=0; x<k; x++) {
                for (y=0; y<l; y++) {
                    s = (buf[(i * k + x + ((0x8 - j) * l - y - 1) * k * 0x10) * 3]) & 0xff;
                    if (s != 0x00 && s != 0xff) {
                        if (y == 0) {
                            letter.width  = x;
                            x = k;
                        } else {
                            letter.height = y;
                        }
                        break;
                    }
                    letter.data[x][y] = s ? 1 : 0;
                }
            }
            letters[i + 0x10 * j] = letter;
        }
    }
    delete [] buf;
    return true;
}

bool CAlphabet::bmp_write (const char *filename) {
    int   i, j, k, l, x, y;
    short s;
    char  *buf;
    FILE  *file;
    file = fopen (filename, "wb");
    if (file == NULL) return false;

    k = 0;
    l = 0;
    const int letter_count = CAlphabet::AsciiSize;
    for (i=0; i<letter_count; i++) {
        if (letters.count(i) == 0)
            continue;
        const CLetter &letter = letters[i];
        if (k < letter.width)  k = letter.width;
        if (l < letter.height) l = letter.height;
    }
    k += 2; if (k < 8) k = 8;
    l += 2; if (l < 8) l = 8;

    fputc ('B', file);
    fputc ('M', file);
    i = k * 0x10 * l * 0x08 * 3 + 54; fwrite (&i, 4, 1, file);
    i = 0;                            fwrite (&i, 4, 1, file);
    i = 54;                           fwrite (&i, 4, 1, file);
    i = 40;                           fwrite (&i, 4, 1, file);
    i = k * 0x10;                     fwrite (&i, 4, 1, file);
    i = l * 0x08;                     fwrite (&i, 4, 1, file);
    s = 1;                            fwrite (&s, 2, 1, file);
    s = 24;                           fwrite (&s, 2, 1, file);
    i = 0;                            fwrite (&i, 4, 1, file);
    i = k * 0x10 * l * 0x08 * 3;      fwrite (&i, 4, 1, file);
    i = 3790;                         fwrite (&i, 4, 1, file);
    i = 3780;                         fwrite (&i, 4, 1, file);
    i = 0;                            fwrite (&i, 4, 1, file);
    i = 0;                            fwrite (&i, 4, 1, file);

    buf = new char [k * l * letter_count * 3];
    if (buf == NULL) return false;
    memset (buf, 0x99, k * l * letter_count * 3);

    for (i=0; i<0x10; i++) {
        for (j=0; j<0x8; j++) {
            if (letters.count(i + 0x10 * j) == 0)
                continue;
            const CLetter &letter = letters[i + 0x10 * j];
            for (x=0; x<k && x<letter.width; x++) {
                for (y=0; y<l && y<letter.height; y++) {
                    buf[(i * k + x + ((0x8 - j) * l - y - 1) * k * 0x10) * 3]     = letter.data[x][y] ? 0xff : 0;
                    buf[(i * k + x + ((0x8 - j) * l - y - 1) * k * 0x10) * 3 + 1] = letter.data[x][y] ? 0xff : 0;
                    buf[(i * k + x + ((0x8 - j) * l - y - 1) * k * 0x10) * 3 + 2] = letter.data[x][y] ? 0xff : 0;
                }
            }
        }
    }
    fwrite (buf, k * l * letter_count * 3, 1, file);

    if (ferror (file)) { fclose (file); return false; }
    fclose (file);
    delete [] buf;
    return true;
}
