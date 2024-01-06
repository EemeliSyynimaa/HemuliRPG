#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"
#include "screens.h"

typedef struct Button
{
	Rectangle rect;
	char text[255];
	int fontSize;

	Color buttonColor;
	Color hoverColor;
	Color textColor;
} Button;

bool IsButtonClicked(Button* button)
{
	bool returnValue = false;

	if (IsMouseButtonPressed(0))
	{
		returnValue = CheckCollisionPointRec(GetMousePosition(), button->rect);
	}

	return returnValue;
}

void DrawButton(Button* button)
{
	int buttonCenterX = (int)(button->rect.x + button->rect.width * 0.5f);
	int buttonCenterY = (int)(button->rect.y + button->rect.height * 0.5f);

	int textCenterX = (int)(MeasureText(button->text, button->fontSize) * 0.5f);
	int textCenterY = (int)(button->fontSize * 0.5f);

	int textX = buttonCenterX - textCenterX;
	int textY = buttonCenterY - textCenterY;

	Color color = CheckCollisionPointRec(GetMousePosition(), button->rect) ? button->hoverColor : button->buttonColor;

	DrawRectangleRec(button->rect, color);
	DrawText(button->text, textX, textY, button->fontSize, button->textColor);
}

#endif