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

CLetter::CLetter() {
    height = 1;
    width = 1;
    memset(data, 0, sizeof(data));
}

CAlphabet::CAlphabet (int prealloc) {
    line_h = 0;
    for (int i=0; i<prealloc; i++) {
        CLetter letter;
        letter.width  = 1;
        letter.height = 1;
        letters[i] = letter;
    }
}

bool CAlphabet::sci_read (const char *filename) {
    FILE *file;
    int  i, j, k, l, n,
         num,
         off [0x100];

    file = fopen (filename, "rb");
    if (file == NULL) return false;

    /* Font header: 0x87000000 */
    if (fgetc (file) != 0x87) return false;
    if (fgetc (file) != 0x00) return false;
    if (fgetc (file) != 0x00) return false;
    if (fgetc (file) != 0x00) return false;
    *this = CAlphabet();

    /* Number of characters in font, usually 0x80 */
    num =  fgetc (file);
    num += fgetc (file) * 0x100;

    /* Font line height */
    line_h =  fgetc (file);
    line_h += fgetc (file) * 0x100;

    /* Offsets of individual letters */
    for (i=0; i<num; i++) {
        off[i] =  fgetc (file) + 2;
        off[i] += fgetc (file) * 0x100;
    }

    /* Binary letter data */
    for (i=0; i<num; i++) {
        fseek (file, off[i], SEEK_SET);
        CLetter letter;
        letter.width  = fgetc (file);
        letter.height = fgetc (file);
        for (j=0; j<letter.height; j++) {
            for (k=0; k<(letter.width + 7) / 8; k++) {
                n = fgetc (file);
                for (l=0; l<8; l++)
                    letter.data[k * 8 + (7 - l)][j] = (n >> l) & 1;
            }
        }
        letters[i] = letter;
    }
    if (ferror (file)) { fclose (file); return false; }
    fclose (file);
    return true;
}

bool CAlphabet::sci_write (const char *filename) {
    FILE *file;
    int  i, j, k, l, n,
         off [0x100];

    file = fopen (filename, "wb");
    if (file == NULL) return false;

    /* Font header: 0x87000000 */
    fputc (0x87, file);
    fputc (0x00, file);
    fputc (0x00, file);
    fputc (0x00, file);

    /* Saved font will always have 0x80 characters */
    const int letter_count = CAlphabet::AsciiSize;
    fputc (letter_count, file);
    fputc (0x00, file);

    /* Font line height */
    fputc (line_h % 0x100, file);
    fputc (line_h / 0x100, file);

    /* Offsets of individual letters */
    for (i=0; i<letter_count; i++) {
        fputc (0, file);
        fputc (0, file);
    }

    /* Binary letter data */
    for (i=0; i<letter_count; i++) {
        off[i] = ftell (file);
        if (letters.count(i) == 0)
            continue;
        const CLetter &letter = letters[i];
        fputc (letter.width,  file);
        fputc (letter.height, file);
        for (j=0; j<letter.height; j++) {
            for (k=0; k<(letter.width + 7) / 8; k++) {
                n = 0;
                for (l=0; l<8; l++)
                    n |= (letter.data[k * 8 + (7 - l)][j] & 1) << l;
                fputc (n, file);
            }
        }
    }

    /* Add offsets to file header */
    fseek (file, 0x08, SEEK_SET);
    for (i=0; i<letter_count; i++) {
        fputc ((off[i] - 2) % 0x100, file);
        fputc ((off[i] - 2) / 0x100, file);
    }

    if (ferror (file)) { fclose (file); return false; }
    fclose (file);
    return true;
}
