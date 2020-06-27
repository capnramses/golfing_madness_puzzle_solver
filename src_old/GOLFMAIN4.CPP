// Anton's Brute-Force Puzzle Solver
// Designed to tackle the notorious 'Golf Jigsaw'
// 17/06/05

#include <stdio.h>

#define VERSION 2
#define ROWS 10
#define COLS 10
#define MAXBITS 10

void InitPieces();
void InitBoard();
void PrintBoard();
void CheckFit();
bool FindFree(int &row, int &col);
void AddPiece(char name,int row,int col);
void RemovePiece();
void ResetPiece();
void RotatePiece();

struct sPiece {
	char Name;
	int NumBits;
	int RelRow[MAXBITS];
	int RelCol[MAXBITS];
	
	int Rotation;
	int BitUsed;
	
	sPiece *Next;
};

int Move = 0;
int FreePieces = 0;
int UsedPieces = 0;
char Board[ROWS][COLS];
sPiece *Pieces = NULL;
sPiece *LastAdded = NULL;

int main() {
	printf("Antons Brute-Force Puzzle Solver v%i\n\n",VERSION);
	InitBoard();
	InitPieces();
	PrintBoard();
	
	while (FreePieces > 0) {
		getchar();
		int Row = -1;
		int Col = -1;
		FindFree(Row,Col);
		printf("Found Free at Row[%i],Col[%i]\n",Row,Col);
		if(Move == 0) AddPiece('A',Row,Col);
		if(Move == 1) AddPiece('B',Row,Col);
	
		Move++;
		PrintBoard();		
	}
}

bool FindFree(int &row, int &col) {
	for (int  i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			if (Board[i][j] == '.') {
				row = i;
				col = j;
				return true;
			}
		}
	}
	return false;
}

void AddPiece(char name,int row,int col) {
	// Look for piece in list
	sPiece *Ptr = Pieces;
	while (Ptr != NULL) {
		if (Ptr->Name == name) { break; }
		Ptr = Ptr->Next;
	}
// *** Need to subtract offset
	for (int i = 0; i < Ptr->NumBits; i++) {
		Board[row + Ptr->RelRow[i]][col + Ptr->RelCol[i]] = Ptr->Name;
	}
	UsedPieces++;
	FreePieces--;
}

void InitBoard() {
	for (int  i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			Board[i][j] = '.';
		}
	}
}

void PrintBoard() {
	printf("Board at Move %i\n",Move);
	for (int  i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			printf("%c ",Board[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

// Could replace with 'rotatepiece()'
void ResetPiece(char name) {
	// Look for piece in list
	sPiece *Ptr = Pieces;
	while (Ptr != NULL) {
		if (Ptr->Name == name) { break; }
		Ptr = Ptr->Next;
	}
	if (name == 'A') {
		Ptr->Rotation = 0;
		Ptr->BitUsed = 0;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
	} else if (name == 'B') {
		Ptr->Rotation = 0;
		Ptr->BitUsed = 0;
	
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		Ptr->RelRow[1] = 0;
		Ptr->RelCol[1] = +1;
		Ptr->RelRow[2] = +1;
		Ptr->RelCol[2] = +1;
	}
}

void InitPieces() {
	{
		sPiece *Ptr = new sPiece;
		
		Ptr->Name = 'B';
		Ptr->NumBits = 3;
		Ptr->Rotation = 0;
		Ptr->BitUsed = 0;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		Ptr->RelRow[1] = 0;
		Ptr->RelCol[1] = +1;
		Ptr->RelRow[2] = +1;
		Ptr->RelCol[2] = +1;
		
		Ptr->Next = Pieces;
		Pieces = Ptr;
		
		FreePieces++;
	}
	{
		sPiece *Ptr = new sPiece;
		
		Ptr->Name = 'A';
		Ptr->NumBits = 1;
		Ptr->Rotation = 0;
		Ptr->BitUsed = 0;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->Next = Pieces;
		Pieces = Ptr;
		
		FreePieces++;
	}
}

