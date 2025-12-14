#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <time.h>

/* ===================== Global Variables =================== */
char mPixelOff = ' ';
char mPixelOn = '#';
char mHasColors = 0;

int mBitNumber;
int mMaxHeight = 30;
int mMaxWidth = 60;
int mWinPosX = 2;
int mWinPosY = 2;
int mStep = 1;


/* ===================== Functions =================== */

/* Calculates the index in the character array
   from the 2D coordinates x and y */
int m_GetIndex(int x, int y)
{
  return y * mMaxWidth + x;
}


/* Reads the state of the cell (x, y)
   in the provided grid */
int m_GetMem(char *grid, int x, int y)
{
  int index;
  int bytePos;
  int bitPos;
  
  index   = m_GetIndex(x,y);
  bytePos = index / mBitNumber;
  bitPos  = index % mBitNumber;

  return ( (grid[bytePos] & (1 << bitPos)) != 0 ? 1 : 0);
}


/* Sets the state of the cell (x, y)
   in the provided grid */
void m_SetMem(char *grid, int x, int y, int val)
{
  int index;
  int bytePos;
  int bitPos;
  
  index   = m_GetIndex(x,y);
  bytePos = index / mBitNumber;
  bitPos  = index % mBitNumber;

  if (val == 1)
    {
      grid[bytePos] |= (1 << bitPos);
    }
  else
    {
      grid[bytePos] &= ~(1 << bitPos);
    }
}


/* Calculates the new state of the cell based on
   the content of the input grid */
int m_ComputeCellStatus(char *grid, int x, int y)
{
  int isLive;
  int cnt;

  /* The state is calculated using the rules of Conway's Game of Life.
     To check the status of neighboring cells, we first ensure that 
     the neighboring cells do not "go out" of the grid */
  isLive = m_GetMem(grid, x, y );
  cnt = 0;
  
  if ( y-1 >= 0 )
    {
      if ( x-1 >= 0 )
	cnt += m_GetMem(grid, x-1, y-1);
      
      cnt += m_GetMem(grid, x, y-1);

      if ( x + 1 < mMaxWidth )
	cnt += m_GetMem(grid, x+1, y-1);
    }

  if ( x-1 >= 0 )
    cnt += m_GetMem(grid, x-1, y);
  
  if ( x+1 < mMaxWidth )
    cnt += m_GetMem(grid, x+1, y);

   if ( y+1 < mMaxHeight )
    {
      if ( x-1 >= 0 )
	cnt += m_GetMem(grid, x-1, y+1);
      
      cnt += m_GetMem(grid, x, y+1);

      if ( x + 1 < mMaxWidth )
	cnt += m_GetMem(grid, x+1, y+1);
    }

   
   return
     ( (isLive == 1) && ( (cnt == 2) || (cnt == 3) ) ) ||
     ( (isLive == 0) && ( cnt == 3 ) ); 
}


/* Draws a random figure at the center of the grid */
void m_ComputeFirstStep(char *grid)
{
  int cX, cY;
  int offset;

  offset = 3;

  cY = mMaxHeight / 2;
  cX = mMaxWidth / 2;
  
  srand(time(NULL));
	  
  for(int dx = -offset; dx <= offset; dx++ )
    {
      for(int dy = -offset; dy <= offset; dy++ )
	{
	  /* writes a random value in the
	     position (cX + dx, cY + dy) */
	  m_SetMem(grid, cX + dx, cY + dy, rand() % 2);
	}
    }

  mStep = 1;
}


/* Calculates the new state of the grid curGrid.
   The new state is stored in newGrid */ 
void m_ComputeChanges(char *curGrid, char *newGrid)
{
  int newCellStatus;

  for(int x = 0; x < mMaxWidth; x++ )
    {
      for(int y = 0; y < mMaxHeight; y++ )
	{
	  /* calculates the new state of the cell based on
	     the current state of the curGrid */
	  newCellStatus = m_ComputeCellStatus(curGrid, x, y);

	  /* stores the new state in the newGrid */
	  m_SetMem(newGrid, x, y, newCellStatus);	  
	}
    }

  ++mStep;
}

/* Draws a cell on the screen */
void m_DrawCell(WINDOW *win, int x, int y, int isOn)
{
  int pixel;
  int cpair;
  
  if (isOn)
    {
      /* PIXEL ON */
      pixel = mPixelOn;
      cpair = 1;
    }
  else
    {
      /* PIXEL OFF */
      pixel = mPixelOff;
      cpair = 2;
    }

  if (mHasColors)
    attron(COLOR_PAIR(cpair));	  

  mvwaddch(win, y, x, pixel);

  if (mHasColors)
    attroff(COLOR_PAIR(cpair));
}

/* Updates the grid on the screen, only changing
   the cells that have been modified */
void m_DrawGridChanges(WINDOW *win, char *curGrid, char *newGrid)
{
  int curStatus;
  int newStatus;

  for(int x = 0; x < mMaxWidth; x++ )
    {
      for(int y = 0; y < mMaxHeight; y++ )
	{
	  newStatus = m_GetMem(newGrid, x, y); 
	  curStatus = m_GetMem(curGrid, x, y);
	  
	  if (curStatus != newStatus )
	    {
	      m_DrawCell(win, mWinPosX + x, mWinPosY + y, newStatus == 1); 
	    }
	}
    }

  wrefresh(win); 
}


/* Redraws the entire grid on the screen */
void m_DrawAllGrid(WINDOW *win, char *grid)
{
  int cellStatus;

  for(int x = 0; x < mMaxWidth; x++ )
    {
      for(int y = 0; y < mMaxHeight; y++ )
	{
	  cellStatus = m_GetMem(grid, x, y);
	  
	  m_DrawCell(win, mWinPosX + x, mWinPosY + y, cellStatus == 1); 
	}
    }
  
  wrefresh(win);  
}


/* Prints a string on the screen at position x,y
   using the pre-set color for the text */
void m_DrawString(WINDOW *win, int x, int y,  char *string)
{
  if (mHasColors) attron(COLOR_PAIR(3));

  mvwaddstr( win, y, x, string );

  if (mHasColors) attroff(COLOR_PAIR(3));
}


/* Prints the current number of iterations
   in the top left corner */
void m_DrawStep(WINDOW *win)
{
  char step[20];

  sprintf(step, "STEP %d", mStep);

  m_DrawString( win, mWinPosX, mWinPosY-1, step ); 
}


/* =============== MAIN ============== */

int main(void)
{
  char *curGrid;
  char *newGrid;
  char *grid1;
  char *grid2;
 
  WINDOW * mainwin;
  int ch;
  int memSize;

  
  if ( (mainwin = initscr()) == NULL )
    {
      fprintf(stderr, "Error initialising ncurses.\n");
      exit(EXIT_FAILURE);
    }
  
  if (has_colors())
    {
      start_color();

      mHasColors = 1;

      init_pair(1, COLOR_BLACK, COLOR_BLACK);
      init_pair(2, COLOR_WHITE, COLOR_WHITE);
      init_pair(3, COLOR_WHITE, COLOR_BLACK);
    }
  
  noecho();
  keypad(mainwin, TRUE);
  curs_set(0);
  
  mBitNumber = sizeof(char) * 8; // numero di bit in un char
  memSize = sizeof(char) * mMaxHeight * mMaxWidth / mBitNumber;
  
  grid1 = malloc(memSize);
  memset(grid1, 0x0, memSize);
  
  grid2 = malloc(memSize);
  memset(grid2, 0x0, memSize);
  
  curGrid = grid1;
  newGrid = grid2;

  
  m_ComputeFirstStep(curGrid);
  
  m_DrawAllGrid(mainwin, curGrid);
  m_DrawStep(mainwin);
  m_DrawString(mainwin, mWinPosX, mWinPosY + mMaxHeight, "press any key to compute the next step and 'q' to quit");
  
  while ( (ch = getch()) != 'q' )
    {
      m_ComputeChanges(curGrid, newGrid);
      
      m_DrawGridChanges(mainwin, curGrid, newGrid);
      
      m_DrawStep(mainwin);
      
      if ( curGrid == grid1 )
	{
	  curGrid = grid2;
	  newGrid = grid1;
	}
      else
	{
	  curGrid = grid1;
	  newGrid = grid2;
	}
    }

  delwin(mainwin);
  endwin();
  refresh();
    
  return EXIT_SUCCESS;
}
