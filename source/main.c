#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include "graphics.h"
#include "game.h"
#include "rect.h"

int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);

	rect ball;
	rect computerPlayer;
	rect humanPlayer;

	drawSplashscreen();

	bool gameStarted = false;
    bool pointScored = false;
	bool nextGame = false;

	initGame(&ball, &computerPlayer, &humanPlayer);

	while (1) {
		VBlankIntrWait();

  		scanKeys();
    	int keys_pressed = keysDown();
    	int keys_released = keysUp();
		if (!gameStarted && keys_pressed == KEY_START) {
			gameStarted = true;
			drawInitialGraphics();
		}

		if (gameStarted) {
			playGame(keys_pressed, keys_released, &pointScored, &nextGame, &ball, &computerPlayer, &humanPlayer);
			drawGraphics(pointScored, &ball, &computerPlayer, &humanPlayer);	
		}

		if (nextGame) {
			nextGame = false;
			drawInitialGraphics();
		}
	}
}