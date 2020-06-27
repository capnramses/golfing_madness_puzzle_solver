// Anton's Brute-Force Puzzle Solver "HERCULES"
// Designed to tackle the notorious 'Golf Jigsaw'
// 17/06/05

/*
	NOTES:
	
	This program implements a BRUTAL and SHAMELESS algorthim for solving the Golf Jigsaw
	Finds one solution, does not investigate all cases
	
	BRUTAL because it uses an Exhaustive, recursive search to find a valid combination
	(I couldn't figure out any logical shortcuts or else I would'nt have needed this program)
	'MIGHT IS RIGHT'
	
	SHAMELESS because all of the piece data is hard-coded. All rotation transformations are also
	hard-coded (no sinus functions here). (This is also faster, but involves painful data-entry and
	drawing lots of little pictures).

	NEEDED FUTURE FEATURES:
	
	TODO:
	->Finish 'Tabs' management system (approx 1-2 hours)
		++Change tabs in RotatePiece()
		++Reset tabs in ResetPiece()
		+Check Tabs in NextFit() aswell as CheckFit()
		+Functions to find tabs int TabAbove(),int TabBelow()... (encompace border checkng aswell)
	
	->ACTUAL PIECES from game to encode	
	->ACUTAL SIZE of board (and tab information)	
		
	POSSIBLE OPTIMISATIONS:
	
	->Pointer passing (instead of name passing of structures) to remove extra traversal
	So everyehere a LETTER is used to record a piece, a Pointer to that piece is used instead.
	Letters will then ONLY be used in the printout. (by accessing pointeronthatsquare->name)
	
	->Removing sneaky hacks and sorting out recursive algorithm clearly
	
	->Could find ALL solutions by not returning from recursion if BranchFunc() returns true
		+Could keep recursive results in a stack and print/save them if returns true
		+And just pop off as recursion collapses and continue anyway (as if failed)
	->(Finding all solutions was NOT the original design plan, however)
	->I suspect that there is only one solution anyway and this would just be a time-consuming exercise
*/

#include <stdio.h>

#define VERSION 15
#define ROWS 7
#define COLS 7
#define MAXBITS 10

void InitPieces();
void InitBoard();
void PrintBoard();
bool CheckFit(char name,int row,int col);
bool NextFit(char name,int row,int col);
bool FindFree(int &row, int &col);
void AddPiece(char name,int row,int col);
void RemovePiece(char name);
void ResetPiece(char name);
void RotatePiece(char name);
void DisplacePiece(char name,int bit);
void GetLowestUnused(char &name);
bool BranchFunc();

struct sPiece {
	bool Used;
	char Name;
	int NumBits;
	int RelRow[MAXBITS];
	int RelCol[MAXBITS];
	int NorthTabs[MAXBITS];
	int WestTabs[MAXBITS];
	int SouthTabs[MAXBITS];
	int EastTabs[MAXBITS];	
	
	int Rotation;
	int BitUsed;
	
	sPiece *Next;
};

int Move = 0;
int FreePieces = 0;
int UsedPieces = 0;
char Board[ROWS][COLS];
int Top[COLS],Left[ROWS],Right[ROWS],Bottom[COLS];
sPiece *Pieces = NULL;
sPiece *LastAdded = NULL;

int main() {
	printf("Antons Brute-Force Puzzle Solver v%i\n",VERSION);
	InitBoard();
	InitPieces();
	PrintBoard();
	
	if (BranchFunc()) {
		printf("Solution FOUND by Anton's Brute-Force Puzzle Solver v%i\n",VERSION);
	} else {
		printf("Solution NOT Found.\n");
	}
}

bool BranchFunc() {
	printf("START\n");
	printf("Free Pieces = %i, Used Pieces = %i\n",FreePieces,UsedPieces);
	getchar();		// Ability to step through

	sPiece *Ptr = Pieces;
	while (Ptr != NULL) {
		if (Ptr->Used == true) {
			Ptr = Ptr->Next; continue;
		}
		int Row,Col;
		FindFree(Row,Col);
		if (CheckFit(Ptr->Name,Row,Col)) {
			printf("CHECKFIT\n");
			Move++;
			PrintBoard();
			if (BranchFunc()) { return true; }	
			
			// *** MODIFY PIECE PLACEMENT HERE **
			for (int  i = 0; i < ROWS; i++) {
				for (int j = 0; j < COLS; j++) {
					if (Board[i][j] == Ptr->Name) { Board[i][j] = '.'; }
				}
			}
			UsedPieces--;
			FreePieces++;
			Ptr->Used = false;
			Move++;
			printf("MODIFY\n");
			PrintBoard();
			printf("Free Pieces = %i, Used Pieces = %i\n",FreePieces,UsedPieces);
			if (NextFit(Ptr->Name,Row,Col)) {
				printf("NEXTFIT\n");
				Move++;
				PrintBoard();
				if (BranchFunc()) { return true; }
			} else {
			// To avoiud DOUBLE REMOVAL (sneaky hack)
				UsedPieces++;
				FreePieces--;
			}
			
			// *** Remove PIECE HERE ***	
			printf("REMOVE\n");
			RemovePiece(Ptr->Name);
		}
		Ptr = Ptr->Next;
	}
	if (FreePieces == 0) { return true; }
	return false;
}

bool NextFit(char name,int row,int col) {
	// Look for piece in list
	sPiece *Ptr = Pieces;
	while (Ptr != NULL) {
		if (Ptr->Name == name) { break; }
		Ptr = Ptr->Next;
	}
	
	bool Fit;
	// Start from CURRENT rotation
	for (int k = Ptr->Rotation; k < 4; k++) {
		Fit = false;
		// Start from NEXT DISPLACEMENT ON (?)
		for (int j = Ptr->BitUsed; j < Ptr->NumBits; j++) {
			if (!Fit) {
				if (j < Ptr->NumBits - 1) {DisplacePiece(name,j+1);}
				Fit = true;
			}
			// Check if all the bits fit into existing puzzle
//******** THIS IS WHERE ADD 'TAB' checking
			for (int i = 0; i < Ptr->NumBits; i++) {
				if ((row + Ptr->RelRow[i] < 0) || (col + Ptr->RelCol[i] < 0)) {
					Fit = false;
				} else if ((row + Ptr->RelRow[i] >= ROWS) || (col + Ptr->RelCol[i] >= COLS)) {
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
		// Rotate the piece 90 degrees and try again
		RotatePiece(name);
	}
	return false;
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
	
	bool Fit;
	// Go through possible rotations
	for (int k = 0; k < 4; k++) {	
		Fit = true;
		// Go through possible displacements
		for (int j = 0; j < Ptr->NumBits; j++) {
			if (!Fit) {
				if (j < Ptr->NumBits - 1) {DisplacePiece(name,j+1);}
				Fit = true;
			}
			// Check if all the bits fit into existing puzzle
				// FreeSquare checking
			for (int i = 0; i < Ptr->NumBits; i++) {
				if ((row + Ptr->RelRow[i] < 0) || (col + Ptr->RelCol[i] < 0)) {
					Fit = false;
				} else if ((row + Ptr->RelRow[i] >= ROWS) || (col + Ptr->RelCol[i] >= COLS)) {
					Fit = false;	
				} else if (Board[row + Ptr->RelRow[i]][col + Ptr->RelCol[i]] != '.') {
					Fit = false;
				}
				
				// ** NEED TO MODIFY SOMETHING FOR ROTATION
				// Check if lines up with edges of board tabs
				if (row + Ptr->RelRow[i] == 0) {
					if (Top[col + Ptr->RelCol[i]] == Ptr->NorthTabs[i]) {
						Fit = false;
					}
				} else {
					// Check square above
					// Is it free?
					if (Board[row + Ptr->RelRow[i] - 1][col + Ptr->RelCol[i]] != '.') {
						// Check for compatability
							// Find square
							// Find the bit in question
								// Check against row/col
							// Check tabs
					}				
				}
				if (row + Ptr->RelRow[i] == ROWS - 1) {
					if (Bottom[col + Ptr->RelCol[i]] == Ptr->SouthTabs[i]) {
						Fit = false;
					}
				} else {
				
				
				}
				if (col + Ptr->RelCol[i] == 0) {
					if (Left[row + Ptr->RelRow[i]] == Ptr->WestTabs[i]) {
						Fit = false;
					}
				} else {
				
				
				
				}
				if (col + Ptr->RelCol[i] == COLS - 1) {
					if (Right[row + Ptr->RelRow[i]] == Ptr->EastTabs[i]) {
						Fit = false;
					}
				} else {
				
				
				
				}
			}
			
			if (Fit) {
				AddPiece(name,row,col);
/// ** Take out 'reset' later ->it's only for reusing pieces
				//ResetPiece(name);
				return true;
			}
		}
		// Rotate the piece 90 degrees and try again
		RotatePiece(name);
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
	Ptr->Used = true;
	UsedPieces++;
	FreePieces--;
	LastAdded = Ptr;
}

void InitBoard() {
	// Set Board Squares to Empty
	for (int  i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			Board[i][j] = '.';
		}
	}
	// Setup the Sides of the Board (hardcoded)
	// 0 = Inney, 1 = Outey, -1 = N/A
	Top[0] = 0;Top[1] = 1;Top[2] = 0;Top[3] = 1;Top[4] = 0;Top[5] = 1;Top[6] = 0;Top[7] = 1;
	Left[0] = 0;Left[1] = 1;Left[2] = 0;Left[3] = 1;Left[4] = 0;Left[5] = 1;Left[6] = 0;Left[7] = 1;
	Right[0] = 1;Right[1] = 0;Right[2] = 1;Right[3] = 0;Right[4] = 1;Right[5] = 0;Right[6] = 1;Right[7] = 0;
	Bottom[0] = 1;Bottom[1] = 0;Bottom[2] = 1;Bottom[3] = 0;Bottom[4] = 1;Bottom[5] = 0;Bottom[6] = 0;Bottom[7] = 0;
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
	
	int Rot = ++Ptr->Rotation;
	Ptr->BitUsed = 0;
	
	// Change Tabs around
	for (int i = 0; i < Ptr->NumBits; i++) {
		// Copy North Tab
		int NorthBak = Ptr->NorthTabs[i];
		Ptr->NorthTabs[i] = Ptr->WestTabs[i];
		Ptr->WestTabs[i] = Ptr->SouthTabs[i];
		Ptr->SouthTabs[i] = Ptr->EastTabs[i];
		Ptr->EastTabs[i] = NorthBak;
	}
	
	// Change bit positions around
	if (name == 'A') {
		if (Rot == 1) {
			Ptr->RelRow[0] = 0;
			Ptr->RelCol[0] = 0;
			Ptr->RelRow[1] = +1;
			Ptr->RelCol[1] = 0;
			Ptr->RelRow[2] = +2;
			Ptr->RelCol[2] = 0;
			Ptr->RelRow[3] = +2;
			Ptr->RelCol[3] = -1;	
		} else if (Rot == 2) {
			Ptr->RelRow[0] = 0;
			Ptr->RelCol[0] = 0;
			Ptr->RelRow[1] = 0;
			Ptr->RelCol[1] = -1;
			Ptr->RelRow[2] = 0;
			Ptr->RelCol[2] = -2;
			Ptr->RelRow[3] = -1;
			Ptr->RelCol[3] = -2;	
		} else if (Rot == 3) {
			Ptr->RelRow[0] = 0;
			Ptr->RelCol[0] = 0;
			Ptr->RelRow[1] = +1;
			Ptr->RelCol[1] = 0;
			Ptr->RelRow[2] = +2;
			Ptr->RelCol[2] = 0;
			Ptr->RelRow[3] = +2;
			Ptr->RelCol[3] = +1;
		}		
	} else if (name == 'B') {
		if (Rot == 1) {
			Ptr->RelRow[0] = 0;
			Ptr->RelCol[0] = 0;
			Ptr->RelRow[1] = 0;
			Ptr->RelCol[1] = -1;
			Ptr->RelRow[2] = -1;
			Ptr->RelCol[2] = -1;
		} else if (Rot == 2) {
			Ptr->RelRow[0] = 0;
			Ptr->RelCol[0] = 0;
			Ptr->RelRow[1] = -1;
			Ptr->RelCol[1] = 0;
			Ptr->RelRow[2] = -1;
			Ptr->RelCol[2] = +1;
		} else if (Rot == 3) {
			Ptr->RelRow[0] = 0;
			Ptr->RelCol[0] = 0;
			Ptr->RelRow[1] = 0;
			Ptr->RelCol[1] = +1;
			Ptr->RelRow[2] = +1;
			Ptr->RelCol[2] = 1;
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
		Ptr->Used = false;
		
		Ptr->NorthTabs[0] = 0;
		Ptr->WestTabs[0] = 0;
		Ptr->EastTabs[0] = -1;
		Ptr->SouthTabs[0] = 0;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = 1;
		Ptr->WestTabs[1] = -1;
		Ptr->EastTabs[1] = -1;
		Ptr->SouthTabs[1] = 1;
		
		Ptr->RelRow[1] = 0;
		Ptr->RelCol[1] = 1;
		
		Ptr->NorthTabs[2] = 0;
		Ptr->WestTabs[2] = -1;
		Ptr->EastTabs[2] = 0;
		Ptr->SouthTabs[2] = -1;
		
		Ptr->RelRow[2] = 0;
		Ptr->RelCol[2] = 2;
		
		Ptr->NorthTabs[3] = -1;
		Ptr->WestTabs[3] = 1;
		Ptr->EastTabs[3] = 1;
		Ptr->SouthTabs[3] = 1;
		
		Ptr->RelRow[3] = 1;
		Ptr->RelCol[3] = 2;
	} else if (name == 'B') {
		Ptr->Rotation = 0;
		Ptr->BitUsed = 0;
		Ptr->Used = false;
		
		Ptr->NorthTabs[0] = 1;
		Ptr->WestTabs[0] = 1;
		Ptr->EastTabs[0] = 1;
		Ptr->SouthTabs[0] = -1;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = -1;
		Ptr->WestTabs[1] = -1;
		Ptr->EastTabs[1] = 1;
		Ptr->SouthTabs[1] = 0;
		
		Ptr->RelRow[1] = 1;
		Ptr->RelCol[1] = 0;
		
		Ptr->NorthTabs[2] = 1;
		Ptr->WestTabs[2] = 1;
		Ptr->EastTabs[2] = -1;
		Ptr->SouthTabs[2] = 1;
		
		Ptr->RelRow[2] = 1;
		Ptr->RelCol[2] = -1;
	}
}

void InitPieces() {
	{
		sPiece *Ptr = new sPiece;
		
		/*
			A A A
			    A		
		*/
		
		Ptr->Name = 'A';
		Ptr->NumBits = 4;
		Ptr->Rotation = 0;
		Ptr->BitUsed = 0;
		Ptr->Used = false;
		
		Ptr->NorthTabs[0] = 0;
		Ptr->WestTabs[0] = 0;
		Ptr->EastTabs[0] = -1;
		Ptr->SouthTabs[0] = 0;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = 1;
		Ptr->WestTabs[1] = -1;
		Ptr->EastTabs[1] = -1;
		Ptr->SouthTabs[1] = 1;
		
		Ptr->RelRow[1] = 0;
		Ptr->RelCol[1] = 1;
		
		Ptr->NorthTabs[2] = 0;
		Ptr->WestTabs[2] = -1;
		Ptr->EastTabs[2] = 0;
		Ptr->SouthTabs[2] = -1;
		
		Ptr->RelRow[2] = 0;
		Ptr->RelCol[2] = 2;
		
		Ptr->NorthTabs[3] = -1;
		Ptr->WestTabs[3] = 1;
		Ptr->EastTabs[3] = 1;
		Ptr->SouthTabs[3] = 1;
		
		Ptr->RelRow[3] = 1;
		Ptr->RelCol[3] = 2;
		
		Ptr->Next = Pieces;
		Pieces = Ptr;
		
		FreePieces++;
	}
	{
		sPiece *Ptr = new sPiece;
		
		/*
			  B
			B B		
		*/
		
		Ptr->Name = 'B';
		Ptr->NumBits = 3;
		Ptr->Rotation = 0;
		Ptr->BitUsed = 0;
		Ptr->Used = false;
		
		Ptr->NorthTabs[0] = 1;
		Ptr->WestTabs[0] = 1;
		Ptr->EastTabs[0] = 1;
		Ptr->SouthTabs[0] = -1;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = -1;
		Ptr->WestTabs[1] = -1;
		Ptr->EastTabs[1] = 1;
		Ptr->SouthTabs[1] = 0;
		
		Ptr->RelRow[1] = 1;
		Ptr->RelCol[1] = 0;
		
		Ptr->NorthTabs[2] = 1;
		Ptr->WestTabs[2] = 1;
		Ptr->EastTabs[2] = -1;
		Ptr->SouthTabs[2] = 1;
		
		Ptr->RelRow[2] = 1;
		Ptr->RelCol[2] = -1;
		
		Ptr->Next = Pieces;
		Pieces = Ptr;
		
		FreePieces++;
	}
}

