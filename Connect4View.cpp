
// Connect4View.cpp : implementation of the CConnect4View class
//

#include "stdafx.h"
#include <cstring>
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Connect4.h"
#endif

#include "Connect4Doc.h"
#include "Connect4View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CConnect4View

IMPLEMENT_DYNCREATE(CConnect4View, CView)

BEGIN_MESSAGE_MAP(CConnect4View, CView)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_PLAYERS_ONEPLAYER, &CConnect4View::OnPlayersOneplayer)
	ON_UPDATE_COMMAND_UI(ID_PLAYERS_ONEPLAYER, &CConnect4View::OnUpdatePlayersOneplayer)
	ON_COMMAND(ID_PLAYERS_TWOPLAYER, &CConnect4View::OnPlayersTwoplayer)
	ON_UPDATE_COMMAND_UI(ID_PLAYERS_TWOPLAYER, &CConnect4View::OnUpdatePlayersTwoplayer)
	ON_COMMAND(ID_AIDIFFICULTY_EASY, &CConnect4View::OnAidifficultyEasy)
	ON_UPDATE_COMMAND_UI(ID_AIDIFFICULTY_EASY, &CConnect4View::OnUpdateAidifficultyEasy)
	ON_COMMAND(ID_AIDIFFICULTY_MEDIUM, &CConnect4View::OnAidifficultyMedium)
	ON_UPDATE_COMMAND_UI(ID_AIDIFFICULTY_MEDIUM, &CConnect4View::OnUpdateAidifficultyMedium)
	ON_COMMAND(ID_AIDIFFICULTY_SUPEREASY, &CConnect4View::OnAidifficultySupereasy)
	ON_UPDATE_COMMAND_UI(ID_AIDIFFICULTY_SUPEREASY, &CConnect4View::OnUpdateAidifficultySupereasy)
END_MESSAGE_MAP()

// CConnect4View construction/destruction

CConnect4View::CConnect4View()
{
	// TODO: add construction code here

}

CConnect4View::~CConnect4View()
{
}

BOOL CConnect4View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CConnect4View drawing

void CConnect4View::OnDraw(CDC* pDC)
{
	//First get a pointer to the document
	CConnect4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// Save the current state of the device context
	int nDCSave = pDC->SaveDC();
	// Get the client rectangle
	CRect rcClient;
	GetClientRect(&rcClient);
	// Initialise the colours for the brushes
	COLORREF Black = RGB(0, 0, 0);
	COLORREF Blue = RGB(64,64,192);
	COLORREF Red = RGB(255, 0, 0);
	COLORREF Yellow = RGB(255, 255, 0);
	COLORREF clr;
	// Draw the background first
	pDC->FillSolidRect(&rcClient, Blue);
	// Create the brushes for drawing
	CBrush BlkBrush, RedBrush, YelBrush;
	// Create piece brushes
	BlkBrush.CreateSolidBrush(Black);
	RedBrush.CreateSolidBrush(Red);
	YelBrush.CreateSolidBrush(Yellow);
	
	// Draw the circles
	for (int row = 0; row < pDoc->GetRows(); row++)
	{
		for (int col = 0; col < pDoc->GetColumns(); col++)
		{
			// Get the colour for this board space
			clr = pDoc->GetBoardSpace(row, col);
			// Pick the brush colour based on the board information
			if (clr == Red)
				CBrush* pbrOld = pDC->SelectObject(&RedBrush);
			else if (clr == Yellow)
				CBrush* pbrOld = pDC->SelectObject(&YelBrush);
			else
				CBrush* pbrOld = pDC->SelectObject(&BlkBrush);
			// Calculate the size and position of this space
			CRect rcBlock;
			rcBlock.top = 2 * row*(pDoc->GetRadius()) + (row + 1) * pDoc->GetGap();
			rcBlock.left = 2 * col*(pDoc->GetRadius()) + (col + 1) * pDoc->GetGap();
			rcBlock.bottom = rcBlock.top + 2 * pDoc->GetRadius();
			rcBlock.right = rcBlock.left + 2 * pDoc->GetRadius();
			// Draw circle
			pDC->Ellipse(rcBlock);
		}
	}
	// If two player; draw turn indicator
	if (pDoc->GetAIDifficulty() == 0)
	{
		CRect rcBlock;
		GetParentFrame()->GetClientRect(&rcClient);
		rcBlock.top = rcClient.bottom - pDoc->GetGap();
		rcBlock.bottom = rcClient.bottom;
		rcBlock.left = rcClient.left;
		rcBlock.right = rcClient.right;
		if (pDoc->GetRedTurn())
			clr = Red;
		else clr = Yellow;
		pDC->FillSolidRect(&rcBlock, clr);
	}
	// Restore the device context settings
	pDC->RestoreDC(nDCSave);
	BlkBrush.DeleteObject();
	RedBrush.DeleteObject();
	YelBrush.DeleteObject();
}


// CConnect4View diagnostics

#ifdef _DEBUG
void CConnect4View::AssertValid() const
{
	CView::AssertValid();
}

void CConnect4View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CConnect4Doc* CConnect4View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CConnect4Doc)));
	return (CConnect4Doc*)m_pDocument;
}
#endif //_DEBUG


// CConnect4View message handlers


void CConnect4View::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	//Resize the window
	ResizeWindow();

}

void CConnect4View::ResizeWindow()
{
	// First get a pointer to the document
	CConnect4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// Get the size of the client area and the window
	CRect rcClient, rcWindow;
	GetClientRect(&rcClient);
	GetParentFrame()->GetWindowRect(&rcWindow);
	// Calculate the difference
	int nWidthDiff = rcWindow.Width() - rcClient.Width();
	int nHeightDiff = rcWindow.Height() - rcClient.Height();
	// Change the window size based on the size of the game board
	rcWindow.right = rcWindow.left +
		(2 * pDoc->GetRadius() + pDoc->GetGap()) * pDoc->GetColumns() + nWidthDiff + pDoc->GetGap();
	rcWindow.bottom = rcWindow.top +
		(2 * pDoc->GetRadius() + pDoc->GetGap()) * pDoc->GetRows() + nHeightDiff + 2 * pDoc->GetGap();
	// The MoveWindow function resizes the frame window
	GetParentFrame()->MoveWindow(&rcWindow);
}

void CConnect4View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// First get a pointer to the document
	CConnect4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// Then check the board isn't locked
	if (pDoc->GetBoardLocked())
		return;

	// Check the click happened in a circle
	// First check it didn't happen in a gap
	int top, bottom, left, right;
	for (int row=0; row <= pDoc->GetRows(); row++)
	{
		if (row == pDoc->GetRows())
			return;
		top = pDoc->GetGap() + row*(pDoc->GetGap() + 2 * pDoc->GetRadius());
		bottom = (row + 1)*(pDoc->GetGap() + 2 * pDoc->GetRadius());
		if ( top < point.y && point.y < bottom )
			break;
	}
	for (int col=0; col <= pDoc->GetColumns();col++)
	{
		if (col == pDoc->GetColumns())
			return;
		left = pDoc->GetGap() + col*(pDoc->GetGap() + 2 * pDoc->GetRadius());
		right = (col + 1)*(pDoc->GetGap() + 2 * pDoc->GetRadius());
		if (left < point.x && point.x < right)
			break;
	}
	// Now check it was inside the circle
	// Find centre of circle
	int x, y, rsquared;
	x = (left + right) / 2;
	y = (top + bottom) / 2;
	// If distance between click and centre of circle (r) greater than radius; return
	rsquared = (x - point.x)*(x - point.x) + (y - point.y)*(y - point.y);
	if (rsquared > pDoc->GetRadius()*pDoc->GetRadius())
		return;

	// Click location test
	/*
	CString message, msg1, msg2, msg3, msg4;
	CString msg5, msg6, msg7, msg8;
	msg1 = _T("Row and column picked: ");
	msg2.Format(_T("%d", row));
	msg3 = _T(", ");
	msg4.Format(_T("%d", col));
	msg5.Format(_T("\nrsquared: %d"), rsquared);
	msg6.Format(_T("\ny: %d"), y);
	message = msg1 + msg2 + msg3 + msg4 + msg5 + msg6;
	MessageBox(message, _T("Title"), MB_OK | MB_ICONINFORMATION);
	*/

	// Now check which circle was clicked on
	int row, col;
	row = (top - pDoc->GetGap()) / (2 * pDoc->GetRadius() + pDoc->GetGap());
	col = (left - pDoc->GetGap()) / (2 * pDoc->GetRadius() + pDoc->GetGap());
	// If space is occupied, then ignore the click
	if (pDoc->GetBoardSpace(row, col) != 0)
		return;

	// Then drop a counter into the column
	pDoc->PlaceCounter(col);
	
	// Next person's turn
	if (pDoc->GetRedTurn())
		pDoc->SetRedTurn(FALSE);
	else pDoc->SetRedTurn(TRUE);

	// Refresh window
	Invalidate();
	UpdateWindow();

	// If game is over, declare winner and delete board
	CString GameOver;
	if (pDoc->IsGameOver(col))
	{
		pDoc->SetBoardLocked(TRUE);
		if (pDoc->BoardFull())
		{
			GameOver.Format(_T("It's a tie!"));
			MessageBox(GameOver, _T("Game Over"), MB_OK | MB_ICONINFORMATION);
		}
		else if (pDoc->GetRedTurn())
		{
			GameOver.Format(_T("Yellow Wins!"));
			MessageBox(GameOver, _T("Game Over"), MB_OK | MB_ICONINFORMATION);
		}
		else
		{
			GameOver.Format(_T("Red Wins!"));
			MessageBox(GameOver, _T("Game Over"), MB_OK | MB_ICONINFORMATION);
		}
	}

	// If AI is switched on and it's their go, take the turn
	if (pDoc->GetAIDifficulty() != 0 && !pDoc->GetRedTurn() && !pDoc->GetBoardLocked())
	{
		if (pDoc->GetAIDifficulty() == 1)
			col = pDoc->AITurn1();
		else if (pDoc->GetAIDifficulty() == 2)
			col = pDoc->AITurn2();
		else if (pDoc->GetAIDifficulty() == 3)
			col = pDoc->AITurn2();
		// Force redraw
		Invalidate();
		UpdateWindow();
		// Set it to red's turn
		pDoc->SetRedTurn(TRUE);
		// Check if game is over
		if (pDoc->IsGameOver(col))
		{
			pDoc->SetBoardLocked(TRUE);
			if (pDoc->BoardFull())
			{
				GameOver.Format(_T("It's a tie!"));
				MessageBox(GameOver, _T("Game Over"), MB_OK | MB_ICONINFORMATION);
			}
			else if (pDoc->GetRedTurn())
			{
				GameOver.Format(_T("Yellow Wins!"));
				MessageBox(GameOver, _T("Game Over"), MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				GameOver.Format(_T("Red Wins!"));
				MessageBox(GameOver, _T("Game Over"), MB_OK | MB_ICONINFORMATION);
			}
		}
	}
	
	CView::OnLButtonDown(nFlags, point);
}


void CConnect4View::OnPlayersOneplayer()
{
	// First get a pointer to the document
	CConnect4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// If it isn't already, set AI difficulty to 2
	if (pDoc->GetAIDifficulty() != 2)
		pDoc->SetAIDifficulty(2);
	// Set it to red's turn
	pDoc->SetRedTurn(TRUE);
	// Refresh window
	Invalidate();
	UpdateWindow();
}


void CConnect4View::OnUpdatePlayersOneplayer(CCmdUI *pCmdUI)
{
	// First get a pointer to the document
	CConnect4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// Set check if the AI is activated
	pCmdUI->SetCheck(pDoc->GetAIDifficulty() != 0);
}


void CConnect4View::OnPlayersTwoplayer()
{
	// First get a pointer to the document
	CConnect4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// If AI is on, turn it off
	if (pDoc->GetAIDifficulty() > 0)
		pDoc->SetAIDifficulty(0);
	// Set it to red's turn
	pDoc->SetRedTurn(TRUE);
	// Refresh window
	Invalidate();
	UpdateWindow();
}


void CConnect4View::OnUpdatePlayersTwoplayer(CCmdUI *pCmdUI)
{
	// First get a pointer to the document
	CConnect4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// Set check if the AI isn't activated
	pCmdUI->SetCheck(pDoc->GetAIDifficulty() == 0);
}


void CConnect4View::OnAidifficultySupereasy()
{
	// First get a pointer to the document
	CConnect4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// If it isn't already, set difficulty to 1
	if (pDoc->GetAIDifficulty() != 1)
		pDoc->SetAIDifficulty(1);
	// Set it to red's turn
	pDoc->SetRedTurn(TRUE);
	// Refresh window
	Invalidate();
	UpdateWindow();
}


void CConnect4View::OnUpdateAidifficultySupereasy(CCmdUI *pCmdUI)
{
	// First get a pointer to the document
	CConnect4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// Set check if the AI difficulty is set to 1
	pCmdUI->SetCheck(pDoc->GetAIDifficulty() == 1);
	// Disable if AI is switched off
	if (pDoc->GetAIDifficulty() == 0)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}



void CConnect4View::OnAidifficultyEasy()
{
	// First get a pointer to the document
	CConnect4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// If it isn't already, set difficulty to 2
	if (pDoc->GetAIDifficulty() != 2)
		pDoc->SetAIDifficulty(2);
	// Set it to red's turn
	pDoc->SetRedTurn(TRUE);
	// Refresh window
	Invalidate();
	UpdateWindow();
}


void CConnect4View::OnUpdateAidifficultyEasy(CCmdUI *pCmdUI)
{
	// First get a pointer to the document
	CConnect4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// Set check if the AI difficulty is set to 2
	pCmdUI->SetCheck(pDoc->GetAIDifficulty() == 2);
	// Disable if AI is switched off
	if (pDoc->GetAIDifficulty() == 0)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}


void CConnect4View::OnAidifficultyMedium()
{
	// First get a pointer to the document
	CConnect4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// If it isn't already, set difficulty to 3
	if (pDoc->GetAIDifficulty() != 3)
		pDoc->SetAIDifficulty(3);
	// Set it to red's turn
	pDoc->SetRedTurn(TRUE);
	// Refresh window
	Invalidate();
	UpdateWindow();
}


void CConnect4View::OnUpdateAidifficultyMedium(CCmdUI *pCmdUI)
{
	// First get a pointer to the document
	CConnect4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// Set check if the AI difficulty is set to 3
	pCmdUI->SetCheck(pDoc->GetAIDifficulty() == 3);
	// Disable if AI is switched off
	if (pDoc->GetAIDifficulty() == 0)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}