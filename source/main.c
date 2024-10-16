#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include "graphics.h"

int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);

	/* Set GBA to Mode 3 (A Bitmap Mode) */
	SetMode( MODE_3 | BG2_ON );

	/* Draw Center Line */
	drawLine();

	/* print "Press start button" */
	displayStartText();	

	bool gameStarted = false;

	while (1) {
		VBlankIntrWait();

		/* Respond to input*/
		scanKeys();
		int keys_pressed = keysDown();
		int keys_released = keysUp();
		
		if(keys_pressed & KEY_START && !gameStarted) {
			initGraphics();
			gameStarted = true; 
		}
		
		if(gameStarted) {
			playGame(keys_pressed, keys_released);
		}
	}
}