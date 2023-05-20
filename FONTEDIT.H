#ifdef _FontEdit
#error Double include.
#endif
#define _FontEdit
#if _MSC_VER > 1000
#pragma once
#endif

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif
#include "resource.h"

#include "Alphabet.h"

class CFontEditDlg : public CDialog {
    public:
    CFontEditDlg (CWnd* pParent = NULL);

    //{{AFX_DATA(CFontEditDlg)
	enum { IDD = IDD_FONTEDIT_DIALOG };
	CSpinButtonCtrl	WidthSpin;
	CEdit	Width;
	CSpinButtonCtrl	LetterSpin;
	CEdit	Height;
	CSpinButtonCtrl	HeightSpin;
	CEdit	LetNum;
	CEdit	LetChar;
    CStatic Picture;
    CEdit   RegCode;
    CEdit   UserCode;
	//}}AFX_DATA
    //{{AFX_VIRTUAL(CFontEditDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

    protected:
    HICON   m_hIcon;
    char    buf        [32];
    int     cur_ltr,
            mode,
            drawcol,
            changed;
    CBitmap bmp;
    CDC     bdc;
    CLetter *ltr,
            pastebuf,
            undobuf;
    CString filename,
            fileext;
    void    DrawLetter (void);
    void    SetCurLtr  (int, bool force = false);
    void    SetMode    (int);
    void    DoLoad     (const char*);

    //{{AFX_MSG(CFontEditDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClear();
	afx_msg void OnCopy();
	afx_msg void OnAbout();
	afx_msg void OnLetter();
	afx_msg void OnLoad();
	afx_msg void OnPandown();
	afx_msg void OnPanleft();
	afx_msg void OnPanright();
	afx_msg void OnPanup();
	afx_msg void OnPaste();
	afx_msg void OnSave();
	afx_msg void OnUndo();
	afx_msg void OnChangeLetchar();
	afx_msg void OnHeightdown();
	afx_msg void OnHeightup();
	afx_msg void OnWidthdown();
	afx_msg void OnWidthup();
	afx_msg void OnLetterdown();
	afx_msg void OnLetterup();
	afx_msg void OnHeight();
	afx_msg void OnLetnum();
	afx_msg void OnWidth();
	afx_msg void OnKillfocusHeight();
	afx_msg void OnKillfocusWidth();
	afx_msg void OnSetfocusLetchar();
	afx_msg void OnKillfocusLetchar();
	afx_msg void OnKillfocusLetnum();
	afx_msg void OnSetfocusLetnum();
	virtual void OnCancel();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnOutline();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

class CFontEditApp : public CWinApp {
    public:
    CFontEditDlg *dlg;
    //{{AFX_VIRTUAL(CFontEditApp)
    public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL
    //{{AFX_MSG(CFontEditApp)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

extern CFontEditApp theApp;

//{{AFX_INSERT_LOCATION}}
