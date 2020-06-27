// Anton's Brute-Force Puzzle Solver
// Designed to tackle the notorious 'Golf Jigsaw'
// 17/06/05

#include <stdio.h>

#define VERSION 5
#define ROWS 10
#define COLS 10
#define MAXBITS 10

void InitPieces();
void InitBoard();
void PrintBoard();
bool CheckFit(char name,int row,int col);
bool FindFree(int &row, int &col);
void AddPiece(char name,int row,int col);
void RemovePiece(char name);
void ResetPiece(char name);
void RotatePiece();
void DisplacePiece(char name,int bit);

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
	printf("Antons Brute-Force Puzzle Solver v%i\n",VERSION);
	InitBoard();
	InitPieces();
	PrintBoard();
	
	while (true) {
		getchar();
		int Row = -1;
		int Col = -1;
		FindFree(Row,Col);
		printf("Found Free at Row[%i],Col[%i]\n",Row,Col);
		if(Move == 0) {
			CheckFit('C',Row,Col);
		}
		if(Move == 1) {
			CheckFit('C',Row,Col);
		}
		if(Move == 2) {
			RemovePiece('A');
		}
		if(Move == 3) {
			CheckFit('B',Row,Col);
		}
		if(Move == 4) {
			RemovePiece('C');
		}
		if(Move == 5) {
			if (!CheckFit('C',Row,Col)) printf("C didnt fit\n");
		}
	
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

bool CheckFit(char name,int row,int col) {
	// Look for piece in list
	sPiece *Ptr = Pieces;
	while (Ptr != NULL) {
		if (Ptr->Name == name) { break; }
		Ptr = Ptr->Next;
	}
	
	bool Fit = true;
	for (int j = 0; j < Ptr->NumBits; j++) {
		if (!Fit) {
			if (j < Ptr->NumBits - 1) {
				DisplacePiece(name,j+1);
			}
			Fit = true;
		}	
		for (int i = 0; i < Ptr->NumBits; i++) {
			printf("Tryin ta put Bit %i on [%i][%i]\n",i,row + Ptr->RelRow[i],col + Ptr->RelCol[i]);
			if ((row + Ptr->RelRow[i] < 0) || (col + Ptr->RelCol[i] < 0)) {
				Fit = false;
			} else if (Board[row + Ptr->RelRow[i]][col + Ptr->RelCol[i]] != '.') {
				Fit = false;
			}
		}	
		if (Fit) {
			AddPiece(name,row,col);
			return true;
		}
	}
	
	
	
	
	return Fit;
}

void DisplacePiece(char name,int bit) {
	// Look for piece in list
	sPiece *Ptr = Pieces;
	while (Ptr != NULL) {
		if (Ptr->Name == name) { break; }
		Ptr = Ptr->Next;
	}
	
	int RowOffset = Ptr->RelRow[bit];
	int ColOffset = Ptr->RelCol[bit];
	
	for (int i = 0; i < Ptr->NumBits; i++) {
		Ptr->RelRow[i] -= RowOffset;
		Ptr->RelCol[i] -= ColOffset;	
	}
	
	Ptr->BitUsed = bit;
}

void RemovePiece(char name) {
	ResetPiece(name);
	for (int  i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			if (Board[i][j] == name) { Board[i][j] = '.'; }
		}
	}
	UsedPieces--;
	FreePieces++;
}

void AddPiece(char name,int row,int col) {
	// Look for piece in list
	sPiece *Ptr = Pieces;
	while (Ptr != NULL) {
		if (Ptr->Name == name) { break; }
		Ptr = Ptr->Next;
	}
	for (int i = 0; i < Ptr->NumBits; i++) {
		Board[row + Ptr->RelRow[i]][col + Ptr->RelCol[i]] = Ptr->Name;
	}
	UsedPieces++;
	FreePieces--;
	LastAdded = Ptr;
}

void InitBoard() {
	for (int  i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			Board[i][j] = '.';
		}
	}
}

void PrintBoard() {
	printf("\nBoard at Move %i:\n",Move);
	for (int  i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			printf("%c ",Board[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

// CLOCKWISE ROTATION
void RotatePiece(char name) {
	// Look for piece in list
	sPiece *Ptr = Pieces;
	while (Ptr != NULL) {
		if (Ptr->Name == name) { break; }
		Ptr = Ptr->Next;
	}
	
	int Rot = Ptr->Rotation++;
	Ptr->BitUsed = 0;
	
	if (name == 'B') {
		if (Rot == 1) {
			Ptr->RelRow[0] = 0;
			Ptr->RelCol[0] = 0;
			Ptr->RelRow[1] = +1;
			Ptr->RelCol[1] = 0;
			Ptr->RelRow[2] = +1;
			Ptr->RelCol[2] = -1;	
		} else if (Rot == 2) {
			Ptr->RelRow[0] = 0;
			Ptr->RelCol[0] = 0;
			Ptr->RelRow[1] = 0;
			Ptr->RelCol[1] = -1;
			Ptr->RelRow[2] = -1;
			Ptr->RelCol[2] = -1;
		} else if (Rot == 3) {
			Ptr->RelRow[0] = 0;
			Ptr->RelCol[0] = 0;
			Ptr->RelRow[1] = -1;
			Ptr->RelCol[1] = 0;
			Ptr->RelRow[2] = -1;
			Ptr->RelCol[2] = +1;
		}		
	} else if (name == 'C') {
		if (Rot == 1) {
			Ptr->RelRow[0] = 0;
			Ptr->RelCol[0] = 0;
			Ptr->RelRow[1] = +1;
			Ptr->RelCol[1] = 0;
			Ptr->RelRow[2] = +2;
			Ptr->RelCol[2] = 0;
			Ptr->RelRow[3] = +1;
			Ptr->RelCol[3] = +1;
		} else if (Rot == 2) {
			Ptr->RelRow[0] = 0;
			Ptr->RelCol[0] = 0;
			Ptr->RelRow[1] = 0;
			Ptr->RelCol[1] = -1;
			Ptr->RelRow[2] = 0;
			Ptr->RelCol[2] = -2;
			Ptr->RelRow[3] = +1;
			Ptr->RelCol[3] = -1;
		} else if (Rot == 3) {
			Ptr->RelRow[0] = 0;
			Ptr->RelCol[0] = 0;
			Ptr->RelRow[1] = -1;
			Ptr->RelCol[1] = 0;
			Ptr->RelRow[2] = -2;
			Ptr->RelCol[2] = 0;
			Ptr->RelRow[3] = -1;
			Ptr->RelCol[3] = -1;
		}
	}
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
	} else if (name == 'C') {
		Ptr->Rotation = 0;
		Ptr->BitUsed = 0;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		Ptr->RelRow[1] = 0;
		Ptr->RelCol[1] = +1;
		Ptr->RelRow[2] = 0;
		Ptr->RelCol[2] = +2;
		Ptr->RelRow[3] = -1;
		Ptr->RelCol[3] = +1;	
	}
}

void InitPieces() {
	{
		sPiece *Ptr = new sPiece;
		
		Ptr->Name = 'C';
		Ptr->NumBits = 4;
		Ptr->Rotation = 0;
		Ptr->BitUsed = 0;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		Ptr->RelRow[1] = 0;
		Ptr->RelCol[1] = +1;
		Ptr->RelRow[2] = 0;
		Ptr->RelCol[2] = +2;
		Ptr->RelRow[3] = -1;
		Ptr->RelCol[3] = +1;
		
		Ptr->Next = Pieces;
		Pieces = Ptr;
		
		FreePieces++;
	}
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

