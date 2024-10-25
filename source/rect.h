/* Players and Ball Rectangles */
#pragma once
typedef struct _rect rect;
struct _rect {
	int x;
	int y;
	int prevX;
	int prevY;
	int width;
	int height;
	int velocityX;
	int velocityY;
};