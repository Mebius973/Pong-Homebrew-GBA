#include <stdlib.h>
#include <gba_video.h>
#include "background.h"
#include "characters.h"
#include "gameState.h"
#include "graphics.h"
#include "screen.h"

#define MEM_VRAM                0x06000000

typedef u16                     M3LINE[SCREEN_WIDTH];
#define m3_mem                  ((M3LINE*)MEM_VRAM)

#define WHITE                   0x7FFF
#define BLACK                   0x0000

#define CHARS_SIZE              8
#define NUM_CHARS_LINE          SCREEN_WIDTH/CHARS_SIZE

#define BOXED_TEXT_HPADDING     3
#define BOXED_TEXT_VPADDING     4
#define SCORE_ANCHOR_Y          5

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

char humanScore[10];
char computerScore[10];

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
    for (int i = 0; i < CHARS_SIZE; i++) {
        for (int j = 0; j < CHARS_SIZE; j++) {
            if (characterArray[i * CHARS_SIZE + j]) {
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
            printChar(number[textBuffer[i] - 0x30], x+i*8, y);
        // Letters
        } else {
            printChar(alphabet[textBuffer[i] - 0x41], x+i*8, y); 			
        }
    }
}

void displayStartText() {
    displayText("PRESS START BUTTON", SCREEN_WIDTH/2 - 9 * CHARS_SIZE, SCREEN_HEIGHT/2);
}

void displayReadyText() {
    sharedGameState.readyTextCleared = false;
    displayText("PRESS A WHEN READY", SCREEN_WIDTH/2 - 9 * CHARS_SIZE, SCREEN_HEIGHT/2 + CHARS_SIZE + 5);
}

void clearReadyText() {
    for (int j = SCREEN_HEIGHT/2 + CHARS_SIZE + 5; j < SCREEN_HEIGHT/2 + 2* CHARS_SIZE + 5; j++) { 
    for (int i = SCREEN_WIDTH/2 - 9 * CHARS_SIZE; i < SCREEN_WIDTH/2 + 9 * CHARS_SIZE; i++) {
            drawPixel(i, j, backgroundBitmap[(j*SCREEN_WIDTH + i)/2]);
        }   
    }
}

/* Draw Player / Ball */
void drawRect(rect cRect) {
    for (int j = cRect.y; j < cRect.y + cRect.height; j++) {
	    for (int i = cRect.x; i < cRect.x + cRect.width; i++) {
			drawPixel(i, j, WHITE);
		}		
	}
}

void clearPrevious(rect cRect) {
    for (int j = cRect.prevY; j < cRect.prevY + cRect.height; j++) {
	    for (int i = cRect.prevX; i < cRect.prevX + cRect.width; i++) {
			drawPixel(i, j, backgroundBitmap[(j * SCREEN_WIDTH + i)/ 2]);
		}		
	}
}

void redrawRect(rect cRect) {
	clearPrevious(cRect);
	drawRect(cRect);
}

int lengthOfChar(char* text) {
    int length = 0;
    while (text[length] != '\0') length++;
    return length;
}

void clearText(char textBuffer[], int x, int y) {
    for (int i = 0; i < lengthOfChar(textBuffer) * CHARS_SIZE; i++) {
        for (int j = 0; j < CHARS_SIZE; j++) {
            drawPixel(x + i, y + j, backgroundBitmap[(j * SCREEN_WIDTH + i)/ 2]);
        }
    }
}

void printScore(bool humanScore[288], bool computerScore[288]) {
    for (int row = 0; row < 18; row++) {
        for (int column = 0; column < 16; column++) {
            int humanScoreColor = backgroundBitmap[(row * SCREEN_WIDTH + SCREEN_WIDTH/4 - CHARS_SIZE / 2 + column)/2];
            int computerScoreColor = backgroundBitmap[(row * SCREEN_WIDTH + SCREEN_WIDTH * 3/4 - CHARS_SIZE / 2 + column)/2];

            if (humanScore[row * 16 + column]) humanScoreColor = WHITE;
            if (computerScore[row * 16 + column]) computerScoreColor = WHITE;

            drawPixel(SCREEN_WIDTH/4 - CHARS_SIZE / 2 + column, 5 + row, humanScoreColor);
			drawPixel(SCREEN_WIDTH * 3/4 - CHARS_SIZE / 2 + column, 5 + row, computerScoreColor);
        }
    }
}

void drawScore() {
    __itoa(sharedGameState.humanScore, humanScore, 10);
    __itoa(sharedGameState.computerScore, computerScore, 10);
    //clearScore();
    printScore(score[humanScore[0] - 0x30], score[computerScore[0] - 0x30]);
}

void initGraphics() {
	/* Set GBA to Mode 3 (A Bitmap Mode) */
	SetMode( MODE_3 | BG2_ON );

    drawBackground();

	/* Draw Center Line */
	drawLine();

	/* print "Press start button" */
	displayStartText();
}

void drawGraphics() {
    if (sharedGameState.setStarted && !sharedGameState.readyTextCleared) {
        clearReadyText();
        sharedGameState.readyTextCleared = true;
    }

    if (sharedGameState.pointScored) {
        char* text1 = "POINT !";
        if (sharedGameState.humanWins) {
            text1 = "YOU WIN !";
        }
        if (sharedGameState.computerWins) {
            text1 = "YOU LOSE !";
        }
        
        displayText(text1, SCREEN_WIDTH/2 - lengthOfChar(text1)/2*8, SCREEN_HEIGHT/2);

        char* text2 = "PRESS ANY KEY TO CONTINUE";
        displayText(text2, SCREEN_WIDTH/2 - lengthOfChar(text2)/2*8, SCREEN_HEIGHT/2 + CHARS_SIZE + 5);
    } else {
        redrawRect(sharedGameState.ball);
        redrawRect(sharedGameState.computerPlayer);
        redrawRect(sharedGameState.humanPlayer);
    }

    drawScore();
    drawLine();
}

void drawInitialGraphics() {
	drawBackground();
	drawLine();
    drawScore();
    displayReadyText();
}