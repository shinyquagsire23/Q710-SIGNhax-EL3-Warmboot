#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "text.h"
#include "tinyprintf.h"

#include "sbl.h"

#define SCREEN_BORDERCOLOR 0x905090
#define SCREEN_BGCOLOR 0x300040
#define CONSOLE_MARGIN_X 24
#define CONSOLE_MARGIN_Y 64
#define FB_WIDTH 1080
#define FB_HEIGHT 2160
#define FB_BYTES_PER_PIXEL 3

extern const uint8_t msx_font[];
static uint32_t console_x = CONSOLE_MARGIN_X;
static uint32_t console_y = CONSOLE_MARGIN_Y;

void plotPixel(uint16_t x, uint16_t y, uint32_t color)
{
    if (x >= FB_WIDTH || y >= FB_HEIGHT) return;

    uint8_t *pixel = (uint8_t*)&sbl_framebuffer[FB_BYTES_PER_PIXEL * (x + y * FB_WIDTH)];
    char tmp[0x100];
    snprintf(tmp, 0x100, "%x,%x %lx\n\r", x, y, color);
    //sbl_uart_log_msg(tmp);

    pixel[0] = (color & 0xFF);
    pixel[1] = (color & 0xFF00) >> 8;
    pixel[2] = (color & 0xFF0000) >> 16;
}

void drawRoundedRectangle(int x0, int y0, int x1, int y1, int radius, uint32_t col)
{
    int f;
    int ddF_x;
    int ddF_y;
    int xx;
    int yy;
   
    f = 1 - radius;
    ddF_x = 1;
    ddF_y = -2 * radius;
    xx = 0;
    yy = radius;
   
    while (xx < yy)
    {
        if (f >= 0)
        {
            yy -= 1;
            ddF_y += 2;
            f += ddF_y;
        }
        xx += 1;
        ddF_x += 2;
        f += ddF_x;
        plotPixel(x1 + xx - radius, y1 + yy - radius, col);
        plotPixel(x1 + yy - radius, y1 + xx - radius, col);
        plotPixel(x0 - xx + radius, y1 + yy - radius, col);
        plotPixel(x0 - yy + radius, y1 + xx - radius, col);
        plotPixel(x1 + xx - radius, y0 - yy + radius, col);
        plotPixel(x1 + yy - radius, y0 - xx + radius, col);
        plotPixel(x0 - xx + radius, y0 - yy + radius, col);
        plotPixel(x0 - yy + radius, y0 - xx + radius, col);
    }
    
    for (int x = x0 + radius; x <= x1 - radius; x++)
    {
        plotPixel(x, y0, col);
        plotPixel(x, y1, col);
    }
   
    for (int y = y0 + radius; y <= y1 - radius; y++)
    {
        plotPixel(x0, y, col);
        plotPixel(x1, y, col);
    }
}

void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) 
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) 
    {
        if (f >= 0) 
        {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }

        x++;
        ddF_x += 2;
        f     += ddF_x;

        if (cornername & 0x4)
        {
            char tmp[0x80];
            snprintf(tmp, 0x80, "%u %u %x", x0 + x, y0 + y, color);
            sbl_uart_log_msg(tmp);
            plotPixel(x0 + x, y0 + y, color);
            plotPixel(x0 + y, y0 + x, color);
        } 

        if (cornername & 0x2)
        {
            plotPixel(x0 + x, y0 - y, color);
            plotPixel(x0 + y, y0 - x, color);
        }

        if (cornername & 0x8)
        {
            plotPixel(x0 - y, y0 + x, color);
            plotPixel(x0 - x, y0 + y, color);
        }
        
        if (cornername & 0x1)
        {
            plotPixel(x0 - y, y0 - x, color);
            plotPixel(x0 - x, y0 - y, color);
        }
    }
}

#define SCALE 1
#define FONT_WIDTH 32
#define FONT_HEIGHT 32
void plotPixel_scaled(uint16_t x, uint16_t y, uint32_t color)
{
    for (int i = 0; i < SCALE; i++)
    {
        for (int j = 0; j < SCALE; j++)
        {
            plotPixel(x*SCALE + j, y*SCALE + i, color);
        }
    }
}

uint8_t drawCharacter(char c, uint16_t x, uint16_t y, uint32_t color)
{
    uint8_t *font = (uint8_t*)(msx_font + (c - ' ') * (32*4));
    int i, j, k;
    int font_x = 0;
    uint8_t max_x = 0;
    for (i = 0; i < FONT_HEIGHT; i++)
    {
        font_x = 0;
        for (j = 0; j < FONT_WIDTH/8; ++j)
        {
            for (k = 0; k < 8; k++)
            {
                if (*font & (0x80 >> k))
                {
                    if (font_x > max_x)
                        max_x = font_x;
                    plotPixel_scaled(x+font_x, y+i, color);
                }
                ++font_x;
            }
            ++font;
        }
    }

    return max_x;
}

void drawString(char* str, uint16_t x, uint16_t y)
{
    drawStringColor(str,x,y,0xFFFFFF);
}

void drawStringColor(char* str, uint16_t x, uint16_t y, uint32_t color)
{
    if(!str)return;
    int k;
    int dx=0, dy=0;
    for(k=0;k<strlen(str);k++)
    {
        if(str[k] > ' ' && str[k] < 128)
            dx += drawCharacter(str[k],x+dx,y+dy,color) + 4;

        if (str[k] == ' ')
            dx += 24;

        if(str[k]=='\n'){dx=0;dy+=FONT_HEIGHT;}
    }
    
    
}

void centerString(char* str, uint16_t y, uint16_t screen_x)
{
    drawString(str, (screen_x-(strlen(str)*FONT_WIDTH))/2, y);
}

void clearScreen()
{
    memset(sbl_framebuffer, 0, FB_WIDTH*FB_HEIGHT*FB_BYTES_PER_PIXEL);
    //draw_border((uint8_t*)&LINEAR_BUFFER[0x00100000], 10, 10, 375, 215, 3, 0x00a0ff);
}

void screen_puts(char* str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        screen_putc(NULL, str[i]);
    }
}

char screenbuf_tmp[0x100];
int screenbuf_pos = 0;

void screen_flushbuf()
{
    screenbuf_tmp[screenbuf_pos++] = '\r';
    screenbuf_tmp[screenbuf_pos++] = 0;
    //sbl_uart_log_msg(screenbuf_tmp);
    screenbuf_pos = 0;
}

void screen_putc(void* putp, char c)
{
    screenbuf_tmp[screenbuf_pos++] = c;

    if(c == '\n' || console_x+32 >= FB_WIDTH-CONSOLE_MARGIN_X)
    {
        console_x = CONSOLE_MARGIN_X;
        console_y += FONT_HEIGHT;
        sbl_fbupdate();
        
        screen_flushbuf();
    }
    
    if(c > ' ' && c < 128)
        console_x += drawCharacter(c, console_x, console_y, 0xFFFFFF) + 4;
    
    if (c == ' ')
        console_x += 24;

    if (console_y >= FB_HEIGHT-CONSOLE_MARGIN_Y)
    {
        screen_clear();
        screen_flushbuf();
    }
}

void screen_init()
{
    screen_clear();
    draw_border(10, SCREEN_BORDERCOLOR);
    
    init_printf(NULL, screen_putc);
    screenbuf_pos = 0;
}

void screen_clear()
{
    sbl_fbfill(SCREEN_BGCOLOR, 0xFFFFFF);
    console_x = CONSOLE_MARGIN_X;
    console_y = CONSOLE_MARGIN_Y;
}

void draw_border(uint8_t border_width, uint32_t color)
{
    //int x = 0, y = 0;
    
    for (int i = 0; i < border_width; i++)
    {
        drawRoundedRectangle(0+i, 0+i, FB_WIDTH-1-i, FB_HEIGHT-1-i, 64-i, color);
    }

    /*for(y = screen_y; y < screen_h + screen_y + border_width; y++)
    {
        for(x = screen_x; x < screen_w + screen_x + border_width; x++)
        {
            if((x >= screen_x && x <= screen_x + border_width) ||
               (y >= screen_y && y <= screen_y + border_width) ||
               (x >= screen_w + screen_x - 1 && x <= screen_w + screen_x - 1 + border_width) ||
               (y >= screen_h + screen_y - 1 && y <= screen_h + screen_y - 1 + border_width))
                {
                    plotPixel_scaled(x, y, color);
                }
        }
    }*/
}

const uint8_t hexTable[]=
{
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

void hex2str(char* out, uint32_t val)
{
    int i;
    for(i=0;i<8;i++){out[7-i]=hexTable[val&0xf];val>>=4;}
    out[8]=0x00;
}
