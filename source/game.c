#include "game.h"
#include "screen.h"
#include <gba_input.h>
#include <stdlib.h>

#define	PLAYER_WIDTH     8
#define	PLAYER_HEIGHT    36
#define BALL_SIZE        5
#define PLAYER_SPEED     1
#define COMPUTER_SPEED   1
#define BALL_SPEED       2

void moveBall(bool* pointScored,rect* ball, rect* computerPlayer, rect* humanPlayer) {
	ball->prevX = ball->x;
	ball->prevY = ball->y;

	/* Vertical Wall Bounce */
	if (ball->y <= 1) {
		ball->velocityY = BALL_SPEED;
	}
	if (ball->y >= SCREEN_HEIGHT - 1) {
		ball->velocityY = -BALL_SPEED;
	}

	/* Players Bounce */
    if (ball->x <=  humanPlayer->x + humanPlayer->width) {
		if (ball->y >= humanPlayer->y && ball->y <= humanPlayer->y + humanPlayer->height) {
			/* Player caught the ball */
			ball->velocityX = BALL_SPEED;
			ball->velocityY += humanPlayer->velocityY;
		} else {
			/* Point Scored */
			*pointScored = true;
			ball->velocityX = 0;
			ball->velocityY = 0;
		}
	}

	/* Computer Bounce */
	if (ball->x >= computerPlayer->x) {
		if(ball->y >= computerPlayer->y && ball->y <= computerPlayer->y + computerPlayer->height) {
			/* Computer caught the ball */
			ball->velocityX = -BALL_SPEED;
			ball->velocityY += computerPlayer->velocityY;
		} else {
			/* Point Scored */
			*pointScored = true;
			ball->velocityX = 0;
			ball->velocityY = 0;
		}
	}
	
	ball->x += ball->velocityX;
	ball->y += ball->velocityY;
}

void moveComputer(rect* ball, rect* computerPlayer) {
	computerPlayer->prevX = computerPlayer->x;
	computerPlayer->prevY = computerPlayer->y;

	// Need to introduce a dead zone to avoid paddle parkinsoning
	if (ball->y >= computerPlayer->y + PLAYER_HEIGHT * 40 / 100 && ball->y <= computerPlayer->y + PLAYER_HEIGHT * 60 / 100) {
		computerPlayer->velocityY = 0;
	}

	// Ball higher than paddle
	if (ball->y < computerPlayer->y + PLAYER_HEIGHT * 40 / 100 ) {
		computerPlayer->velocityY = -COMPUTER_SPEED;
	}

	// Ball lower than paddle
	if (ball->y > computerPlayer->y + PLAYER_HEIGHT * 60 / 100 ) {
		computerPlayer->velocityY = COMPUTER_SPEED;
	}

	// Ball touching the paddle
	if(ball->y > computerPlayer->y && ball->y < computerPlayer->y + PLAYER_HEIGHT && ball->velocityX > 0 && (ball->x + BALL_SIZE) >= computerPlayer->x) {
		// random number. Can be -1 or +1
		float randomNumber = 2 * (rand() / (double) RAND_MAX) - 1;
		int sign = 1;
		if (randomNumber < 0 ) { sign = -1; }
		computerPlayer->velocityY = sign * COMPUTER_SPEED;
	}

	// Stop computer when limits are reached
	if ((computerPlayer->velocityY < 0 && computerPlayer->y < PLAYER_SPEED + 1 ) ||
	    (computerPlayer->velocityY > 0 && computerPlayer->y > SCREEN_HEIGHT - computerPlayer->height - (PLAYER_SPEED + 1))) {
		computerPlayer->velocityY = 0;
	}

	computerPlayer->y += computerPlayer->velocityY;
}

void movePlayer(int keys_pressed, int keys_released, rect* humanPlayer) {
	// Save current coordinates
	humanPlayer->prevX = humanPlayer->x;
	humanPlayer->prevY = humanPlayer->y;

	// Move Player up
	if (keys_pressed & KEY_UP) {
		humanPlayer->velocityY = -2;
	}

	// Move Player down
	if (keys_pressed & KEY_DOWN) {
		humanPlayer->velocityY = 2;
	}

	// Stop player when limits are reached
	if ((humanPlayer->velocityY < 0 && humanPlayer->y < PLAYER_SPEED + 1 ) ||
		(humanPlayer->velocityY > 0 && humanPlayer->y > SCREEN_HEIGHT - humanPlayer->height - (PLAYER_SPEED + 1))) {
		humanPlayer->velocityY = 0;
	} 

	// Stop player when keys are released
	if (keys_released & KEY_UP || keys_released & KEY_DOWN) { humanPlayer->velocityY = 0; }

	// Update player location
	humanPlayer->y += humanPlayer->velocityY;
}

void initGame(rect* ball,
              rect* computerPlayer,
              rect* humanPlayer) {
	humanPlayer->x = 1;
	humanPlayer->y = SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2;
	humanPlayer->prevX = humanPlayer->x;
	humanPlayer->prevY = humanPlayer->y;
	humanPlayer->width = PLAYER_WIDTH;
	humanPlayer->height = PLAYER_HEIGHT;
	humanPlayer->velocityX = 0;
	humanPlayer->velocityY = 0;

	computerPlayer->x = SCREEN_WIDTH - PLAYER_WIDTH - 1;
	computerPlayer->y = SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2;
	computerPlayer->prevX = computerPlayer->x;
	computerPlayer->prevY = computerPlayer->y;
	computerPlayer->width = PLAYER_WIDTH;
	computerPlayer->height = PLAYER_HEIGHT;
	computerPlayer->velocityX = 0;
	computerPlayer->velocityY = 0;

	ball->x = SCREEN_WIDTH/2 - BALL_SIZE/2;
	ball->y = SCREEN_HEIGHT/2 - BALL_SIZE/2;
	ball->prevX = ball->x;
	ball->prevY = ball->y;
	ball->width = BALL_SIZE;	
	ball->height = BALL_SIZE;
	ball->velocityX = -BALL_SPEED;
	ball->velocityY = 0;
}

void playGame(int keys_pressed,
              int keys_released,
              bool* pointScored,
			  bool* nextGame,
              rect* ball,
              rect* computerPlayer,
              rect* humanPlayer) {

    if (*pointScored && keys_pressed) { 
		*nextGame = true;
		*pointScored = false;
		initGame(ball, computerPlayer, humanPlayer);
	}

	if (!*pointScored) {
		movePlayer(keys_pressed, keys_released, humanPlayer);
		moveBall(pointScored, ball, computerPlayer, humanPlayer);
		moveComputer(ball, computerPlayer);
	}
}