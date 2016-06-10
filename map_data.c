#include "global.h"
#include "stdlib.h"
#include "bmp.h"

// 3x3
#if (DUNGEON_SIZE == 3)
	Player player = {{150,24,050},90,PLAYER_HEALTH,NOT_EQUIPPED,NOT_EQUIPPED,NOT_EQUIPPED,FIST_ATK_DIE,FIST_ATK_SIDES,FIST_ATK_SPEED,NOT_IN_COMBAT,NULL,0};
	const int room_data[DUNGEON_SIZE][DUNGEON_SIZE] = { 
						{ NW ,NS ,NE },
						{ W  ,N  ,E  },
						{ SW ,S  ,SE }};
	Object sword= {swordBMP, {180,34,020},true,ITEM};
	Object armour={armourBMP,{280,34,020},true,ITEM};
	Object shield={shieldBMP,{180,32,120},true,ITEM};
	Object goblin={goblinBMP,{050,32,150},true,ENEMY,GOBLIN_ATK_DIE,GOBLIN_ATK_SIDES,GOBLIN_ATK_SPEED,GOBLIN_HEALTH};
	Object potion={potionBMP,{250,36,250},true,ITEM};
	Object troll= {trollBMP, {050,28,050},true,ENEMY,TROLL_ATK_DIE,TROLL_ATK_SIDES,TROLL_ATK_SPEED,TROLL_HEALTH};
	Object *objects[]={&sword,&armour,&shield,&goblin,&potion,&troll};
// 4x4
#elif (DUNGEON_SIZE == 4)
	Player player = {{150,24,250},270,PLAYER_HEALTH,NOT_EQUIPPED,NOT_EQUIPPED,NOT_EQUIPPED,FIST_ATK_DIE,FIST_ATK_SIDES,FIST_ATK_SPEED,NOT_IN_COMBAT,NULL,0};
	const int room_data[DUNGEON_SIZE][DUNGEON_SIZE] = { 
						{ NW ,NS ,NS ,NE },
						{ EW ,NW ,NES,EW },
						{ W  ,SE ,NW ,SE },
						{ SEW,NW ,S  ,NES}};
	Object sword= {swordBMP, {180,34,020},true,ITEM};
	Object armour={armourBMP,{280,34,020},true,ITEM};
	Object shield={shieldBMP,{180,32,120},true,ITEM};
	Object goblin1={goblinBMP,{050,32,150},true,ENEMY,GOBLIN_ATK_DIE,GOBLIN_ATK_SIDES,GOBLIN_ATK_SPEED,GOBLIN_HEALTH};
	Object goblin2={goblinBMP,{050,32,150},true,ENEMY,GOBLIN_ATK_DIE,GOBLIN_ATK_SIDES,GOBLIN_ATK_SPEED,GOBLIN_HEALTH};
	Object potion={potionBMP,{250,36,250},true,ITEM};
	Object troll= {trollBMP, {050,28,050},true,ENEMY,TROLL_ATK_DIE,TROLL_ATK_SIDES,TROLL_ATK_SPEED,TROLL_HEALTH};
	Object *objects[]={&sword,&armour,&shield,&goblin1,&goblin2,&potion,&troll};
// 6x6
#elif (DUNGEON_SIZE == 6)
	Player player = {{550,24,550},225,PLAYER_HEALTH,NOT_EQUIPPED,NOT_EQUIPPED,NOT_EQUIPPED,FIST_ATK_DIE,FIST_ATK_SIDES,FIST_ATK_SPEED,NOT_IN_COMBAT,NULL,0};
	const int room_data[DUNGEON_SIZE][DUNGEON_SIZE] = { 
						{ NW ,N  ,NS ,N  ,N  ,NE },
						{ W  ,S  ,NE ,W  ,0  ,E  },
						{ EW ,NSW,0  ,0  ,0  ,E  },
						{ W  ,NE ,SEW,SW ,0  ,E  },
						{ W  ,0  ,NS ,NS ,0  ,E  },
						{ SW ,S  ,NS ,NS ,S  ,SE }};
	Object sword= {swordBMP, {180,34,020},true,ITEM};
	Object armour={armourBMP,{280,34,020},true,ITEM};
	Object shield={shieldBMP,{180,32,120},true,ITEM};
	Object goblin1={goblinBMP,{050,32,150},true,ENEMY,GOBLIN_ATK_DIE,GOBLIN_ATK_SIDES,GOBLIN_ATK_SPEED,GOBLIN_HEALTH};
	Object goblin2={goblinBMP,{050,32,150},true,ENEMY,GOBLIN_ATK_DIE,GOBLIN_ATK_SIDES,GOBLIN_ATK_SPEED,GOBLIN_HEALTH};
	Object potion={potionBMP,{250,36,250},true,ITEM};
	Object troll= {trollBMP, {050,28,050},true,ENEMY,TROLL_ATK_DIE,TROLL_ATK_SIDES,TROLL_ATK_SPEED,TROLL_HEALTH};
	Object *objects[]={&sword,&armour,&shield,&goblin1,&goblin2,&potion,&troll};
// 8x8
#elif (DUNGEON_SIZE == 8)
	#define PLAYER_X 750
	#define PLAYER_Y 24
	#define PLAYER_Z 350
	#define PLAYER_ROTATION 180
	const int room_data[DUNGEON_SIZE][DUNGEON_SIZE] = { 
						{ NEW,NSW,N  ,NS ,NS ,N  ,NS ,NE },
						{ W  ,NES,EW ,NW ,NES,EW ,NW ,SE },
						{ EW ,NW ,E  ,W  ,N  ,SE ,SW ,NE },
						{ W  ,E  ,SW ,SE ,EW ,NW ,NS ,SE },
						{ EW ,SW ,N  ,NS ,E  ,W  ,NS ,NS },
						{ EW ,NSW,S  ,NE ,EW ,SW ,NS ,NE },
						{ W  ,N  ,NS ,S  ,0  ,NS ,NES,EW },
						{ SEW,SW ,NS ,NES,SEW,NSW,NS ,SE }};
	Object sword=  {swordBMP, {150,34,180},true,ITEM};
	Object armour= {armourBMP,{780,34,450},true,ITEM};
	Object shield= {shieldBMP,{750,32,520},true,ITEM};
	Object goblin1={goblinBMP,{750,32,250},true,ENEMY,GOBLIN_ATK_DIE,GOBLIN_ATK_SIDES,GOBLIN_ATK_SPEED,GOBLIN_HEALTH};
	Object goblin2={goblinBMP,{250,32,450},true,ENEMY,GOBLIN_ATK_DIE,GOBLIN_ATK_SIDES,GOBLIN_ATK_SPEED,GOBLIN_HEALTH};
	Object potion= {potionBMP,{750,36,050},true,ITEM};
	Object troll=  {trollBMP, {450,28,750},true,ENEMY,TROLL_ATK_DIE,TROLL_ATK_SIDES,TROLL_ATK_SPEED,TROLL_HEALTH};
	Object *objects[]={&sword,&armour,&shield,&goblin1,&goblin2,&potion,&troll};
#endif

Player player = {
	{PLAYER_X,PLAYER_Y,PLAYER_Z},
		PLAYER_ROTATION,
		PLAYER_HEALTH,
		NOT_EQUIPPED,
		NOT_EQUIPPED,
		NOT_EQUIPPED,
		FIST_ATK_DIE,
		FIST_ATK_SIDES,
		FIST_ATK_SPEED,
		NOT_IN_COMBAT,
		NULL,
		0};
