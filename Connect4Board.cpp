#include "stdafx.h"
#include <cstring>
#include "Connect4Board.h"


CConnect4Board::CConnect4Board(void)
	: m_arrBoard(NULL),
	m_nColumns(7), m_nRows(6),
	m_nRadius(40), m_nGap(20),
	m_nDifficulty(0)
{
	m_arrColors[0] = RGB(0, 0, 0);
	m_arrColors[1] = RGB(64, 64, 192);
	m_arrColors[2] = RGB(255, 0, 0);
	m_arrColors[3] = RGB(255, 255, 0);
}


CConnect4Board::~CConnect4Board()
{
	// Simply delete the board
	DeleteBoard();
}

void CConnect4Board::SetupBoard(void)
{
	// Create the board if needed
	if (m_arrBoard == NULL)
		CreateBoard();
	// Set each circle to black
	for (int row = 0; row < m_nRows; row++)
		for (int col = 0; col < m_nColumns; col++)
			m_arrBoard[row][col] = 0;
	// Red goes first, then first player alternates
	if (!GetRedTurn())
		SetRedTurn(FALSE);
	else SetRedTurn(TRUE);
	// Unlock board
	SetBoardLocked(FALSE);

	// If AI is switched on and it's Yellow's turn, the AI takes the first turn
	if (GetAIDifficulty() != 0 && GetRedTurn() == FALSE)
	{
		int col;
		if (GetAIDifficulty() == 1)
			col = AITurn1();
		else if (GetAIDifficulty() == 2)
			col = AITurn2();
		else if (GetAIDifficulty() == 3)
			col = AITurn2();
		// Set it to red's turn
		SetRedTurn(TRUE);
	}
}

COLORREF CConnect4Board::GetBoardSpace(int row, int col)
{
	// Check the bounds of the array
	if (row < 0 || row >= m_nRows || col < 0 || col >= m_nColumns)
		return m_arrColors[0];
	return m_arrColors[m_arrBoard[row][col]];
}

void CConnect4Board::DeleteBoard(void)
{
	// Don't delete a NULL board
	if (m_arrBoard != NULL)
	{
		for (int row = 0; row < m_nRows; row++)
		{
			if (m_arrBoard[row] != NULL)
			{
				// Delete each row first
				delete[] m_arrBoard[row];
				m_arrBoard[row] = NULL;
			}
		}
		// Finally delete the array of rows
		delete[] m_arrBoard;
		m_arrBoard = NULL;
	}
}

void CConnect4Board::CreateBoard(void)
{
	// If there is already a board, delete it
	if (m_arrBoard != NULL)
		DeleteBoard();
	// Create the array of rows
	m_arrBoard = new int*[m_nRows];
	// Create each row
	for (int row = 0; row < m_nRows; row++)
	{
		m_arrBoard[row] = new int[m_nColumns];
		// Set each sqaure to be empty
		for (int col = 0; col < m_nColumns; col++)
			m_arrBoard[row][col] = 0;
	}
	SetBoardLocked(FALSE);
}

void CConnect4Board::PlaceCounter(int col)
{
	int row;
	// Find the lowest row available in the column
	for (row = m_nRows - 1; row >= 0; row--)
		if (m_arrBoard[row][col] == 0)
			break;

	// Find out who's turn it is and place their counter in the next available row
	if (GetRedTurn() == 1)
		m_arrBoard[row][col] = 2;
	else m_arrBoard[row][col] = 3;


	// Work out if the game is over
}

bool CConnect4Board::IsGameOver(int col)
{
	// First check if board is full
	if (BoardFull())
		return true;
	// Find highest occupied row (counter just played)
	int row = m_nRows - 1;
	for (row; row >= 0; row--)
		if (m_arrBoard[row][col] == 0)
			break;
	row += 1;
	// Check if there are four in a row from the counter just placed
	int Count = CheckForRow(row, col, m_arrBoard[row][col]);
	if (Count >= 4)
		return TRUE;
	else return FALSE;
}

bool CConnect4Board::BoardFull()
{
	for (int col = 0; col < m_nColumns; col++)
	{
		if (m_arrBoard[0][col] == 0)
			return FALSE;
	}
	return TRUE;
}

int CConnect4Board::CheckForRow(int row, int col, int clr)
{
	int TLBR = 1, TB = 1, BLTR = 1, LR = 1;

	// Improve efficiency
	if (row > 0 && col > 0)
		if (m_arrBoard[row - 1][col - 1] == clr)
			TLBR += CheckAlongRow(row - 1, col - 1, 0, UpLeft);
	if (row > 0)
		if (m_arrBoard[row - 1][col] == clr)
			TB += CheckAlongRow(row - 1, col, 0, Up);
	if (row > 0 && col < m_nColumns - 1)
		if (m_arrBoard[row - 1][col + 1] == clr)
			BLTR += CheckAlongRow(row - 1, col + 1, 0, UpRight);
	if (col > 0)
		if (m_arrBoard[row][col - 1] == clr)
			LR += CheckAlongRow(row, col - 1, 0, Left);
	if (col < m_nColumns - 1)
		if (m_arrBoard[row][col + 1] == clr)
			LR += CheckAlongRow(row, col + 1, 0, Right);
	if (row < m_nRows - 1 && col > 0)
		if (m_arrBoard[row + 1][col - 1] == clr)
			BLTR += CheckAlongRow(row + 1, col - 1, 0, DownLeft);
	if (row < m_nRows - 1)
		if (m_arrBoard[row + 1][col] == clr)
			TB += CheckAlongRow(row + 1, col, 0, Down);
	if (row < m_nRows - 1 && col < m_nColumns - 1)
		if (m_arrBoard[row + 1][col + 1] == clr)
			TLBR += CheckAlongRow(row + 1, col + 1, 0, DownRight);

	/*
	CString message, msg1, msg2, msg3, msg4;
	CString msg5, msg6, msg7, msg8;
	msg5.Format(_T("(R, C): %d"), row);
	msg6.Format(_T(", %d"), col);
	msg1.Format(_T("\nTLBR: %d"), TLBR);
	msg2.Format(_T("\nTB: %d"), TB);
	msg3.Format(_T("\nBLTR: %d"), BLTR);
	msg4.Format(_T("\nLR: %d"), LR);
	message = msg5 + msg6 + msg1 + msg2 + msg3 + msg4;
	MessageBox(NULL, message, _T("Rows"), MB_OK | MB_ICONINFORMATION);
	*/
	
	return max(max(TLBR, TB), max(BLTR, LR));
}

int CConnect4Board::CheckAlongRow(int row, int col, int count, Direction dir)
{
	// First add a unit to the count
	count += 1;

	// Then check if there are any more in the same direction
	if (dir == UpLeft && row > 0 && col > 0)
	{
		if (m_arrBoard[row - 1][col - 1] == m_arrBoard[row][col])
			return CheckAlongRow(row - 1, col - 1, count, dir);
	}
	else if (dir == Up && row > 0)
	{
		if (m_arrBoard[row - 1][col] == m_arrBoard[row][col])
			return CheckAlongRow(row - 1, col, count, dir);
	}
	else if (dir == UpRight && row > 0 && col < m_nColumns - 1)
	{
		if (m_arrBoard[row - 1][col + 1] == m_arrBoard[row][col])
			return CheckAlongRow(row - 1, col + 1, count, dir);
	}
	else if (dir == Left && col > 0)
	{
		if (m_arrBoard[row][col - 1] == m_arrBoard[row][col])
			return CheckAlongRow(row, col - 1, count, dir);
	}
	else if (dir == Right && col < m_nColumns - 1)
	{
		if (m_arrBoard[row][col + 1] == m_arrBoard[row][col])
			return CheckAlongRow(row, col + 1, count, dir);
	}
	else if (dir == DownLeft && row < m_nRows - 1 && col > 0)
	{
		if (m_arrBoard[row + 1][col - 1] == m_arrBoard[row][col])
			return CheckAlongRow(row + 1, col - 1, count, dir);
	}
	else if (dir == Down && row < m_nRows - 1)
	{
		if (m_arrBoard[row + 1][col] == m_arrBoard[row][col])
			return CheckAlongRow(row + 1, col, count, dir);
	}
	else if (dir == DownRight && row < m_nRows - 1 && col < m_nColumns - 1)
	{
		if (m_arrBoard[row + 1][col + 1] == m_arrBoard[row][col])
			return CheckAlongRow(row + 1, col + 1, count, dir);
	}
	return count;
}

int CConnect4Board::AITurn1(void)
{
	// Generate a random number from 0 to 6
	int col = rand() % 7;
	// Place a counter in that column
	PlaceCounter(col);
	// Check if game is over
	IsGameOver(col);
	return col;
}

int CConnect4Board::AITurn2(void)
{
	int col;
	int Red = 2, Yellow = 3;
	// Check that there are no moves to win the game, and no moves for the opponent to win the game
	//Cycle through columns
	int Wincol = 0, Blockcol1 = 0, Blockcol2 = 0, Randcol = 0;
	bool Wincolfound=FALSE, Blockcol1found = FALSE, Blockcol2found= FALSE;
	
	for (col = 0; col < m_nColumns; col++)
	{
		//Skip full columns
		if (m_arrBoard[0][col] == 0)
		{
			// Next available space in column
			int row = m_nRows - 1;
			for (row; row >= 0; row--)
				if (m_arrBoard[row][col] == 0)
					break;
			// Check if there are at least three yellows in a row from this spot to win
			if (CheckForRow(row, col, Yellow) > 3)
			{
				Wincolfound = TRUE;
				Wincol = col;
				break;
			}
			// Check if there are at least three reds in a row from this spot to prevent losing
			if (CheckForRow(row, col, Red) > 3)
			{
				Blockcol1found = TRUE;
				Blockcol1 = col;
			}
			// Check if there are two reds in a row from this point which have no boundaries, to prevent losing
			if (col < m_nColumns - 4)
			{
				// Check if there's a row of two reds to the right, with a space next to it which is
				// playable on the opponent's next turn
				if (CheckForRow(row, col, Red) > 2 && m_arrBoard[row][col + 3] == 0 )
				{
					if (row = m_nRows)
					{
						Blockcol2found = TRUE;
						Blockcol2 = col;
					}
					else if (m_arrBoard[row + 1][col + 3] != 0)
					{
						Blockcol2found = TRUE;
						Blockcol2 = col;
					}
				}
			}
		}
	}
	// If there are no smart moves, perform a random move that isn't in a full column, and doesn't
	// let the opponent win next turn
	bool Validcolfound = FALSE;
	int attempt = 0;
	while (!Validcolfound && !Wincolfound && !Blockcol1found && !Blockcol2found)
	{
		attempt += 1;
		// Select a random column
		Randcol = rand() % 7;
		// Find next available space in column
		int row = m_nRows - 1;
		for (row; row >= 0; row--)
			if (m_arrBoard[row][Randcol] == 0)
				break;
		// Find out if column is valid
		if (row > 0)
		{
			if (m_arrBoard[0][Randcol] == 0 && CheckForRow(row - 1, Randcol, Red) < 4)
				Validcolfound = TRUE;
		}
		else if (m_arrBoard[0][Randcol] == 0)
			Validcolfound = TRUE;
		else if (attempt = 100)
			break;
	}
	// If no valid column found, pick first available column
	if (!Validcolfound && !Wincolfound && !Blockcol1found && !Blockcol2found)
	{
		for (col = 0; col < m_nColumns; col++)
		{
			if (m_arrBoard[0][col] == 0)
			{
				Randcol = col;
				break;
			}
		}
	}

	// If a winner is found, place the counter in this column
	// Otherwise block the opponent's winning move or pick a random column
	if (Wincolfound)
	{
		PlaceCounter(Wincol);
		return Wincol;
	}
	else if (Blockcol1found)
	{
		PlaceCounter(Blockcol1);
		return Blockcol1;
	}
	else if (Blockcol2found)
	{
		PlaceCounter(Blockcol2);
		return Blockcol2;
	}
	else
	{
		PlaceCounter(Randcol);
		return Randcol;
	}

}

int CConnect4Board::AITurn3(void)
{
	// Generate a random number from 0 to 6
	int col = rand() % 7;
	// Place a counter in that column
	PlaceCounter(col);
	// Check if game if over
	IsGameOver(col);
	return col;
}
