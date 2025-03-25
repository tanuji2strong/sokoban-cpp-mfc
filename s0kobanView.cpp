// s0kobanView.cpp : implementation of the Cs0kobanView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "s0koban.h"
#endif

#include "s0kobanDoc.h"
#include "s0kobanView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cs0kobanView

IMPLEMENT_DYNCREATE(Cs0kobanView, CView)

BEGIN_MESSAGE_MAP(Cs0kobanView, CView)
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// Cs0kobanView construction/destruction

Cs0kobanView::Cs0kobanView() noexcept
{
    // Initialize member variables
    n = 1;
    wx = 0;
    wy = 0;
    wlength = 0;
    width = 0;
    steps = 0;
    arrivals = 0;
    box = 0;
    worker = 0;
    dest = 0;
    m_keyin = false;
    m_hasWon = false; // Initialize the win flag

    // Initialize map
    memset(map, ' ', sizeof(map));
}

Cs0kobanView::~Cs0kobanView()
{
}

BOOL Cs0kobanView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

// Cs0kobanView drawing

void Cs0kobanView::OnDraw(CDC* /*pDC*/)
{
    Cs0kobanDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    if (!m_keyin)
    {
        ReadMap();
        ShowMessage();
    }
    else
    {
        ShowMap();
    }
}

// Cs0kobanView printing

BOOL Cs0kobanView::OnPreparePrinting(CPrintInfo* pInfo)
{
    return DoPreparePrinting(pInfo);
}

void Cs0kobanView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add extra initialization before printing
}

void Cs0kobanView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add cleanup after printing
}

// Cs0kobanView diagnostics

#ifdef _DEBUG
void Cs0kobanView::AssertValid() const
{
    CView::AssertValid();
}

void Cs0kobanView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

Cs0kobanDoc* Cs0kobanView::GetDocument() const
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cs0kobanDoc)));
    return (Cs0kobanDoc*)m_pDocument;
}
#endif //_DEBUG

// Cs0kobanView message handlers

void Cs0kobanView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (m_hasWon && nChar == VK_RETURN) {
        n++;
        Level(n);
        m_hasWon = false; // Reset win flag
        return;
    }
    Cs0kobanDoc* aDoc = GetDocument();
    m_keyin = true;

    auto showAndUpdate = [&]() {
        steps++;
        ShowMap();
        if (CheckWin()) {
            ShowWinMessage();
            m_hasWon = true; // Set win flag
        }
    };

    auto moveWorker = [&](int new_wx, int new_wy, char new_char) {
        map[wy][wx] = (map[wy][wx] == 'Q') ? 'D' : ' ';
        map[new_wy][new_wx] = new_char;
        wx = new_wx;
        wy = new_wy;
        showAndUpdate();
        ShowMessage();
    };

    auto pushBox = [&](int box_new_wx, int box_new_wy, int worker_new_wx, int worker_new_wy, char new_worker_char, char new_box_char) {
        map[box_new_wy][box_new_wx] = new_box_char;
        moveWorker(worker_new_wx, worker_new_wy, new_worker_char);
        ShowMessage();
    };

    auto moveOrPush = [&](int dx, int dy) {
        int new_wx = wx + dx;
        int new_wy = wy + dy;
        int box_new_wx = new_wx + dx;
        int box_new_wy = new_wy + dy;

        switch (map[new_wy][new_wx]) {
        case ' ':
            moveWorker(new_wx, new_wy, (map[wy][wx] == 'Q') ? 'W' : map[wy][wx]);
            break;
        case 'D':
            moveWorker(new_wx, new_wy, 'Q');
            break;
        case 'B':
            if (map[box_new_wy][box_new_wx] == ' ' || map[box_new_wy][box_new_wx] == 'D') {
                pushBox(box_new_wx, box_new_wy, new_wx, new_wy, (map[wy][wx] == 'Q') ? 'Q' : 'W', (map[box_new_wy][box_new_wx] == 'D') ? 'C' : 'B');
                if (map[box_new_wy][box_new_wx] == 'D') arrivals++;
            }
            break;
        case 'C':
            if (map[box_new_wy][box_new_wx] == ' ' || map[box_new_wy][box_new_wx] == 'D') {
                pushBox(box_new_wx, box_new_wy, new_wx, new_wy, 'Q', 'C');
            }
            break;
        }
    };

    switch (nChar) {
    case 'A': case 37: // Move left
        moveOrPush(-1, 0);
        break;
    case 'W': case 38: // Move up
        moveOrPush(0, -1);
        break;
    case 'D': case 39: // Move right
        moveOrPush(1, 0);
        break;
    case 'S': case 40: // Move down
        moveOrPush(0, 1);
        break;
    case 33: // Page up
        if (++n > 100) n = 0;
        else Level(n);
        break;
    case 34: // Page down
        if (--n < 0) n = 0;
        else Level(n);
        break;
    }

    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

bool Cs0kobanView::CheckWin() const
{
    for (int y = 0; y < 200; ++y) {
        for (int x = 0; x < 200; ++x) {
            if (map[y][x] == 'B') {
                return false; // There are still boxes not on destinations
            }
        }
    }
    return true;
}

void Cs0kobanView::ShowWinMessage() const
{
    AfxMessageBox(_T("You pass the Level!!\n Press ENTER to continue.!!"));
}

void Cs0kobanView::ReadMap()
{
    Cs0kobanDoc* aDoc = GetDocument();
    char name[20];
    snprintf(name, sizeof(name), "peta\\map%03d.txt", n);

    FILE* m;
    errno_t err = fopen_s(&m, name, "r");

    memset(map, ' ', sizeof(map));

    if (err == 0)
    {
        int ch;
        for (int i = 0; i < 200 && !feof(m); i++) {
            for (int j = 0; j < 200; j++) {
                ch = fgetc(m);
                if (ch == EOF) break;

                switch (ch) {
                case 'W':
                    map[i][j] = 'W';
                    wx = j;
                    wy = i;
                    worker++;
                    break;
                case 'H':
                    map[i][j] = 'H';
                    break;
                case 'B':
                    map[i][j] = 'B';
                    box++;
                    break;
                case 'D':
                    map[i][j] = 'D';
                    dest++;
                    break;
                case 'C':
                    map[i][j] = 'C';
                    arrivals++; dest++;
                    break;
                case '\t':
                    for (int c = 0; c < 7; c++, j++)
                        map[i][j] = ' ';
                    map[i][j] = ' ';
                    break;
                case '\n':
                    map[i][j] = '\n';
                    j = 200;
                    break;
                case 'F':
                    map[i][j] = 'F';
                    i = 200;
                    j = 200;
                    break;
                default:
                    map[i][j] = ' ';
                }
            }
        }
        fclose(m);
    }

    // Calculate map dimensions
    for (int i = 0; i < 200; i++) {
        for (int j = 0; j < 200; j++) {
            char s = map[i][j];
            if (s == '\n' || s == 'F') {
                if (wlength < j)
                    wlength = j;
            }
            if (s == 'F')
                width = i;
        }
    }

    ShowMap();
}

void Cs0kobanView::ShowMap()
{
    CClientDC bDC(this);
    CBitmap worker, space, arr, box, box1, wall;
    worker.LoadBitmap(IDB_Worker);
    space.LoadBitmap(IDB_Blank);
    arr.LoadBitmap(IDB_Arrives);
    box.LoadBitmap(IDB_Box);
    box1.LoadBitmap(IDB_Box1);
    wall.LoadBitmap(IDB_Wall);

    CDC memDC;
    memDC.CreateCompatibleDC(&bDC);

    int x = 0, y = 0;
    for (int i = 0; i <= width; i++) {
        for (int j = 0; j <= wlength; j++) {
            char ch = map[i][j];
            switch (ch) {
            case 'H':
                memDC.SelectObject(&wall);
                break;
            case 'B':
                memDC.SelectObject(&box1);
                break;
            case 'D':
                memDC.SelectObject(&arr);
                break;
            case 'W':
            case 'Q':
                memDC.SelectObject(&worker);
                break;
            case 'C':
                memDC.SelectObject(&box);
                break;
            default:
                memDC.SelectObject(&space);
            }
            bDC.BitBlt(x, y, 30, 30, &memDC, 0, 0, SRCCOPY);
            x += 30;
        }
        x = 0;
        y += 30;
    }
}

void Cs0kobanView::Level(int l)
{
    n = l; // Update the level number
    m_keyin = false;
    wx = 0;
    wy = 0;
    steps = 0;
    arrivals = 0;
    dest = 0;
    wlength = 0;
    width = 0;
    box = 0;
    worker = 0;

    ReadMap(); // Load the new level's map data
    GetDocument()->UpdateAllViews(NULL);
}

void Cs0kobanView::ShowMessage()
{
    CClientDC bDC(this);
    CString l, s, a, d;
    l.Format(L"Level:  %d", n);
    s.Format(L"Steps:  %d", steps);
    a.Format(L"Arrival: %d", arrivals);
    d.Format(L"Destination:  %d", dest);

    bDC.TextOut(wlength * 40, 50, l);
    bDC.TextOut(wlength * 40, 100, s);
    bDC.TextOut(wlength * 40, 150, a);
    bDC.TextOut(wlength * 40, 200, d);
    bDC.TextOut(wlength * 35, 400, L"page up=next level");
    bDC.TextOut(wlength * 35, 430, L"page down=previous level");
    bDC.TextOut(wlength * 35, 460, L"112321052 陳學仁");


}
