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
    memset (this, 0, sizeof (CAlphabet));
    line_h = i;
    dc->SelectObject (f1);
    brush.CreateSolidBrush (0xff0000);
    
    for (i=0; i<0x80; i++) {
        buf[0] = i;
        buf[1] = 0;
        dc->FillRect (rect, &brush);
        dc->DrawText (buf, rect, 0);
        for (j=0; j<64; j++) {
            for (k=0; k<64; k++) {
                l = dc->GetPixel (rect.left + j, rect.top + k);
                if      (l == 0)        alphabet.letter[i].data[j][k] = 1;
                else if (l == 0xffffff) alphabet.letter[i].data[j][k] = 0;
                else if (k == 0) {      alphabet.letter[i].width  = j; j = 400; break; }
                else {                  alphabet.letter[i].height = k;          break; }
            }
        }
    }
    AfxGetApp ()->GetMainWnd ()->ReleaseDC (dc);
    AfxMessageBox ("Loading TrueType fonts is only partially supported. Only the standard system font can be loaded, at any point size you desire (use the height indicator)");
    return true;
}

bool CAlphabet::ttf_write (const char *filename) {
    AfxMessageBox ("Saving TrueType fonts is not supported, nor will it be.");
    return true;
}
