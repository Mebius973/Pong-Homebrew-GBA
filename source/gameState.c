#include "gameState.h"

gameState sharedGameState;

void initGameState() {
	sharedGameState.computerWins = false;
    sharedGameState.gameStarted = false;
	sharedGameState.humanWins = false;
	sharedGameState.nextGame = false;
    sharedGameState.pointScored = false;
	sharedGameState.readyTextCleared = false;
	sharedGameState.setStarted = false;
}