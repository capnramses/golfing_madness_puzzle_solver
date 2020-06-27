// Anton's Brute-Force Puzzle Solver "HERCULES"
// Designed to tackle the notorious 'Golf Jigsaw'
// 17/06/05

/*
	NOTES:
	
	This program implements a BRUTAL algorthim for solving the Golf Jigsaw
	Finds one solution, does not investigate all cases
	
	BRUTAL because it uses an Exhaustive, recursive search to find a valid combination
	(I couldn't figure out any logical shortcuts or else I would'nt have needed this program)
	'MIGHT IS RIGHT'

	NEEDED FUTURE FEATURES:
	
	TODO:
	+horrible incremental checking and testing
	+check all pieces and tabs
		
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
#include <stdlib.h>

#define VERSION 29
#define ROWS 8
#define COLS 8
#define MAXBITS 10

#define DEBUG
#define NODEBUG2
#define NODEBUGMORE

// Structure defines each piece to use on board
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
	
	int RPos;
	int CPos;
	
	sPiece *Next;
};

void InitPieces();
void InitBoard();
void PrintBoard();
bool CheckFit(sPiece *Ptr,int row,int col);
bool FindFree(int &row, int &col);
void AddPiece(sPiece *Ptr,int row,int col);
void RemovePiece(sPiece *Ptr);
void ResetPiece(sPiece *Ptr);
void RotatePiece(sPiece *Ptr);
void DisplacePiece(sPiece *Ptr,int bit);
void GetLowestUnused(char &name);

// Global (tracking) variables
int Move = 0;
int FreePieces = 0;
int UsedPieces = 0;
char Board[ROWS][COLS];
int Top[COLS],Left[ROWS],Right[ROWS],Bottom[COLS];
sPiece *Pieces = NULL;
sPiece *LastAdded = NULL;

// Main function ->launches recursion and prints summary
int main() {
	printf("Antons Brute-Force Puzzle Solver v%i\n",VERSION);
	InitBoard();
	InitPieces();
	#ifdef DEBUG
	PrintBoard();
	getchar();		// Ability to step through
	#endif
	
	if (CheckFit(Pieces,0,0)) {
		printf("Solution FOUND by Anton's Brute-Force Puzzle Solver v%i\n",VERSION);
		PrintBoard();
		printf("A = Bluehat\n");
		printf("B = Purple Pants\n");
		printf("C = 70s Lady and Green Hat\n");
		printf("D = Morange in Tweed\n");
		printf("E = Fanta Pants\n");
		printf("F = Morange Socks\n");
		printf("G = BumFlag\n");
		printf("H = Yellow Plus Fours\n");
		printf("I = Purple Tiger\n");
		printf("J = Yellow Pants\n");
		printf("K = Many Balls\n");
		printf("L = French Shirt\n");
		printf("M = Gimp in red pants\n");
		printf("N = ZigZag\n");
	} else {
		printf("Solution NOT Found in %i Moves.\n",Move);
	}
}

// Function to check if a bit is over a free square
bool Sub1(sPiece *Ptr,int row, int col, int i) {
	if ((row + Ptr->RelRow[i] < 0) || (col + Ptr->RelCol[i] < 0)) {
		#ifdef DEBUGMORE
		printf("NOFIT due to bit overlapping board\n");
		#endif
					
		return false;
	}
	if ((row + Ptr->RelRow[i] >= ROWS) || (col + Ptr->RelCol[i] >= COLS)) {
		#ifdef DEBUGMORE
		printf("NOFIT due to bit overlapping board\n");
		#endif
					
		return false;
	}
	if (Board[row + Ptr->RelRow[i]][col + Ptr->RelCol[i]] != '.') {
		#ifdef DEBUGMORE
		printf("NOFIT due to bit on existing piece\n");
		#endif
					
		return false;
	}
	return true;
}

// Function to check Tabs vs. Borders
bool Sub2(sPiece *Ptr,int row, int col, int i) {
	if (row + Ptr->RelRow[i] == 0) {
		if (Top[col + Ptr->RelCol[i]] == Ptr->NorthTabs[i]) { 
			#ifdef DEBUGMORE
			printf(" TAB FAILED at Board Top\n");
			#endif
							
			return false;
		}
	}
	if (row + Ptr->RelRow[i] == ROWS - 1) {
		if (Bottom[col + Ptr->RelCol[i]] == Ptr->SouthTabs[i]) {
			#ifdef DEBUGMORE
			printf(" TAB FAILED at Board Bottom\n");
			#endif
							
			return false;
		}
	}
	if (col + Ptr->RelCol[i] == 0) {
		if (Left[row + Ptr->RelRow[i]] == Ptr->WestTabs[i]) {
			#ifdef DEBUGMORE
			printf(" TAB FAILED at Board Left\n");
			#endif
							
			return false;
		}
	}
	if (col + Ptr->RelCol[i] == COLS - 1) {
		if (Right[row + Ptr->RelRow[i]] == Ptr->EastTabs[i]) {
			#ifdef DEBUGMORE
			printf(" TAB FAILED at Board Right\n");
			#endif
							
			return false;
		}
	}
	return true;
}

// Function to check vs. a Neighboring Pieces' Tabs
bool Sub3(sPiece *Ptr,int row, int col, int i) {
	#ifdef DEBUGMORE
	printf(" Checking tab ABOVE (%i)\n",Ptr->NorthTabs[i]);
	#endif

	if ((Ptr->NorthTabs[i] != -1) && (row + Ptr->RelRow[i] > 0)) {
		#ifdef DEBUGMORE
		printf(" -MADE IT\n");
		#endif
		// Find out which piece occupies above square
		int TRow = row + Ptr->RelRow[i] - 1;
		int TCol = col + Ptr->RelCol[i];
		char Letter = Board[TRow][TCol];
			
		if (Letter != '.') {
			// Find Piece
			#ifdef DEBUGMORE
			printf(" Looking for Piece...\n");
			#endif	
			sPiece *T = Pieces;
			while (T != NULL) {
				if (T->Name == Letter) { break; }
				T = T->Next;
			}
			#ifdef DEBUGMORE
			printf(" ...Found %c\n",T->Name);
			#endif	
				
			// Find bit of piece
			for (int j = 0; j < T->NumBits; j++) {
				if ((T->RelRow[j] + T->RPos == TRow) && (T->RelCol[j] + T->CPos == TCol)) {
					// Compare Tabs
					#ifdef DEBUGMORE
					printf(" It is %c bit %i with (%i)\n",T->Name,j,T->SouthTabs[j]);
					#endif	
					
					if (T->SouthTabs[j] == Ptr->NorthTabs[i]) {
						#ifdef DEBUGMORE
						printf(" FAILED Check\n");
						#endif
						
						return false;
					}
				}
			}
			
		} else {
			#ifdef DEBUGMORE
			printf(" It is Empty\n");
			#endif	
		}
	} else {
		#ifdef DEBUGMORE
		printf(" ON EDGE\n");
		#endif	
	}
	#ifdef DEBUGMORE
	printf(" Checking tab BELOW (%i)\n",Ptr->SouthTabs[i]);
	#endif					

	if ((Ptr->SouthTabs[i] != -1) && (row + Ptr->RelRow[i] < ROWS - 1)) {
		#ifdef DEBUGMORE
		printf(" -MADE IT\n");
		#endif
		// Find out which piece occupies below square
		int TRow = row + Ptr->RelRow[i] + 1;
		int TCol = col + Ptr->RelCol[i];
		char Letter = Board[TRow][TCol];
			
		if (Letter != '.') {
			// Find Piece
			#ifdef DEBUGMORE
			printf(" Looking for Piece...\n");
			#endif	
			sPiece *T = Pieces;
			while (T != NULL) {
				if (T->Name == Letter) { break; }
				T = T->Next;
			}
			#ifdef DEBUGMORE
			printf(" ...Found %c\n",T->Name);
			#endif	
				
			// Find bit of piece
			for (int j = 0; j < T->NumBits; j++) {
				if ((T->RelRow[j] + T->RPos == TRow) && (T->RelCol[j] + T->CPos == TCol)) {
					#ifdef DEBUGMORE
					printf(" It is %c bit %i with (%i)\n",T->Name,j,T->NorthTabs[j]);
					#endif	
					
					// Compare Tabs
					if (T->NorthTabs[j] == Ptr->SouthTabs[i]) {
						#ifdef DEBUGMORE
						printf(" FAILED Check\n");
						#endif
						
						return false;
					}
				}
			}
		} else {
			#ifdef DEBUGMORE
			printf(" It is Empty\n");
			#endif	
		}						
	} else {
		#ifdef DEBUGMORE
		printf(" ON EDGE\n");
		#endif	
	}
	#ifdef DEBUGMORE
	printf(" Checking tab LEFT of (%i)\n",Ptr->WestTabs[i]);
	#endif
	
	if ((Ptr->WestTabs[i] != -1) && (col + Ptr->RelCol[i] > 0)) {
		#ifdef DEBUGMORE
		printf(" -MADE IT\n");
		#endif
		// Find out which piece occupies below square
		int TRow = row + Ptr->RelRow[i];
		int TCol = col + Ptr->RelCol[i] - 1;
		char Letter = Board[TRow][TCol];
			
		if (Letter != '.') {
			// Find Piece
			#ifdef DEBUGMORE
			printf(" Looking for Piece...\n");
			#endif	
			sPiece *T = Pieces;
			while (T != NULL) {
				if (T->Name == Letter) { break; }
				T = T->Next;
			}
			#ifdef DEBUGMORE
			printf(" ...Found %c\n",T->Name);
			#endif	
			
			// Find bit of piece
			for (int j = 0; j < T->NumBits; j++) {
				if ((T->RelRow[j] + T->RPos == TRow) && (T->RelCol[j] + T->CPos == TCol)) {
					#ifdef DEBUGMORE
					printf(" It is %c bit %i with (%i)\n",T->Name,j,T->EastTabs[j]);
					#endif	
					// Compare Tabs
					if (T->EastTabs[j] == Ptr->WestTabs[i])  {
						#ifdef DEBUGMORE
						printf(" FAILED Check\n");
						#endif
						
						return false;
					}
				}
			}
		} else {
			#ifdef DEBUGMORE
			printf(" It is Empty\n");
			#endif	
		}						
	} else {
		#ifdef DEBUGMORE
		printf(" ON EDGE\n");
		#endif	
	}
	#ifdef DEBUGMORE
	printf(" Checking tab RIGHT of (%i)\n",Ptr->EastTabs[i]);
	#endif

	if ((Ptr->EastTabs[i] != -1) && (col + Ptr->RelCol[i] < COLS - 1)) {
		#ifdef DEBUGMORE
		printf(" -MADE IT\n");
		#endif
		// Find out which piece occupies below square
		int TRow = row + Ptr->RelRow[i];
		int TCol = col + Ptr->RelCol[i] + 1;
		char Letter = Board[TRow][TCol];
			
		if (Letter != '.') {
			// Find Piece
			#ifdef DEBUGMORE
			printf(" Looking for Piece...\n");
			#endif	
			sPiece *T = Pieces;
			while (T != NULL) {
				if (T->Name == Letter) { break; }
				T = T->Next;
			}
			#ifdef DEBUGMORE
			printf(" ...Found %c\n",T->Name);
			#endif	
				
			// Find bit of piece
			for (int j = 0; j < T->NumBits; j++) {
				if ((T->RelRow[j] + T->RPos == TRow) && (T->RelCol[j] + T->CPos == TCol)) {
					// Compare Tabs
					#ifdef DEBUGMORE
					printf(" It is %c bit %i with (%i)\n",T->Name,j,T->WestTabs[j]);
					#endif	
					
					if (T->WestTabs[j] == Ptr->EastTabs[i])  {
						#ifdef DEBUGMORE
						printf(" FAILED Check\n");
						#endif
						return false;
					}
				}
			}
		} else {
			#ifdef DEBUGMORE
			printf(" It is Empty\n");
			#endif	
		}						
	} else {
		#ifdef DEBUGMORE
		printf(" ON EDGE\n");
		#endif	
	}
	return true;
}

// Function to Try and fit a piece onto a square
bool CheckFit(sPiece *Ptr,int row,int col) {	
	bool Fit;
	#ifdef DEBUG2
	printf("Rotation of %c is %i\n",Ptr->Name,Ptr->Rotation);
	#endif
	
	// Go through possible rotations
	for (int k = 0; k < 4; k++) {	
		#ifdef DEBUGMORE
		printf("ROTATION of %c = %i\n",Ptr->Name,k);
		#endif
		
		// Go through possible displacements
		for (int j = 0; j < Ptr->NumBits; j++) {
			#ifdef DEBUG2
			printf(" Displacing Piece %c to %i\n",Ptr->Name,j);
			#endif
			
			DisplacePiece(Ptr,j);			
			Fit = true;
			// Check if all the bits fit into existing puzzle
			for (int i = 0; i < Ptr->NumBits; i++) {
				#ifdef DEBUGMORE
				printf(" Checking bit %i Offset [%i][%i]\n",i,Ptr->RelRow[i],Ptr->RelCol[i]);
				#endif
				
				if (Sub1(Ptr,row,col,i) == false) {
					Fit = false;
					break;
				} else if (Sub2(Ptr,row,col,i) == false) {
					Fit = false;
					break;
				} else if (Sub3(Ptr,row,col,i) == false) {
					Fit = false;
					break;
				}			
			}
			if (Fit) {
				// Add the piece
				AddPiece(Ptr,row,col);
				Move++;
				#ifdef DEBUG
				PrintBoard();
				getchar();
				#endif
				
				// See if we've finished puzzle
				if (FreePieces == 0) { return true; }
				
				// Find Next Free Square
				int NextRow,NextCol;
				FindFree(NextRow,NextCol);
				
				#ifdef DEBUG2
				printf("Next Free Square is [%i][%i]\n",NextRow,NextCol);
				#endif

				// Find next free piece
				sPiece *NextPiece = Pieces;
				while (NextPiece != NULL) {
					// Skip Used Pieces
					if (NextPiece->Used == true) {
						NextPiece = NextPiece->Next;
						continue;
					}
					
					#ifdef DEBUG2
					printf("Next Piece is %c\n",NextPiece->Name);
					#endif
					
					// Try to fit that
					if (CheckFit(NextPiece,NextRow,NextCol)) { return true; }
					
					NextPiece = NextPiece->Next;
				}
				
				#ifdef DEBUG2
				printf("FAILED TO FIT PIECES, CONTINUEING\n");
				#endif
				
				// Else continue
				RemovePiece(Ptr);
				Move++;
				#ifdef DEBUG
				PrintBoard();
				getchar();
				#endif
			}
		}
		// Rotate the piece 90 degrees and try again
		RotatePiece(Ptr);
		#ifdef DEBUG2
		printf("Rotating %c to %i and trying again\n",Ptr->Name,Ptr->Rotation);
		#endif
	}
	// Give up on piece because we've tried everything
	ResetPiece(Ptr);
	return false;
}

// Function to find the 'next' free square on the board
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

// Procedure to Move a piece around
void DisplacePiece(sPiece *Ptr,int bit) {	
	int RowOffset = Ptr->RelRow[bit];
	int ColOffset = Ptr->RelCol[bit];
	
	for (int i = 0; i < Ptr->NumBits; i++) {
		Ptr->RelRow[i] -= RowOffset;
		Ptr->RelCol[i] -= ColOffset;	
	}
	
	Ptr->BitUsed = bit;
}

// Procedure to remove a piece from the board and reset it to default state
void RemovePiece(sPiece *Ptr) {
	//ResetPiece(Ptr);
	for (int  i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			if (Board[i][j] == Ptr->Name) { Board[i][j] = '.'; }
		}
	}
	UsedPieces--;
	FreePieces++;
}

// Procedure to Insert a Piece into the board
void AddPiece(sPiece *Ptr,int row,int col) {	
	for (int i = 0; i < Ptr->NumBits; i++) {
		Board[row + Ptr->RelRow[i]][col + Ptr->RelCol[i]] = Ptr->Name;
	}
	Ptr->Used = true;
	Ptr->RPos = row;
	Ptr->CPos = col;
	UsedPieces++;
	FreePieces--;
	LastAdded = Ptr;
}

// Procedure to setup an empty board, and define the borders
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

// Procedure to print out the board at current state
void PrintBoard() {
//	system("clear");
	printf("\nBoard at Move %i:\n",Move);
	printf("Free Pieces = %i, Used Pieces = %i\n",FreePieces,UsedPieces);
	for (int  i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			printf("%c ",Board[i][j]);
		}
		printf("\n");
	}
	printf("\n");	
}

// Procedure to rotate a piece CLOCKWISE by 90 degrees
void RotatePiece(sPiece *Ptr) {	
	int Rot = ++Ptr->Rotation;	// increase rotation state for piece
	Ptr->BitUsed = 0;		// start using first bit
	
	// Move bits in piece
	for (int i = 0; i < Ptr->NumBits; i++) {
		// Change Tabs for each bit around
		int NorthBak = Ptr->NorthTabs[i];	// Copy North Tab
		Ptr->NorthTabs[i] = Ptr->WestTabs[i];
		Ptr->WestTabs[i] = Ptr->SouthTabs[i];
		Ptr->SouthTabs[i] = Ptr->EastTabs[i];
		Ptr->EastTabs[i] = NorthBak;
		
		// Change positions for each bit around
		int Row = Ptr->RelRow[i];
		int Col = Ptr->RelCol[i];
		Ptr->RelRow[i] = Col;		
		Ptr->RelCol[i] = Row * -1;
	}
}

// Procedure to setup a piece with its default values
void ResetPiece(sPiece *Ptr) {	
	char name = Ptr->Name;
	// Common default data
	Ptr->RPos = -1;
	Ptr->CPos = -1;
	Ptr->Rotation = 0;
	Ptr->BitUsed = 0;
	Ptr->Used = false;
	
	// Setup default data specific to that piece
	if (name == 'A') {		
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
		Ptr->NorthTabs[0] = 1;
		Ptr->WestTabs[0] = 1;
		Ptr->EastTabs[0] = -1;
		Ptr->SouthTabs[0] = 1;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = -1;
		Ptr->WestTabs[1] = -1;
		Ptr->EastTabs[1] = 1;
		Ptr->SouthTabs[1] = 0;
		
		Ptr->RelRow[1] = 0;
		Ptr->RelCol[1] = 1;
		
		Ptr->NorthTabs[2] = 1;
		Ptr->WestTabs[2] = 1;
		Ptr->EastTabs[2] = 1;
		Ptr->SouthTabs[2] = -1;
		
		Ptr->RelRow[2] = -1;
		Ptr->RelCol[2] = 1;
	} else if (name == 'C') {		
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
		Ptr->EastTabs[2] = -1;
		Ptr->SouthTabs[2] = 0;
		
		Ptr->RelRow[2] = 0;
		Ptr->RelCol[2] = 2;
		
		Ptr->NorthTabs[3] = -1;
		Ptr->WestTabs[3] = -1;
		Ptr->EastTabs[3] = 1;
		Ptr->SouthTabs[3] = 1;
		
		Ptr->RelRow[3] = 0;
		Ptr->RelCol[3] = 3;
		
		Ptr->NorthTabs[4] = 0;
		Ptr->WestTabs[4] = 0;
		Ptr->EastTabs[4] = 0;
		Ptr->SouthTabs[4] = -1;
		
		Ptr->RelRow[4] = -1;
		Ptr->RelCol[4] = 3;
	} else if (name == 'D') {
		Ptr->NorthTabs[0] = 1;
		Ptr->WestTabs[0] = 1;
		Ptr->EastTabs[0] = -1;
		Ptr->SouthTabs[0] = 1;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = -1;
		Ptr->WestTabs[1] = -1;
		Ptr->EastTabs[1] = 0;
		Ptr->SouthTabs[1] = 0;
		
		Ptr->RelRow[1] = 0;
		Ptr->RelCol[1] = 1;
		
		Ptr->NorthTabs[2] = -1;
		Ptr->WestTabs[2] = 1;
		Ptr->EastTabs[2] = 1;
		Ptr->SouthTabs[2] = -1;
		
		Ptr->RelRow[2] = -1;
		Ptr->RelCol[2] = 1;
		
		Ptr->NorthTabs[3] = 0;
		Ptr->WestTabs[3] = 0;
		Ptr->EastTabs[3] = -1;
		Ptr->SouthTabs[3] = -1;
		
		Ptr->RelRow[3] = -2;
		Ptr->RelCol[3] = 1;
		
		Ptr->NorthTabs[4] = 1;
		Ptr->WestTabs[4] = -1;
		Ptr->EastTabs[4] = 1;
		Ptr->SouthTabs[4] = 1;
		
		Ptr->RelRow[4] = -2;
		Ptr->RelCol[4] = 2;
	} else if (name == 'E') {		
		Ptr->NorthTabs[0] = 0;
		Ptr->WestTabs[0] = 0;
		Ptr->EastTabs[0] = -1;
		Ptr->SouthTabs[0] = 0;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = 1;
		Ptr->WestTabs[1] = -1;
		Ptr->EastTabs[1] = 1;
		Ptr->SouthTabs[1] = -1;
		
		Ptr->RelRow[1] = 0;
		Ptr->RelCol[1] = 1;
		
		Ptr->NorthTabs[2] = -1;
		Ptr->WestTabs[2] = 0;
		Ptr->EastTabs[2] = 0;
		Ptr->SouthTabs[2] = -1;
		
		Ptr->RelRow[2] = 1;
		Ptr->RelCol[2] = 1;
		
		Ptr->NorthTabs[3] = -1;
		Ptr->WestTabs[3] = 1;
		Ptr->EastTabs[3] = 1;
		Ptr->SouthTabs[3] = 1;
		
		Ptr->RelRow[3] = 2;
		Ptr->RelCol[3] = 1;
	} else if (name == 'F') {
		Ptr->NorthTabs[0] = 0;
		Ptr->WestTabs[0] = 0;
		Ptr->EastTabs[0] = 0;
		Ptr->SouthTabs[0] = -1;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = 0;
		Ptr->WestTabs[1] = 0;
		Ptr->EastTabs[1] = -1;
		Ptr->SouthTabs[1] = 0;
		
		Ptr->RelRow[1] = 1;
		Ptr->RelCol[1] = -1;
		
		Ptr->NorthTabs[2] = -1;
		Ptr->WestTabs[2] = -1;
		Ptr->EastTabs[2] = -1;
		Ptr->SouthTabs[2] = 1;
		
		Ptr->RelRow[2] = 1;
		Ptr->RelCol[2] = 0;
		
		Ptr->NorthTabs[3] = 0;
		Ptr->WestTabs[3] = -1;
		Ptr->EastTabs[3] = 0;
		Ptr->SouthTabs[3] = 0;
		
		Ptr->RelRow[3] = 1;
		Ptr->RelCol[3] = 1;
	} else if (name == 'G') {
		Ptr->NorthTabs[0] = 1;
		Ptr->WestTabs[0] = 1;
		Ptr->EastTabs[0] = 1;
		Ptr->SouthTabs[0] = -1;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = -1;
		Ptr->WestTabs[1] = 0;
		Ptr->EastTabs[1] = 0;
		Ptr->SouthTabs[1] = -1;
		
		Ptr->RelRow[1] = 1;
		Ptr->RelCol[1] = 0;
		
		Ptr->NorthTabs[2] = -1;
		Ptr->WestTabs[2] = 1;
		Ptr->EastTabs[2] = -1;
		Ptr->SouthTabs[2] = -1;
		
		Ptr->RelRow[2] = 2;
		Ptr->RelCol[2] = 0;
		
		Ptr->NorthTabs[3] = 0;
		Ptr->WestTabs[3] = -1;
		Ptr->EastTabs[3] = 0;
		Ptr->SouthTabs[3] = 0;
		
		Ptr->RelRow[3] = 2;
		Ptr->RelCol[3] = 1;
		
		Ptr->NorthTabs[4] = -1;
		Ptr->WestTabs[4] = 0;
		Ptr->EastTabs[4] = 0;
		Ptr->SouthTabs[4] = 0;
		
		Ptr->RelRow[4] = 3;
		Ptr->RelCol[4] = 0;
	} else if (name == 'H') {
		Ptr->NorthTabs[0] = 0;
		Ptr->WestTabs[0] = 0;
		Ptr->EastTabs[0] = -1;
		Ptr->SouthTabs[0] = 0;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = 1;
		Ptr->WestTabs[1] = -1;
		Ptr->EastTabs[1] = 1;
		Ptr->SouthTabs[1] = -1;
		
		Ptr->RelRow[1] = 0;
		Ptr->RelCol[1] = 1;
		
		Ptr->NorthTabs[2] = -1;
		Ptr->WestTabs[2] = 0;
		Ptr->EastTabs[2] = 0;
		Ptr->SouthTabs[2] = -1;
		
		Ptr->RelRow[2] = 1;
		Ptr->RelCol[2] = 1;
		
		Ptr->NorthTabs[3] = -1;
		Ptr->WestTabs[3] = 1;
		Ptr->EastTabs[3] = -1;
		Ptr->SouthTabs[3] = 1;
		
		Ptr->RelRow[3] = 2;
		Ptr->RelCol[3] = 1;
		
		Ptr->NorthTabs[4] = 0;
		Ptr->WestTabs[4] = -1;
		Ptr->EastTabs[4] = 0;
		Ptr->SouthTabs[4] = 0;
		
		Ptr->RelRow[4] = 2;
		Ptr->RelCol[4] = 2;
	} else if (name == 'I') {
		Ptr->NorthTabs[0] = 1;
		Ptr->WestTabs[0] = 1;
		Ptr->EastTabs[0] = 1;
		Ptr->SouthTabs[0] = -1;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = -1;
		Ptr->WestTabs[1] = -1;
		Ptr->EastTabs[1] = 0;
		Ptr->SouthTabs[1] = 0;
		
		Ptr->RelRow[1] = 1;
		Ptr->RelCol[1] = 0;
		
		Ptr->NorthTabs[2] = 1;
		Ptr->WestTabs[2] = 1;
		Ptr->EastTabs[2] = -1;
		Ptr->SouthTabs[2] = -1;
		
		Ptr->RelRow[2] = 1;
		Ptr->RelCol[2] = -1;
		
		Ptr->NorthTabs[3] = -1;
		Ptr->WestTabs[3] = 0;
		Ptr->EastTabs[3] = 0;
		Ptr->SouthTabs[3] = -1;
		
		Ptr->RelRow[3] = 2;
		Ptr->RelCol[3] = -1;
		
		Ptr->NorthTabs[4] = -1;
		Ptr->WestTabs[4] = 1;
		Ptr->EastTabs[4] = 1;
		Ptr->SouthTabs[4] = 1;
		
		Ptr->RelRow[4] = 3;
		Ptr->RelCol[4] = -1;
	} else if (name == 'J') {
		Ptr->NorthTabs[0] = 1;
		Ptr->WestTabs[0] = -1;
		Ptr->EastTabs[0] = 1;
		Ptr->SouthTabs[0] = 1;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = 0;
		Ptr->WestTabs[1] = 0;
		Ptr->EastTabs[1] = -1;
		Ptr->SouthTabs[1] = -1;
		
		Ptr->RelRow[1] = 0;
		Ptr->RelCol[1] = -1;
		
		Ptr->NorthTabs[2] = -1;
		Ptr->WestTabs[2] = 1;
		Ptr->EastTabs[2] = 1;
		Ptr->SouthTabs[2] = -1;
		
		Ptr->RelRow[2] = 1;
		Ptr->RelCol[2] = -1;
		
		Ptr->NorthTabs[3] = -1;
		Ptr->WestTabs[3] = 0;
		Ptr->EastTabs[3] = 0;
		Ptr->SouthTabs[3] = -1;
		
		Ptr->RelRow[3] = 2;
		Ptr->RelCol[3] = -1;
		
		Ptr->NorthTabs[4] = -1;
		Ptr->WestTabs[4] = 1;
		Ptr->EastTabs[4] = 1;
		Ptr->SouthTabs[4] = 1;
		
		Ptr->RelRow[4] = 3;
		Ptr->RelCol[4] = -1;	
	} else if (name == 'K') {
		Ptr->NorthTabs[0] = 1;
		Ptr->WestTabs[0] = 1;
		Ptr->EastTabs[0] = 1;
		Ptr->SouthTabs[0] = -1;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = -1;
		Ptr->WestTabs[1] = 0;
		Ptr->EastTabs[1] = -1;
		Ptr->SouthTabs[1] = -1;
		
		Ptr->RelRow[1] = 1;
		Ptr->RelCol[1] = 0;
		
		Ptr->NorthTabs[2] = 1;
		Ptr->WestTabs[2] = -1;
		Ptr->EastTabs[2] = 1;
		Ptr->SouthTabs[2] = 1;
		
		Ptr->RelRow[2] = 1;
		Ptr->RelCol[2] = 1;
		
		Ptr->NorthTabs[3] = -1;
		Ptr->WestTabs[3] = 1;
		Ptr->EastTabs[3] = 1;
		Ptr->SouthTabs[3] = -1;
		
		Ptr->RelRow[3] = 2;
		Ptr->RelCol[3] = 0;
		
		Ptr->NorthTabs[4] = -1;
		Ptr->WestTabs[4] = 0;
		Ptr->EastTabs[4] = 0;
		Ptr->SouthTabs[4] = 0;
		
		Ptr->RelRow[4] = 3;
		Ptr->RelCol[4] = 0;	
	} else if (name == 'L') {
		Ptr->NorthTabs[0] = 0;
		Ptr->WestTabs[0] = 0;
		Ptr->EastTabs[0] = 0;
		Ptr->SouthTabs[0] = -1;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = -1;
		Ptr->WestTabs[1] = 1;
		Ptr->EastTabs[1] = -1;
		Ptr->SouthTabs[1] = 1;
		
		Ptr->RelRow[1] = 1;
		Ptr->RelCol[1] = 0;
		
		Ptr->NorthTabs[2] = 0;
		Ptr->WestTabs[2] = -1;
		Ptr->EastTabs[2] = 0;
		Ptr->SouthTabs[2] = -1;
		
		Ptr->RelRow[2] = 1;
		Ptr->RelCol[2] = 1;
		
		Ptr->NorthTabs[3] = -1;
		Ptr->WestTabs[3] = 1;
		Ptr->EastTabs[3] = 1;
		Ptr->SouthTabs[3] = 1;
		
		Ptr->RelRow[3] = 2;
		Ptr->RelCol[3] = 1;	
	} else if (name == 'M') {
		Ptr->NorthTabs[0] = 0;
		Ptr->WestTabs[0] = 0;
		Ptr->EastTabs[0] = 0;
		Ptr->SouthTabs[0] = -1;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = -1;
		Ptr->WestTabs[1] = 1;
		Ptr->EastTabs[1] = -1;
		Ptr->SouthTabs[1] = 1;
		
		Ptr->RelRow[1] = 1;
		Ptr->RelCol[1] = 0;
		
		Ptr->NorthTabs[2] = 0;
		Ptr->WestTabs[2] = -1;
		Ptr->EastTabs[2] = 0;
		Ptr->SouthTabs[2] = -1;
		
		Ptr->RelRow[2] = 1;
		Ptr->RelCol[2] = 1;
		
		Ptr->NorthTabs[3] = -1;
		Ptr->WestTabs[3] = 1;
		Ptr->EastTabs[3] = 1;
		Ptr->SouthTabs[3] = -1;
		
		Ptr->RelRow[3] = 2;
		Ptr->RelCol[3] = 1;
		
		Ptr->NorthTabs[4] = -1;
		Ptr->WestTabs[4] = 0;
		Ptr->EastTabs[4] = 0;
		Ptr->SouthTabs[4] = 0;
		
		Ptr->RelRow[4] = 3;
		Ptr->RelCol[4] = 1;	
	} else if (name == 'N') {
		Ptr->NorthTabs[0] = 1;
		Ptr->WestTabs[0] = 1;
		Ptr->EastTabs[0] = -1;
		Ptr->SouthTabs[0] = 1;
		
		Ptr->RelRow[0] = 0;
		Ptr->RelCol[0] = 0;
		
		Ptr->NorthTabs[1] = 0;
		Ptr->WestTabs[1] = -1;
		Ptr->EastTabs[1] = 0;
		Ptr->SouthTabs[1] = -1;
		
		Ptr->RelRow[1] = 0;
		Ptr->RelCol[1] = 1;
		
		Ptr->NorthTabs[2] = -1;
		Ptr->WestTabs[2] = 1;
		Ptr->EastTabs[2] = -1;
		Ptr->SouthTabs[2] = 1;
		
		Ptr->RelRow[2] = 1;
		Ptr->RelCol[2] = 1;
		
		Ptr->NorthTabs[3] = 0;
		Ptr->WestTabs[3] = -1;
		Ptr->EastTabs[3] = 0;
		Ptr->SouthTabs[3] = -1;
		
		Ptr->RelRow[3] = 1;
		Ptr->RelCol[3] = 2;
		
		Ptr->NorthTabs[4] = -1;
		Ptr->WestTabs[4] = 1;
		Ptr->EastTabs[4] = 1;
		Ptr->SouthTabs[4] = 1;
		
		Ptr->RelRow[4] = 2;
		Ptr->RelCol[4] = 2;
	}
}

// Procedure to add all new pieces to a list
void InitPieces() {
	{
		//	A A A
		//	    A		
		sPiece *Ptr = new sPiece;
		Ptr->Name = 'A';
		Ptr->NumBits = 4;		
		Ptr->Next = Pieces;
		Pieces = Ptr;		
		FreePieces++;
	}
	{
		//	  2
		//	0 1		
		sPiece *Ptr = new sPiece;
		Ptr->Name = 'B';
		Ptr->NumBits = 3;
		Ptr->Next = Pieces;
		Pieces = Ptr;
		FreePieces++;
	}
	{	
		//	      C
		//	C C C C	
		sPiece *Ptr = new sPiece;
		Ptr->Name = 'C';
		Ptr->NumBits = 5;
		Ptr->Next = Pieces;
		Pieces = Ptr;		
		FreePieces++;
	}
	{	
		//	  D D
		//	  D
		//	D D
		sPiece *Ptr = new sPiece;
		Ptr->Name = 'D';
		Ptr->NumBits = 5;		
		Ptr->Next = Pieces;
		Pieces = Ptr;		
		FreePieces++;
	}
	{
		//	Z E
		//	  E
		//	  E
		sPiece *Ptr = new sPiece;
		Ptr->Name = 'E';
		Ptr->NumBits = 4;		
		Ptr->Next = Pieces;
		Pieces = Ptr;		
		FreePieces++;
	}
	{
		//	  F
		//	F F F
		sPiece *Ptr = new sPiece;
		Ptr->Name = 'F';
		Ptr->NumBits = 4;		
		Ptr->Next = Pieces;
		Pieces = Ptr;		
		FreePieces++;
	}
	{
		//	  Z
		//	  G
		//	  G G
		//	  G
		sPiece *Ptr = new sPiece;
		Ptr->Name = 'G';
		Ptr->NumBits = 5;		
		Ptr->Next = Pieces;
		Pieces = Ptr;		
		FreePieces++;
	}
	{
		//	Z H
		//	  H
		//	  H H
		sPiece *Ptr = new sPiece;
		Ptr->Name = 'H';
		Ptr->NumBits = 5;		
		Ptr->Next = Pieces;
		Pieces = Ptr;		
		FreePieces++;
	}
	{
		//	  Z
		//	I I
		//	I
		//	I
		sPiece *Ptr = new sPiece;
		Ptr->Name = 'I';
		Ptr->NumBits = 5;		
		Ptr->Next = Pieces;
		Pieces = Ptr;		
		FreePieces++;
	}
	{
		//	J Z
		//	J
		//	J
		//	J
		sPiece *Ptr = new sPiece;
		Ptr->Name = 'J';
		Ptr->NumBits = 5;		
		Ptr->Next = Pieces;
		Pieces = Ptr;		
		FreePieces++;
	}
	{
		//	Z
		//	K K
		//	K
		//	K
		sPiece *Ptr = new sPiece;
		Ptr->Name = 'K';
		Ptr->NumBits = 5;		
		Ptr->Next = Pieces;
		Pieces = Ptr;		
		FreePieces++;
	}
	{
		//	Z
		//	L L
		//	  L
		sPiece *Ptr = new sPiece;
		Ptr->Name = 'L';
		Ptr->NumBits = 4;		
		Ptr->Next = Pieces;
		Pieces = Ptr;		
		FreePieces++;
	}
	{
		//	Z
		//	M M
		//	  M
		//	  M
		sPiece *Ptr = new sPiece;
		Ptr->Name = 'M';
		Ptr->NumBits = 5;		
		Ptr->Next = Pieces;
		Pieces = Ptr;		
		FreePieces++;
	}
	{
		//	Z N
		//	  N N
		//	    N
		sPiece *Ptr = new sPiece;
		Ptr->Name = 'N';
		Ptr->NumBits = 5;		
		Ptr->Next = Pieces;
		Pieces = Ptr;		
		FreePieces++;
	}
	
	sPiece *Temp = Pieces;
	while (Temp != NULL) {
		ResetPiece(Temp);
		Temp = Temp->Next;
	}
}

