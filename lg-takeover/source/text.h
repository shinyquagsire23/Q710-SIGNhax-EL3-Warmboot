#ifndef TEXT_H
#define TEXT_H

uint8_t drawCharacter(char c, uint16_t x, uint16_t y, uint32_t color);
void drawString(char* str, uint16_t x, uint16_t y);
void drawStringColor(char* str, uint16_t x, uint16_t y, uint32_t color);

void centerString(char* str, uint16_t y, uint16_t screen_x);
void clearScreen();
void draw_border(uint8_t border_width, uint32_t color);

void hex2str(char* out, uint32_t val);

void screen_puts(char* str);
void screen_putc(void* putp, char c);
void screen_init();
void screen_clear();

#endif
