
// s0koban.h : main header file for the s0koban application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// Cs0kobanApp:
// See s0koban.cpp for the implementation of this class
//

class Cs0kobanApp : public CWinApp
{
public:
	Cs0kobanApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern Cs0kobanApp theApp;
