#include "FontEdit.h"
#include "Alphabet.h"

CAlphabet::CAlphabet (void) {
    int i;
    memset (this, 0, sizeof (CAlphabet));
    for (i=0; i<0x80; i++) {
        letter[i].width  = 1;
        letter[i].height = 1;
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
    memset (this, 0, sizeof (CAlphabet));

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
        letter[i].width  = fgetc (file);
        letter[i].height = fgetc (file);
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
    fputc (0x80, file);
    fputc (0x00, file);

    /* Font line height */
    fputc (line_h % 0x100, file);
    fputc (line_h / 0x100, file);

    /* Offsets of individual letters */
    for (i=0; i<0x80; i++) {
        fputc (0, file);
        fputc (0, file);
    }

    /* Binary letter data */
    for (i=0; i<0x80; i++) {
        off[i] = ftell (file);
        fputc (letter[i].width,  file);
        fputc (letter[i].height, file);
        for (j=0; j<letter[i].height; j++) {
            for (k=0; k<(letter[i].width + 7) / 8; k++) {
                n = 0;
                for (l=0; l<8; l++)
                    n |= (letter[i].data[k * 8 + (7 - l)][j] & 1) << l;
                fputc (n, file);
            }
        }
    }

    /* Add offsets to file header */
    fseek (file, 0x08, SEEK_SET);
    for (i=0; i<0x80; i++) {
        fputc ((off[i] - 2) % 0x100, file);
        fputc ((off[i] - 2) / 0x100, file);
    }

    if (ferror (file)) { fclose (file); return false; }
    fclose (file);
    return true;
}
