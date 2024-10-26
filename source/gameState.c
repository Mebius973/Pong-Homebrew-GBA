#include "gameState.h"

gameState sharedGameState;

void initGameState() {
    sharedGameState.gameStarted = false;
    sharedGameState.pointScored = false;
	sharedGameState.nextGame = false;
	sharedGameState.humanWins = false;
	sharedGameState.computerWins = false;
}