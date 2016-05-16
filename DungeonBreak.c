// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the edX Lab 15
// In order for other students to play your game
// 1) You must leave the hardware configuration as defined
// 2) You must not add/remove any files from the project
// 3) You must add your code only this this C file
// I.e., if you wish to use code from sprite.c or sound.c, move that code in this file
// 4) It must compile with the 32k limit of the free Keil

// April 10, 2014
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
 /* Hardware Info
// ******* Required Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// SSI0Fss       (SCE, pin 3) connected to PA3
// Reset         (RST, pin 4) connected to PA7
// Data/Command  (D/C, pin 5) connected to PA6
// SSI0Tx        (DN,  pin 6) connected to PA5
// SSI0Clk       (SCLK, pin 7) connected to PA2
// back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total
*/

#include <stdlib.h>
#include <stdbool.h>
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "global.h"
#include "dungeon.h"


Sprite monster_health[3];
Sprite shield, sword, potion, goblin, troll;
Sprite* sprite_list[8];
const int nsprites = (sizeof(sprite_list)/sizeof(sprite_list[0]));

// Global game parameters
int score;
int health;
PointXYZ player = {5*32, 5*32, 50};
int rotation;
#define DUNGEON_SIZE 4
Room dungeon[DUNGEON_SIZE][DUNGEON_SIZE];
const int room_data[DUNGEON_SIZE][DUNGEON_SIZE] = { 
						{ NW ,NS ,NS ,NE },
						{ EW ,NW ,NES,EW },
						{ W  ,SE ,NW ,SE },
						{ SEW,NW ,S  ,NES}};

// Game functions
void init_sprites(void);
void draw_board(void);
void reset_board(void);

/*********************************** GRAPHICS ******************************************/

#define BMP_WIDTH 18
#define BMP_HEIGHT 22
#define SCREENH 48
#define SCREENW 84

extern char Screen[SCREENW*SCREENH/8];

void myNokia5110_PrintBMP(unsigned char xpos, unsigned char ypos, const unsigned char *ptr, unsigned char threshold);

/********************************** CONTROLS ******************************************/

long ADCdata;

unsigned long TimerCount2A;
unsigned long Semaphore2A;

// Control functions
void ADC0_Init(void);
unsigned long ADC0_In(void);
void Button_Init(void);
void LED_Init(void);
void LED1(int);
void LED2(int);
void Timer2_Init(unsigned long period);
void Timer2A_Handler(void);
	
void game_over(void);
void draw_line(PointXY *a, PointXY *b);
void fill(PointXY *a, PointXY *b, PointXY *c, PointXY *d);
bool my3Dto2D (PointXY *screen_pos, const PointXYZ *point);

#define WALL_SIZE 100
#define TOPLEFT 0
#define TOPRIGHT 1
#define BOTTOMRIGHT 2
#define BOTTOMLEFT 3

void init_game()
{
	int i,j,k,wall,c;

	for (i=0; i<DUNGEON_SIZE; i++)
	{
		for (j=0; j<DUNGEON_SIZE; j++)
		{
			for (k=0, wall=N; k<4; k++, wall *= 2)
			{
#ifdef AERIAL_VIEW
				int x=j*WALL_SIZE;
				int y=i*WALL_SIZE;

				dungeon[i][j].wall[k].exists = room_data[i][j] & wall;
				if (dungeon[i][j].wall[k].exists)
				{
					for (c=TOPLEFT; c<=BOTTOMLEFT; c++)
					{
						if (c<BOTTOMLEFT)
							dungeon[i][j].wall[k].corners[c].z = WALL_SIZE;
						else
							dungeon[i][j].wall[k].corners[c].z = 0;
						switch(wall)
						{
							case N:
								if (c%2)
									dungeon[i][j].wall[k].corners[c].x = x + WALL_SIZE;
								else
									dungeon[i][j].wall[k].corners[c].x = x;
								dungeon[i][j].wall[k].corners[c].y = y;
								break;
							case E:
								if (c%2)
									dungeon[i][j].wall[k].corners[c].y = y + WALL_SIZE;
								else
									dungeon[i][j].wall[k].corners[c].y = y;
								dungeon[i][j].wall[k].corners[c].x = x + WALL_SIZE;
								break;
							case S:
								if (c%2)
									dungeon[i][j].wall[k].corners[c].x = x;
								else
									dungeon[i][j].wall[k].corners[c].x = x + WALL_SIZE;
								dungeon[i][j].wall[k].corners[c].y = y + WALL_SIZE;
								break;
							case W:
								if (c%2)
									dungeon[i][j].wall[k].corners[c].y = y;
								else
									dungeon[i][j].wall[k].corners[c].y = y + WALL_SIZE;
								dungeon[i][j].wall[k].corners[c].x = x;
								break;
							default:
								break;
						}
					}
				}
#else
				int x=i*WALL_SIZE;
				int z=j*WALL_SIZE;

				dungeon[i][j].wall[k].exists = room_data[i][j] & wall;
				if (dungeon[i][j].wall[k].exists)
				{
					for (c=TOPLEFT; c<=BOTTOMLEFT; c++)
					{
						if (c<BOTTOMRIGHT)
							dungeon[i][j].wall[k].corners[c].y = WALL_SIZE;
						else
							dungeon[i][j].wall[k].corners[c].y = 0;
						switch(wall)
						{
							case NORTH:
								if (c%3)
									dungeon[i][j].wall[k].corners[c].z = z + WALL_SIZE;
								else
									dungeon[i][j].wall[k].corners[c].z = z;
								dungeon[i][j].wall[k].corners[c].x = x;
								break;
							case EAST:
								if (c%3)
									dungeon[i][j].wall[k].corners[c].x = x + WALL_SIZE;
								else
									dungeon[i][j].wall[k].corners[c].x = x;
								dungeon[i][j].wall[k].corners[c].z = z + WALL_SIZE;
								break;
							case SOUTH:
								if (c%3)
									dungeon[i][j].wall[k].corners[c].z = z;
								else
									dungeon[i][j].wall[k].corners[c].z = z + WALL_SIZE;
								dungeon[i][j].wall[k].corners[c].x = x + WALL_SIZE;
								break;
							case WEST:
								if (c%3)
									dungeon[i][j].wall[k].corners[c].x = x;
								else
									dungeon[i][j].wall[k].corners[c].x = x + WALL_SIZE;
								dungeon[i][j].wall[k].corners[c].z = z;
								break;
							default:
								break;
						}
					}
				}
#endif
			}
		}
	}
	for (i=0; i<DUNGEON_SIZE; i++)
	{
		for (j=0; j<DUNGEON_SIZE; j++)
		{
			const int SCALE=8;
			PointXY p1,p2,p3,p4;
			p1.x = p4.x = j * SCALE;
			p2.x = p3.x = j * SCALE + SCALE;
			p1.y = p2.y = i * SCALE;
			p3.y = p4.y = i * SCALE + SCALE;
			if (room_data[i][j] & N) draw_line(&p1,&p2);
			if (room_data[i][j] & E) draw_line(&p2,&p3);
			if (room_data[i][j] & S) draw_line(&p3,&p4);
			if (room_data[i][j] & W) draw_line(&p4,&p1);
		}
	}
	Nokia5110_DisplayBuffer();
	Delay1ms(3000);
}

/*************************** CODE *****************************/

/* Main program */
int main(void)
{
	init();
	Nokia5110_DisplayBuffer();
	init_game();

	while (1)
	{
#ifdef TEST2D_DISPLAY_CODE
		int i;
		int step = 3;
		PointXY one, two, three, four;
		one.x=-SCREENW; one.y=0; two.x = SCREENW*2; two.y = SCREENH-1; 
		for (i=-SCREENW; i<SCREENW*2; i+=step)
		{
			one.x += step;
			two.x -= step;
			draw_line(&one,&two);
			Nokia5110_DisplayBuffer();
		}
		one.x=5; one.y=7; two.x = 55; two.y=15; three.x=one.x; three.y=44; four.x=two.x; four.y=47;
		fill(&one,&two,&three,&four);
		one.x=12; one.y=22; two.x = 32; two.y=18; three.x=12; three.y=32; four.x=32; four.y=34;
		fill(&one,&two,&three,&four);
		one.x=42; one.y=18; two.x = 62; two.y=22; three.x=42; three.y=34; four.x=62; four.y=30;
		fill(&one,&two,&three,&four);
		Nokia5110_DisplayBuffer();
#else
		PointXY corner[4];
		bool visible=false;
		int i,j,wall,c;
		player.x = 150;
		player.y = 060;
		player.z = 250;
		rotation = 0;
		for (rotation=0;;rotation = ++rotation % 360)
		{
			Nokia5110_ClearBuffer();

			for (i=0; i<DUNGEON_SIZE; i++)
			{
				for (j=0; j<DUNGEON_SIZE; j++)
				{
					for (wall=0; wall<4; wall++)
					{
						if ((wall==0 && player.x < i*WALL_SIZE) ||
								(wall==1 && player.z < j*WALL_SIZE) ||
								(wall==2 && player.x > i*WALL_SIZE+WALL_SIZE) ||
								(wall==3 && player.z > j*WALL_SIZE+WALL_SIZE))
							break;
						if (dungeon[i][j].wall[wall].exists)
						{
							visible = true;
							for (c=TOPLEFT; c<=BOTTOMLEFT; c++)
							{
								visible &= my3Dto2D (&corner[c], &dungeon[i][j].wall[wall].corners[c]);
							}
							if (visible)
							{
								fill(&corner[TOPLEFT],&corner[TOPRIGHT],&corner[BOTTOMLEFT],&corner[BOTTOMRIGHT]);
							}
						}
					}
				}
			}
			Delay1ms(50);
			Nokia5110_DisplayBuffer();
		}
#endif
		break;
	}
	Delay100ms(1); // delay 5 seconds
	game_over();
	while(1){}
}
/* Initialisation of constant global sprite values */
void init_sprites(void){
	int n=0; // sprite_list index
	// Shield
	shield.bitmap[0] = shield1BMP; // far
	shield.bitmap[1] = shield2BMP; // mid
	shield.bitmap[2] = shield3BMP; // near
	shield.bitmap[3] = shield4BMP; // equipped
	shield.exists = TRUE;
	sprite_list[n++] = &shield;
	// Sword
	sword.bitmap[0] = sword1BMP; // far
	sword.bitmap[1] = sword2BMP; // mid
	sword.bitmap[2] = sword3BMP; // near
	sword.bitmap[3] = sword4BMP; // equipped
	sword.exists = TRUE;
	// Goblin
	goblin.bitmap[0] = goblin1BMP; // far
	goblin.bitmap[1] = goblin2BMP; // mid
	goblin.bitmap[2] = goblin3BMP; // near
	goblin.bitmap[3] = goblin4BMP; // dead
	goblin.exists = TRUE;
	sprite_list[n++] = &goblin;
	// Troll
	troll.bitmap[0] = troll1BMP; // far
	troll.bitmap[1] = troll2BMP; // mid
	troll.bitmap[2] = troll3BMP; // near
	troll.bitmap[3] = troll4BMP; // dead
	troll.exists = TRUE;
	sprite_list[n++] = &troll;
}

/* Turn off a sprite */
void destroy(Sprite *s){
	s->exists=0;
	s->bitmapn=0;
}

/* Output a bitmap to the screen buffer */
void myNokia5110_PrintBMP(unsigned char xpos, unsigned char ypos, const unsigned char *ptr, unsigned char threshold){
  long width = ptr[BMP_WIDTH], height = ptr[BMP_HEIGHT], i, j;
  unsigned short screenx, screeny;
  unsigned char mask;
  // check for clipping
  if((height <= 0) ||              // bitmap is unexpectedly encoded in top-to-bottom pixel order
     ((width%2) != 0) ||           // must be even number of columns
     ((xpos + width) > SCREENW) || // right side cut off
     (ypos < (height - 1)) ||      // top cut off
     (ypos > SCREENH))           { // bottom cut off
		 return;
  }
  if(threshold > 14){
    threshold = 14;             // only full 'on' turns pixel on
  }
  // bitmaps are encoded backwards, so start at the bottom left corner of the image
  screeny = ypos/8;
  screenx = xpos + SCREENW*screeny;
  mask = ypos%8;                // row 0 to 7
  mask = 0x01<<mask;            // now stores a mask 0x01 to 0x80
  j = ptr[10];                  // byte 10 contains the offset where image data can be found
  for(i=1; i<=(width*height/2); i=i+1){
    // the left pixel is in the upper 4 bits
    if(((ptr[j]>>4)&0xF) > threshold)
      Screen[screenx] |= mask;
    screenx = screenx + 1;
    // the right pixel is in the lower 4 bits
    if((ptr[j]&0xF) > threshold)
      Screen[screenx] |= mask;
    screenx = screenx + 1;
    j = j + 1;
    if((i%(width/2)) == 0){     // at the end of a row
      if(mask > 0x01){
        mask = mask>>1;
      } else{
        mask = 0x80;
        screeny = screeny - 1;
      }
      screenx = xpos + SCREENW*screeny;
      // bitmaps are 32-bit word aligned
      switch((width/2)%4){      // skip any padding
        case 0: j = j + 0; break;
        case 1: j = j + 3; break;
        case 2: j = j + 2; break;
        case 3: j = j + 1; break;
      }
    }
  }
}
/* Draw all active sprites */
void draw_board(void){
	int i;
	Nokia5110_ClearBuffer();
	for (i=0; i<nsprites; i++)
		if (sprite_list[i]->exists)
			myNokia5110_PrintBMP(	sprite_list[i]->screenx,
													sprite_list[i]->screeny,
													sprite_list[i]->bitmap[sprite_list[i]->bitmapn],
													0);
  Nokia5110_DisplayBuffer();     // draw buffer
}
/* Hardware, Game and Sprite Initialisation */
void init (void){
  TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
  Random_Init(1);
	DAC_Init();
	ADC0_Init();
	Sound_Init();
	Button_Init();
	LED_Init();
  Nokia5110_Init();
	init_sprites();
	Timer2_Init(CLOCK_SPEED/60); // Create 60Hz interrupt
}

/* Display the GAME OVER screen */
void game_over(void){
	Nokia5110_Clear();
  Nokia5110_SetCursor(1, 1);
  Nokia5110_OutString("");
}

/* Initialise Timer 2 used for main control loop*/
void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R2; // 0a) activate timer2
//  SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3; // 0b) activate timer3
  delay = SYSCTL_RCGCTIMER_R;
// configure timer 2A
  TimerCount2A = 0;
  Semaphore2A = 0;
  TIMER2_CTL_R = 0x00000000;              // 1a) disable timer2 during setup
  TIMER2_CFG_R = TIMER_CFG_32_BIT_TIMER;  // 2a) configure timer2 for 32-bit mode
	TIMER2_TAMR_R |= TIMER_TAMR_TAMR_PERIOD; // 3a) periodic count down is default but needed by simulator
  TIMER2_TAILR_R = period-1;              // 4a) timer2A reload value
  TIMER2_TAPR_R = 0;                      // 5a) timer2A bus clock resolution
  TIMER2_ICR_R |= TIMER_ICR_TATOCINT;     // 6a) clear timer2A timeout flag
  TIMER2_IMR_R |= TIMER_IMR_TATOIM;       // 7a) arm timer2A timeout interrupt
	TIMER2_CTL_R |= TIMER_CTL_TAEN;         // 10a) enable timer2A
// interrupts are enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_PRI5_R =                           // 8) priority 4
			(NVIC_PRI5_R & (~NVIC_PRI5_INT23_M))
			| (4UL << NVIC_PRI5_INT23_S);
  NVIC_EN0_R = 1<<23;                     // 9) enable IRQ 23 in NVIC
}
/* Update player position ADC data and enable main loop to run */
void Timer2A_Handler(void){ 
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;   // acknowledge timer2A timeout
  TimerCount2A++;
  Semaphore2A = 1; // trigger
  ADCdata = (((long)ADC0_In()-515L)*28L)>>10;    // 3 - collect an ADC sample of player x pos
}
/* General purpose 100ms delay */
void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
/* General purpose 1ms delay */
void Delay1ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 7272;  // 0.001sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
/* Initialise the player position ADC hardware */
void ADC0_Init(void){
// This initialization function sets up the ADC 
// Max sample rate: <=125,000 samples/second
// SS3 triggering event: software trigger
// SS3 1st sample source:  channel 1
// SS3 interrupts: enabled but not promoted to controller
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= BIT(4);         // Enable GPIO Port E clock
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTE_DIR_R &= ~(BIT(2));    // PE2 is input
	GPIO_PORTE_DEN_R &= ~(BIT(2));    // Disable digital input on PE2
	GPIO_PORTE_AFSEL_R |= BIT(2);     // Enable PE2 alternate function
	GPIO_PORTE_AMSEL_R |= BIT(2);     // Enable PE2 analog functions
	GPIO_PORTE_PCTL_R &= ~(PCTL(2));    // Clear PCTL content for PE2 
  SYSCTL_RCGC0_R |= BIT(16);        // Enable ADC clock
	delay = SYSCTL_RCGC0_R;
	SYSCTL_RCGC0_R &= ~(SYSCTL_RCGC0_ADC0SPD_M); // Max Sample speed MAXADCSPD = 125 KHz
	SYSCTL_RCGC0_R |= SYSCTL_RCGC0_ADC0SPD_125K;
	ADC0_SSPRI_R = 0x3210L;           // SS3 is lowest priority
	ADC0_EMUX_R &= ~(0xF000L);        // EM3 is software triggered
	ADC0_SSCTL3_R = (BIT(1)|BIT(2));  // Interrupt enabled IE0=1, END0=1, TS0=0, D0=0
	ADC0_PSSI_R &= ~(BIT(3));         // Clear S/W request for SS3 sample
	ADC0_ACTSS_R |= BIT(3);           // Enable sequencer 3
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R & ~(0xF)) | 1L;
	ADC0_ISC_R |= BIT(3);             // Clear SS3 ADC reading ready flag
}
/* Initialise the game buttons */
void Button_Init(void){
	/* Fire button is positive logic switch on PE0 */
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE; // Enable GPIO Port E clock
	delay = SYSCTL_RCGC2_R;
	// PE0 is fire button
	GPIO_PORTE_DIR_R &= ~(BIT(0));        // PE0 is input
	GPIO_PORTE_DEN_R |= BIT(0);           // Enable digital input on PE0
	GPIO_PORTE_PDR_R |= BIT(0);           // Enable pull-down resistor on PE0
	GPIO_PORTE_AFSEL_R &= BIT(0);         // Disable PE0 alternate function
	GPIO_PORTE_AMSEL_R &= BIT(0);         // Disable PE0 analog functions
	GPIO_PORTE_PCTL_R &= ~(PCTL(0));      // Clear PCTL content for PE0
	// PE1 is special weapon
	GPIO_PORTE_DIR_R &= ~(BIT(1));        // PE1 is input
	GPIO_PORTE_DEN_R |= BIT(1);           // Enable digital input on PE1
	GPIO_PORTE_PDR_R |= BIT(1);           // Enable pull-down resistor on PE1
	GPIO_PORTE_AFSEL_R &= BIT(1);         // Disable PE1 alternate function
	GPIO_PORTE_AMSEL_R &= BIT(1);         // Disable PE1 analog functions
	GPIO_PORTE_PCTL_R &= ~(PCTL(1));      // Clear PCTL content for PE1
}
/* Request and wait for an ADC sample */
unsigned long ADC0_In(void){
//------------ADC0_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
	unsigned long ADC_value;
	ADC0_PSSI_R |= BIT(3);              // Request SS3 reading
	while (!(ADC0_RIS_R & BIT(3))){};   // wait for ADC sample capture
	ADC_value = ADC0_SSFIFO3_R & 0xFFF; // get sample
	ADC0_ISC_R |= BIT(3);               // Clear SS3 ADC reading ready flag
  return (ADC_value);
}
/* Initialise sound DAC */
void DAC_Init(void){
// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= GPIO_PORTB;
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTB_AMSEL_R &= ~(DAC_PINS);
	GPIO_PORTB_AFSEL_R &= ~(DAC_PINS);
	GPIO_PORTB_DIR_R   |=   DAC_PINS;
	GPIO_PORTB_PCTL_R  &= ~(DAC_PCTL_PINS);
	GPIO_PORTB_DR8R_R  |=   DAC_PINS;
	GPIO_PORTB_DEN_R   |=   DAC_PINS;
}
/* Output a single sound DAC step */ 
void DAC_Out(unsigned long data){
// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
	data = (data & (DAC_PINS)) | (GPIO_PORTB_DATA_R & ~(DAC_PINS));
  GPIO_PORTB_DATA_R = data;
}
/* Initialise the DAC hardware for 5.5 KHz wav sound playback */
void Sound_Init(void){
// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
  DAC_Init();
	NVIC_ST_CTRL_R = 0;
//	NVIC_ST_RELOAD_R = 7526; // 11 KHz sample rate
	NVIC_ST_RELOAD_R = 15052; // 5.5 KHz sample rate
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 7;
}
/* Turn LED1 on or off */
void LED1 (int on) {
	if (on)
		GPIO_PORTB_DATA_R |= BIT(4);
	else
		GPIO_PORTB_DATA_R &= ~(BIT(4));
}
/* Turn LED2 on or off */
void LED2 (int on) {
	if (on)
		GPIO_PORTB_DATA_R |= BIT(5);
	else
		GPIO_PORTB_DATA_R &= ~(BIT(5));
}
/* Start playing a sound */
void Sound_Tone(enum Sound sound){
// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
// this routine sets the RELOAD and starts SysTick
	sound_enabled[sound] = TRUE;
	if (sound == INVADER_SOUND)
		LED1(TRUE);
	else if (sound == SHOOT_SOUND)
		LED2(TRUE);
	DAC_step[sound] = 0;
}
/* Stop playing a sound */
void Sound_Off(int sound){
// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
 // this routine stops the sound output
	sound_enabled[sound] = FALSE;
	if (sound == INVADER_SOUND)
		LED1(FALSE);
	else if (sound == SHOOT_SOUND)
		LED2(FALSE);
}
/* Initialise LED hardware */
void LED_Init(void){
	/* LEDs are on PB4 and PB5 */
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // Enable GPIO Port B clock
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTB_DIR_R |=    (BIT(4)|BIT(5));    // PB4,5 are output
	GPIO_PORTB_AFSEL_R &= ~(BIT(4)|BIT(5));    // Disable alternate function
	GPIO_PORTB_AMSEL_R &= ~(BIT(4)|BIT(5));    // Disable analog functions
	GPIO_PORTB_PCTL_R &= ~(PCTL(4)|PCTL(5));   // Clear PCTL content
	GPIO_PORTB_DEN_R |=    (BIT(4)|BIT(5));    // Enable digital
}
/* Combine active sounds and pass to DAC hardware */
void SysTick_Handler(void){
// Interrupt service routine
// Executed at 11KHz
	int i;
	unsigned long DAC_acc=0;
	int num_sounds_active=0;
	for (i=0; i<MAX_SOUND; i++)
	{
		if (sound_enabled[i])
		{
			unsigned char DAC_val;
			num_sounds_active++;
			DAC_val = *(DAC_wav[i]+(DAC_step[i]/2));
			if (DAC_step[i]%2)
				DAC_acc += DAC_val&0xF;
			else
				DAC_acc += DAC_val>>4;
			if (DAC_step[i]/2 >= DAC_max[i])
				Sound_Off(i);
			DAC_step[i]++;
		}
		if (num_sounds_active > 0)
			DAC_Out(DAC_acc/num_sounds_active);
	}
}

/***************************************************************************************************
****************************************************************************************************
****************************************************************************************************/


void draw_pixel(PointXY *p, colour_e colour)
{
	if (p->x >= 0 && p->x < SCREENW && p->y >= 0 && p->y < SCREENH)
	{
		if (colour == BLACK)
			Screen[p->x+p->y/8*SCREENW] |= 1<<(p->y%8);
		else
			Screen[p->x+p->y/8*SCREENW] &= ~(1<<(p->y%8));
	}
}

void draw_line(PointXY *a, PointXY *b)
{
	int x, y, scale;
	PointXY *lo=a;
	PointXY *hi=b;
	if (abs(a->x - b->x) > abs(a->y - b->y))
	{
		if (a->x > b->x)
		{ lo = b; hi = a; }
		scale = ((hi->y - lo->y) << 6) / (hi->x - lo->x);
		for (x=lo->x; x<=hi->x; x++)
		{
			PointXY p;
			p.x = x;
			p.y = (((x - lo->x) * scale) >> 6) + lo->y;
			draw_pixel(&p, BLACK);
		}
	}
	else
	{
		if (a->y > b->y)
		{ lo = b; hi = a; }
		scale = ((hi->x - lo->x) << 6) / (hi->y - lo->y);
		for (y=lo->y; y<=hi->y; y++)
		{
			PointXY p;
			p.x = (((y - lo->y) * scale) >> 6) + lo->x;
			p.y = y;
			draw_pixel(&p, BLACK);
		}
	}
}

bool sign (PointXY *p, PointXY *a, PointXY *b)
{
	return ((p->x - b->x) * (a->y - b->y) - (a->x - b->x) * (p->y - b->y))>>31;
}

bool inside_triangle(PointXY *p, PointXY *a, PointXY *b, PointXY *c)
{
	bool b1,b2,b3;

	b1 = sign(p,a,b);
	b2 = sign(p,b,c);
	b3 = sign(p,c,a);

	return ((b1 == b2)&&(b2==b3));
}

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

bool inside_wall(PointXY *p, PointXY *topleft, PointXY *topright, PointXY *bottomleft, PointXY *bottomright)
{
	// sanity check corners
	if ((topleft->x != bottomleft->x) ||
			(topright->x != bottomright->x))
		for (;;)
		{
		// hang for debug
			break;
		}
	// check vertical bounds are ok
	if (p->x >= MAX(topleft->x,bottomleft->x) &&
			p->x <= MIN(topright->x,bottomright->x))
	{
		// check if its in the middle of the shape
		if (p->y >= MAX(topleft->y,topright->y) && p->y <= MIN(bottomleft->y,bottomright->y))
			return true;
		// Check if it could be in the top triangle
		else if (p->y >= MIN(topleft->y,topright->y) && p->y <= MAX(topleft->y,topright->y))
		{
			PointXY p2;
			if (topleft->y > topright->y)
			{
				p2.x = topright->x;
				p2.y = topleft->y;
			}
			else
			{
				p2.x = topleft->x;
				p2.y = topright->y;
			}
			return inside_triangle(p,topleft,topright,&p2);
		}
		// Check if it could be in the bottom triangle
		else if (p->y >= MIN(bottomleft->y,bottomright->y) && p->y <= MAX(bottomleft->y,bottomright->y))
		{
			PointXY p2;
			if (bottomleft->y > bottomright->y)
			{
				p2.x = bottomleft->x;
				p2.y = bottomright->y;
			}
			else
			{
				p2.x = bottomright->x;
				p2.y = bottomleft->y;
			}
			return inside_triangle(p,topleft,topright,&p2);
		}
	}
	return false;
}

void fill (PointXY *a, PointXY *b, PointXY *c, PointXY *d)
{
	int i,j;

	enum {
		topleft=0,
		bottomleft=1,
		topright=2,
		bottomright=3
	};
	
	PointXY *pXY[4];
	PointXY *pSwap;

	pXY[0]=a;
	pXY[1]=b;
	pXY[2]=c;
	pXY[3]=d;
	// sort points to get correct orientation for wall
	for (i=0; i<3; i++)
	{
		if (pXY[i]->x > pXY[i+1]->x || (pXY[i]->x == pXY[i+1]->x && pXY[i]->y > pXY[i+1]->y))
		{
			pSwap = pXY[i];
			pXY[i] = pXY[i+1];
			pXY[i+1] = pSwap;
			i=0;
		}
	}
	// normalize screen positions to -1 <= x <= SCREENW, -1 <= y <= SCREENH
	
	
	for (j=MIN(pXY[topleft]->y,pXY[topright]->y); j<=MAX(pXY[bottomright]->y,pXY[bottomleft]->y); j++)
	{
		for (i=MIN(pXY[topleft]->x,pXY[bottomleft]->x); i<=MAX(pXY[bottomright]->x,pXY[topright]->x); i++)
		{
			PointXY p;
			p.x = i;
			p.y = j;
			if (inside_wall(&p,pXY[topleft],pXY[topright],pXY[bottomleft],pXY[bottomright]))
				draw_pixel(&p,WHITE);
		}
		draw_line(pXY[topleft],pXY[topright]);
		draw_line(pXY[topleft],pXY[bottomleft]);
		draw_line(pXY[topright],pXY[bottomright]);
		draw_line(pXY[bottomleft],pXY[bottomright]);
	}
}

int isqrt(int n)
{
  int b = 0;
	n++;
  while(n >= 0)
  {
    n = n - b;
    b = b + 1;
    n = n - b;
  }
  return b - 1;
}

// one quarter of a sine wave in 1 degree steps scaled 2^15 to be transformed to full 360 degrees for sin and cos
const int sinS15[90] = {0,571,1143,1714,2285,2855,3425,3993,4560,5126,5690,6252,6812,7371,7927,8480,9032,9580,10125,10668,11207,11743,12275,12803,13327,13848,14364,14876,15383,15886,16384,16876,17364,17846,18323,18794,19260,19720,20173,20621,21062,21497,21926,22347,22762,23170,23571,23964,24351,24730,25101,25465,25821,26169,26509,26841,27165,27481,27788,28087,28377,28659,28932,29196,29451,29697,29935,30163,30381,30591,30791,30982,31164,31336,31498,31651,31794,31928,32051,32165,32270,32364,32449,32523,32588,32643,32688,32723,32748,32763};

int sine(int degrees)
{
	int i = degrees % 360;
	if (i < 0)
		degrees += 360;
	if (i < 90)
		return sinS15[i];
	if (i < 180)
		return sinS15[89-i%90];
	if (i < 270)
		return -sinS15[i%90];
	return -sinS15[89-i%90];
}

int cosine(int degrees)
{
	return (sine(degrees+90));
}

#define ZOOM 32
bool my3Dto2D (PointXY *screen_pos, const PointXYZ *point)
{
	int x = point->x - player.x;
	int y = point->y - player.y;
	int z = point->z - player.z;
	int sinR = sine(rotation);
	int cosR = cosine(rotation);
	long dx = cosR*x - sinR*z;
	long dy = y<<15;
	long dz = cosR*z + sinR*x;
	if (dz <= 0) return false;
	screen_pos->x = ZOOM*dx/dz + SCREENW/2;
	screen_pos->y = ZOOM*dy/dz + SCREENH/2;
	if (screen_pos->x < -SCREENW/2) screen_pos->x = -SCREENW/2;
	else if (screen_pos->x > SCREENW+SCREENW/2) screen_pos->x = SCREENW+SCREENW/2;
	if (screen_pos->y < -SCREENH/2) screen_pos->y = -SCREENH/2;
	else if (screen_pos->y > SCREENH+SCREENH/2) screen_pos->y = SCREENH+SCREENH/2;
	return true;
}
