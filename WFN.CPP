#include "FontEdit.h"
#include "Alphabet.h"

const char wfn_tag[] = "WGT Font File  ";

bool CAlphabet::wfn_read (const char *filename) {
    FILE *file;
    int  i, j, k, l, n,
         off [0x100];

    file = fopen (filename, "rb");
    if (file == NULL) return false;

    for (i=0; wfn_tag[i]; i++)
        if (fgetc (file) != wfn_tag[i]) return false;

    memset (this, 0, sizeof (CAlphabet));
    i =  fgetc (file);
    i += fgetc (file) * 0x100;
    fseek (file, i, SEEK_SET);
    for (i=0; i<0x80; i++) {
        off[i] =  fgetc (file);
        off[i] += fgetc (file) * 0x100;
    }

    line_h = 0;
    for (i=0; i<0x80; i++) {
        fseek (file, off[i], SEEK_SET);
        letter[i].width  =  fgetc (file);
        letter[i].width  += fgetc (file) * 0x100;
        letter[i].height =  fgetc (file);
        letter[i].height += fgetc (file) * 0x100;
        if (letter[i].height > line_h) line_h = letter[i].height;
        for (j=0; j<letter[i].height; j++) {
            for (k=0; k<(letter[i].width + 7) / 8; k++) {
                n = fgetc (file);
                for (l=0; l<8; l++)
                    letter[i].data[k * 8 + (7 - l)][j] = (n >> l) & 1;
            }
        }
    }
    if (ferror (file)) { fclose (file); return false; }
    fclose (file);
    return true;
}

bool CAlphabet::wfn_write (const char *filename) {
    FILE *file;
    int  i, ii, j, k, l, n,
         o1, o2,
         off [0x100];

    file = fopen (filename, "wb");
    if (file == NULL) return false;

    /* Font header */
    for (i=0; wfn_tag[i]; i++)
        fputc (wfn_tag[i], file);
    o1 = ftell (file);
    fputc (0, file);
    fputc (0, file);

    /* Binary letter data */
    for (i=0; i<0x80; i++) {
        ii = i;
        //if (i >= 38 && i <= 126) ii = ((i + 89 - 38 - 27) % 89) + 38;
        off[i] = ftell (file);
        fputc (letter[ii].width  % 0x100, file);
        fputc (letter[ii].width  / 0x100, file);
        fputc (letter[ii].height % 0x100, file);
        fputc (letter[ii].height / 0x100, file);
        for (j=0; j<letter[ii].height; j++) {
            for (k=0; k<(letter[ii].width + 7) / 8; k++) {
                n = 0;
                for (l=0; l<8; l++)
                    n |= (letter[ii].data[k * 8 + (7 - l)][j] & 1) << l;
                fputc (n, file);
            }
        }
    }

    /* Add offsets to file header */
    o2 = ftell (file);
    for (i=0; i<0x80; i++) {
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
