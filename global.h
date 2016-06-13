#ifndef _GLOBAL_H_
#define _GLOBAL_H_

/*************** GENERAL PURPOSE DEFINES ************/
#define CLOCK_SPEED 80000000L
#define TRUE  (1)
#define FALSE (0)
#define ON    (1)
#define OFF   (0)
#define LEFT (-1) // x-pos offset for left movement
#define RIGHT (1) // x-pos offset for right movement
#define UP   (-1) // y-pos offset for up movement
#define DOWN  (1) // y-pos offset for down movement
#define BIT(X) (1L<<(X)) // single bit set
#define PCTL(X) (7L<<(X*3)) // three bits set

/* GAME SPECIFIC DEFINES */

#define DUNGEON_SIZE 8

#define WALL_LENGTH 100
#define WALL_HEIGHT 45
#define MAX_VIEW_RANGE 10

#define ASPECT_RATIO_ZOOM 50

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

#define TROLL_ATK_DIE 2
#define TROLL_ATK_SIDES 12
#define TROLL_ATK_SPEED 60
#define TROLL_HEALTH 200
#define GOBLIN_ATK_DIE 1
#define GOBLIN_ATK_SIDES 4
#define GOBLIN_ATK_SPEED 25
#define GOBLIN_HEALTH 60
#define FIST_ATK_SPEED 30
#define FIST_ATK_DIE 1
#define FIST_ATK_SIDES 3
#define SWORD_ATK_SPEED 45
#define SWORD_ATK_DIE 2
#define SWORD_ATK_SIDES 6
#define PLAYER_HEALTH 99

#define NOT_IN_COMBAT false
#define IN_COMBAT true
#define NOT_EQUIPPED false
#define EQUIPPED true

// General and intialisation functions
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // Wait for interrupts
void Timer2_Init(unsigned long period);
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
void Delay1ms(unsigned long count); // time delay in 0.1 seconds
void init(void);

/*********************************** SOUND ******************************************/

// DAC configuration
#define DAC_STEPS 16L
#define GPIO_PORTB (1L<<1)
#define DAC_PORT (GPIO_PORTB)
#define DAC_PINS (BIT(0)|BIT(1)|BIT(2)|BIT(3))
#define DAC_PCTL_PINS (PCTL(0)|PCTL(1)|PCTL(2)|PCTL(3))

/* my sounds - original 11KHz wav file data downsampled to 5.5k and compressed to one nibble per 4-bit dac step */
typedef enum Sound {
	EXPLOSION_SOUND=0,
	INVADER_SOUND=1,
	INVADER_KILLED_SOUND=2,
	SHOOT_SOUND=3,
	MOTHERSHIP_SOUND=4,
	MAX_SOUND
} Sound;

int sound_enabled[MAX_SOUND];
int DAC_step[MAX_SOUND];
int DAC_max[MAX_SOUND];
const unsigned char *DAC_wav[MAX_SOUND];

// Sound functions
void DAC_Init(void);
void DAC_Out(unsigned long data);
void Sound_Init(void);
void Sound_Tone(enum Sound sound);
void Sound_Off(int sound);
void SysTick_Handler(void);

#include <stdbool.h>

typedef enum colour_e
{
	WHITE=0,
	BLACK=1
} colour_e;

typedef struct PointXYZ
{
	int x;
	int y;
	int z;
} PointXYZ;

typedef struct PointXY
{
	int x;
	int y;
} PointXY;

typedef enum ObjectTypeE
{
	ENEMY,
	ITEM
} ObjectTypeE;

typedef struct Object
{
	const unsigned char* bitmap;
	PointXYZ pos;
	bool exists;
	ObjectTypeE type;
	int atk_die_num;
	int atk_die_sides;
	int atk_speed;
	int health;
	// uninitialised data
	PointXY screenpos;
} Object;

typedef struct Player
{
	PointXYZ pos;
	int rotation;
	int health;
	bool shield_equipped;
	bool sword_equipped;
	bool armour_equipped;
	int atk_die_num;
	int atk_die_sides;
	int atk_speed;
	bool in_combat;
	// uninitialised data
	int last_attack_value;
	int last_attack_time;
	bool shield_active;
	Object *in_combat_with;
} Player;

typedef enum wall_e
{
	NORTH=1,
	EAST=2,
	SOUTH=4,
	WEST=8,
	N=NORTH,
	W=WEST,
	E=EAST,
	S=SOUTH,
	NE=N|E,
	NW=N|W,
	NS=N|S,
	EW=E|W,
	SE=E|S,
	SW=S|W,
	NES=NE|S,
	NEW=NE|W,
	SEW=SE|W,
	NSW=NS|W,
	NSEW=NS|EW
} wall_e;

typedef struct WallXYZ
{
	PointXYZ corners[4];
	bool exists;
} WallXYZ;

typedef struct WallXY
{
	PointXY corner[4];
} WallXY;


typedef struct Room
{
	Object* objects;
	WallXYZ wall[4];
} Room;

#endif
