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

// Sprites
typedef struct Sprite{
	int mapx;
	int mapy;
	int screenx;
	int screeny;
	int exists;
	int bitmapn;
	const unsigned char* bitmap[4];
} Sprite;

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

/*********** GRaphics **************/
const unsigned char* goblin1BMP;
const unsigned char* goblin2BMP;
const unsigned char* goblin3BMP;
const unsigned char* goblin4BMP;
const unsigned char* troll1BMP;
const unsigned char* troll2BMP;
const unsigned char* troll3BMP;
const unsigned char* troll4BMP;
const unsigned char* sword1BMP;
const unsigned char* sword2BMP;
const unsigned char* sword3BMP;
const unsigned char* sword4BMP;
const unsigned char* shield1BMP;
const unsigned char* shield2BMP;
const unsigned char* shield3BMP;
const unsigned char* shield4BMP;
