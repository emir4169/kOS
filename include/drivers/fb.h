#include <stdint.h>
#define FONT_WIDTH 8
void init_graphics(void);
static void set_pixel(int x, int y, uint8_t color);
void draw_rectangle(int x, int y, int width, int height, uint8_t color);
void draw_line(int x1, int y1, int x2, int y2, uint8_t color);

// Helper function for setting a pixel, used internally
static void set_pixel(int x, int y, uint8_t color);