#include "characters.h"
#include "rect.h"
#include "background.h"
#include <stdlib.h>

#define MEM_VRAM        0x06000000
#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   160

typedef u16             M3LINE[SCREEN_WIDTH];
#define m3_mem          ((M3LINE*)MEM_VRAM)

#define WHITE            0x7FFF
#define BLACK            0x0000

#define	PLAYER_WIDTH     8
#define	PLAYER_HEIGHT    36
#define BALL_SIZE        5
#define PLAYER_SPEED     1
#define COMPUTER_SPEED   1
#define BALL_SPEED       2
#define CHARS_SIZE       8
#define NUM_CHARS_LINE   SCREEN_WIDTH/CHARS_SIZE

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})


struct rect humanPlayer;
struct rect computerPlayer;
struct rect ball;
bool pointScored = false;

void drawPixel(int x, int y, int color) {
	m3_mem[y][x] = color;
}

void clearScreen() {
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			drawPixel(i, j, BLACK);
		}
	}
}

void drawLine() {
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		drawPixel(SCREEN_WIDTH/2, i, WHITE);
	}	
}

void drawBackground() {
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			drawPixel(i, j, backgroundBitmap[(j*SCREEN_WIDTH + i)/2]);
		}
	}
}

/* Print Individual Character (Normal Text) */
void printChar(bool characterArray[64], int x, int y) {
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            if (characterArray[i*8+j]) {
				drawPixel(x + j, y + i, WHITE);
			}
        }
    }
}

size_t strlen(const char *str)
{
    const char *s;
    for (s = str; *s; ++s);
    return(s - str);
}

/*  Display text string (made of characters.h chars). Only capital
    letters, numbers, and some punctuation, not full ascii.
    Limited to NUM_CHARS_LINE characters per line.
*/
void displayText(char textBuffer[], int x, int y) {
    for (int i=0; i<min(NUM_CHARS_LINE, strlen(textBuffer)); i++) {
        // Space
        if (textBuffer[i] == 0x20) { 										
            printChar(selector[0], x+i*8, y);
        // Exclamation point
        } else if (textBuffer[i] == 0x21) { 								
            printChar(punctuation[1], x+i*8, y);
        // Period
        } else if (textBuffer[i] == 0x2E) { 								
            printChar(punctuation[0], x+i*8, y);
        // Numbers
        } else if (textBuffer[i] >= 0x30 && textBuffer[i] <= 0x39) { 		
            printChar(score[textBuffer[i] - 0x30], x+i*8, y);
        // Letters
        } else {
            printChar(alphabet[textBuffer[i] - 0x41], x+i*8, y); 			
        }
    }
}

void displayStartText() {
    displayText("PRESS START BUTTON", SCREEN_WIDTH/2 - 9*8, SCREEN_HEIGHT/2);
}

/* Draw Player / Ball */
void drawRect(struct rect* cRect) {
	for (int i = cRect->x; i < cRect->x + cRect->width; i++) {
		for (int j = cRect->y; j < cRect->y + cRect->height; j++) {
			drawPixel(i, j, 0x7FFF);
		}		
	}
}

void clearPrevious(struct rect* cRect) {
	for (int i = cRect->prevX; i < cRect->prevX + cRect->width; i++) {
		for (int j = cRect->prevY; j < cRect->prevY + cRect->height; j++) {
			drawPixel(i, j, backgroundBitmap[(j * SCREEN_WIDTH + i)/ 2]);
		}		
	}
}

void moveBall() {
	/* Vertical Wall Bounce */
	if (ball.y <= 1) {
		ball.velocityY = BALL_SPEED;
	}
	if (ball.y >= SCREEN_HEIGHT - 1) {
		ball.velocityY = -BALL_SPEED;
	}

	/* Players Bounce */
    if (ball.x <=  humanPlayer.x + humanPlayer.width) {
		if (ball.y >= humanPlayer.y && ball.y <= humanPlayer.y + humanPlayer.height) {
			/* Player caught the ball */
			ball.velocityX = BALL_SPEED;
			ball.velocityY = humanPlayer.velocityY;
		} else {
			/* Point Scored */
			pointScored = true;
			ball.velocityX = 0;
			ball.velocityY = 0;
		}
	}

	/* Computer Bounce */
	if (ball.x >= computerPlayer.x) {
		if(ball.y >= computerPlayer.y && ball.y <= computerPlayer.y + computerPlayer.height) {
			/* Computer caught the ball */
			ball.velocityX = -BALL_SPEED;
			ball.velocityY = computerPlayer.velocityY;
		} else {
			/* Point Scored */
			pointScored = true;
			ball.velocityX = 0;
			ball.velocityY = 0;
		}
	}
	
	ball.x += ball.velocityX;
	ball.y += ball.velocityY;

	clearPrevious(&ball);
	drawRect(&ball);

	ball.prevX = ball.x;
	ball.prevY = ball.y;
}

void moveComputer() {
	// Need to introduce a dead zone to avoid paddle parkinsoning
	if (ball.y >= computerPlayer.y + PLAYER_HEIGHT * 40 / 100 && ball.y <= computerPlayer.y + PLAYER_HEIGHT * 60 / 100) {
		computerPlayer.velocityY = 0;
	}

	// Ball higher than paddle
	if (ball.y < computerPlayer.y + PLAYER_HEIGHT * 40 / 100 ) {
		computerPlayer.velocityY = -COMPUTER_SPEED;
	}

	// Ball lower than paddle
	if (ball.y > computerPlayer.y + PLAYER_HEIGHT * 60 / 100 ) {
		computerPlayer.velocityY = COMPUTER_SPEED;
	}

	// Ball touching the paddle
	if(ball.y > computerPlayer.y && ball.y < computerPlayer.y + PLAYER_HEIGHT && ball.velocityX > 0 && (ball.x + BALL_SIZE) >= computerPlayer.x) {
		// random number. Can be -1 or +1
		float randomNumber = 2 * (rand() / (double) RAND_MAX) - 1;
		int sign = 1;
		if (randomNumber < 0 ) { sign = -1; }
		computerPlayer.velocityY = sign * COMPUTER_SPEED;
	}

	// Stop computer when limits are reached
	if ((computerPlayer.velocityY < 0 && computerPlayer.y < PLAYER_SPEED + 1 ) ||
	    (computerPlayer.velocityY > 0 && computerPlayer.y > SCREEN_HEIGHT - computerPlayer.height - (PLAYER_SPEED + 1))) {
		computerPlayer.velocityY = 0;
	}

	computerPlayer.y += computerPlayer.velocityY;
	
	clearPrevious(&computerPlayer);
	drawRect(&computerPlayer);

	computerPlayer.prevX = computerPlayer.x;
	computerPlayer.prevY = computerPlayer.y;
}

void movePlayer(int keys_pressed, int keys_released) {
	// Move Player up
	if (keys_pressed & KEY_UP) {
		humanPlayer.velocityY = -2;
	}

	// Move Player down
	if (keys_pressed & KEY_DOWN) {
		humanPlayer.velocityY = 2;
	}

	// Stop player when limits are reached
	if ((humanPlayer.velocityY < 0 && humanPlayer.y < PLAYER_SPEED + 1 ) ||
		(humanPlayer.velocityY > 0 && humanPlayer.y > SCREEN_HEIGHT - humanPlayer.height - (PLAYER_SPEED + 1))) {
		humanPlayer.velocityY = 0;
	} 

	// Stop player when keys are released
	if (keys_released & KEY_UP || keys_released & KEY_DOWN) { humanPlayer.velocityY = 0; }

	// Update player location
	humanPlayer.y += humanPlayer.velocityY;

	/* Clear Previous */
	clearPrevious(&humanPlayer);

	/* Draw Player */
	drawRect(&humanPlayer);

	// Save current coordinates
	humanPlayer.prevX = humanPlayer.x;
	humanPlayer.prevY = humanPlayer.y;
}

void initGraphics(){
    clearScreen();
	drawBackground();
	drawLine();

	humanPlayer.x = 1;
	humanPlayer.y = SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2;
	humanPlayer.prevX = humanPlayer.x;
	humanPlayer.prevY = humanPlayer.y;
	humanPlayer.width = PLAYER_WIDTH;
	humanPlayer.height = PLAYER_HEIGHT;
	humanPlayer.velocityX = 0;
	humanPlayer.velocityY = 0;

	computerPlayer.x = SCREEN_WIDTH - PLAYER_WIDTH - 1;
	computerPlayer.y = SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2;
	computerPlayer.prevX = computerPlayer.x;
	computerPlayer.prevY = computerPlayer.y;
	computerPlayer.width = PLAYER_WIDTH;
	computerPlayer.height = PLAYER_HEIGHT;
	computerPlayer.velocityX = 0;
	computerPlayer.velocityY = 0;

	ball.x = SCREEN_WIDTH/2 - BALL_SIZE/2;
	ball.y = SCREEN_HEIGHT/2 - BALL_SIZE/2;
	ball.prevX = ball.x;
	ball.prevY = ball.y;
	ball.width = BALL_SIZE;	
	ball.height = BALL_SIZE;
	ball.velocityX = -BALL_SPEED;
	ball.velocityY = BALL_SPEED;
}

void playGame(int keys_pressed, int keys_released) {
	if (pointScored && keys_pressed) { 
		pointScored = false;
		initGraphics();
	}

	if (pointScored) {
		displayText("POINT !", SCREEN_WIDTH/2 - 3*8, SCREEN_HEIGHT/2);
	} else {
		movePlayer(keys_pressed, keys_released);
		moveBall();
		moveComputer();
		drawLine();
	}
}