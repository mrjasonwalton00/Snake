#include <fcntl.h>
#include <ncurses.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h> //for random
#include <unistd.h> //for sleep
#include <signal.h> //for signal handler

const double speedMod = .99;
time_t seconds;
time_t new;

void makeSnake();
void snakeShift();
bool snakeEatCheck(int x, int y);
int nextX = 0, nextY = 0;
int snakeLength = 3;
int currentHeadRow;
int dir = 3;
bool snakeJustAte = true;
bool snakeHitBorder;
int snakeCounter = 0;
int score = 0;
int trophyValue = 0;
int max_x, max_y;

// This struct is for each body segment of the snake
// Each struct contains an x and y coordinate value. It's also used to track the trophies.
struct space // Gaston
{
  int x;
  int y;
};

struct space snakeSegment[200];
struct space trophy;
struct timespec rem, req = {0, 90000000};

// Gaston -- Spawns the head of the snake in the center, then the rest of the snake body to its left.
void SnakeSpawn() 
{
  for (int i = snakeLength; i != 0; i--) 
  {
    snakeSegment[i].x = COLS / 2 - (snakeLength - i);
    snakeSegment[i].y = LINES / 2;
  }
}

bool snakeCollision(int x, int y) //
{
  for(int i=0; i<snakeLength-1; i++)
    if(snakeSegment[i].x == x && snakeSegment[i].y == y)
      return true;
  return false;
}

void newTime()  //Gaston -- resets global current time
{
    seconds = time(NULL);
}

bool trophyTimer() // Gaston compares new time vs previous to check if the trophy has expired.
{
    new = time(NULL); // checks new time
    double x = 1 + rand() % 9;

    if(difftime(new, seconds) >= x) // compares time difference since trophy spawn till x.
    {
        newTime();
        return true;
    }

    return false;
}

void snakeMove(int right, int down) //Gaston
{
  nextX = snakeSegment[snakeLength].x + right;
  nextY = snakeSegment[snakeLength].y + down;
  if(nextX == trophy.x && nextY == trophy.y) 
  {
    snakeJustAte = true;
    snakeLength++;
    snakeCounter = trophyValue -1;
    score += trophyValue;
    req.tv_nsec *= speedMod;
    newTime();
  }
    else if(snakeCounter > 0)
    {
        snakeLength++;
        snakeCounter--;
        req.tv_nsec *= speedMod;
    }
    for (int i = 1; i < snakeLength; i++)
        snakeSegment[i] = snakeSegment[i + 1];
        snakeSegment[snakeLength].x = nextX;
        snakeSegment[snakeLength].y = nextY;
    
}
/* Switch checks if we're doing a movement, if so it takes that case, it compares with lastch (global variable that holds)
the last movement, and moves each piece of the tail to the next one, then it moves the head accordingly and establishes the new
last ch. If no proper key is pressed, it goes to default which repeats the last operation based on lastch.*/
void handleInput(int movement) //Gaston & Jenna
{
  nextX = 0;
  nextY = 0;

  //Up: 0; Down: 1; Left: 2; Right: 3
  switch (movement)
  {
    case KEY_UP:
        dir = 0;
      break;
    case KEY_DOWN:
        dir = 1;
      break;
    case KEY_LEFT:
        dir = 2;
      break;
    case KEY_RIGHT:
        dir = 3;
      break;
  }
  switch (dir) 
  {
    //Up
    case 0:
      snakeMove(0, -1);
      break;
    //Down
    case 1:
      snakeMove(0, 1);
      break;
    //Left
    case 2:
      snakeMove(-1, 0);
      break;
    //Right
    case 3:
      snakeMove(1, 0);
      break;
  }
  int snakeHeadX = snakeSegment[snakeLength].x, snakeHeadY = snakeSegment[snakeLength].y;
  //if snake hits wall -Jason
  if (snakeHeadX == COLS-1 || snakeHeadX == 1 || snakeHeadY == LINES-1|| snakeHeadY == 1
     || snakeCollision(snakeHeadX, snakeHeadY))
    snakeHitBorder = true;
}

void randomTrophy() //Jon & Gaston -- Trophy of rand val 1-9 is picked, it's placed on a random spot within a certain distance of the snake head so it can be acquired in time.
{
  bool trophyChosen = false;
  while (!trophyChosen) 
  {
    srand(time(0));
    trophyValue = 1 + rand() % 9 ;
    trophy.x = COLS +1;
    trophy.y = LINES +1;
    if(snakeLength < 20)
    {
    while(trophy.x >= COLS || trophy.x <= 0) 
    {
      trophy.x = (snakeSegment[snakeLength].x - COLS/12) + rand() % COLS/6;
    }
     while(trophy.y >= LINES || trophy.y <= 1) 
    {
      trophy.y = (snakeSegment[snakeLength].y - LINES/12) + rand() % LINES/6;
    }
    }
    else if(snakeLength >= 20 && snakeLength < 50)
    {
    while(trophy.x >= COLS || trophy.x <= 0) 
    {
      trophy.x = (snakeSegment[snakeLength].x - COLS/10) + rand() % COLS/5;
    }
     while(trophy.y >= LINES || trophy.y <= 1) 
    {
      trophy.y = (snakeSegment[snakeLength].y - LINES/10) + rand() % LINES/5;
    }

    }
    else
    {
    while(trophy.x >= COLS || trophy.x <= 0) 
    {
      trophy.x = (snakeSegment[snakeLength].x - COLS/8) + rand() % COLS/4;
    }
     while(trophy.y >= LINES || trophy.y <= 1) 
    {
      trophy.y = (snakeSegment[snakeLength].y - LINES/8) + rand() % LINES/4;
    }
    }

    if(!snakeCollision(trophy.x, trophy.y)) // Snake can't spawn on top of the snake.
    {
    trophyChosen = true;
    break;
    }
  }
}

// Loops through each snake segment, printing "o" at the xy coordinate of each
// segment
void snakeDraw() // Gaston
{
  for (int i = 1; i <= snakeLength; i++)
    mvprintw(snakeSegment[i].y, snakeSegment[i].x, "o");
}

// Prints border -Jason
void drawBorder() 
{
  int i;
  // prints top and bottom
  for (i = 0; i < COLS; i++) {
    mvprintw(1, i, "#");
    mvprintw(LINES - 1, i, "#");
    refresh();
  }
  // prints left and right edge
  for (i = 0; i < LINES - 1; i++) {
    mvprintw(i+1, 0, "#");
    mvprintw(i+1, COLS - 1, "#");
    refresh();
  }
}

// kbhit checks if user has hit a key and will return the key hit
int keyHit() //Jon
{
  keypad(stdscr, TRUE);
  int ch;
  int check;
  nodelay(stdscr, true);
  noecho();
  ch = getch();
  if (ch == ERR) // No keypress{
    check = 0;
  else // Keypress{
    check = ch;
  echo();
  nodelay(stdscr, false);
  return (check);
}

// Alice | Displays current score. Also displays win screen when score is sufficient.
int scoreTracker() 
{
    if (score == 100) 
    {         // score required to win the game
        attron(COLOR_PAIR(1));
        mvprintw((LINES/2)-2, (COLS/2)-8, "CONGRATULATIONS!");
        mvprintw((LINES/2)-1, (COLS/2)-9, "The snake is happy.");
        attroff(COLOR_PAIR(1));
        refresh();
        sleep(5);
        endwin();
        exit(0);
    }
    attron(COLOR_PAIR(1));
    mvprintw(0, (COLS/2)-4, "Score: %d", score);
    attroff(COLOR_PAIR(1));
    return 0;
}

// Alice | Reprograms signal handler to pause game and quit if desired.
void check(int signum) 
{
    int ch = 0;
    
    noecho();
    attron(COLOR_PAIR(1));
    mvprintw((LINES/2)-2, (COLS/2)-3, "PAUSED.");
    mvprintw((LINES/2)-1, (COLS/2)-7, "Press Q to quit.");
    refresh();
    ch = getch();

    if(ch == 'q' || ch == 'Q') {
        mvprintw((LINES/2)+1, (COLS/2)-9, "Thanks for playing!");
        refresh();
        sleep(2);
        endwin();
        exit(0);
    } if(ch == 'w') {               // display win screen - for demonstration only
        score = 100;
        refresh();
    } else {
    mvprintw((LINES/2)+1, (COLS/2)-6, "CONTINUING...");
    refresh();
    sleep(1);
    }
    attroff(COLOR_PAIR(1));
}


// Main method for Snake Game
int main() 
{
  getmaxyx(stdscr, max_y, max_x);
  //int x = max_x / 2, y = max_y / 2;

  initscr();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  SnakeSpawn();
  signal(SIGINT, check);
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_GREEN);
  refresh();

  // Loop that animates the snake movement and border
  while (1) 
  {
    int ch;
    handleInput(keyHit());
    drawBorder();
    clear();
    scoreTracker();
    drawBorder();
    snakeDraw();

    if(snakeJustAte) //Jon
    {
      randomTrophy();
      snakeJustAte = false;
    }
    if(trophyTimer() == true) //Gaston -- if timer expires, spawn new trophy.
    randomTrophy();

    /* Original version of border collision end
    if(snakeHitBorder) {
      clear();
      endwin();
      refresh();
      mvprintw(20, 40, "GAME OVER");
      mvprintw(30, 60, "Press any key");
      refresh();
    }
    */

    // Alice | Alternative version of border collision end? Remove if unwanted.
    if(snakeHitBorder) {
      attron(COLOR_PAIR(1));
      mvprintw((LINES/2)+1, (COLS/2)-5, "GAME OVER.");
      attroff(COLOR_PAIR(1));
      refresh();
      sleep(2);
      endwin();
      exit(0);
    }

    mvprintw(trophy.y, trophy.x, "X");
    refresh();

    nanosleep(&req , &rem);
  }
}