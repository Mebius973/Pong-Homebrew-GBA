#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include "game.h"
#include "gameState.h"
#include "graphics.h"

int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);

	initGameState();

	initGame();
	initGraphics();

	while (1) {
		VBlankIntrWait();

  		scanKeys();
    	int keys_pressed = keysDown();
    	int keys_released = keysUp();
		if (!sharedGameState.gameStarted && keys_pressed == KEY_START) {
			sharedGameState.gameStarted = true;
			drawInitialGraphics();
		}

		if (sharedGameState.gameStarted) {
			playGame(keys_pressed, keys_released);
			drawGraphics();	
		}

		if (sharedGameState.nextGame) {
			sharedGameState.nextGame = false;
			drawInitialGraphics();
		}
	}
}