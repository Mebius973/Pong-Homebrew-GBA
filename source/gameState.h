#include <stdbool.h>
#include "rect.h"

#pragma once
typedef struct _gameState gameState;
struct _gameState {
	rect ball;
	rect computerPlayer;
	rect humanPlayer;

	int computerScore;
	int humanScore;
	
	bool computerWins;
	bool gameStarted;
	bool humanWins;
	bool nextGame;
	bool pointScored;
	bool readyTextCleared;
	bool setStarted;
};

extern gameState sharedGameState;

void initGameState();