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

// shadow font (simply move all letters 1 pix SE)
// or raise entire font by one pixel, etc


/*
 * Great! - SSH
 * Nice tool! - SteveMcCrea
 */

#include "FontEdit.h"
#include "Alphabet.h"

CFontEditApp theApp;
CAlphabet    alphabet;
CString      Title     = "Radiant FontEdit";
bool         resize    = true;

BOOL CFontEditApp::InitInstance (void) {
    dlg = new CFontEditDlg;
    m_pMainWnd = dlg;
    dlg->DoModal ();
    return FALSE;
}

BEGIN_MESSAGE_MAP(CFontEditApp, CWinApp)
    //{{AFX_MSG_MAP(CFontEditApp)
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CFontEditDlg::CFontEditDlg (CWnd* pParent) : CDialog(CFontEditDlg::IDD, pParent) {
    m_hIcon = AfxGetApp()->LoadIcon (ICN_MAIN);
    //{{AFX_DATA_INIT(CFontEditDlg)
    //}}AFX_DATA_INIT
}

BOOL CFontEditDlg::OnInitDialog (void) {
    HKEY    key;
    char    dir [512],
            exe [512];
    int     i;
    CString s1;
    CDialog::OnInitDialog  ();
    bmp.LoadBitmap         (IDB_TILE);
    bdc.CreateCompatibleDC (GetWindowDC ());
    bdc.SelectObject       (&bmp);
    SetIcon                (m_hIcon, TRUE);
    SetIcon                (m_hIcon, FALSE);
    CenterWindow           ();
    LetChar.SetLimitText   (1);
    SetMode                (0);
    if (RegCreateKey (HKEY_CLASSES_ROOT, ".wfn", &key) == 0) {
        RegSetValue (key, "", REG_SZ, Title, 0);
        RegCloseKey (key);
    }
    if (RegCreateKey (HKEY_CLASSES_ROOT, ".0", &key) == 0) {
        RegSetValue (key, "", REG_SZ, Title, 0);
        RegCloseKey (key);
    }
    if (RegCreateKey (HKEY_CLASSES_ROOT, Title, &key) == 0) {
        GetCurrentDirectory (512, dir);
        RegSetValue (key, "", REG_SZ, "Adventure game font file", 0);
        sprintf     (exe, "%s\\%s.exe %%1", dir, theApp.m_pszAppName);
        RegSetValue (key, "shell\\open\\command", REG_SZ, exe, 0xff);
        RegCloseKey (key);
    }
    pastebuf.height = 1;
    pastebuf.width  = 1;
    undobuf. height = 1;
    undobuf. width  = 1; 
    if (theApp.m_lpCmdLine) if (theApp.m_lpCmdLine[0]) {
        s1 = theApp.m_lpCmdLine;
        i  = s1.ReverseFind ('.');
        s1.Remove ('"');
        if (i > 0) {
            filename = s1; filename.Delete (i - 1, s1.GetLength () - i + 1);
            fileext  = s1; fileext. Delete (0, i);
        } else {
            filename = s1;
            fileext  = "";
        }
        i = filename.ReverseFind ('\\');
        if (i > 0) filename.Delete (0, i + 1);
        DoLoad (s1);
    }
    return FALSE;
}

HCURSOR CFontEditDlg::OnQueryDragIcon (void) {
    return (HCURSOR) m_hIcon;
}

void CFontEditDlg::DoDataExchange (CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CFontEditDlg)
        DDX_Control(pDX, IDC_WIDTH, Width);
        DDX_Control(pDX, IDC_HEIGHT, Height);
        DDX_Control(pDX, IDC_LETNUM, LetNum);
        DDX_Control(pDX, IDC_LETCHAR, LetChar);
        //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFontEditDlg, CDialog)
    //{{AFX_MSG_MAP(CFontEditDlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_LBUTTONDOWN()
        ON_WM_LBUTTONDBLCLK()
        ON_WM_RBUTTONDOWN()
        ON_WM_RBUTTONDBLCLK()
        ON_BN_CLICKED(IDC_CLEAR, OnClear)
        ON_BN_CLICKED(IDC_COPY, OnCopy)
        ON_BN_CLICKED(IDC_ABOUT, OnAbout)
        ON_BN_CLICKED(IDC_LETTER, OnLetter)
        ON_BN_CLICKED(IDC_LOAD, OnLoad)
        ON_BN_CLICKED(IDC_PANDOWN, OnPandown)
        ON_BN_CLICKED(IDC_PANLEFT, OnPanleft)
        ON_BN_CLICKED(IDC_PANRIGHT, OnPanright)
        ON_BN_CLICKED(IDC_PANUP, OnPanup)
        ON_BN_CLICKED(IDC_PASTE, OnPaste)
        ON_BN_CLICKED(IDC_SAVE, OnSave)
        ON_BN_CLICKED(IDC_UNDO, OnUndo)
        ON_EN_CHANGE(IDC_LETCHAR, OnChangeLetchar)
        ON_BN_CLICKED(IDC_HEIGHTDOWN, OnHeightdown)
        ON_BN_CLICKED(IDC_HEIGHTUP, OnHeightup)
        ON_BN_CLICKED(IDC_WIDTHDOWN, OnWidthdown)
        ON_BN_CLICKED(IDC_WIDTHUP, OnWidthup)
        ON_BN_CLICKED(IDC_LETTERDOWN, OnLetterdown)
        ON_BN_CLICKED(IDC_LETTERUP, OnLetterup)
        ON_EN_CHANGE(IDC_HEIGHT, OnHeight)
        ON_EN_CHANGE(IDC_LETNUM, OnLetnum)
        ON_EN_CHANGE(IDC_WIDTH, OnWidth)
        ON_EN_KILLFOCUS(IDC_HEIGHT, OnKillfocusHeight)
        ON_EN_KILLFOCUS(IDC_WIDTH, OnKillfocusWidth)
        ON_EN_SETFOCUS(IDC_LETCHAR, OnSetfocusLetchar)
        ON_EN_KILLFOCUS(IDC_LETCHAR, OnKillfocusLetchar)
        ON_EN_KILLFOCUS(IDC_LETNUM, OnKillfocusLetnum)
        ON_EN_SETFOCUS(IDC_LETNUM, OnSetfocusLetnum)
        ON_WM_MOUSEMOVE()
        ON_BN_CLICKED(IDC_OUTLINE, OnOutline)
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFontEditDlg::SetMode (int newmode) {
    mode = newmode;
    Width.EnableWindow  (mode);
    sprintf (buf, "%d", mode ? ltr->height : alphabet.line_h);
    Height.SetWindowText (buf);
}

void CFontEditDlg::DrawLetter (void) {
    CDC *dc;
    int x, y, i, j, k, l, dd = 2;

    const int letter_count = CAlphabet.AsciiSize;
    dc = GetDC ();
    if (dc) {
        if (mode == 0) {
            k = 0;
            l = 0;
            for (i=0; i<letter_count; i++) {
                if (alphabet.letters.count(i) == 0)
                    continue;
                const CLetter &letter = alphabet.letters[i];
                if (k < letter.width)  k = letter.width;
                if (l < letter.height) l = letter.height;
            }
            k += 2; if (k < 8) k = 8;
            l += 2; if (l < 8) l = 8;
            if (k > 32 || l > 32) dd = 1;

            CRect rect;
            GetWindowRect (&rect);
            if (rect.top > 0 && rect.left > 0 && resize) {
                rect.right  = rect.left + 120 + 0x20 * k;
                rect.bottom = rect.top  +  57 + 0x10 * l;
                if (rect.Width  () < 245) rect.right  = rect.left + 245;
                if (rect.Height () < 231) rect.bottom = rect.top  + 231;
                MoveWindow (rect);
                resize = false;
            }

            for (i=0; i<0x10; i++) {
                for (j=0; j<0x8; j++) {
                    if (alphabet.letters.count(i + 0x10 * j) == 0)
                        continue;
                    const CLetter &letter = alphabet.letters[i + 0x10 * j];
                    for (x=0; x<k && x<letter.width; x++) {
                        for (y=0; y<l && y<letter.height; y++) {
                            dc->BitBlt ((i * k + x) * dd + 120, (j * l + y) * dd + 32, dd, dd,
                                &bdc, letter.data[x][y] ? 16 : 0, 0, SRCCOPY);
                        }
                    }
                }
            }
        } else {
            ltr = &alphabet.letters[cur_ltr]; // FIXME: dangerous
            for (y=0; y<ltr->height; y++) {
                for (x=0; x<ltr->width; x++) {
                    dc->BitBlt (x * 16 + 184, y * 16 + 32, 16, 16, &bdc, ltr->data[x][y] ? 16 : 0, 0, SRCCOPY);
                    dc->BitBlt (x *  2 + 120, y *  2 + 32,  2,  2, &bdc, ltr->data[x][y] ? 16 : 0, 0, SRCCOPY);
                }
            }
            for (y=0; y<pastebuf.height; y++) {
                for (x=0; x<pastebuf.width; x++) {
                    dc->BitBlt (x *  2 + 120, y *  2 + 152, 2, 2, &bdc, pastebuf.data[x][y] ? 16 : 0, 0, SRCCOPY);
                }
            }
        }
    }
    ReleaseDC (dc);
}

void CFontEditDlg::SetCurLtr (int newltr, bool force) {
    if (cur_ltr == newltr && force == false && mode == 1) return;
    cur_ltr = newltr % 128;
    ltr     = &alphabet.letters[cur_ltr];
    SetMode               (1);
    sprintf               (buf, "%d", cur_ltr);
    if (GetFocus () != &LetNum)
    LetNum.SetWindowText  (buf);
    sprintf               (buf, "%c", cur_ltr);
    LetChar.SetWindowText (buf);
    sprintf               (buf, "%d", ltr->width);
    Width.SetWindowText   (buf);
    sprintf               (buf, "%d", ltr->height);
    Height.SetWindowText  (buf);
    RedrawWindow          ();
    memcpy                (&undobuf, ltr, sizeof (CLetter));
}

void CFontEditDlg::OnPaint (void) {
    if (IsIconic ()) {
        CPaintDC dc (this);
        SendMessage (WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc (), 0);
        int cxIcon = GetSystemMetrics (SM_CXICON);
        int cyIcon = GetSystemMetrics (SM_CYICON);
        CRect rect;
        GetClientRect (&rect);
        int x = (rect.Width  () - cxIcon + 1) / 2;
        int y = (rect.Height () - cyIcon + 1) / 2;
        dc.DrawIcon (x, y, m_hIcon);
    } else {
        CDialog::OnPaint ();
        DrawLetter ();
    }
}

void CFontEditDlg::OnLButtonDown (UINT nFlags, CPoint point) {
    int i, k, l;
    if (mode == 0) {
        if (point.x < 120 || point.y < 32) return;
        k = 0;
        l = 0;
        const int letter_count = CAlphabet::AsciiSize;
        for (i=0; i<letter_count; i++) {
            if (alphabet.letters.count(i) == 0)
                continue;
            const CLetter &letter = alphabet.letters[i];
            if (k < letter.width)  k = letter.width;
            if (l < letter.height) l = letter.height;
        }
        k += 2; if (k < 8) k = 8; if (k <= 32) k *= 2;
        l += 2; if (l < 8) l = 8; if (l <= 32) l *= 2;
        point.x = (point.x - 120) / k;
        point.y = (point.y - 32)  / l;
        if (point.x < 16 && point.y < 8)
            SetCurLtr (point.x + 16 * point.y);
    } else {
        if (point.x < 184 || point.y < 32) return;
        point.x = (point.x - 184) / 16;
        point.y = (point.y - 32)  / 16;
        if (point.x < ltr->width && point.y < ltr->height) {
            ltr->data[point.x][point.y] ^= 1;
            drawcol = ltr->data[point.x][point.y];
            DrawLetter ();
            changed = true;
        }
    }
}

void CFontEditDlg::OnMouseMove (UINT nFlags, CPoint point) {
        CDialog::OnMouseMove (nFlags, point);

    if (mode == 0) return;
    if (nFlags == 0) return;
    if (point.x < 184 || point.y < 32) return;
    point.x = (point.x - 184) / 16;
    point.y = (point.y - 32)  / 16;
    if (point.x < ltr->width && point.y < ltr->height) {
        ltr->data[point.x][point.y] = drawcol;
        DrawLetter ();
        changed = true;
    }
}

void CFontEditDlg::OnRButtonDown (UINT nFlags, CPoint point) {
    if (mode == 0) OnLButtonDown (nFlags, point);
}

void CFontEditDlg::OnLButtonDblClk (UINT nFlags, CPoint point) {
    OnLButtonDown (nFlags, point);
}

void CFontEditDlg::OnRButtonDblClk (UINT nFlags, CPoint point) {
        OnRButtonDown (nFlags, point);
}



/********** Select letter **********/

void CFontEditDlg::OnLetter (void) {
    if (mode == 1) {
        SetMode      (0);
        RedrawWindow ();
    }
}

void CFontEditDlg::OnLetnum (void) {
    int n;
    LetNum.GetWindowText (buf, 10);
    n = atoi (buf);
    if (n > 0 && n <= 127) SetCurLtr (n);
}

void CFontEditDlg::OnSetfocusLetnum (void) {
        LetNum.SetWindowText ("");      
}

void CFontEditDlg::OnKillfocusLetnum (void) {
    sprintf              (buf, "%d", cur_ltr);
    LetNum.SetWindowText (buf);
}

void CFontEditDlg::OnChangeLetchar (void) {
    LetChar.GetWindowText (buf, 10);
    if (buf[0] != 0)
    SetCurLtr             (buf[0]);
    LetChar.SetSel        (0, 1);
}

void CFontEditDlg::OnSetfocusLetchar (void) {
    LetChar.SetWindowText ("");
}

void CFontEditDlg::OnKillfocusLetchar (void) {
    buf[0] = cur_ltr;
    buf[1] = 0;
    LetChar.SetWindowText (buf);
}

void CFontEditDlg::OnLetterdown (void) {
    LetChar.GetWindowText (buf, 10);
    buf[0] = (buf[0] + 127) % 128;
    buf[1] = 0;
    LetChar.SetWindowText (buf);
}

void CFontEditDlg::OnLetterup (void) {
    LetChar.GetWindowText (buf, 10);
    buf[0] = (buf[0] + 1) % 128;
    buf[1] = 0;
    LetChar.SetWindowText (buf);
}



/********** Pan (scroll) current image **********/

void CFontEditDlg::OnPanup (void) {
    int tmp, x, y;
    if (mode == 0) return;
    for (x=0; x<ltr->width; x++) {
        tmp = ltr->data[x][0];
        for (y=0; y<ltr->height - 1; y++)
            ltr->data[x][y] = ltr->data[x][y + 1];
        ltr->data[x][y] = tmp;
    }
    DrawLetter ();
    changed = true;
}

void CFontEditDlg::OnPandown (void) {
    int tmp, x, y;
    if (mode == 0) return;
    for (x=0; x<ltr->width; x++) {
        tmp = ltr->data[x][ltr->height - 1];
        for (y=ltr->height - 1; y>0; y--)
            ltr->data[x][y] = ltr->data[x][y - 1];
        ltr->data[x][y] = tmp;
    }
    DrawLetter ();
    changed = true;
}

void CFontEditDlg::OnPanleft (void) {
    int tmp, x, y;
    if (mode == 0) return;
    for (y=0; y<ltr->height; y++) {
        tmp = ltr->data[0][y];
        for (x=0; x<ltr->width - 1; x++)
            ltr->data[x][y] = ltr->data[x + 1][y];
        ltr->data[x][y] = tmp;
    }
    DrawLetter ();
    changed = true;
}

void CFontEditDlg::OnPanright (void) {
    int tmp, x, y;
    if (mode == 0) return;
    for (y=0; y<ltr->height; y++) {
        tmp = ltr->data[ltr->width - 1][y];
        for (x=ltr->width - 1; x>0; x--)
            ltr->data[x][y] = ltr->data[x - 1][y];
        ltr->data[x][y] = tmp;
    }
    DrawLetter ();
    changed = true;
}



/********** Height and Width **********/

void CFontEditDlg::OnWidth (void) {
    int n;
    Width.GetWindowText (buf, 10);
    n = atoi (buf);
    if (n >= 1 && n <= 64) {
        ltr->width = n;
            RedrawWindow ();
        changed = true;
    }
}

void CFontEditDlg::OnKillfocusWidth (void) {
    sprintf (buf, "%d", ltr->width);
    Width.SetWindowText (buf);
}

void CFontEditDlg::OnWidthdown (void) {
    int n;
    if (mode == 0) return;
    Width.GetWindowText (buf, 10);
    n = atoi (buf);
    if (n <= 1) n = 64; else n --;
    sprintf (buf, "%d", n);
    Width.SetWindowText (buf);
}

void CFontEditDlg::OnWidthup (void) {
    int n;
    if (mode == 0) return;
    Width.GetWindowText (buf, 10);
    n = atoi (buf);
    if (n >= 64) n = 1; else n ++;
    sprintf (buf, "%d", n);
    Width.SetWindowText (buf);
}

void CFontEditDlg::OnHeight (void) {
    int n;
    Height.GetWindowText (buf, 10);
    n = atoi (buf);
    if (n >= 1 && n <= 64) {
        if (mode == 0) {
            alphabet.line_h = n;
        } else {
            ltr->height     = n;
            RedrawWindow ();
            changed = true;
        }
    }
}

void CFontEditDlg::OnKillfocusHeight (void) {
    sprintf (buf, "%d", mode ? ltr->height : alphabet.line_h);
    Height.SetWindowText (buf);
}

void CFontEditDlg::OnHeightdown (void) {
    int n;
    Height.GetWindowText (buf, 10);
    n = atoi (buf);
    if (n <= 1) n = 64; else n --;
    sprintf (buf, "%d", n);
    Height.SetWindowText (buf);
}

void CFontEditDlg::OnHeightup (void) {
    int n;
    Height.GetWindowText (buf, 10);
    n = atoi (buf);
    if (n >= 64) n = 1; else n ++;
    sprintf (buf, "%d", n);
    Height.SetWindowText (buf);
}



/********** Copy and Paste **********/

void CFontEditDlg::OnCopy (void) {
    if (mode == 0) return;
    memcpy       (&pastebuf, ltr, sizeof (CLetter));
    RedrawWindow ();
}

void CFontEditDlg::OnPaste (void) {
    if (mode == 0) return;
    memcpy       (ltr, &pastebuf, sizeof (CLetter));
    SetCurLtr    (cur_ltr, true);
    RedrawWindow ();
    changed = true;
}

void CFontEditDlg::OnUndo (void) {
    if (mode == 0) return;
    memcpy       (ltr, &undobuf, sizeof (CLetter));
    SetCurLtr    (cur_ltr, true);
    RedrawWindow ();
}

void CFontEditDlg::OnClear (void) {
    int x, y;
    if (mode == 0) return;
    ltr->data[0][0] = !ltr->data[0][0];
    for (x=0; x<64; x++) for (y=0; y<64; y++)
        ltr->data[x][y] = ltr->data[0][0];
    DrawLetter ();
    changed = true;
}



/********** Disk access, quit etc **********/

void CFontEditDlg::OnSave (void) {
    int n = false;
    CFileDialog fd (false, fileext, filename, OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
                    "AGS font (*.wfn)|*.wfn|"
                    "SCI font (*.0)|*.0|"
                    "Windows bitmap (*.bmp)|*.bmp||", this);
    if (fd.DoModal () == 2) return;
    filename = fd.GetFileTitle ();
    fileext  = fd.GetFileExt   ();
    if      (fileext.CompareNoCase ("bmp") == 0) n = alphabet.bmp_write (fd.GetPathName ());
    else if (fileext.CompareNoCase ("ttf") == 0) n = alphabet.ttf_write (fd.GetPathName ());
    else if (fileext.CompareNoCase ("wfn") == 0) n = alphabet.wfn_write (fd.GetPathName ());
    else                                         n = alphabet.sci_write (fd.GetPathName ());
    if (n == false) {
        MessageBox ("Error writing file.");
    } else {
        SetWindowText (Title + " (" + fd.GetFileName () + ")");
        changed = false;
    }
}

void CFontEditDlg::OnLoad (void) {
    int n = false;
    if (changed) {
            n = MessageBox ("Do you want to save the changes made to this font?,", "Save changes?", MB_YESNOCANCEL);
        if (n == 2) return;
        if (n == 6) OnSave ();
    }
    CFileDialog fd (true, fileext, filename, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
                    "All supported font files|font.*;*.0;*.ttf;*.wfn;*.bmp|"
                    "AGS font (*.wfn)|*.wfn|"
                    "SCI font (font.*)|font.*;*.0|"
                    "TrueType font (*.ttf)|*.ttf|"
                    "Windows bitmap (*.bmp)|*.bmp|"
                    "All files (*.*)|*.*||", this);
    if (fd.DoModal () == 2) return;
    filename = fd.GetFileTitle ();
    fileext  = fd.GetFileExt   ();
    DoLoad (fd.GetPathName ());
}

void CFontEditDlg::DoLoad (const char *file) {
    int n;
    if      (fileext.CompareNoCase ("bmp") == 0) n = alphabet.bmp_read (file);
    else if (fileext.CompareNoCase ("ttf") == 0) n = alphabet.ttf_read (file);
    else if (fileext.CompareNoCase ("wfn") == 0) n = alphabet.wfn_read (file);
    else                                         n = alphabet.sci_read (file);
    if (n == false) {
        MessageBox ("Invalid file type, or miscellaneous read error.");
    } else {
        resize  = true;
        changed = false;
        SetWindowText (Title + " (" + filename + "." + fileext + ")");
        SetMode       (0);
        RedrawWindow  ();
    }
}

void CFontEditDlg::OnAbout (void) {
    MessageBox   ("Created in April 2004 for use with AGS\nby Pieter Simoons, aka Radiant\nThis is freeware.", "Radiant FontEdit");
    ShellExecute (NULL, NULL, "http://crystalshard.net", NULL, NULL, SW_MAXIMIZE);
}

void CFontEditDlg::OnCancel (void) {
    int n;
    if (changed) {
            n = MessageBox ("Do you want to save the changes made to this font?,", "Save changes?", MB_YESNOCANCEL);
        if (n == 2) return;
        if (n == 6) OnSave ();
    }
    CDialog::OnCancel ();
}

void CFontEditDlg::OnOutline (void) {
    int i, j, k, j1, k1;
    if (changed) {
            i = MessageBox ("Outline warning", "Warning: outlining changes the entire font and cannot be undone. Proceed?", MB_YESNO);
        if (i != 6) return;
    }
    const int letter_count = CAlphabet::AsciiSize;
    for (i=0; i<letter_count; i++) {
        ltr = &alphabet.letters[i];
        ltr->height += 2; if (ltr->height >= 64) ltr->height = 64;
        ltr->width  += 2; if (ltr->width  >= 64) ltr->width  = 64;
        for (j=0; j<ltr->width; j++)  ltr->data[j][ltr->height - 1] = 0;
        for (j=0; j<ltr->width; j++)  ltr->data[j][ltr->height - 2] = 0;
        for (k=0; k<ltr->height; k++) ltr->data[ltr->width - 1][k]  = 0;
        for (k=0; k<ltr->height; k++) ltr->data[ltr->width - 2][k]  = 0;
        memset (&undobuf, 0, sizeof (CLetter));
        for (j=0; j<ltr->width - 2; j++) {
            for (k=0; k<ltr->height - 2; k++) {
                if (ltr->data[j][k]) {
                    for (j1=0; j1<3; j1++) for (k1=0; k1<3; k1++)
                        undobuf.data[j + j1][k + k1] = 1;
                }
            }
        }
        for (j=ltr->width; j>0; j--)
            for (k=ltr->height; k>0; k--)
                ltr->data[j][k] = ltr->data[j-1][k-1];
        for (j=0; j<ltr->width; j++)  ltr->data[j][0] = 0;
        for (k=0; k<ltr->height; k++) ltr->data[0][k] = 0;
        for (j=0; j<ltr->width; j++) {
            for (k=0; k<ltr->height; k++) {
                if      (ltr->   data[j][k]) ltr->data[j][k] = 0;
                else if (undobuf.data[j][k]) ltr->data[j][k] = 1;
            }
        }
    }
    ltr = &alphabet.letters[cur_ltr];
    memcpy (&undobuf, ltr, sizeof (CLetter));
    RedrawWindow ();
}
