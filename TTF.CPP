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

bool CAlphabet::ttf_read (const char *filename) {
    CFont  f1;
    CDC    *dc;
    CRect  rect (120, 32, 184, 96);
    CBrush brush;
    int    i, j, k, l;
    char   buf[32];

    dc = AfxGetApp ()->GetMainWnd ()->GetDC ();
    if (dc == NULL) return false;
    if (f1.CreatePointFont (10 * line_h, "") == false) return false;
    i = line_h;
    *this = CAlphabet();
    line_h = i;
    dc->SelectObject (f1);
    brush.CreateSolidBrush (0xff0000);
    
    const int letter_count = CAlphabet::AsciiSize;
    for (i=0; i<letter_count; i++) {
        buf[0] = i;
        buf[1] = 0;
        dc->FillRect (rect, &brush);
        dc->DrawText (buf, rect, 0);
        CLetter letter;
        for (j=0; j<64; j++) {
            for (k=0; k<64; k++) {
                l = dc->GetPixel (rect.left + j, rect.top + k);
                if      (l == 0)        letter.data[j][k] = 1;
                else if (l == 0xffffff) letter.data[j][k] = 0;
                else if (k == 0) {      letter.width  = j; j = 400; break; }
                else {                  letter.height = k;          break; }
            }
        }
        alphabet.letters[i] = letter;
    }
    AfxGetApp ()->GetMainWnd ()->ReleaseDC (dc);
    AfxMessageBox ("Loading TrueType fonts is only partially supported. Only the standard system font can be loaded, at any point size you desire (use the height indicator)");
    return true;
}

bool CAlphabet::ttf_write (const char *filename) {
    AfxMessageBox ("Saving TrueType fonts is not supported, nor will it be.");
    return true;
}
