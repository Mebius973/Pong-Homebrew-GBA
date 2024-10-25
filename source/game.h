#include <stdbool.h>
#include "rect.h"

void initGame(rect* ball,
              rect* computerPlayer,
              rect* humanPlayer);

void playGame(int keys_pressed,
              int keys_released,
              bool* pointScored,
			  bool* nextGame,
              rect* ball,
              rect* computerPlayer,
              rect* humanPlayer);