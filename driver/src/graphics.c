#include <graphics.h>

void draw_centered_text(char* text, int textSize, int x, int y, Color color) {
    DrawText(text, x - MeasureText(text, textSize) / 2, y - textSize / 2, textSize, color);
}