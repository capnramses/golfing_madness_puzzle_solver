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
void FindFree();
void AddPiece();
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
	PrintBoard();






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

void InitPieces() {
	sPiece *Temp = new sPiece;
	
	Temp->Name = 'A';
	Temp->NumBits = 1;
	Temp->Rotation = 0;
	Temp->BitUsed = 0;
	
	Temp->RelRow[0] = 0;
	Temp->RelCol[0] = 0;
	
	Temp->Next = Pieces;
	Pieces = Temp;
	
	FreePieces++;
}

