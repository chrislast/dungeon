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
#include "Texas.h"
#include "Random.h"
#include "global.h"
#include "dungeon.h"

#define WALL_LENGTH 100
#define WALL_HEIGHT 45
#define MAX_VIEW_RANGE 10
#define DUNGEON_SIZE 8

#define ASPECT_RATIO_ZOOM 40

#define TOP_LEFT_CORNER (0)
#define FIRST_CORNER (TOP_LEFT_CORNER)
#define TOP_RIGHT_CORNER (1)
#define BOTTOM_RIGHT_CORNER (2)
#define BOTTOM_LEFT_CORNER (3)
#define LAST_CORNER (BOTTOM_LEFT_CORNER)

#define NORTH_WALL (0)
#define FIRST_WALL (NORTH_WALL)
#define EAST_WALL (1)
#define SOUTH_WALL (2)
#define WEST_WALL (3)
#define LAST_WALL (WEST_WALL)

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))

// bitmap files
#include "bmp.txt"

// Global game parameters
int score;
int health;
bool shield_equipped = false;
bool sword_equipped = false;
bool armour_equipped = false;
bool in_combat = false;

typedef struct Player
{
	PointXYZ pos;
	int rotation;
} Player;
		
Room dungeon[DUNGEON_SIZE][DUNGEON_SIZE];

#if (DUNGEON_SIZE == 3)
	Player player = {{150,24,050},90};
	const int room_data[DUNGEON_SIZE][DUNGEON_SIZE] = { 
						{ NW ,NS ,NE },
						{ W  ,N  ,E  },
						{ SW ,S  ,SE }};
	Object sword= {swordBMP, {180,34,020},true,NULL};
	Object armour={armourBMP,{280,34,020},true,NULL};
	Object shield={shieldBMP,{180,32,120},true,NULL};
	Object goblin1={goblinBMP,{050,32,150},true,NULL};
	Object goblin2={goblinBMP,{050,32,150},true,NULL};
	Object potion={potionBMP,{250,36,250},true,NULL};
	Object troll= {trollBMP, {050,28,050},true,NULL};
#elif (DUNGEON_SIZE == 4)
	Player player = {{150,24,250},270};
	const int room_data[DUNGEON_SIZE][DUNGEON_SIZE] = { 
						{ NW ,NS ,NS ,NE },
						{ EW ,NW ,NES,EW },
						{ W  ,SE ,NW ,SE },
						{ SEW,NW ,S  ,NES}};
	Object sword= {swordBMP, {180,34,020},true,NULL};
	Object armour={armourBMP,{280,34,020},true,NULL};
	Object shield={shieldBMP,{180,32,120},true,NULL};
	Object goblin1={goblinBMP,{050,32,150},true,NULL};
	Object goblin2={goblinBMP,{050,32,150},true,NULL};
	Object potion={potionBMP,{250,36,250},true,NULL};
	Object troll= {trollBMP, {050,28,050},true,NULL};
#elif (DUNGEON_SIZE == 6)
	Player player = {{550,24,550},315};
	const int room_data[DUNGEON_SIZE][DUNGEON_SIZE] = { 
						{ NW ,N  ,NS ,N  ,N  ,NE },
						{ W  ,S  ,NE ,W  ,0  ,E  },
						{ EW ,NSW,0  ,0  ,0  ,E  },
						{ W  ,NE ,SEW,SW ,0  ,E  },
						{ W  ,0  ,NS ,NS ,0  ,E  },
						{ SW ,S  ,NS ,NS ,S  ,SE }};
	Object sword= {swordBMP, {180,34,020},true,NULL};
	Object armour={armourBMP,{280,34,020},true,NULL};
	Object shield={shieldBMP,{180,32,120},true,NULL};
	Object goblin1={goblinBMP,{050,32,150},true,NULL};
	Object goblin2={goblinBMP,{050,32,150},true,NULL};
	Object potion={potionBMP,{250,36,250},true,NULL};
	Object troll= {trollBMP, {050,28,050},true,NULL};
#elif (DUNGEON_SIZE == 8)
	Player player = {{750,24,350},315};
	const int room_data[DUNGEON_SIZE][DUNGEON_SIZE] = { 
						{ NEW,NSW,N  ,NS ,NS ,N  ,NS ,NE },
						{ W  ,NES,EW ,NW ,NES,EW ,NW ,SE },
						{ EW ,NW ,E  ,W  ,N  ,SE ,SW ,NE },
						{ W  ,E  ,SW ,SE ,EW ,NW ,NS ,SE },
						{ EW ,SW ,N  ,NS ,E  ,W  ,NS ,NS },
						{ EW ,NSW,S  ,NE ,EW ,SW ,NS ,NE },
						{ W  ,N  ,NS ,S  ,0  ,NS ,NES,EW },
						{ SEW,SW ,NS ,NES,SEW,NSW,NS ,SE }};
	Object sword=  {swordBMP, {150,34,180},true,NULL};
	Object armour= {armourBMP,{780,34,450},true,NULL};
	Object shield= {shieldBMP,{750,32,520},true,NULL};
	Object goblin1={goblinBMP,{450,32,250},true,NULL};
	Object goblin2={goblinBMP,{250,32,450},true,NULL};
	Object potion= {potionBMP,{750,36,050},true,NULL};
	Object troll=  {trollBMP, {450,28,750},true,NULL};
#endif
	Object *objects[]={&sword,&armour,&shield,&goblin1,&goblin2,&potion,&troll};
	
	const unsigned char *compass[8]={compassNBMP,compassNEBMP,compassEBMP,compassSEBMP,compassSBMP,compassSWBMP,compassWBMP,compassNWBMP};

// Game functions
void init_sprites(void);
void draw_board(void);
void reset_board(void);

/*********************************** GRAPHICS ******************************************/

extern char Screen[SCREENW*SCREENH/8];

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

void init_game()
{
	int i,j,k,wall,c;
	// For each room
	for (i=0; i<DUNGEON_SIZE; i++)
	{
		for (j=0; j<DUNGEON_SIZE; j++)
		{
			// Clear the object list
			dungeon[i][j].objects = NULL;
			// Store the wall co-ordinates
			for (k=FIRST_WALL, wall=NORTH; k<=LAST_WALL; k++, wall *= 2)
			{
				int x=i*WALL_LENGTH;
				int z=j*WALL_LENGTH;

				dungeon[i][j].wall[k].exists = room_data[i][j] & wall;
				if (dungeon[i][j].wall[k].exists)
				{
					for (c=FIRST_CORNER; c<=LAST_CORNER; c++)
					{
						if (c == TOP_LEFT_CORNER || c == TOP_RIGHT_CORNER)
							dungeon[i][j].wall[k].corners[c].y = WALL_HEIGHT;
						else
							dungeon[i][j].wall[k].corners[c].y = 0;
						switch(wall)
						{
							case NORTH:
								if (c == TOP_RIGHT_CORNER || c == BOTTOM_RIGHT_CORNER)
									dungeon[i][j].wall[k].corners[c].z = z + WALL_LENGTH;
								else
									dungeon[i][j].wall[k].corners[c].z = z;
								dungeon[i][j].wall[k].corners[c].x = x;
								break;
							case EAST:
								if (c == TOP_RIGHT_CORNER || c == BOTTOM_RIGHT_CORNER)
									dungeon[i][j].wall[k].corners[c].x = x + WALL_LENGTH;
								else
									dungeon[i][j].wall[k].corners[c].x = x;
								dungeon[i][j].wall[k].corners[c].z = z + WALL_LENGTH;
								break;
							case SOUTH:
								if (c == TOP_RIGHT_CORNER || c == BOTTOM_RIGHT_CORNER)
									dungeon[i][j].wall[k].corners[c].z = z;
								else
									dungeon[i][j].wall[k].corners[c].z = z + WALL_LENGTH;
								dungeon[i][j].wall[k].corners[c].x = x + WALL_LENGTH;
								break;
							case WEST:
								if (c == TOP_RIGHT_CORNER || c == BOTTOM_RIGHT_CORNER)
									dungeon[i][j].wall[k].corners[c].x = x;
								else
									dungeon[i][j].wall[k].corners[c].x = x + WALL_LENGTH;
								dungeon[i][j].wall[k].corners[c].z = z;
								break;
							default:
								break;
						}
					}
				}
			}
		}
	}
	// add objects to rooms
	for (i=0; i < (sizeof(objects)/sizeof(Object *)); i++)
	{
		int x = objects[i]->pos.z/WALL_LENGTH;
		int y = objects[i]->pos.x/WALL_LENGTH;
		dungeon[y][x].objects = objects[i];
		// if height is not defined use half bitmap height
		if (!dungeon[y][x].objects->pos.y)
			dungeon[y][x].objects->pos.y = WALL_HEIGHT - dungeon[y][x].objects->bitmap[22]/2;
	}
	// Preview the map on screen briefly
	show_map();
	Delay1ms(5000);
}

void show_map(void)
{
	int i,j,px=20,py=20;

	Nokia5110_ClearBuffer();
	for (i=0; i<DUNGEON_SIZE; i++)
	{
		for (j=0; j<DUNGEON_SIZE; j++)
		{
			const int SCALE=(SCREENH/DUNGEON_SIZE);
			PointXY p1,p2,p3,p4;
			p1.x = p4.x = j * SCALE;
			p2.x = p3.x = j * SCALE + SCALE;
			p1.y = p2.y = i * SCALE;
			p3.y = p4.y = i * SCALE + SCALE; 
			if (room_data[i][j] & NORTH) draw_line(&p1,&p2);
			if (room_data[i][j] & EAST)  draw_line(&p2,&p3);
			if (room_data[i][j] & SOUTH) draw_line(&p3,&p4);
			if (room_data[i][j] & WEST)  draw_line(&p4,&p1);
			if (dungeon[i][j].objects && dungeon[i][j].objects->exists)
				myNokia5110_PrintResizedBMP (p4.x, p4.y, SCALE, 0x09, dungeon[i][j].objects->bitmap);
			if ((player.pos.x/100) == i && (player.pos.z/100) == j)
			{
				// save the player position x,y coordinates
				px = p4.x; py = p4.y;
			}
		}
	}
	// show player position
	myNokia5110_PrintBMP (px, py, playerBMP, 0x09);
	Nokia5110_DisplayBuffer();
}
/*************************** CODE *****************************/

void draw_maze (void)
{
	// start at maximum (possibly off map) distance from camera
	int px = player.pos.z/100;
	int py = player.pos.x/100;
	int x,y,wall,range;
	const WallXY ceiling = {{{0,SCREENH/2},{SCREENW-1,SCREENH/2},{SCREENW-1,0},{0,0}}};
	const WallXY floor = {{{0,SCREENH-1},{SCREENW-1,SCREENH-1},{SCREENW-1,SCREENH/2},{0,SCREENH/2}}};
	fill(&ceiling,1);
	fill(&floor,4);
	for (range = MIN(DUNGEON_SIZE,MAX_VIEW_RANGE); range >=0; range--)
	{
		for (y=py-range; y <= py+range; y++)
		{
			for ( x=px-(range-(abs(py-y))); x <= px+(range-(abs(py-y))); x += MAX(1,2*(range-(abs(py-y)))))
			{
				// if it's not on the map
				if (x < 0 || y < 0 || x >= DUNGEON_SIZE || y >= DUNGEON_SIZE)
				{
					// then get next cell
					continue;
				}
				// show walls of current cell
				for (wall=FIRST_WALL; wall<=LAST_WALL; wall++)
				{
					if (dungeon[y][x].wall[wall].exists)
					{
						int corner;
						bool in_front = false;  // wall is in front of camera
						WallXY wall2d;
						// if wall is not visible from camera position then get the next wall
						if (((wall == NORTH_WALL) && (player.pos.x/WALL_LENGTH < y)) ||
						  	((wall == EAST_WALL)  && (player.pos.z/WALL_LENGTH > x+1)) ||
								((wall == SOUTH_WALL) && (player.pos.x/WALL_LENGTH > y+1)) ||
								((wall == WEST_WALL)  && (player.pos.z/WALL_LENGTH < x)))
						{
							continue;
						}
						// calculate screen-based coordinates of each corner
						for (corner = FIRST_CORNER; corner <= LAST_CORNER; corner++)
						{
							in_front |= my3Dto2D (&wall2d.corner[corner], &dungeon[y][x].wall[wall].corners[corner]);
						}
						// if any of the corners are in front of the camera then attempt to draw the wall (still could be off screen)
						if (in_front)
						{
							PointXY tpos2d;
							PointXYZ *ptpos3d = dungeon[y][x].wall[wall].corners;
							PointXYZ tpos3d;
							int dist;
							// Draw the wall
							fill(&wall2d,10);
							// and add a torch to the centre of the wall
							tpos3d.x = (ptpos3d[0].x + ptpos3d[1].x + ptpos3d[2].x + ptpos3d[3].x) / 4;
							tpos3d.y = (ptpos3d[0].y + ptpos3d[1].y + ptpos3d[2].y + ptpos3d[3].y) / 4;
							tpos3d.z = (ptpos3d[0].z + ptpos3d[1].z + ptpos3d[2].z + ptpos3d[3].z) / 4;
							my3Dto2D (&tpos2d, &tpos3d);
							dist = distance(player.pos,tpos3d);
							myNokia5110_PrintResizedBMP(
								tpos2d.x, tpos2d.y, 										// screen x,y
								ASPECT_RATIO_ZOOM * torch0BMP[18] / dist,	// resized width
								9, torch0BMP);
						}
						if (dungeon[y][x].objects && dungeon[y][x].objects->exists)
						{
							Object* pObj = dungeon[y][x].objects;
							while (pObj)
							{
								PointXY spos;
								bool visible;

								int dist = distance(pObj->pos,player.pos);
								int width = ASPECT_RATIO_ZOOM * pObj->bitmap[18] / dist;
								int height = ASPECT_RATIO_ZOOM * pObj->bitmap[22] / dist;
								visible = my3Dto2D (&spos, &pObj->pos);
								if ( visible && width > 0 && height > 0 )
								{
									myNokia5110_PrintResizedBMP (spos.x-width/2, spos.y+height/2, width, 0x09, pObj->bitmap);
								}
								// Get next Object
								pObj = dungeon[y][x].objects->next;
							}
						}
					}
				}
			}
		}
	}
}

int isqrt(int n)
{
  int b = 0;
  while(n >= 0)
  {
    n = n - b;
    b = b + 1;
    n = n - b;
  }
  return b - 1;
}

int distance (PointXYZ a, PointXYZ b)
{
	int dx = MAX(a.z,b.z) - MIN(a.z,b.z);
	int dy = MAX(a.x,b.x) - MIN(a.x,b.x);
	return isqrt(dx*dx+dy*dy);
}

void draw_fixed_objects(void)
{
	// equipped items
	if (shield_equipped) myNokia5110_PrintBMP(0,SCREENH-1,equippedshieldBMP,9);
	if (sword_equipped) myNokia5110_PrintBMP(SCREENW-1-equippedswordBMP[18],SCREENH-1,equippedswordBMP,9);
	if (armour_equipped) myNokia5110_PrintBMP(SCREENW/2-equippedarmourBMP[18]/2,SCREENH-1,equippedarmourBMP,9);
	// show compass
	myNokia5110_PrintBMP(0,9,compass[((player.rotation+112)%360)/45],9);
	// show health
	
}	

void show_message (char *message, const int delayms)
{
		Nokia5110_Clear();
		Nokia5110_SetCursor(0,0);
		Nokia5110_OutString(message);
		Delay1ms(delayms);
}

/* Main program */
int main(void)
{
 	init();
	Nokia5110_DisplayBuffer();
	init_game();

	while (1)
	{
		Nokia5110_ClearBuffer();
		draw_maze();
		draw_fixed_objects();
		Delay1ms(20);
		Nokia5110_DisplayBuffer();
		player.rotation = (player.rotation + ADCdata + 360) % 360;
		if (GPIO_PORTE_DATA_R & BIT(0))
		{
			Object *pObj;
			int oldx=player.pos.x;
			int oldz=player.pos.z;
			int x,y;
			player.pos.x += ((sine(player.rotation)>>13)+1)>>1;
			y = player.pos.x / WALL_LENGTH;
			player.pos.z += ((cosine(player.rotation)>>13)+1)>>1;
			x = player.pos.z / WALL_LENGTH;
			if (dungeon[y][x].wall[SOUTH_WALL].exists && (player.pos.x % WALL_LENGTH > WALL_LENGTH * 9 / 10))
				player.pos.x = y * WALL_LENGTH + WALL_LENGTH * 9 / 10;
			else if (dungeon[y][x].wall[NORTH_WALL].exists && (player.pos.x % WALL_LENGTH < WALL_LENGTH * 1 / 10))
				player.pos.x = y * WALL_LENGTH + WALL_LENGTH * 1 / 10;
			if (dungeon[y][x].wall[EAST_WALL].exists && (player.pos.z % WALL_LENGTH > WALL_LENGTH * 9 / 10))
				player.pos.z = x * WALL_LENGTH + WALL_LENGTH * 9 / 10;
			else if (dungeon[y][x].wall[WEST_WALL].exists && (player.pos.z % WALL_LENGTH < WALL_LENGTH * 1 / 10))
				player.pos.z = x * WALL_LENGTH + WALL_LENGTH * 1 / 10;
			pObj = dungeon[y][x].objects;
			while (pObj)
			{
				if (pObj->exists && distance(player.pos,pObj->pos) < MAX(pObj->bitmap[22],pObj->bitmap[18])/2)
				{
					if (pObj == &armour)
					{
						armour_equipped = true;
						armour.exists = false;
						show_message (
							"            "
							"You've found"
							"the armor it"
							"will reduce "
							"damage taken",
							2000);
					}
					else if (pObj == &sword)
					{
						sword_equipped = true;
						sword.exists = false;
						show_message (
							"You've found"
							" the sword  "
							"Keep enemies"
							"on the right"
							"to do damage"
							"  with it",
							3000);
					}
					else if (pObj == &shield)
					{
						shield_equipped = true;
						shield.exists = false;
						show_message (
							"You've found"
							" the shield "
							"Keep enemies"
							" on left to "
							" block more "
							"   damage",
							3000);
					}
					else if (pObj == &potion && health < 100)
					{
						health = 100;
						potion.exists = false;
						show_message (
							"            "
							" The health "
							"   potion   "
							"  restores  "
							"    you",
							2000);
					}
					else // implement no-go zone for monsters!
					{
						player.pos.x = oldx;
						player.pos.z = oldz;
						in_combat = true;
					}
				}
				pObj = pObj->next;
			}
		}
		if (GPIO_PORTE_DATA_R & BIT(1))
		{
			show_map();
			while (!GPIO_PORTE_DATA_R & BIT(1));
		}
	}
}
/* Initialisation of constant global sprite values */
void init_sprites(void){
}

/* Turn off a sprite */
void destroy(Sprite *s){
	s->exists=0;
	s->bitmapn=0;
}


/* Draw all active sprites 
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
}*/
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
  ADCdata = ((long)(ADC0_In()-0x7FF))>>8;    // 3 - collect an ADC sample of player x pos
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

/* Draw the visible part of a line between two screen co-ordinates */
void draw_line(const PointXY *a, const PointXY *b)
{
	int slope,x,y;

	// if line is not vertical
//	if (a->x != b->x)
	if (MAX(a->x,b->x)-MIN(a->x,b->x) > MAX(a->y,b->y)-MIN(a->y,b->y))
	{
  	// draw a pixel for each valid x position between line start and end
		for (x=MAX(0,MIN(a->x,b->x)); x<MIN(SCREENW,MAX(a->x,b->x)); x++)
		{
			slope = (((a->y - b->y)<<15)/(a->x - b->x));
			y = (slope*(x - a->x)>>15) + a->y;
			if (y>=0 && y<SCREENH)
				Screen[x+y/8*SCREENW] |= 1<<(y%8);
		}
	}
	// if line is not horizontal
//	if (a->y != b->y)
	else
	{
	// draw a pixel for each valid y position between line start and end
		for (y=MAX(0,MIN(a->y,b->y)); y<MIN(SCREENH,MAX(a->y,b->y)); y++)
		{
			slope = (((a->x - b->x)<<15)/(a->y - b->y));
			x = (slope*(y - a->y)>>15) + a->x;
			if (x>=0 && x<SCREENW)
				Screen[x+y/8*SCREENW] |= 1<<(y%8);
		}
	}
}

void fill (const WallXY *wall, int shading)
{
	int xmin = wall->corner[TOP_LEFT_CORNER].x+1;
	int xmax = wall->corner[TOP_RIGHT_CORNER].x-1;
	int x,y,ymin,ymax;
	
	// fill the wall pixels
	for (x=xmin; x<=xmax; x++)
	{
		// Move to first on-screen X position
		if (x<0) x=0;
		// If X is off right side of screen or already past end of shape then we're finished
		if (x > xmax || x >= SCREENW)
			break;
		// calculate the y values for current x position
		ymax = wall->corner[TOP_LEFT_CORNER].y + 
					(((wall->corner[TOP_RIGHT_CORNER].y - wall->corner[TOP_LEFT_CORNER].y) * 
					(((x - wall->corner[TOP_LEFT_CORNER].x) << 16) /
						(wall->corner[TOP_RIGHT_CORNER].x - wall->corner[TOP_LEFT_CORNER].x))) >> 16);
		ymin = wall->corner[BOTTOM_LEFT_CORNER].y + 
					(((wall->corner[BOTTOM_RIGHT_CORNER].y - wall->corner[BOTTOM_LEFT_CORNER].y) * 
					(((x - wall->corner[BOTTOM_LEFT_CORNER].x) << 16) / 
						(wall->corner[BOTTOM_RIGHT_CORNER].x - wall->corner[BOTTOM_LEFT_CORNER].x))) >> 16);
		for (y = ymin; y < ymax; y++)
		{
			if (y < 0) y=0;
			// If Y is off bottom of screen or already past end of shape then fill next column
			if (y > ymax || y >= SCREENH)
				break;
			// paint a white pixel - or a texture pixel
			if ((!((x+player.rotation)%shading) && !((y+player.rotation)%shading)))
				Screen[x+y/8*SCREENW] |= 1<<(y%8);
			else
				Screen[x+y/8*SCREENW] &= ~(1<<(y%8));
		}
	}
	// draw the wall outline
	draw_line(&wall->corner[TOP_LEFT_CORNER],
						&wall->corner[TOP_RIGHT_CORNER]);
	draw_line(&wall->corner[TOP_RIGHT_CORNER],
						&wall->corner[BOTTOM_RIGHT_CORNER]);
	draw_line(&wall->corner[BOTTOM_RIGHT_CORNER],
						&wall->corner[BOTTOM_LEFT_CORNER]);
	draw_line(&wall->corner[BOTTOM_LEFT_CORNER],
						&wall->corner[TOP_LEFT_CORNER]);
}

// one quarter of a sine wave in 1 degree steps scaled 2^15 to be transformed to full 360 degrees for sin and cos
const int sinS15[91] = {0,571,1143,1714,2285,2855,3425,3993,4560,5126,5690,6252,6812,7371,7927,8480,9032,9580,10125,10668,11207,11743,12275,12803,13327,13848,14364,14876,15383,15886,16384,16876,17364,17846,18323,18794,19260,19720,20173,20621,21062,21497,21926,22347,22762,23170,23571,23964,24351,24730,25101,25465,25821,26169,26509,26841,27165,27481,27788,28087,28377,28659,28932,29196,29451,29697,29935,30163,30381,30591,30791,30982,31164,31336,31498,31651,31794,31928,32051,32165,32270,32364,32449,32523,32588,32643,32688,32723,32748,32763,32767};

int sine(int degrees)
{
	int i = degrees % 360;
	if (i < 0)
		degrees += 360;
	if (i < 90)
		return sinS15[i];
	if (i < 180)
		return sinS15[90-i%90];
	if (i < 270)
		return -sinS15[i%90];
	return -sinS15[90-i%90];
}

int cosine(int degrees)
{
	return (sine(degrees+90));
}

bool my3Dto2D (PointXY *screen_pos, const PointXYZ *point)
{
	bool retval = true;
	int x = point->x - player.pos.x;
	int y = point->y - player.pos.y;
	int z = point->z - player.pos.z;
	long sinR = sine(player.rotation);
	long cosR = cosine(player.rotation);
	long dx = cosR*x - sinR*z;
	long dy = y<<15;
	long dz = cosR*z + sinR*x;
	if (dz <= 0)
	{
		screen_pos->x = (ASPECT_RATIO_ZOOM*dx>>15) + SCREENW/2;
		screen_pos->y = (ASPECT_RATIO_ZOOM*dy>>15) + SCREENH/2;
		retval = false;
	}
	else
	{
		screen_pos->x = ASPECT_RATIO_ZOOM*dx/dz + SCREENW/2;
		screen_pos->y = ASPECT_RATIO_ZOOM*dy/dz + SCREENH/2;
	}
	return retval;
}
