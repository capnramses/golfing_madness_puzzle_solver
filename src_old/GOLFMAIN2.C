#include <stdio.h>

// Board Width and Board Hight
#define BW 12
#define BH 12
// Intro (Version) Message
#define INTRO printf("Golf Puzzle Solver By Anton version 1.1\n");

struct sPiece {
  bool Used;
  char Name;
  int NumBits;
  
  // 'Tabs' for each bit (inney = 0, outey = 1, joined = -1)
  int NTabs[10];
  int WTabs[10];
  int ETabs[10];
  int STabs[10];
  int HOffs[10];
  int VOffs[10];
  
  // 0-3 (0/90/180/270) degrees
  int Rotation;
  int BitOntheFreeSquare;
  
  sPiece *Next;
};

int Move = 0;      // Number of moves so far
char Board[BW][BH];
// Lists of pieces
sPiece *Pieces = NULL;
int UnusedCount = 0;
int UsedCount = 0;

// Function to printout everything in board
void PrintBoard() {
  printf("  - Board at Move %i -\n",Move);
  for (int i = 0; i < BW; i++) {
    for (int j = 0; j < BH; j++) {
        printf("%c ",Board[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

// Func to add all the pieces to Unused List
void SetupPieces() {
  // Do A last, do Z first!!!!

  {  // Setup 2nd Piece
    /*
          B
    */
    sPiece *Temp = new sPiece;
    
    Temp->Used = false;
    Temp->Name = 'B';
    Temp->NumBits = 1;
    Temp->HOffs[0] = 0;
    Temp->VOffs[0] = 0;
    Temp->NTabs[0] = 0;
    Temp->WTabs[0] = 0;
    Temp->ETabs[0] = 0;
    Temp->STabs[0] = 0;
    Temp->Rotation = 0;
    Temp->BitOntheFreeSquare = 0;
    
    Temp->Next = Pieces;
    Pieces = Temp;
    UnusedCount++;
  }




  {  // Setup First Piece
    /*
            A
          A A A
    */
    sPiece *Temp = new sPiece;
    
    Temp->Used = false;
    Temp->Name = 'A';
    Temp->NumBits = 4;
    Temp->HOffs[0] = 0;
    Temp->VOffs[0] = 0;
    Temp->HOffs[1] = -1;
    Temp->VOffs[1] = 1;
    Temp->HOffs[2] = 0;
    Temp->VOffs[2] = 1;
    Temp->HOffs[3] = 1;
    Temp->VOffs[3] = 1;
    Temp->NTabs[0] = 0;
    Temp->WTabs[0] = 0;
    Temp->ETabs[0] = 0;
    Temp->STabs[0] = -1;
    Temp->NTabs[1] = 1;
    Temp->WTabs[1] = 1;
    Temp->ETabs[1] = -1;
    Temp->STabs[1] = 1;
    Temp->NTabs[2] = -1;
    Temp->WTabs[2] = -1;
    Temp->ETabs[2] = -1;
    Temp->STabs[2] = 0;
    Temp->NTabs[3] = 1;
    Temp->WTabs[3] = -1;
    Temp->ETabs[3] = 1;
    Temp->STabs[3] = 1;
    Temp->Rotation = 0;
    Temp->BitOntheFreeSquare = 0;
    
    Temp->Next = Pieces;
    Pieces = Temp;
    UnusedCount++;
  }
}

bool AddPiece(char name,int row,int col) {
  bool Fits = false;
  sPiece *ptr = Pieces;
  
  // Find piece in list
  while (ptr != NULL) {
    if (ptr->Name == name) { break; }
    ptr = ptr->Next;  
  }
  
  printf("Trying to fit piece [%c] of %i bits\n",name,ptr->NumBits);
  
  // FOR each ROTATION setting (Turn piece around)
//for (int i = 0; i < 4; i++) {
//    printf("Trying rotation %i\n",i);
//    ptr->Rotation = i;
    // ***SETUP ROTATION HERE
    
    // FOR each SQUARE on piece (Move piece around)
    for (int j = 0; j < ptr->NumBits; j++) {
      ptr->BitOntheFreeSquare = j;
      // Set 'offsets' to relative position of that bit in the piece
      int RowOffs = ptr->HOffs[j];
      int ColOffs = ptr->VOffs[j];
      printf(" Trying to put bit %i on the free space\n",j);
      printf(" RowOffs = %i\n",RowOffs);
      printf(" ColOffs = %i\n",ColOffs);
        
      // See if all of the bits can fit
      for (int k = 0; k < ptr->NumBits; k++) {
        printf("  Seeing if bit (%i) fits\n",k);
        printf("  bit %i HRel to bit 0  = %i\n",ptr->HOffs[k]);
        printf("  bit %i VRel to bit 0  = %i\n",ptr->VOffs[k]);
        printf("  bit %i HRel + FreeSpaceH = %i\n",ptr->HOffs[k]+row);
        printf("  bit %i VRel + FreeSpaceV = %i\n",ptr->VOffs[k]+col);
        printf("  bit %i - HOffset = %i\n",ptr->HOffs[k]+row-RowOffs);
        printf("  bit %i - VOffset = %i\n",ptr->VOffs[k]+col-ColOffs);
        
        // Determine where that bit is on the board
        int BitBoardH = (ptr->HOffs[k] - RowOffs) + row;
        int BitBoardV = (ptr->VOffs[k] - ColOffs) + col;
          
        // See if this is actually on the board
        if ((BitBoardH < 0) || (BitBoardV < 0)) { Fits = false; break; }
          
        // See if this is a free space on board
        if (Board[BitBoardH][BitBoardV] == '.') {
          printf("--This IS a free SPACE\n");
          Fits = true;
        } else {
          Fits = false;
          break;
        }
          
        // See if all of the bits TABS fit with neighbors' tabs          
        // ** CHECK AGAINST BOUNDARIES
      }  
      
      if (Fits == true) {
        printf("**Piece FITS**\n");
        // put in USED list
        ptr->Used = true;
        UnusedCount--;
        UsedCount++;

        // Add the piece to the board 
        for (int i = 0; i < ptr->NumBits; i++) {
          
          // Determine where that bit is on the board
          int HOffset = ptr->HOffs[ptr->BitOntheFreeSquare];
          int VOffset = ptr->VOffs[ptr->BitOntheFreeSquare];
          int BitBoardH = (ptr->HOffs[i] - HOffset) + row;
          int BitBoardV = (ptr->VOffs[i] - VOffset) + col;
          
          Board[BitBoardH][BitBoardV] = ptr->Name;
        }
        return true;    
      } 
    }
 // }
  return false;
}

// Try to place a piece on the board
void MakeMove() {
  Move++;
  // Find first free square
  for (int i = 0; i < BH; i++) {
    for (int j = 0; j < BW; j++) {
      if (Board[j][i] == '.') {
        printf("Found free square at [%i][%i]\n",i,j);
        if (!AddPiece('A',i,j)) {
          printf("Piece [A] does not fit in freesquare\n");
          if (!AddPiece('B',i,j)) {
            printf("Piece [B] does not fit in freesquare\n");
          }
        }
        // Try to add each remaining piece (in alphabetical order)
        // if none fit, remove last added piece and try everything from that one on
        
        
        return;    
      }       
    }
  }
}

int main() {
  INTRO
  
  // Setup Board as empty char array
  for (int i = 0; i < BW; i++) {for (int j = 0; j < BH; j++) { Board[i][j] = '.'; }}
  
  // Setup Pieces
  SetupPieces();
  
  // Print Empty Board
  PrintBoard();
  
  // WHILE PIECES LEFT Main Program Loop
    MakeMove();
    PrintBoard();
    MakeMove();
    PrintBoard();
} 
