
// RealTimeMFC_ToTestBothWinCLibsDlg.h : header file
//

#pragma once


// CRealTimeMFC_ToTestBothWinCLibsDlg dialog
class CRealTimeMFC_ToTestBothWinCLibsDlg : public CDialogEx
{
// Construction
public:
	CRealTimeMFC_ToTestBothWinCLibsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REALTIMEMFC_TOTESTBOTHWINCLIBS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedOk3();
};
