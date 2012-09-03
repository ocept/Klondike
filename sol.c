//Card numbering:
// 1-26 = Red, 27-52 = Black
// 0 = Blank, 1 = Ace, 11 = Jack, 12 = Queen, 13 = King
//TODO: Implement hidden cards, improve algorithm to solve a higher % of games
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#define DEBUG 20 //debug level, higher prints more 

void printTable(int table[7][20]);
void printCard(int card);
void layTable(int table[7][20],int deck[52]);
int shuffleDeck(int[]);
void checkTable(int table[7][20],int founds[4][13]);
int checkSwap(int c1, int c2);
int checkColumn(int table[7][20], int column, int stackDepth);
void doMove(int table[7][20], int x0, int x1, int stackdepth);
int checkWaste(int table[7][20], int deck[52], int wastePos, int founds[4][13]);
void tableToFounds(int table[4][13], int founds[4][13]);
void printFounds(int founds[4][13]);
int checkFoundMove(int source, int dest);
void setColor(int ForgC);
void clearScreenToColors(int ForgC, int BackC);

int main()
{
	clearScreenToColors(0, FOREGROUND_GREEN);

	int ti = time(NULL);
	int wincount = 0;
	
	srand(ti); //seed random
	printf("seed = %d\n",ti);
	
	if(playGame()) //if game won
	{
		wincount++;
		FILE *fout = fopen("solnotes.txt", "a");
		fprintf(fout, "%d\n", ti);
		fclose(fout);
	}
	
	//printf("%d wins\n", wincount);
}

int playGame() //simulates 1 game
{
	int table[7][20];
	int deck[52];
	int founds[4][13];
	int i,j,h;
	int wastePos;
	int win;
	
	//clear table
	for(i=0;i<7;i++){ 
		for(j=0;j<20;j++){
			table[i][j]=0;
		}
	}
	//clear foundations
	for(i=0;i<4;i++){ 
		for(j=0;j<13;j++){
			founds[i][j]=0;
		}
	}
	shuffleDeck(deck);
	layTable(table,deck);
	if(DEBUG >= 10) 
	{
		printf("Table at start of game:\n");
		printTable(table);
	}

	//solve
	for(i=0;i<20;i++){
		checkTable(table,founds);
		checkTable(table,founds);
		for(wastePos=0; wastePos<52; wastePos++) //check waste cards
		{
			if(deck[wastePos]!=99) checkWaste(table,deck, wastePos, founds);
		}
		
		//check for win
		win=1;
		for(j=0;j<4;j++){
			if(founds[j][12] == 0) win = 0;
		}
		if(win)
		{
			if(DEBUG >= 10) printf("Win after %d cycles", i);
			return 1;
		}
	}
	
	//print results
	if(DEBUG >= 10)
	{
		printf("\nFoundations at finish:\n");
		printFounds(founds);
		printf("\nTable at finish:\n");
		printTable(table);
	}

	if(DEBUG >= 10)
	{
		printf("\nRemaining cards in waste:\n");
		for(i=0;i<52;i++)
		{
			if(deck[i]!=99)
			{
				printCard(deck[i]);
				printf("\n");
			}
			//printf("%d\n",deck[i]%13);
		}
	}
	return 0;
}
void printTable(int table[7][20]){
	int i,j;
	int empty;
	char card;
	for(i=19;i>=0;i--){
		//don't print when whole row is empty
		empty=1;
		for(j=0;j<7;j++){
			if(table[j][i]!=0) empty = 0;
		}
		if(empty) continue;
		
		for(j=0;j<7;j++){
			printCard(table[j][i]);
		}
		printf("\n");
	}
}

void printCard(int card)
{
	if (card==0) setColor(255); //set empty's white
	else {
		if(card<=26)setColor(FOREGROUND_RED);
		else setColor(0);
	}
	
	if (card==0) printf("XX "); //print blank space if no card
	else{
		switch(card%13)
		{
			case 1:  printf(" A "); break;
			case 11: printf(" J "); break;
			case 12: printf(" Q "); break;
			case 0:  printf(" K "); break;
			default: //card = (int)table[j][i]%13; break;
				printf("%.2d ",card%13);
				break;
		}
	}
	setColor(0); //set colour back to black
}
void layTable(int table[7][20], int deck[52]){
	int i,j;
	int index=0;
	//lay table
	for(i=0;i<7;i++)
	{
		for(j=0;j<=i;j++){
			table[i][j]=deck[index];
			deck[index]=99;
			index++;
		}
	}
}

int shuffleDeck(int deck[52]){
	int i,temp,rand1,rand2;
	for(i=0;i<52;i++){
		deck[i]=i+1;
	}

	//int t = 1249920072;
	//int t = 1249942340;
	//if(DEBUG > 20) printf("%d\n",t);

	for(i=0;i<1000;i++) //swap two random cards
	{ 
		rand1 = rand()%52;
		rand2 = rand()%52;
		if(rand1 == rand2) continue;
		temp = deck[rand1];
		deck[rand1] = deck[rand2];
		deck[rand2] = temp;
	}
	return 0;
}

void checkTable(int table[7][20], int founds[4][13]){ //check for valid table->table / table->foundations move, do it
	int i,j,k,i0,i1,y;
	//check for table -> foundation moves
	for(i=0;i<7;i++) 
	{
		for(j=0;j<4;j++)
		{
			if(checkFoundMove(table[i][0], founds[j][0])){
				if(DEBUG >= 30) printf("T->F: %.2d, %.2d\n",table[i][0], founds[j][0]);
				for(k=12;k>0;k--){
					founds[j][k] = founds[j][k-1];
				}
				founds[j][0] = table[i][0];
				for(k=0;k<(19);k++){//move down cards at source
					table[i][k] = table[i][k+1];
				}
			}
		}
	}
	//check for table -> table moves
	for(y=19;y>=0;y--)//go down the rows
	{
		for(i0=0;i0<7;i0++)//source card
		{
			for(i1=0;i1<7;i1++)//dest card
			{
				if(i0!=i1)//no move if same row
				{
					if(checkSwap(table[i0][y],table[i1][0])) //if top card matches dest
					{
						if(checkColumn(table,i0,y)){
							if(DEBUG >= 30) printf("T->T: %d:  %.2d, %.2d\n",y+1,table[i0][y]%13,table[i1][0]%13);
							doMove(table, i0,i1,y+1);
						}
					}
				}
			}
		}
	}
}

int checkSwap(int c1, int c2) //check for valid move between two cards, source card 1st
{ 
	if(c2 == 0) //if moving king to 0
	{
		return ((c1 != 0) && (c1%13 == 0));
	}
	else if(((c1<=26)^(c2<=26)) && ((c1%13) != 0)) //check for colours and not king
	{ 
		if(c1 == 0 || c2 == 0) printf("mismatch, %d : %d\n",c1,c2);
		return(((c1+1)%13) == c2%13); //test for adjacent no's
	}
	else return 0; 
}

int checkColumn(int table[7][20], int column, int stackDepth)
{
	int i;
	for(i=stackDepth;i>0;i--)
	{
		if(!checkSwap(table[column][i-1],table[column][i])) return 0;
	}
	return 1;
}

void doMove(int table[7][20], int x0, int x1, int stackDepth)
{
	int i;
	int temp[20];
	for(i=0;i<stackDepth;i++){//copy source cards
		temp[i] = table[x0][i];
	}
	
	for(i=0;i<(20-stackDepth);i++){//move down cards at source
		table[x0][i] = table[x0][i+stackDepth];
	}
	for(i=20-stackDepth;i<20;i++){ //clear cards at top of column
		table[x0][i] = 0;
	}
	for(i=19;i>=0;i--){//move up cards at dest
		table[x1][i] = table[x1][i-stackDepth];
	}
	for(i=0;i<stackDepth;i++){//insert cards at dest
		table[x1][i] = temp[i];
	}
}

int checkWaste(int table[7][20], int deck[52], int wastePos, int founds[4][13]) //checks for waste->table move
{
	//printCard(deck[wastePos]);
	//printf("\n");
	int i,j;
	for(i=0;i<4;i++) //check each foundation column
	{
		if(checkFoundMove(deck[wastePos],founds[i][0]))
		{
			if(DEBUG >= 30) printf("W->F: %.2d, %.2d\n",deck[wastePos]%13,founds[i][j]%13);
			for(j=12;j>0;j--){
				founds[i][j] = founds[i][j-1];
			}
			founds[i][0] = deck[wastePos];
			deck[wastePos] = 99;
			
			return wastePos+1;
		}
	
	}
	for(i=0;i<7;i++) //check each table column
	{
		
		if(checkSwap(deck[wastePos], table[i][0])) 
		{	
			if(DEBUG >= 30) printf("W->T: %.2d, %.2d\n", deck[wastePos]%13,table[i][0]%13);
			for(j=19;j>=0;j--){//move up cards at dest
				table[i][j] = table[i][j-1];
			}
			table[i][0] = deck[wastePos];
			deck[wastePos] = 99;
			
			break;
		}
	}			
	
	return wastePos+1;
}

void printFounds(int founds[4][13])
{
	int i,j;
	for(j=12;j>=0;j--)
	{
		for(i=0;i<4;i++)
		{
			printCard(founds[i][j]);
		}
		printf("\n");
	}
}

int checkFoundMove(int source, int dest) //Checks if move to foundation is valid
{
	if ((source%13 == 1) && (dest == 0)) //moving ace onto blank space
	{
		return 1;
	}
	if (dest == source - 1) //all other moves
	{
		return 1;
	}
	return 0;
}

void clearScreenToColors(int ForgC, int BackC)
{
     WORD wColour = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
     //get the handle to the current output buffer
     HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
     
     COORD coord = {0, 0}; //used to reset the carat/cursor to the top left
     DWORD count;

     CONSOLE_SCREEN_BUFFER_INFO csbi;
	 
	 //set current colour
     SetConsoleTextAttribute(hStdOut, wColour);
     if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
     {
          //This fills the buffer with a given character (in this case 32=space).
          FillConsoleOutputCharacter(hStdOut, (TCHAR) 32, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
          
          FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count );
          //This will set our cursor position for the next print statement.
          SetConsoleCursorPosition(hStdOut, coord);
     }
     return;
}

void setColor(int ForgC)
{
     WORD wColor; 
	 

     HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
     CONSOLE_SCREEN_BUFFER_INFO csbi;
     
     if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
     {
        //Mask out all but the background attribute, and add in the forgournd color
          wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
          SetConsoleTextAttribute(hStdOut, wColor);     
     }
     return;
}
