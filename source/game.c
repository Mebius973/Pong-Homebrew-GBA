#include <stdlib.h>
#include <gba_input.h>
#include "game.h"
#include "gameState.h"
#include "screen.h"

#define	PLAYER_WIDTH     8
#define	PLAYER_HEIGHT    36
#define BALL_SIZE        5
#define PLAYER_SPEED     1
#define COMPUTER_SPEED   1
#define BALL_SPEED       2
#define MAX_POINT        10

void moveBall() {
	sharedGameState.ball.prevX = sharedGameState.ball.x;
	sharedGameState.ball.prevY = sharedGameState.ball.y;

	/* Vertical Wall Bounce */
	if (sharedGameState.ball.y <= 1) {
		sharedGameState.ball.velocityY = BALL_SPEED;
	}
	if (sharedGameState.ball.y >= SCREEN_HEIGHT - 1) {
		sharedGameState.ball.velocityY = -BALL_SPEED;
	}

	/* Players Bounce */
    if (sharedGameState.ball.x <=  sharedGameState.humanPlayer.x + sharedGameState.humanPlayer.width) {
		if (sharedGameState.ball.y >= sharedGameState.humanPlayer.y && sharedGameState.ball.y <= sharedGameState.humanPlayer.y + sharedGameState.humanPlayer.height) {
			/* Player caught the ball */
			sharedGameState.ball.velocityX = BALL_SPEED;
			sharedGameState.ball.velocityY += sharedGameState.humanPlayer.velocityY;
		} else {
			/* Point Scored */
			sharedGameState.pointScored = true;
			sharedGameState.computerScore += 1;
			if (sharedGameState.computerScore == MAX_POINT) { sharedGameState.computerWins = true; }
			sharedGameState.ball.velocityX = 0;
			sharedGameState.ball.velocityY = 0;
		}
	}

	/* Computer Bounce */
	if (sharedGameState.ball.x >= sharedGameState.computerPlayer.x) {
		if(sharedGameState.ball.y >= sharedGameState.computerPlayer.y && sharedGameState.ball.y <= sharedGameState.computerPlayer.y + sharedGameState.computerPlayer.height) {
			/* Computer caught the ball */
			sharedGameState.ball.velocityX = -BALL_SPEED;
			sharedGameState.ball.velocityY += sharedGameState.computerPlayer.velocityY;
		} else {
			/* Point Scored */
			sharedGameState.pointScored = true;
			sharedGameState.humanScore += 1;
			if (sharedGameState.humanScore == MAX_POINT) { sharedGameState.humanWins = true; }
			sharedGameState.ball.velocityX = 0;
			sharedGameState.ball.velocityY = 0;
		}
	}
	
	sharedGameState.ball.x += sharedGameState.ball.velocityX;
	sharedGameState.ball.y += sharedGameState.ball.velocityY;
}

void moveComputer() {
	sharedGameState.computerPlayer.prevX = sharedGameState.computerPlayer.x;
	sharedGameState.computerPlayer.prevY = sharedGameState.computerPlayer.y;

	// Need to introduce a dead zone to avoid paddle parkinsoning
	if (sharedGameState.ball.y >= sharedGameState.computerPlayer.y + PLAYER_HEIGHT * 40 / 100 && sharedGameState.ball.y <= sharedGameState.computerPlayer.y + PLAYER_HEIGHT * 60 / 100) {
		sharedGameState.computerPlayer.velocityY = 0;
	}

	// Ball higher than paddle
	if (sharedGameState.ball.y < sharedGameState.computerPlayer.y + PLAYER_HEIGHT * 40 / 100 ) {
		sharedGameState.computerPlayer.velocityY = -COMPUTER_SPEED;
	}

	// Ball lower than paddle
	if (sharedGameState.ball.y > sharedGameState.computerPlayer.y + PLAYER_HEIGHT * 60 / 100 ) {
		sharedGameState.computerPlayer.velocityY = COMPUTER_SPEED;
	}

	// Ball touching the paddle
	if(sharedGameState.ball.y > sharedGameState.computerPlayer.y && sharedGameState.ball.y < sharedGameState.computerPlayer.y + PLAYER_HEIGHT && sharedGameState.ball.velocityX > 0 && (sharedGameState.ball.x + BALL_SIZE) >= sharedGameState.computerPlayer.x) {
		// random number. Can be -1 or +1
		float randomNumber = 2 * (rand() / (double) RAND_MAX) - 1;
		int sign = 1;
		if (randomNumber < 0 ) { sign = -1; }
		sharedGameState.computerPlayer.velocityY = sign * COMPUTER_SPEED;
	}

	// Stop computer when limits are reached
	if ((sharedGameState.computerPlayer.velocityY < 0 && sharedGameState.computerPlayer.y < PLAYER_SPEED + 1 ) ||
	    (sharedGameState.computerPlayer.velocityY > 0 && sharedGameState.computerPlayer.y > SCREEN_HEIGHT - sharedGameState.computerPlayer.height - (PLAYER_SPEED + 1))) {
		sharedGameState.computerPlayer.velocityY = 0;
	}

	sharedGameState.computerPlayer.y += sharedGameState.computerPlayer.velocityY;
}

void movePlayer(int keys_pressed, int keys_released) {
	// Save current coordinates
	sharedGameState.humanPlayer.prevX = sharedGameState.humanPlayer.x;
	sharedGameState.humanPlayer.prevY = sharedGameState.humanPlayer.y;

	// Move Player up
	if (keys_pressed & KEY_UP) {
		sharedGameState.humanPlayer.velocityY = -2;
	}

	// Move Player down
	if (keys_pressed & KEY_DOWN) {
		sharedGameState.humanPlayer.velocityY = 2;
	}

	// Stop player when limits are reached
	if ((sharedGameState.humanPlayer.velocityY < 0 && sharedGameState.humanPlayer.y < PLAYER_SPEED + 1 ) ||
		(sharedGameState.humanPlayer.velocityY > 0 && sharedGameState.humanPlayer.y > SCREEN_HEIGHT - sharedGameState.humanPlayer.height - (PLAYER_SPEED + 1))) {
		sharedGameState.humanPlayer.velocityY = 0;
	} 

	// Stop player when keys are released
	if (keys_released & KEY_UP || keys_released & KEY_DOWN) { sharedGameState.humanPlayer.velocityY = 0; }

	// Update player location
	sharedGameState.humanPlayer.y += sharedGameState.humanPlayer.velocityY;
}

void initGame() {
	sharedGameState.humanPlayer.x = 1;
	sharedGameState.humanPlayer.y = SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2;
	sharedGameState.humanPlayer.prevX = sharedGameState.humanPlayer.x;
	sharedGameState.humanPlayer.prevY = sharedGameState.humanPlayer.y;
	sharedGameState.humanPlayer.width = PLAYER_WIDTH;
	sharedGameState.humanPlayer.height = PLAYER_HEIGHT;
	sharedGameState.humanPlayer.velocityX = 0;
	sharedGameState.humanPlayer.velocityY = 0;

	sharedGameState.computerPlayer.x = SCREEN_WIDTH - PLAYER_WIDTH - 1;
	sharedGameState.computerPlayer.y = SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2;
	sharedGameState.computerPlayer.prevX = sharedGameState.computerPlayer.x;
	sharedGameState.computerPlayer.prevY = sharedGameState.computerPlayer.y;
	sharedGameState.computerPlayer.width = PLAYER_WIDTH;
	sharedGameState.computerPlayer.height = PLAYER_HEIGHT;
	sharedGameState.computerPlayer.velocityX = 0;
	sharedGameState.computerPlayer.velocityY = 0;

	sharedGameState.ball.x = SCREEN_WIDTH/2 - BALL_SIZE/2;
	sharedGameState.ball.y = SCREEN_HEIGHT/2 - BALL_SIZE/2;
	sharedGameState.ball.prevX = sharedGameState.ball.x;
	sharedGameState.ball.prevY = sharedGameState.ball.y;
	sharedGameState.ball.width = BALL_SIZE;	
	sharedGameState.ball.height = BALL_SIZE;
	sharedGameState.ball.velocityX = -BALL_SPEED;
	sharedGameState.ball.velocityY = 0;
}

void playGame(int keys_pressed,
              int keys_released) {

    if (sharedGameState.pointScored && keys_pressed) { 
		sharedGameState.nextGame = true;
		sharedGameState.pointScored = false;
		initGame();
	}

	if (!sharedGameState.pointScored) {
		movePlayer(keys_pressed, keys_released);
		moveBall();
		moveComputer();
	}
}