
#include <stdlib.h>
#include <gba_video.h>
#include "graphics.h"
#include "characters.h"
#include "background.h"
#include "screen.h"

#define MEM_VRAM        0x06000000

typedef u16             M3LINE[SCREEN_WIDTH];
#define m3_mem          ((M3LINE*)MEM_VRAM)

#define WHITE            0x7FFF
#define BLACK            0x0000

#define CHARS_SIZE       8
#define NUM_CHARS_LINE   SCREEN_WIDTH/CHARS_SIZE

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})


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
void drawRect(rect* cRect) {
	for (int i = cRect->x; i < cRect->x + cRect->width; i++) {
		for (int j = cRect->y; j < cRect->y + cRect->height; j++) {
			drawPixel(i, j, 0x7FFF);
		}		
	}
}

void clearPrevious(rect* cRect) {
	for (int i = cRect->prevX; i < cRect->prevX + cRect->width; i++) {
		for (int j = cRect->prevY; j < cRect->prevY + cRect->height; j++) {
			drawPixel(i, j, backgroundBitmap[(j * SCREEN_WIDTH + i)/ 2]);
		}		
	}
}

void redrawRect(rect* cRect) {
	clearPrevious(cRect);
    drawLine();
	drawRect(cRect);
}

void drawSplashscreen() {
	/* Set GBA to Mode 3 (A Bitmap Mode) */
	SetMode( MODE_3 | BG2_ON );

    drawBackground();

	/* Draw Center Line */
	drawLine();

	/* print "Press start button" */
	displayStartText();	
}

void drawGraphics(bool pointScored, rect* ball, rect* computerPlayer, rect* humanPlayer) {
    if (pointScored) {
            displayText("POINT !", SCREEN_WIDTH/2 - 3*8, SCREEN_HEIGHT/2);
    } else {
        redrawRect(ball);
        redrawRect(computerPlayer);
        redrawRect(humanPlayer);
    }
}

void drawInitialGraphics() {
    clearScreen();
	drawBackground();
	drawLine();
}