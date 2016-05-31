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

typedef struct Object
{
	const unsigned char* bitmap[5];
	PointXYZ pos;
	struct Object* next;
} Object;

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
