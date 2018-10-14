
// RealTimeMFC_ToTestBothWinCLibs.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CRealTimeMFC_ToTestBothWinCLibsApp:
// See RealTimeMFC_ToTestBothWinCLibs.cpp for the implementation of this class
//

class CRealTimeMFC_ToTestBothWinCLibsApp : public CWinApp
{
public:
	CRealTimeMFC_ToTestBothWinCLibsApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CRealTimeMFC_ToTestBothWinCLibsApp theApp;