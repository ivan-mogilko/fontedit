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
    memset (this, 0, sizeof (CAlphabet));

    k = x / 0x10;
    l = y / 0x08;

    for (i=0; i<0x10; i++) {
        for (j=0; j<0x8; j++) {
            for (x=0; x<k; x++) {
                for (y=0; y<l; y++) {
                    s = (buf[(i * k + x + ((0x8 - j) * l - y - 1) * k * 0x10) * 3]) & 0xff;
                    if (s != 0x00 && s != 0xff) {
                        if (y == 0) {
                            letter[i + 0x10 * j].width  = x;
                            x = k;
                        } else {
                            letter[i + 0x10 * j].height = y;
                        }
                        break;
                    }
                    letter[i + 0x10 * j].data[x][y] = s ? 1 : 0;
                }
            }
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
    for (i=0; i<0x80; i++) {
        if (k < letter[i].width)  k = letter[i].width;
        if (l < letter[i].height) l = letter[i].height;
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

    buf = new char [k * l * 0x80 * 3];
    if (buf == NULL) return false;
    memset (buf, 0x99, k * l * 0x80 * 3);

    for (i=0; i<0x10; i++) {
        for (j=0; j<0x8; j++) {
            for (x=0; x<k && x<letter[i + 0x10 * j].width; x++) {
                for (y=0; y<l && y<letter[i + 0x10 * j].height; y++) {
                    buf[(i * k + x + ((0x8 - j) * l - y - 1) * k * 0x10) * 3]     = letter[i + 0x10 * j].data[x][y] ? 0xff : 0;
                    buf[(i * k + x + ((0x8 - j) * l - y - 1) * k * 0x10) * 3 + 1] = letter[i + 0x10 * j].data[x][y] ? 0xff : 0;
                    buf[(i * k + x + ((0x8 - j) * l - y - 1) * k * 0x10) * 3 + 2] = letter[i + 0x10 * j].data[x][y] ? 0xff : 0;
                }
            }
        }
    }
    fwrite (buf, k * l * 0x80 * 3, 1, file);

    if (ferror (file)) { fclose (file); return false; }
    fclose (file);
    delete [] buf;
    return true;
}
