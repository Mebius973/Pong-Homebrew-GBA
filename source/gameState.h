#include <stdbool.h>
#include "rect.h"

#pragma once
typedef struct _gameState gameState;
struct _gameState {
	rect ball;
	rect computerPlayer;
	rect humanPlayer;
	bool gameStarted;
    bool pointScored;
	bool nextGame;
	bool playerWins;
	bool computerWins;
 	int humanScore;
	int computerScore;
};

extern gameState sharedGameState;

void initGameState();