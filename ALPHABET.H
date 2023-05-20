#ifndef __ALPHABET_H
#define __ALPHABET_H

struct CLetter {
    int height,
        width,
        data   [64][64];
};

class CAlphabet {
    public: CAlphabet (void);
    bool    bmp_read  (const char*);
    bool    bmp_write (const char*);
    bool    sci_read  (const char*);
    bool    sci_write (const char*);
    bool    ttf_read  (const char*);
    bool    ttf_write (const char*);
    bool    wfn_read  (const char*);
    bool    wfn_write (const char*);

    int     line_h;
    CLetter letter    [128];
};

extern CAlphabet alphabet;

#endif
