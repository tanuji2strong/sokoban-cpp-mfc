
// s0kobanView.h : interface of the Cs0kobanView class
//

#pragma once


class Cs0kobanView : public CView
{
protected: // create from serialization only
	Cs0kobanView() noexcept;
	DECLARE_DYNCREATE(Cs0kobanView)

// Attributes
public:
	Cs0kobanDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~Cs0kobanView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void ReadMap();
	void ShowMap();
	void Level(int l);
	int n;
	int wx, wy;
	int steps;
	int arrivals;
	int dest, box, worker;
	int wlength, width;
	bool m_keyin;
	bool m_hasWon;
	char map[200][200];
	bool CheckWin() const; // Added const to match the implementation
	void ShowWinMessage() const; // Added const to match the implementation
	void ShowMessage();
};

#ifndef _DEBUG  // debug version in s0kobanView.cpp
inline Cs0kobanDoc* Cs0kobanView::GetDocument() const
   { return reinterpret_cast<Cs0kobanDoc*>(m_pDocument); }
#endif

