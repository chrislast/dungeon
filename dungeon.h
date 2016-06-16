#ifndef _DUNGEON_H_
#define _DUNGEON_H_

#include "global.h"

// Control functions
void ADC0_Init(void);
unsigned long ADC0_In(void);
void Button_Init(void);
void LED_Init(void);
void LED1(int);
void LED2(int);
void Timer2_Init(unsigned long period);
void Timer2A_Handler(void);
int sine(int degrees);
int cosine(int degrees);
void game_over(void);
void draw_line(const PointXY *a, const PointXY *b);
void fill (const WallXY *wall, int shading);
bool my3Dto2D (PointXY *screen_pos, const PointXYZ *point);
void draw_pixel(PointXY *p, colour_e colour);
int isqrt(int n);
int distance (PointXYZ a, PointXYZ b);	
void show_map(void);
void handle_movement(void);
void UART0_OutChar(unsigned char data);
void trace (unsigned char *message);
void draw_maze(void);
void init_game(void);
void draw_fixed_objects(void);
void handle_combat(void);
void update_display(void);
void display_small_int(char *screen_char, int value, int shift);
void set_pixel(int screenx, int screeny);

#endif
