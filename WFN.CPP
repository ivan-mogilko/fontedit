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

#include <vector>
#include "FontEdit.h"
#include "Alphabet.h"

const char wfn_tag[] = "WGT Font File  ";

bool CAlphabet::wfn_read (const char *filename) {
    FILE *file;

    file = fopen (filename, "rb");
    if (file == NULL) return false;

    for (int i=0; wfn_tag[i]; i++) {
        if (fgetc (file) != wfn_tag[i]) return false;
    }

    *this = CAlphabet();
    // read offset table address
    int off_table =  fgetc (file);
    off_table     += fgetc (file) * 0x100;
    // assume that offsets take all the space to the file end
    fseek(file, 0, SEEK_END);
    int end = ftell(file);
    int letters_in_file = (end - off_table) / sizeof(short);
    // seek to the offset table and read them
    fseek (file, off_table, SEEK_SET);
    const int letter_count = std::min(letters_in_file, CAlphabet::MaxSize);
    std::vector<int> off; off.resize(letter_count);
    for (int i=0; i<letter_count; i++) {
        off[i] =  fgetc (file);
        off[i] += fgetc (file) * 0x100;
    }

    line_h = 0;
    for (int i=0; i<letter_count; i++) {
        if (off[i] == 0)
            continue; // no letter
        fseek (file, off[i], SEEK_SET);
        CLetter letter;
        letter.width  =  fgetc (file);
        letter.width  += fgetc (file) * 0x100;
        letter.height =  fgetc (file);
        letter.height += fgetc (file) * 0x100;
        if (letter.height > line_h) line_h = letter.height;
        for (int j=0; j<letter.height; j++) {
            for (int k=0; k<(letter.width + 7) / 8; k++) {
                int n = fgetc (file);
                for (int l=0; l<8; l++)
                    letter.data[k * 8 + (7 - l)][j] = (n >> l) & 1;
            }
        }
        letters[i] = letter;
    }
    if (ferror (file)) { fclose (file); return false; }
    fclose (file);
    return true;
}

bool CAlphabet::wfn_write (const char *filename) {
    FILE *file;

    file = fopen (filename, "wb");
    if (file == NULL) return false;

    /* Font header */
    for (int i=0; wfn_tag[i]; i++){
        fputc (wfn_tag[i], file);
    }
    int o1 = ftell (file);
    fputc (0, file);
    fputc (0, file);

    /* Binary letter data */
    // get the topmost letter to avoid spending extra space
    // because std::map is sorted, it is always the last item
    int letter_count = 0;
    if (letters.size() > 0) {
        letter_count = std::min(letters.rbegin()->first + 1, CAlphabet::MaxSize);
    }

    std::vector<int> off; off.resize(letter_count);

    for (int i=0; i<letter_count; i++) {
        if (letters.count(i) == 0) {
            off[i] = 0;
            continue;
        }
        off[i] = ftell (file);
        const CLetter &letter = letters[i];
        fputc (letter.width  % 0x100, file);
        fputc (letter.width  / 0x100, file);
        fputc (letter.height % 0x100, file);
        fputc (letter.height / 0x100, file);
        for (int j=0; j<letter.height; j++) {
            for (int k=0; k<(letter.width + 7) / 8; k++) {
                int n = 0;
                for (int l=0; l<8; l++)
                    n |= (letter.data[k * 8 + (7 - l)][j] & 1) << l;
                fputc (n, file);
            }
        }
    }

    /* Add offsets to file header */
    int o2 = ftell (file);
    for (int i=0; i<letter_count; i++) {
        fputc (off[i] % 0x100, file);
        fputc (off[i] / 0x100, file);
    }
    fseek (file, o1, SEEK_SET);
    fputc (o2 % 0x100, file);
    fputc (o2 / 0x100, file);

    if (ferror (file)) { fclose (file); return false; }
    fclose (file);
    return true;
}
