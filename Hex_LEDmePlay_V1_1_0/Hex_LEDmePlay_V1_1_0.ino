// Hex
// for the LEDmePlay
//
// www.mithotronic.de
//
// Version: 1.1.0
// Author: Michael Rosskopf (2016)
// 
// Thanks to Thomas Laubach! 
// 
// Release Notes:
// V1.1.0: Support for LEDmePlay Joypad and LEDmePlayBoy (2018)
// V1.0.0: First release

// Include libraries for adafruit matrix
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <avr/pgmspace.h> // Necessary in order to maintain the maze data in program memory

// Setup adafruit matrix
#define CLK 50
#define OE  51
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

// Audio out
int audio = 2;

// Define colors
#define TP 0 // transparent
#define BL 1 // black
#define R1 2
#define R2 3
#define R3 4
#define G1 5
#define G2 6
#define G3 7
#define B1 8
#define B2 9
#define B3 10
#define Y1 11
#define Y2 12
#define Y3 13
#define P1 14
#define P2 15
#define P3 16
#define T1 17
#define T2 18
#define T3 19
#define W1 20
#define W2 21
#define W3 22

// Define notes
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

// Joystick 1
int buttonU1 = 30;
int buttonD1 = 32;
int buttonL1 = 34;
int buttonR1 = 36;
int buttonFire1 = 38;
int analogX1 = 8;
int analogY1 = 9;

// Joystick 2
int buttonU2 = 31;
int buttonD2 = 33;
int buttonL2 = 35;
int buttonR2 = 37;
int buttonFire2 = 39;
int analogX2 = 10;
int analogY2 = 11;

// Sensitivity of analog thumb joysticks (of the LEDmePlay Joypad) in case of "digital usage" (detects movement if deviates from center position value of 512 by sensitivity value)
const int sensitivity = 192;

// Other buttons
int buttonReset = 42;
int buttonPause = 43;

// Game synchronization
unsigned long engineLoopStartPoint; // Contains millisconds since start of LEDmePlay at begin of each engine loop

boolean reset;

// Maze
byte currentMaze;
byte mazeWidth;
byte mazeHeight;

// Maze color
byte mazeColor;  // Level platform color: 0 == Invisible, 1 == Blue, 2 == Green, 3 == Turquoise, 4 == Red, 5 == Violet, 6 == Yellow, 7 == White
byte red;
byte green;
byte blue;

// Define game specifc stuff
#define EMPTY         0
#define WALL          1
#define PLAYER_START  2
#define EXIT          3
#define DOOR          4

#define KEY           5
#define ELIXIR        6
#define MAGIC_POTION  7

#define GLADIUS       8
#define SWORD         9
#define CLAYMORE      10
#define CROSSBOW      11
#define LONGBOW       12

#define LEATHER       13
#define HEAVY         14
#define INVISIBILITY  15
#define MAGIC         16

#define HEALING       17
#define IMMORTAL      18
#define FIREBALL      19
#define LIGHTNING     20
#define HASTE         21

#define RAT           22
#define SPIDER        23
#define GOBLIN        24
#define MOTHMAN       25
#define GHOST         26
#define WARRIOR       27
#define BEHOLDER      28
#define ENDBOSS       29

const byte numberOfMazes = 10;
const uint8_t maps[] PROGMEM  = {
                      16, 16, 3,                                            
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                      1, PLAYER_START, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, DOOR, SPIDER, EXIT,
                      1, 0, 1, DOOR, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1,
                      1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1,
                      1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1,
                      1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1,
                      1, 0, 0, 0, 1, ELIXIR, 1, 0, 1, 0, 1, 0, 0, RAT, ELIXIR, 1,
                      1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1,
                      1, KEY, 1, RAT, 1, CROSSBOW, 1, SPIDER, 1, 0, 1, 0, 0, 0, KEY, 1,
                      1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1,
                      1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
                      1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1,
                      1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

                      16, 16, 1,                                            
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                      1, KEY, 0, 0, 0, 1, 1, 1, 1, 1, GOBLIN, GOBLIN, 0, 0, DOOR, EXIT, 
                      1, 0, 1, 1, 0, 1, 1, 1, 1, 1, DOOR, 1, 1, 1, 1, 1, 
                      1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 
                      1, 0, 0, 0, SPIDER, 0, 0, SPIDER, 0, 0, 0, 0, 0, 0, 1, 1, 
                      1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 
                      1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 
                      1, 0, 0, 0, SPIDER, 0, 0, SPIDER, 0, 0, 0, 1, KEY, 0, 1, 1, 
                      1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 
                      1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 
                      1, CROSSBOW, 0, 0, 0, 1, 1, RAT, 0, 0, RAT, 0, 0, ELIXIR, 1, 1, 
                      1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 
                      1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 
                      1, 0, 1, PLAYER_START, 1, LEATHER, 0, 0, 1, 1, FIREBALL, 0, 0, 0, 1, 1, 
                      1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 

                      16, 16, 2,                                            
                      1, 1, 1, 1, 1, 1, 1, 1, 1, EXIT, 1, 1, 1, 1, 1, 1, 
                      1, 1, 1, 1, 1, 1, PLAYER_START, 1, 1, DOOR, 1, 1, 1, 1, 1, 1, 
                      1, 1, 1, 1, 1, 1, MAGIC_POTION, 1, 1, 0, 1, 1, 1, 1, 1, 1, 
                      1, RAT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
                      1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 
                      1, 0, 1, ELIXIR, 0, 0, 0, 0, 0, 0, 0, 0, GOBLIN, 1, 0, 1, 
                      1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 
                      1, 0, 1, 0, 1, 0, 0, 0, 0, 0, GOBLIN, 1, 0, 1, 0, 1, 
                      1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 
                      1, 0, 1, 0, 1, 0, 1, 1, KEY, 1, 0, 1, 0, 1, 0, 1, 
                      1, 0, 1, 0, 1, MOTHMAN, 0, 0, 0, 0, ELIXIR, 1, 0, 1, 0, 1, 
                      1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 
                      1, 0, 1, GOBLIN, 0, 0, 0, 0, 0, 0, 0, 0, SWORD, 1, 0, 1, 
                      1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 
                      1, CROSSBOW, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RAT, 1, 
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 

                      16, 16, 5,                                            
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                      1, GOBLIN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, GOBLIN, 1, 
                      1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 
                      1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 
                      1, 1, 1, 1, 1, 1, 1, 1, DOOR, 1, 0, 0, 0, 0, KEY, 1, 
                      1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 
                      1, 1, 1, 1, 1, RAT, 0, 0, 0, 0, 0, 1, HEAVY, 1, 1, 1, 
                      EXIT, 0, MOTHMAN, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, PLAYER_START, 1, 
                      1, 1, MOTHMAN, DOOR, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 
                      1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 
                      1, 1, 1, 1, 1, RAT, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 
                      1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 
                      1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 
                      1, 1, 0, 1, KEY, 1, ELIXIR, 1, 1, FIREBALL, 1, HEALING, 1, 0, 1, 1, 
                      1, 1, SPIDER, 0, 0, 0, 0, SPIDER, SPIDER, 0, 0, 0, 0, SPIDER, 1, 1, 
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 

                      16, 16, 6,                                            
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                      1, KEY, 0, 0, 0, MOTHMAN, 1, KEY, 0, MOTHMAN, 1, 1, 1, 1, PLAYER_START, 1, 
                      1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 
                      1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 
                      1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 
                      1, 0, 1, 1, 1, GOBLIN, 0, 0, 0, 0, 1, HEAVY, LONGBOW, 1, 0, 1, 
                      1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 
                      1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 
                      1, ELIXIR, 0, 0, 0, 0, 0, WARRIOR, 0, 0, 0, 0, 0, GOBLIN, 0, 1, 
                      1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 
                      1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 
                      1, 0, 1, 1, 1, GOBLIN, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 
                      1, DOOR, 1, LIGHTNING, 1, 1, 1, MAGIC_POTION, 1, 1, 1, 0, 1, 0, 0, 1, 
                      1, GOBLIN, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 
                      1, 0, 0, 0, 0, DOOR, 0, 0, 0, WARRIOR, 0, 0, 1, 0, 0, EXIT, 
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 

                      16, 16, 3,                                            
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                      1, PLAYER_START, 1, HEALING, 0, 0, 0, GHOST, 1, INVISIBILITY, GHOST, 0, 0, 0, 0, 1, 
                      1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 
                      1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 
                      1, 0, 0, 0, 0, 0, 0, RAT, 0, RAT, 0, 0, 0, 0, 0, 1, 
                      1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 
                      1, 0, 1, KEY, 0, 0, 1, 0, MAGIC_POTION, 0, 1, 1, 1, 1, 1, 1, 
                      1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                      1, 0, 1, WARRIOR, 0, 0, 0, 0, WARRIOR, 0, 0, 0, 1, 1, 1, 1, 
                      1, DOOR, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, EXIT, 
                      1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 
                      1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, KEY, 1, 0, 1, 1, 
                      1, 1, 1, WARRIOR, 0, 0, 0, 0, WARRIOR, 0, 0, 1, 1, 0, 1, 1, 
                      1, 1, 1, 1, 1, 1, 1, LONGBOW, 1, 1, 0, 0, DOOR, 0, ELIXIR, 1, 
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 

                      16, 16, 1,                                            
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                      1, PLAYER_START, 1, HASTE, 0, 0, RAT, 0, DOOR, 0, 0, 0, 0, 0, MOTHMAN, 1, 
                      1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 
                      1, 0, 1, 0, 0, 0, 0, 1, LIGHTNING, 0, 0, 1, 0, 1, 0, 1, 
                      1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, CLAYMORE, 1, 
                      1, 0, 1, 0, 1, 0, SPIDER, 1, 0, 0, 0, 1, 0, 1, 1, 1, 
                      1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 
                      1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 
                      1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, GHOST, 1, 
                      1, GOBLIN, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 
                      1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 
                      1, 0, 1, 1, 0, MAGIC_POTION, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 
                      1, 0, KEY, 1, 0, 1, 1, 1, 0, 0, ELIXIR, 1, LONGBOW, 1, DOOR, 1, 
                      1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, BEHOLDER, 1, 
                      1, 0, 0, 0, 0, 1, KEY, 1, 0, WARRIOR, 0, 0, 0, 1, 0, 1, 
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, EXIT, 1,

                      16, 16, 2,                                            
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                      1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1,
                      1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1,
                      1, 0, 1, KEY, GHOST, 1, 0, 1, 1, 0, 1, WARRIOR, KEY, 1, 0, 1,
                      1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1,
                      1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1,
                      1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1,
                      1, PLAYER_START, MAGIC_POTION, CLAYMORE, MAGIC, IMMORTAL, 0, 0, DOOR, DOOR, MOTHMAN, DOOR, DOOR, BEHOLDER, BEHOLDER, EXIT,
                      1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1,
                      1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1,
                      1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1,
                      1, 0, 1, KEY, GHOST, 1, 0, 1, 1, 0, 1, BEHOLDER, KEY, 1, 0, 1,
                      1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1,
                      1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1,
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

                      16, 16, 5,                                            
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                      1, KEY, 0, 0, 0, 0, ELIXIR, 1, 1, LONGBOW, 0, 0, 0, 0, KEY, 1, 
                      1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 
                      1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 
                      1, ENDBOSS, 0, SPIDER, 0, 0, 0, 0, 0, 0, 0, 0, SPIDER, 0, ENDBOSS, 1, 
                      1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 
                      1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 
                      1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 
                      1, HASTE, 0, SPIDER, 0, 0, 1, DOOR, 1, 1, 0, 0, SPIDER, 0, CROSSBOW, 1, 
                      1, BEHOLDER, 1, 1, 0, 1, 1, DOOR, 1, 1, 1, 0, 1, 1, BEHOLDER, 1, 
                      1, 0, 0, 1, 0, 1, 1, DOOR, 1, 1, 1, 0, 1, 0, 0, 1, 
                      1, 1, 0, 1, 0, 1, 1, DOOR, 0, EXIT, 1, 0, 1, 0, 1, 1, 
                      1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 
                      1, 0, 1, 1, 0, 1, PLAYER_START, 0, 0, 0, 1, 0, 1, 1, 0, 1, 
                      1, KEY, 0, SPIDER, 0, 0, 0, 1, 1, 0, 0, 0, SPIDER, 0, KEY, 1, 
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

                      16, 16, 4,                                            
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                      1, PLAYER_START, 1, 1, KEY, 1, GHOST, 1, GHOST, 1, GHOST, 1, 0, 0, 0, 1, 
                      1, 0, 1, ENDBOSS, 0, 0, 0, 0, 0, 0, 0, ENDBOSS, 0, 1, GHOST, 1, 
                      1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, ENDBOSS, 1, 
                      1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, DOOR, 1, 
                      1, 0, 1, 0, 1, 0, 1, KEY, 1, 0, 1, 0, 1, 0, 1, 1, 
                      1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, DOOR, 0, 1, 
                      1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 
                      1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, DOOR, 1, 
                      1, 0, 1, 0, 1, KEY, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 
                      1, LONGBOW, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, DOOR, 0, 1, 
                      1, MAGIC_POTION, 1, 0, 1, 0, 1, 0, 1, KEY, 1, 0, 1, 1, 0, 1, 
                      1, KEY, 1, 0, 0, 0, 0, 0, 0, 0, 0, ENDBOSS, 1, 0, 0, 1, 
                      1, DOOR, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 
                      1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, EXIT, 
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

const uint8_t ratPics[] PROGMEM = {
                      4, 10, 28, 78,

                      2, 2,
                      R1, R1,
                      R1, R1,

                      4, 4,
                      TP, B1, B1, TP,
                      TP, R1, R1, TP,
                      TP, R1, R1, TP,
                      R1, R2, R2, R1,
                      
                      6, 8,
                      TP, TP, R1, R1, TP, TP,
                      TP, B1, R1, R1, B1, TP,
                      TP, TP, R1, R1, TP, TP,
                      TP, TP, R1, R1, TP, TP,
                      TP, R1, R2, R2, R1, TP,
                      TP, R1, R2, R2, R1, TP,
                      TP, R1, R1, R1, R1, TP,
                      R1, R1, TP, TP, R1, R1,

                      6, 9,
                      TP, TP, R1, R1, TP, TP,
                      TP, B1, R1, R1, B1, TP,
                      TP, TP, R1, R1, TP, TP,
                      TP, R1, W1, W1, R1, TP, 
                      TP, TP, R1, R1, TP, TP,
                      TP, R1, R2, R2, R1, TP,
                      TP, R1, R2, R2, R1, TP,
                      TP, R1, R1, R1, R1, TP,
                      R1, R1, TP, TP, R1, R1
};

const uint8_t spiderPics[] PROGMEM = {
                      4, 8, 18, 52,

                      2, 1,
                      Y1, Y1,

                      4, 2,
                      TP, Y1, Y1, TP,
                      Y1, Y2, Y2, Y1,
                      
                      8, 4,
                      TP, TP, Y1, Y2, Y2, Y1, TP, TP,
                      TP, TP, Y2, B1, B1, Y2, TP, TP,
                      TP, Y2, Y2, Y3, Y3, Y2, Y2, TP,
                      Y2, TP, Y3, TP, TP, Y3, TP, Y2,

                      8, 5,
                      TP, TP, Y1, Y2, Y2, Y1, TP, TP,
                      TP, TP, Y2, B1, B1, Y2, TP, TP,
                      Y2, TP, Y2, BL, BL, Y2, TP, Y2,
                      TP, Y2, Y2, Y3, Y3, Y2, Y2, TP,
                      TP, TP, Y2, TP, TP, Y2, TP, TP
};

const uint8_t goblinPics[] PROGMEM = {
                      4, 10, 28, 94,

                      2, 2,
                      G2, G2,
                      G1, G1,

                      4, 4,
                      TP, G1, G1, TP,
                      G1, R1, R1, G1,
                      TP, G1, G1, TP,
                      G1, TP, TP, G1,
                      
                      8, 8,
                      TP, TP, G1, G1, G1, G1, TP, TP,
                      TP, G1, R1, G1, G1, R1, G1, TP,
                      G2, G2, G2, G2, G2, G2, G2, G2,
                      TP, G1, G1, G1, G1, G1, G1, TP,
                      TP, TP, G1, G1, G1, G1, TP, TP,
                      TP, TP, G1, TP, TP, G1, TP, TP,
                      TP, G1, TP, TP, TP, TP, G1, TP,
                      G1, G1, TP, TP, TP, TP, G1, G1,

                      8, 10,
                      TP, TP, G1, G1, G1, G1, TP, TP,
                      TP, G1, R1, G1, G1, R1, G1, TP,
                      G2, G2, G2, G2, G2, G2, G2, G2,
                      G2, BL, W2, BL, BL, W2, BL, G2,
                      G2, BL, BL, BL, BL, BL, BL, G2,
                      TP, G1, G1, G1, G1, G1, G1, TP,
                      TP, TP, G1, G1, G1, G1, TP, TP,
                      TP, TP, G1, TP, TP, G1, TP, TP,
                      TP, G1, TP, TP, TP, TP, G1, TP,
                      G1, G1, TP, TP, TP, TP, G1, G1
};

const uint8_t mothmanPics[] PROGMEM = {
                      4, 12, 50, 172,

                      2, 3,
                      T1, T1,
                      T1, T1,
                      T1, T1,

                      6, 6,
                      TP, TP, T1, T1, TP, TP,
                      T1, TP, B1, B1, TP, T1,
                      T1, T1, T2, T2, T1, T1,
                      TP, TP, T2, T2, TP, TP,
                      TP, TP, T1, T1, TP, TP,
                      TP, T1, TP, TP, T1, TP,
                      
                      10, 12,
                      TP, TP, TP, T2, TP, TP, T2, TP, TP, TP,
                      T1, T1, T1, BL, T1, T1, BL, TP, T1, T1,
                      T1, B1, T1, T1, B2, B2, T1, T1, B1, T1,
                      T1, B1, T1, Y1, T1, T1, Y1, T1, B1, T1,
                      T1, B1, Y1, Y1, T2, T2, Y1, Y1, B1, T1,
                      TP, T1, Y1, Y1, T2, T2, Y1, Y1, T1, TP,
                      TP, TP, T1, T1, T1, T1, T1, T1, TP, TP,
                      TP, TP, TP, T2, T1, T1, T2, TP, TP, TP,
                      TP, TP, TP, T2, T1, T1, T2, TP, TP, TP,
                      TP, TP, TP, T2, T1, T1, T2, TP, TP, TP,
                      TP, TP, T1, T2, TP, TP, T2, T1, TP, TP,
                      TP, TP, T1, T2, TP, TP, T2, T1, TP, TP,

                      12, 12,
                      TP, TP, TP, TP, T2, TP, TP, T2, TP, TP, TP, TP,
                      TP, TP, TP, TP, BL, T1, T1, BL, TP, TP, TP, TP,
                      TP, TP, TP, TP, T1, Y3, Y3, T1, TP, TP, TP, TP,
                      T1, T1, T1, T1, T1, T1, T1, T1, T1, T1, T1, T1,
                      T1, B1, Y1, Y1, Y1, T2, T2, Y1, Y1, Y1, B1, T1,
                      T1, B1, Y1, Y2, Y1, T2, T2, Y1, Y2, Y1, B1, T1,
                      T1, B1, T1, Y1, T1, T1, T1, T1, Y1, T1, B1, T1,
                      TP, T1, T1, T1, T2, T1, T1, T2, T1, T1, T1, TP,
                      TP, TP, TP, TP, T2, T1, T1, T2, TP, TP, TP, TP,
                      TP, TP, TP, TP, T2, T1, T1, T2, TP, TP, TP, TP,
                      TP, TP, TP, T1, T2, TP, TP, T2, T1, TP, TP, TP,
                      TP, TP, TP, T1, T2, TP, TP, T2, T1, TP, TP, TP
};

const uint8_t ghostPics[] PROGMEM = {
                      4, 14, 48, 146,

                      2, 4,
                      W1, W1,
                      W1, W1,
                      W1, W1,
                      W1, W1,

                      4, 8,
                      TP, W1, W1, TP,
                      TP, W1, W1, TP,
                      W1, BL, BL, W1,
                      W1, BL, BL, W1,
                      TP, W1, W1, TP,
                      TP, W1, W1, TP,
                      W1, BL, BL, W1,
                      W1, BL, BL, W1,
                      
                      6, 16,
                      TP, TP, W1, W1, TP, TP,
                      TP, W1, BL, BL, W1, TP,
                      TP, W1, BL, BL, W1, TP,
                      TP, W1, BL, BL, W1, TP,
                      W1, BL, BL, BL, BL, W1,
                      W1, BL, BL, BL, BL, W1,
                      W1, BL, BL, BL, BL, W1,
                      TP, W1, BL, BL, W1, TP,
                      TP, W1, BL, BL, W1, TP,
                      TP, W1, BL, BL, W1, TP,
                      TP, W1, BL, BL, W1, TP,
                      W1, BL, BL, BL, BL, W1,
                      W1, BL, BL, BL, BL, W1,
                      W1, BL, BL, BL, BL, W1,
                      W1, BL, BL, BL, BL, W1,
                      W1, BL, BL, BL, BL, W1,

                      6, 16,
                      TP, TP, W1, W1, TP, TP,
                      TP, W1, R3, R3, W1, TP,
                      TP, W1, R3, R3, W1, TP,
                      TP, W1, R3, R3, W1, TP,
                      W1, BL, BL, BL, BL, W1,
                      W1, BL, BL, BL, BL, W1,
                      W1, BL, BL, BL, BL, W1,
                      TP, W1, BL, BL, W1, TP,
                      TP, W1, BL, BL, W1, TP,
                      TP, W1, BL, BL, W1, TP,
                      TP, W1, BL, BL, W1, TP,
                      W1, BL, BL, BL, BL, W1,
                      W1, BL, BL, BL, BL, W1,
                      W1, BL, BL, BL, BL, W1,
                      W1, BL, BL, BL, BL, W1,
                      W1, BL, BL, BL, BL, W1
};

const uint8_t warriorPics[] PROGMEM = {
                      4, 14, 66, 228,

                      2, 4,
                      W1, W1,
                      W2, W2,
                      W2, W2,
                      W1, W1,

                      5, 10,
                      W1, TP, W1, W1, TP,
                      W1, TP, W1, W1, TP,
                      W1, W2, W1, W1, W1,
                      W1, W1, W2, B1, B1,
                      TP, W1, W2, B1, B1,
                      TP, TP, W2, B1, B1,
                      TP, W1, TP, TP, W1,
                      TP, W1, TP, TP, W1,
                      TP, W1, TP, TP, W1,
                      TP, W1, TP, TP, W1,
                      
                      10, 16,
                      TP, TP, TP, TP, TP, W1, W1, TP, TP, TP,
                      TP, W1, TP, TP, W1, B2, B2, W1, TP, TP,
                      TP, W1, TP, TP, W1, W2, W2, W1, TP, TP,
                      TP, W1, TP, TP, TP, W1, W1, TP, TP, TP,
                      TP, W1, TP, B1, B1, W1, W1, B1, B1, TP,
                      TP, W1, TP, B1, W1, W1, W1, W1, B1, TP,
                      W1, W2, W1, B1, W1, W1, B2, B2, B2, B2,
                      TP, W2, B2, B1, W1, W1, B2, W1, W1, B2,
                      TP, TP, B1, TP, W2, W1, B2, W1, W1, B2,
                      TP, TP, TP, TP, W2, W1, B2, W1, W1, B2,
                      TP, TP, TP, W2, W1, W1, W1, B2, B2, TP,
                      TP, TP, TP, W2, W1, TP, TP, W1, W2, TP,
                      TP, TP, TP, W2, W1, TP, TP, W1, W2, TP,
                      TP, TP, TP, W2, W1, TP, TP, W1, W2, TP,
                      TP, TP, TP, W2, W1, TP, TP, W1, W2, TP,
                      TP, TP, W2, W1, W1, TP, TP, W1, W1, W2,

                      9, 16,
                      TP, TP, TP, TP, W1, W1, TP, TP, TP,
                      TP, TP, TP, W1, B2, B2, W1, TP, TP,
                      TP, TP, TP, W1, W2, W2, W1, TP, TP,
                      TP, B1, TP, TP, W1, W1, TP, TP, TP,
                      B1, B2, B1, B1, W1, W1, B1, B1, TP,
                      TP, W3, B1, W1, W1, W1, W1, B1, TP,
                      TP, TP, W3, W1, W1, B2, B2, B2, B2,
                      TP, TP, B1, W3, W1, B2, W1, W1, B2,
                      TP, TP, TP, W2, W3, B2, W1, W1, B2,
                      TP, TP, TP, W2, W1, W3, W1, W1, B2,
                      TP, TP, W2, W1, W1, W1, B2, B2, TP,
                      TP, TP, W2, W1, TP, TP, W1, W2, TP,
                      TP, TP, W2, W1, TP, TP, W1, W2, TP,
                      TP, TP, W2, W1, TP, TP, W1, W2, TP,
                      TP, TP, W2, W1, TP, TP, W1, W2, TP,
                      TP, W2, W1, W1, TP, TP, W1, W1, W2
};

const uint8_t beholderPics[] PROGMEM = {
                      4, 10, 48, 206,

                      2, 2,
                      R2, R2,
                      R1, R1,

                      6, 6,
                      TP, R1, R1, R1, R1, TP,
                      TP, R2, R2, R2, R2, TP,
                      R1, R2, G1, G1, R2, R1,
                      R1, R2, R2, R2, R2, R1,
                      R1, R2, Y1, Y1, R2, R1,
                      TP, R1, R1, R1, R1, TP,
                      
                      12, 13,
                      TP, TP, TP, R2, TP, G1, TP, TP, TP, G1, TP, TP,
                      TP, TP, G1, R2, TP, TP, R2, TP, R2, TP, TP, TP,
                      TP, TP, TP, R2, TP, R2, TP, TP, R2, TP, TP, TP,
                      TP, TP, TP, R1, R1, R1, R1, R1, R1, TP, TP, TP,
                      TP, R1, R1, R2, R2, R2, R2, R2, R2, R1, R1, TP,
                      R1, R2, R2, R2, W1, W1, W1, W1, R2, R2, R2, R1,
                      R1, R2, R2, W1, BL, G3, G3, BL, W1, R2, R2, R1,
                      R1, R2, R2, R2, W1, W1, W1, W1, R2, R2, R2, R1,
                      R1, R2, R2, R2, R2, R2, R2, R2, R2, R2, R2, R1,
                      TP, R1, R2, Y1, Y1, Y1, Y1, Y1, Y1, R2, R1, TP,
                      TP, R1, R2, R2, Y1, Y1, Y1, Y1, R2, R2, R1, TP,
                      TP, TP, R1, R2, R2, R2, R2, R2, R2, R1, TP, TP,
                      TP, TP, TP, R1, R1, R1, R1, R1, R1, TP, TP, TP,

                      12, 13,
                      TP, TP, TP, R2, TP, G1, TP, TP, TP, G1, TP, TP,
                      TP, TP, G1, R2, TP, TP, R2, TP, R2, TP, TP, TP,
                      TP, TP, TP, R2, TP, R2, TP, TP, R2, TP, TP, TP,
                      TP, TP, TP, R1, R1, R1, R1, R1, R1, TP, TP, TP,
                      TP, R1, R1, R2, R2, R2, R2, R2, R2, R1, R1, TP,
                      R1, R2, R2, R2, W1, W1, W1, W1, R2, R2, R2, R1,
                      R1, R2, R2, W1, BL, G3, G3, BL, W1, R2, R2, R1,
                      R1, R2, R2, R2, W1, W1, W1, W1, R2, R2, R2, R1,
                      R1, R2, R2, Y1, Y1, Y1, Y1, Y1, Y1, R2, R2, R1,
                      TP, R1, Y1, BL, W1, BL, BL, W1, BL, Y1, R1, TP,
                      TP, R1, Y1, BL, BL, BL, BL, BL, BL, Y1, R1, TP,
                      TP, TP, R1, Y1, Y1, Y1, Y1, Y1, Y1, R1, TP, TP,
                      TP, TP, TP, R1, R1, R1, R1, R1, R1, TP, TP, TP
};

const uint8_t endbossPics[] PROGMEM = {
                      4, 22, 88, 346,

                      4, 4,
                      P1, P2, P2, P1,
                      P2, G1, G1, P2,
                      P2, P2, P2, P2,
                      P1, P2, P2, P1,

                      8, 8,
                      TP, P1, TP, TP, TP, TP, P1, TP,
                      P1, TP, P1, TP, TP, P1, TP, P1,
                      TP, TP, P1, P1, P1, P1, TP, TP,
                      TP, P1, G2, P2, P2, G2, P1, TP,
                      TP, P1, P2, P2, P2, P2, P1, TP,
                      P1, P1, P2, Y1, Y1, P2, P1, P1,
                      TP, P1, Y1, P2, P2, Y1, P1, TP,
                      TP, P1, P1, P1, P1, P1, P1, TP,
                      
                      16, 16,
                      TP, TP, TP, TP, P1, TP, TP, TP, TP, TP, TP, P1, TP, TP, TP, TP,
                      TP, TP, TP, P1, P1, P1, TP, TP, TP, TP, P1, P1, P1, TP, TP, TP,
                      TP, TP, P1, TP, P1, P1, TP, TP, TP, TP, P1, P1, TP, P1, TP, TP,
                      TP, TP, TP, TP, P1, P2, P1, P1, P1, P1, P2, P1, TP, TP, TP, TP,
                      TP, TP, TP, P1, P2, P2, P2, P2, P2, P2, P2, P2, P1, TP, TP, TP,
                      TP, TP, P1, P2, P2, G2, G2, P2, P2, G2, G2, P2, P2, P1, TP, TP,
                      TP, TP, P1, P2, P2, G2, G3, P2, P2, G3, G2, P2, P2, P1, TP, TP,
                      TP, TP, P1, P2, P2, P2, G2, P2, P2, G2, P2, P2, P2, P1, TP, TP,
                      P1, TP, P1, P2, P2, P2, P2, P2, P2, P2, P2, P2, P2, P1, TP, P1,
                      P1, P1, P1, P2, P2, P2, P2, Y1, Y1, P2, P2, P2, P2, P1, P1, P1,
                      P1, P1, P1, P2, P2, P2, Y1, Y2, Y2, Y1, P2, P2, P2, P1, P1, P1,
                      TP, TP, P1, P2, P2, Y1, Y2, Y1, Y1, Y2, Y1, P2, P2, P1, TP, TP,
                      TP, TP, P1, P2, Y1, Y1, Y1, P2, P2, Y1, Y1, Y1, P2, P1, TP, TP,
                      TP, TP, TP, P1, P2, P2, P2, P2, P2, P2, P2, P2, P1, TP, TP, TP,
                      TP, TP, TP, TP, P1, P1, P1, P1, P1, P1, P1, P1, TP, TP, TP, TP,
                      TP, TP, P1, P1, P1, P1, TP, TP, TP, TP, P1, P1, P1, P1, TP, TP,

                      16, 16,
                      TP, TP, TP, TP, P1, TP, TP, TP, TP, TP, TP, P1, TP, TP, TP, TP,
                      TP, TP, TP, P1, P1, P1, TP, TP, TP, TP, P1, P1, P1, TP, TP, TP,
                      TP, TP, P1, TP, P1, P1, TP, TP, TP, TP, P1, P1, TP, P1, TP, TP,
                      TP, TP, TP, TP, P1, P2, P1, P1, P1, P1, P2, P1, TP, TP, TP, TP,
                      TP, TP, TP, P1, P2, P2, P2, P2, P2, P2, P2, P2, P1, TP, TP, TP,
                      TP, TP, P1, P2, P2, G2, G2, P2, P2, G2, G2, P2, P2, P1, TP, TP,
                      TP, TP, P1, P2, P2, G2, G3, P2, P2, G3, G2, P2, P2, P1, TP, TP,
                      TP, TP, P1, P2, P2, P2, G2, P2, P2, G2, P2, P2, P2, P1, TP, TP,
                      TP, TP, P1, P2, P2, P2, P2, P2, P2, P2, P2, P2, P2, P1, TP, TP,
                      P1, P1, P1, P2, P2, P2, P2, Y1, Y1, P2, P2, P2, P2, P1, P1, P1,
                      P1, P1, P1, P2, P2, Y1, Y1, BL, BL, Y1, Y1, P2, P2, P1, P1, P1,
                      P1, TP, P1, P2, Y1, BL, W2, BL, BL, W2, BL, Y1, P2, P1, TP, P1,
                      TP, TP, P1, P2, Y1, BL, BL, W2, W2, BL, BL, Y1, P2, P1, TP, TP,
                      TP, TP, TP, P1, P2, Y1, Y1, Y1, Y1, Y1, Y1, P2, P1, TP, TP, TP,
                      TP, TP, TP, TP, P1, P1, P1, P1, P1, P1, P1, P1, TP, TP, TP, TP,
                      TP, TP, P1, P1, P1, P1, TP, TP, TP, TP, P1, P1, P1, P1, TP, TP
};

const uint8_t hackepeter[] PROGMEM = {
                      3, 7, 17,

                      2, 1,
                      R1, R1,

                      4, 2,
                      R1, R2, R2, R1,
                      R2, R3, R3, R2,
                      
                      8, 4,
                      TP, TP, R1, R2, R2, R1, TP, TP,
                      R1, R2, R2, R3, R3, R2, R2, R1,
                      R1, R2, R2, R3, R3, R2, R2, R1,
                      TP, TP, R1, R2, R2, R1, TP, TP                      
};

byte maze[1024]; // Contains the current map after loading from PROGMEM

#define LEFT -1
#define MIDDLE 0
#define RIGHT 1
#define NORTH
#define EAST
#define SOUTH
#define WEST

// Player
byte hitPoints;
byte magicPoints;
byte playerWeapons; // Encode the weapon the player carries
byte playerSpells;  // Encode the spells the player carries
byte playerArmors;  // Encode the armors the player carries
byte currentWeapon;
byte currentSpell;
byte currentArmor;
byte playerKeys;
byte playerElixirs;
byte playerMagicPotions;
byte playerBolts;
byte playerArrows;
byte playerX;
byte playerY;
byte playerDirection; // 1 == North, 2 == East, 3 == South, 4 == West
int playerDelay;
int playerDelayCounter;
int playerMagicCounter;
byte playerFireButtonStatus; // 0 == Nothing, 1 == Pressed (counter incrementing), 2 == Hold (ready for actions), 3 == Keep pressed after action (counter incrementing) 
int playerFireButtonCounter;
int playerFireButtonCounterHoldTime; // Depends on player delay. If couter reaches value, status is set to 2 (ready for actions)
byte playerMap[256];
boolean playerMoved;     // true if player has moved (redraw required)
boolean playerAction;    // true if player attacks or uses a spell
boolean playerFootsteps;
boolean playerImmortal;  // true if player has used the IMMORTAL spell
boolean playerHaste;     // true if player has used the HASTE spell

// Saved game (status if a maze greater the first one is entered)
byte savedMaze;
byte savedHitPoints;
byte savedMagicPoints;
byte savedPlayerWeapons;
byte savedPlayerSpells;
byte savedPlayerArmors;
byte savedCurrentWeapon;
byte savedCurrentSpell;
byte savedCurrentArmor;
byte savedPlayerKeys;
byte savedPlayerElixirs;
byte savedPlayerMagicPotions;
byte savedPlayerBolts;
byte savedPlayerArrows;

// Monsters
byte monsterX[16];
byte monsterY[16];
byte monsterType[16];
byte monsterHitPoints[16];
byte monsterAttack[16];
byte monsterAttackLongRange[16];
byte monsterDefend[16];
byte monsterStrategy[16];  // 1 == random, 2 == guided, 3 == patrol
byte monsterDirection[16];
byte monsterStatus[16];    // 0 == not initialized, 1 == active, 2 == killed
int monsterDelay[16];
int monsterDelayCounter[16];
boolean monsterMovedWithinEyeshot;

// Items
byte itemX[16];
byte itemY[16];
byte itemType[16];
byte itemStatus[16]; // 0 == not initialized, 1 == active

// Start method
void setup() 
{
  // Initialize serial connection
  Serial.begin(9600);

  // Initialize random number generator
  randomSeed(analogRead(40));

  // Initialize joysticks and  buttons
  pinMode(buttonL1, INPUT);     
  pinMode(buttonR1, INPUT);     
  pinMode(buttonU1, INPUT);     
  pinMode(buttonD1, INPUT);     
  pinMode(buttonFire1, INPUT);     
  pinMode(buttonL2, INPUT);     
  pinMode(buttonR2, INPUT);     
  pinMode(buttonU2, INPUT);     
  pinMode(buttonD2, INPUT);     
  pinMode(buttonFire2, INPUT);     
  pinMode(buttonReset, INPUT);     
  pinMode(buttonPause, INPUT);    

  // Activate internal pull-up resistors 
  digitalWrite(buttonL1, HIGH);
  digitalWrite(buttonR1, HIGH);
  digitalWrite(buttonU1, HIGH);
  digitalWrite(buttonD1, HIGH);
  digitalWrite(buttonFire1, HIGH);
  digitalWrite(buttonL2, HIGH);
  digitalWrite(buttonR2, HIGH);
  digitalWrite(buttonU2, HIGH);
  digitalWrite(buttonD2, HIGH);
  digitalWrite(buttonFire2, HIGH);
  digitalWrite(buttonReset, HIGH);
  digitalWrite(buttonPause, HIGH);

  // Initialize matrix and define text mode
  matrix.begin();
  matrix.setTextSize(1);
  matrix.setTextWrap(false);

  // Resets the saved game to maze 1 (initial value)
  savedMaze = 1;

  // Enter the game loop
  loop();
}

// Joystick inquiry (allows to use a classic joystick or a LEDmePlay Joypad without any configuration)
boolean joy1Up()
{
  if((digitalRead(buttonU1) == LOW && digitalRead(buttonD1) != LOW) || (digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW && analogRead(analogY1) > (512 + sensitivity))){ return true; }
  return false;
}
boolean joy1Down()
{
  if((digitalRead(buttonD1) == LOW && digitalRead(buttonU1) != LOW) || (digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW && analogRead(analogY1) < (512 - sensitivity))){ return true; }
  return false;
}
boolean joy1Left()
{
  if((digitalRead(buttonL1) == LOW && digitalRead(buttonR1) != LOW) || (digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW && analogRead(analogX1) > (512 + sensitivity))){ return true; }
  return false;
}
boolean joy1Right()
{
  if((digitalRead(buttonR1) == LOW && digitalRead(buttonL1) != LOW) || (digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW && analogRead(analogX1) < (512 - sensitivity))){ return true; }
  return false;
}
boolean joy1Fire()
{
  if(digitalRead(buttonFire1) == LOW || (digitalRead(buttonU1) == LOW && digitalRead(buttonD1) == LOW)){ return true; }
  return false;
}
boolean joy1FireA()
{
  if(digitalRead(buttonFire1) == LOW){ return true; }
  return false;
}
boolean joy1FireB()
{
  if(digitalRead(buttonU1) == LOW && digitalRead(buttonD1) == LOW){ return true; }
  return false;
}
boolean joy1IsLEDmePlayJoypad()
{
  if(digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW){ return true; }
  return false;
}
boolean joy2Up()
{
  if((digitalRead(buttonU2) == LOW && digitalRead(buttonD2) != LOW) || (digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW && analogRead(analogY2) > (512 + sensitivity))){ return true; }
  return false;
}
boolean joy2Down()
{
  if((digitalRead(buttonD2) == LOW && digitalRead(buttonU2) != LOW) || (digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW && analogRead(analogY2) < (512 - sensitivity))){ return true; }
  return false;
}
boolean joy2Left()
{
  if((digitalRead(buttonL2) == LOW && digitalRead(buttonR2) != LOW) || (digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW && analogRead(analogX2) > (512 + sensitivity))){ return true; }
  return false;
}
boolean joy2Right()
{
  if((digitalRead(buttonR2) == LOW && digitalRead(buttonL2) != LOW) || (digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW && analogRead(analogX2) < (512 - sensitivity))){ return true; }
  return false;
}
boolean joy2Fire()
{
  if(digitalRead(buttonFire2) == LOW || (digitalRead(buttonU2) == LOW && digitalRead(buttonD2) == LOW)){ return true; }
  return false;
}
boolean joy2FireA()
{
  if(digitalRead(buttonFire2)){ return true; }
  return false;
}
boolean joy2FireB()
{
  if(digitalRead(buttonU2) == LOW && digitalRead(buttonD2) == LOW){ return true; }
  return false;
}
boolean joy2IsLEDmePlayJoypad()
{
  if(digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW){ return true; }
  return false;
}

// Draw the M of the Mithotronic logo
void drawM(int x, int y)
{
  matrix.fillRect(x + 2, y + 2, 6, 1, matrix.Color333(0, 0, 0));
  matrix.fillRect(x, y + 3, 10, 1, matrix.Color333(0, 0, 0));

  matrix.fillRect(x, y + 4, 2, 6, matrix.Color333(3, 3, 3));
  matrix.fillRect(x + 2, y + 3, 2, 2, matrix.Color333(3, 3, 3));
  matrix.fillRect(x + 4, y + 4, 2, 6, matrix.Color333(3, 3, 3));
  matrix.fillRect(x + 6, y + 3, 2, 2, matrix.Color333(3, 3, 3));
  matrix.fillRect(x + 8, y + 4, 2, 6, matrix.Color333(3, 3, 3));
}

// Draw the T of the Mithotronic logo
void drawT(int x, int y)
{
  matrix.fillRect(x, y + 5, 6, 1, matrix.Color333(0, 0, 0));
  matrix.fillRect(x + 2, y + 10, 3, 1, matrix.Color333(0, 0, 0));

  matrix.fillRect(x, y + 3, 6, 2, matrix.Color333(0, 0, 7));
  matrix.fillRect(x + 2, y, 2, 10, matrix.Color333(0, 0, 7));
  matrix.fillRect(x + 4, y + 8, 1, 2, matrix.Color333(0, 0, 7));
}

// Draw the animated Mithotronic logo and play jingle
void mithotronic()
{
  int i = -10;
  do
  {
    drawM(7, i);
    drawT(19, 22 - i);
    i++;
    delay(50);
  }
  while(i <= 11);

  // SOUND: Jingle on start screen
  tone(audio,NOTE_C4,200);
  delay(400+20);
  tone(audio,NOTE_C4,90);
  delay(200-20);
  tone(audio,NOTE_G4,140);
  delay(400+20);
  tone(audio,NOTE_G4,140);
  delay(200-20);
  tone(audio,NOTE_C5,450);
  delay(600);
  tone(audio,NOTE_AS4,140);
  delay(200-20);
  tone(audio,NOTE_A4,130);
  delay(200-10);
  tone(audio,NOTE_F4,120);
  delay(200);
  tone(audio,NOTE_G4,1000);
  delay(3000);
}

// Sets color for the next character to show the LEDmePLay logo
void setLEDMePlayColor(int i)
{
  switch(i % 9)
  {
    case 0:
    matrix.setTextColor(matrix.Color333(5,0,0));
    break;
    case 1:
    matrix.setTextColor(matrix.Color333(5,2,0));
    break;
    case 2:
    matrix.setTextColor(matrix.Color333(2,5,0));
    break;
    case 3:
    matrix.setTextColor(matrix.Color333(0,5,0));
    break;
    case 4:
    matrix.setTextColor(matrix.Color333(0,5,2));
    break;
    case 5:
    matrix.setTextColor(matrix.Color333(0,2,5));
    break;
    case 6:
    matrix.setTextColor(matrix.Color333(0,0,5));
    break;
    case 7:
    matrix.setTextColor(matrix.Color333(2,0,5));
    break;
    case 8:
    matrix.setTextColor(matrix.Color333(5,0,2));
    break;
  }
}

// Draw the LEDmePlay logo
void ledMePlay()
{
  // Clear screen
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  int i = 0;
  do
  {
    // Write 'LEDmePlay'
    setLEDMePlayColor(i++);
    matrix.setCursor(7, 5);
    matrix.println("L");
    setLEDMePlayColor(i++);
    matrix.setCursor(13, 5);
    matrix.println("E");
    setLEDMePlayColor(i++);
    matrix.setCursor(19, 5);
    matrix.println("D");

    setLEDMePlayColor(i++);
    matrix.setCursor(10, 11);
    matrix.println("m");
    setLEDMePlayColor(i++);
    matrix.setCursor(16, 11);
    matrix.println("e");

    setLEDMePlayColor(i++);
    matrix.setCursor(4, 19);
    matrix.println("P");
    setLEDMePlayColor(i++);
    matrix.setCursor(10, 19);
    matrix.println("l");
    setLEDMePlayColor(i++);
    matrix.setCursor(16, 19);
    matrix.println("a");
    setLEDMePlayColor(i++);
    matrix.setCursor(22, 19);
    matrix.println("y");
    
    i++;
    if(i > 81)
    {
      i = 0;
    }

    int j = 0;
    do
    {
      j++;
      delay(1);
    }
    while(j < 250 && !joy1Fire() && !joy2Fire());
  }
  while(!joy1Fire() && !joy2Fire());
  // SOUND: Leave logo screen
  tone(audio,1024,20);
  delay(200);
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
}

// Show title screen
void title()
{
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  matrix.setCursor(7, 3);
  matrix.setTextColor(matrix.Color333(5, 2, 0));
  matrix.println("HEX");
  
  drawMonster(1, GOBLIN, true, true, -8, 0);
  drawMonster(1, WARRIOR, true, true, 0, 0);
  drawMonster(1, RAT, true, true, 9, 1);

  matrix.drawLine(0, 28, 31, 28, matrix.Color333(0, 0, 1));
  
  // Wait for fire button to be pressed
  do
  {
    delay(1);
  }
  while (!joy1Fire());
  do
  {
    delay(1);
  }
  while (joy1Fire());
  // SOUND: Start game
  tone(audio, 1024, 20);
  delay(100);
}

// Initialize all variables for a new game
void setupGame()
{
  hitPoints = 8;
  magicPoints = 8;
  currentWeapon = GLADIUS;
  currentSpell = 0;
  currentArmor = 0;
  playerKeys = 0;
  playerElixirs = 0;
  playerMagicPotions = 0;
  playerBolts = 0;
  playerArrows = 0;
  playerWeapons = 1;
  playerArmors = 0;
  playerSpells = 0;

  currentMaze = 1;
  mazeColor = 1;
  updateMazeColors();

  // Check whether player has already reached a higher maze in a previous game (enables option to go back to this level)
  if(savedMaze != 1)
  {
    loadGameMenu();
  }
}

// Automatically saves the status of the player if a new level is reached
void autoSaveGame()
{
  savedMaze = currentMaze;
  savedHitPoints = hitPoints;
  savedMagicPoints = magicPoints;
  savedPlayerWeapons = playerWeapons;
  savedPlayerSpells = playerSpells;
  savedPlayerArmors = playerArmors;
  savedCurrentWeapon = currentWeapon;
  savedCurrentSpell = currentSpell;
  savedCurrentArmor = currentArmor;
  savedPlayerKeys = playerKeys;
  savedPlayerElixirs = playerElixirs;
  savedPlayerMagicPotions = playerMagicPotions;
  savedPlayerBolts = playerBolts;
  savedPlayerArrows = playerArrows;
}

// Loads the status of the last game in the level in which the player has died
void loadLastGame()
{
  currentMaze = savedMaze;
  hitPoints = savedHitPoints;
  magicPoints = savedMagicPoints;
  playerWeapons = savedPlayerWeapons;
  playerSpells = savedPlayerSpells;
  playerArmors = savedPlayerArmors;
  currentWeapon = savedCurrentWeapon;
  currentSpell = savedCurrentSpell;
  currentArmor = savedCurrentArmor;
  playerKeys = savedPlayerKeys;
  playerElixirs = savedPlayerElixirs;
  playerMagicPotions = savedPlayerMagicPotions;
  playerBolts = savedPlayerBolts;
  playerArrows = savedPlayerArrows;  
}

// Initialize variables for a new maze
void setupMaze()
{
  // Save game automatically
  autoSaveGame();
  
  // Reset player status
  playerDirection = 3;
  playerDelay = 200;
  playerFireButtonStatus = 0;
  playerFireButtonCounter = 0;
  playerFireButtonCounterHoldTime = playerDelay * 2;
  playerFootsteps = 0;
  playerImmortal = false;
  playerHaste = false;
  playerMagicCounter = 0;

  // Reset monsters
  byte monsterCounter = 0;
  for(int i = 0; i < 16; i++)
  {
    monsterX[i] = 0;
    monsterY[i] = 0;
    monsterType[i] = 0;
    monsterAttack[i] = 0;
    monsterAttackLongRange[i] = 0;
    monsterDefend[i] = 0;
    monsterStrategy[i] = 0;
    monsterDirection[i] = 0;
    monsterHitPoints[i] = 0;
    monsterStatus[i] = 0;
    monsterDelay[i] = 0;
  }

  // Reset items
  byte itemCounter = 0;
  for(int i = 0; i < 16; i++)
  {
    itemX[i] = 0;
    itemY[i] = 0;
    itemType[i] = 0;
    itemStatus[i] = 0;
  }

  // Reset maze
  for(int i = 0; i < 1024; i++)
  {
    maze[i] = 0;  
  }
  
  // Copy level map and enemies from progmem
  int k = 0;
  for(int i = 0; i < currentMaze; i++)
  {
    mazeWidth = byte(pgm_read_byte_near(maps + k));
    mazeHeight = byte(pgm_read_byte_near(maps + 1 + k));
    mazeColor = byte(pgm_read_byte_near(maps + 2 + k));
    updateMazeColors();

    if(i < currentMaze - 1)
    {
      k = k + ((mazeWidth * mazeHeight) + 3);
    }
  }

  byte j = 0;
  for(int i = 0; i < (mazeWidth * mazeHeight); i++)
  {
    j = byte(pgm_read_byte_near(maps + 3 + i + k));

     // Wall element
    if(j == EMPTY)
    {
      maze[i] = EMPTY;
    }

    // Wall element
    if(j == WALL)
    {
      maze[i] = WALL;
    }

    // Player start position
    if(j == PLAYER_START)
    {
      playerX = i % mazeWidth;
      playerY = i / mazeHeight;
      j = 0;
    }

    // Exit
    if(j == EXIT)
    {
      maze[i] = EXIT;
    }

    // Door
    if(j == DOOR)
    {
      maze[i] = DOOR;
    }

    // Items detected
    if(j > 4 && j < 22)
    {
      itemX[itemCounter] = i % mazeWidth;
      itemY[itemCounter] = i / mazeHeight;
      itemStatus[itemCounter] = 1;
      if(j == KEY){ itemType[itemCounter] = KEY; } // Key
      if(j == ELIXIR){ itemType[itemCounter] = ELIXIR; } // Elixir
      if(j == MAGIC_POTION){ itemType[itemCounter] = MAGIC_POTION; } // Magic Potion
      if(j == GLADIUS){ itemType[itemCounter] = GLADIUS; } // Gladius
      if(j == SWORD){ itemType[itemCounter] = SWORD; } // Sword
      if(j == CLAYMORE){ itemType[itemCounter] = CLAYMORE; } // Claymore
      if(j == CROSSBOW){ itemType[itemCounter] = CROSSBOW; } // Crossbow
      if(j == LONGBOW){ itemType[itemCounter] = LONGBOW; } // Longbow
      if(j == LEATHER){ itemType[itemCounter] = LEATHER; } // Leather Armor
      if(j == HEAVY){ itemType[itemCounter] = HEAVY; } // Heavy Armor
      if(j == INVISIBILITY){ itemType[itemCounter] = INVISIBILITY; } // Invisibility Armor
      if(j == MAGIC){ itemType[itemCounter] = MAGIC; } // Magic Armor
      if(j == HEALING){ itemType[itemCounter] = HEALING; } // Healing Spell
      if(j == IMMORTAL){ itemType[itemCounter] = IMMORTAL; } // Immortality Spell
      if(j == FIREBALL){ itemType[itemCounter] = FIREBALL; } // Fireball Spell
      if(j == LIGHTNING){ itemType[itemCounter] = LIGHTNING; } // Lightning Spell
      if(j == HASTE){ itemType[itemCounter] = HASTE; } // Haste Spell
      itemCounter++;
    }
    
    // Rat detected
    if(j == RAT)
    {
      monsterX[monsterCounter] = i % mazeWidth;
      monsterY[monsterCounter] = i / mazeHeight;
      monsterType[monsterCounter] = RAT;
      monsterAttack[monsterCounter] = 1;
      monsterAttackLongRange[monsterCounter] = 1;
      monsterDefend[monsterCounter] = 1;
      monsterStrategy[monsterCounter] = 1;
      monsterDirection[monsterCounter] = 1;
      monsterHitPoints[monsterCounter] = 2;
      monsterStatus[monsterCounter] = 1;
      monsterDelay[monsterCounter] = 600;
      monsterCounter++;
      j = 0;
    }
    // Spider detected
    if(j == SPIDER)
    {
      monsterX[monsterCounter] = i % mazeWidth;
      monsterY[monsterCounter] = i / mazeHeight;
      monsterType[monsterCounter] = SPIDER;
      monsterAttack[monsterCounter] = 2;
      monsterAttackLongRange[monsterCounter] = 0;
      monsterDefend[monsterCounter] = 1;
      monsterStrategy[monsterCounter] = 1;
      monsterDirection[monsterCounter] = 1;
      monsterHitPoints[monsterCounter] = 2;
      monsterStatus[monsterCounter] = 1;
      monsterDelay[monsterCounter] = 400;
      monsterCounter++;
      j = 0;
    }
    // Goblin detected
    if(j == GOBLIN)
    {
      monsterX[monsterCounter] = i % mazeWidth;
      monsterY[monsterCounter] = i / mazeHeight;
      monsterType[monsterCounter] = GOBLIN;
      monsterAttack[monsterCounter] = 2;
      monsterAttackLongRange[monsterCounter] = 0;
      monsterDefend[monsterCounter] = 1;
      monsterStrategy[monsterCounter] = 3;
      monsterDirection[monsterCounter] = 1;
      monsterHitPoints[monsterCounter] = 3;
      monsterStatus[monsterCounter] = 1;
      monsterDelay[monsterCounter] = 600;
      monsterCounter++;
      j = 0;
    }
    // Mothman detected
    if(j == MOTHMAN)
    {
      monsterX[monsterCounter] = i % mazeWidth;
      monsterY[monsterCounter] = i / mazeHeight;
      monsterType[monsterCounter] = MOTHMAN;
      monsterAttack[monsterCounter] = 3;
      monsterAttackLongRange[monsterCounter] = 2;
      monsterDefend[monsterCounter] = 2;
      monsterStrategy[monsterCounter] = 1;
      monsterDirection[monsterCounter] = 1;
      monsterHitPoints[monsterCounter] = 3;
      monsterStatus[monsterCounter] = 1;
      monsterDelay[monsterCounter] = 400;
      monsterCounter++;
      j = 0;
    }
    // Ghost detected
    if(j == GHOST)
    {
      monsterX[monsterCounter] = i % mazeWidth;
      monsterY[monsterCounter] = i / mazeHeight;
      monsterType[monsterCounter] = GHOST;
      monsterAttack[monsterCounter] = 3;
      monsterAttackLongRange[monsterCounter] = 0;
      monsterDefend[monsterCounter] = 2;
      monsterStrategy[monsterCounter] = 1;
      monsterDirection[monsterCounter] = 1;
      monsterHitPoints[monsterCounter] = 4;
      monsterStatus[monsterCounter] = 1;
      monsterDelay[monsterCounter] = 800;
      monsterCounter++;
      j = 0;
    }
    // Warrior detected
    if(j == WARRIOR)
    {
      monsterX[monsterCounter] = i % mazeWidth;
      monsterY[monsterCounter] = i / mazeHeight;
      monsterType[monsterCounter] = WARRIOR;
      monsterAttack[monsterCounter] = 3;
      monsterAttackLongRange[monsterCounter] = 0;
      monsterDefend[monsterCounter] = 3;
      monsterStrategy[monsterCounter] = 2;
      monsterDirection[monsterCounter] = 1;
      monsterHitPoints[monsterCounter] = 3;
      monsterStatus[monsterCounter] = 1;
      monsterDelay[monsterCounter] = 800;
      monsterCounter++;
      j = 0;
    }
    // Beholder detected
    if(j == BEHOLDER)
    {
      monsterX[monsterCounter] = i % mazeWidth;
      monsterY[monsterCounter] = i / mazeHeight;
      monsterType[monsterCounter] = BEHOLDER;
      monsterAttack[monsterCounter] = 4;
      monsterAttackLongRange[monsterCounter] = 3;
      monsterDefend[monsterCounter] = 3;
      monsterStrategy[monsterCounter] = 2;
      monsterDirection[monsterCounter] = 1;
      monsterHitPoints[monsterCounter] = 4;
      monsterStatus[monsterCounter] = 1;
      monsterDelay[monsterCounter] = 800;
      monsterCounter++;
      j = 0;
    }
    // Endboss detected
    if(j == ENDBOSS)
    {
      monsterX[monsterCounter] = i % mazeWidth;
      monsterY[monsterCounter] = i / mazeHeight;
      monsterType[monsterCounter] = ENDBOSS;
      monsterAttack[monsterCounter] = 5;
      monsterAttackLongRange[monsterCounter] = 4;
      monsterDefend[monsterCounter] = 4;
      monsterStrategy[monsterCounter] = 2;
      monsterDirection[monsterCounter] = 1;
      monsterHitPoints[monsterCounter] = 6;
      monsterStatus[monsterCounter] = 1;
      monsterDelay[monsterCounter] = 800;
      monsterCounter++;
      j = 0;
    }
  }

  // Initialize player map  
  for(int i = 0; i < mazeWidth * mazeHeight; i++)
  {
    playerMap[i] = 0;
  }  
}

// Enables the player to resume a game in level in which the player has died last time
void loadGameMenu()
{
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
  matrix.drawRect(1, 1, 30, 30, matrix.Color333(5, 2, 0));

  matrix.setTextColor(matrix.Color333(red * 3, green * 3, blue * 3));
  matrix.setCursor(1, 8);
  matrix.print(" New");    
  matrix.setCursor(1, 16);
  matrix.print(">Last");    

  boolean newGame = false;

  // Wait for fire button to be pressed
  do
  {
    delay(1);
    if(joy1Up())
    {
      newGame = true;
      matrix.setCursor(1, 8);
      matrix.print(">");    
      matrix.fillRect(2, 16, 4, 7, matrix.Color333(0, 0, 0));
      delay(200);
    }
    if(joy1Down())
    {
      newGame = false;
      matrix.setCursor(1, 16);
      matrix.print(">");    
      matrix.fillRect(2, 8, 4, 7, matrix.Color333(0, 0, 0));
      delay(200);
    }
  }
  while(!joy1Fire());
  
  do
  {
    delay(1);
  }
  while (joy1Fire());
  // SOUND: Start game
  tone(audio, 1024, 20);
  delay(100);

  // Load old game
  if(!newGame){ loadLastGame(); }
}

// Shows the number of the next maze
void showMaze()
{
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
  matrix.drawRect(1, 1, 30, 30, matrix.Color333(5, 2, 0));

  matrix.setTextColor(matrix.Color333(red * 3, green * 3, blue * 3));
  matrix.setCursor(4, 8);
  matrix.print("Maze");    
  matrix.setCursor(13, 16);
  matrix.print(currentMaze);      

  // SOUND: New maze
  tone(audio,NOTE_D4, 300);
  delay(400);
  tone(audio,NOTE_E4, 300);
  delay(400);
  tone(audio,NOTE_F4, 600);
  delay(800);

  // Wait for fire button to be pressed
  do
  {
    matrix.setCursor(13, 16);
    matrix.print(currentMaze);      
    delay(1);
  }
  while(!joy1Fire());
  
  do
  {
    delay(1);
  }
  while (joy1Fire());
  // SOUND: Start game
  tone(audio, 1024, 20);
  delay(100);
}

// Shows end sequence after final maze has been completed
void endSequence()
{
  // Show all monsters
  for(byte i = RAT; i < ENDBOSS + 1; i++)
  {
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
    drawMonster(3, i, false, true, 0, 0);
    delay(1000); 
    drawMonster(3, i, false, false, 0, 0);
    drawMonster(2, i, false, true, 0, 0);
    delay(1000); 
    drawMonster(2, i, false, false, 0, 0);
    drawMonster(1, i, false, true, 0, 0);
    delay(1000); 
    drawMonster(1, i, false, false, 0, 0);
    drawMonster(1, i, true, true, 0, 0);

    for(int j = 1; j < 50; j++)
    {
      // SOUND: Player gets a hit
      tone(audio, 1000 - (j * 10), 1);
      delay(1);
    }
    
    delay(1000); 
    drawMonster(1, i, true, false, 0, 0);
    drawMonster(1, i, false, true, 0, 0);
    delay(1000); 

    tone(audio, 800, 20);
    drawBloodFountain(1, true);
    // SOUND: Monster killed
    for (int j = 0; j < 500; j++)
    {
      tone(audio, 1000 - j * 2, 1);
    }

    drawMonster(1, i, false, false, 0, 0);
    drawBloodFountain(1, false);
    drawHackepeter(1, i);
    delay(1000); 
  }

  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
  matrix.drawRect(1, 1, 30, 30, matrix.Color333(5, 2, 0));

  matrix.setTextColor(matrix.Color333(0, 0, 3));
  matrix.setCursor(4, 8);
  matrix.print("You");    
  matrix.setCursor(4, 16);
  matrix.print(" won");    

  // Wait for fire button to be pressed
  do
  {
    delay(1);
  }
  while(!joy1Fire());
  
  do
  {
    delay(1);
  }
  while (joy1Fire());
  // SOUND: Back to start screen
  tone(audio, 1024, 20);
  delay(100);
}

// Set RGB-values depending on maze color
// 0 == Invisible
// 1 == Blue
// 2 == Green
// 3 == Turquoise
// 4 == Red
// 5 == Violet
// 6 == Yellow
// 7 == White
void updateMazeColors()
{
  switch(mazeColor)
  {
    case 0:
    red = 0;
    green = 0;
    blue = 0;
    break;

    case 1:
    red = 0;
    green = 0;
    blue = 1;
    break;

    case 2:
    red = 0;
    green = 1;
    blue = 0;
    break;

    case 3:
    red = 0;
    green = 1;
    blue = 1;
    break;

    case 4:
    red = 1;
    green = 0;
    blue = 0;
    break;

    case 5:
    red = 1;
    green = 0;
    blue = 1;
    break;

    case 6:
    red = 1;
    green = 1;
    blue = 0;
    break;

    case 7:
    red = 1;
    green = 1;
    blue = 1;
    break;
  }
}

// Draw item (weapon, armor, spell etc. at position (_x, _y)
void drawItem(byte _item, byte _x, byte _y)
{
  if(_item == KEY)
  {
    // Key
    matrix.fillRect(_x + 2, _y, 2, 2, matrix.Color333(3,3,0));
    matrix.drawLine(_x + 2, _y + 2, _x + 2, _y + 3, matrix.Color333(1,1,0));
    matrix.drawLine(_x + 1, _y + 4, _x + 3, _y + 4, matrix.Color333(3,3,0));
    matrix.drawLine(_x + 1, _y + 6, _x + 3, _y + 6, matrix.Color333(3,3,0));
    matrix.drawPixel(_x, _y + 5, matrix.Color333(1,1,0));
    matrix.drawPixel(_x + 4, _y + 5, matrix.Color333(1,1,0));
  }
  else if(_item == ELIXIR)
  {    
    // Elexir
    matrix.drawRect(_x, _y, 3, 3, matrix.Color333(1,1,1));
    matrix.drawCircle(_x + 1, _y + 4, 2, matrix.Color333(1,1,1));
    matrix.drawPixel(_x + 1, _y + 2, matrix.Color333(0,0,0));
    matrix.drawLine(_x + 1, _y + 3, _x + 1, _y + 5, matrix.Color333(3,0,0));
    matrix.drawLine(_x, _y + 4, _x + 2, _y + 4, matrix.Color333(3,0,0));  
  }
  else if(_item == MAGIC_POTION)
  {    
    // Magic Potion
    matrix.drawRect(_x, _y, 3, 3, matrix.Color333(0,0,3));
    matrix.drawCircle(_x + 1, _y + 4, 2, matrix.Color333(0,0,3));
    matrix.drawPixel(_x + 1, _y + 2, matrix.Color333(0,0,0));
    matrix.fillRect(_x, _y + 4, 3, 2, matrix.Color333(3,3,0));
  }
  else if(_item == GLADIUS)
  {    
    // Gladius
    matrix.drawLine(_x + 1, _y, _x + 1, _y + 1, matrix.Color333(4,4,4));    
    matrix.drawLine(_x, _y + 2, _x + 2, _y + 2, matrix.Color333(1,1,1));    
    matrix.drawPixel(_x + 1, _y + 3, matrix.Color333(1,1,1));    
  }
  else if(_item == SWORD)
  {    
    // Sword
    matrix.drawLine(_x + 1, _y, _x + 1, _y + 2, matrix.Color333(4,4,4));    
    matrix.drawLine(_x, _y + 3, _x + 2, _y + 3, matrix.Color333(1,1,1));    
    matrix.drawPixel(_x + 1, _y + 4, matrix.Color333(1,1,1));    
  }
  else if(_item == CLAYMORE)
  {    
    // Claymore
    matrix.drawLine(_x + 1, _y, _x + 1, _y + 4, matrix.Color333(4,4,4));    
    matrix.drawLine(_x, _y + 5, _x + 2, _y + 5, matrix.Color333(1,1,1));    
    matrix.drawPixel(_x + 1, _y + 6, matrix.Color333(1,1,1));    
  }
  else if(_item == CROSSBOW)
  {    
    // Crossbow
    matrix.drawLine(_x + 1, _y, _x + 3, _y, matrix.Color333(3,3,0));    
    matrix.drawLine(_x + 2, _y + 1, _x + 2, _y + 4, matrix.Color333(1,1,0));    
    matrix.drawPixel(_x, _y + 1, matrix.Color333(3,3,0));    
    matrix.drawPixel(_x + 4, _y + 1, matrix.Color333(3,3,0));    

  }
  else if(_item == LONGBOW)
  {
    // Longbow
    matrix.drawLine(_x + 2, _y + 1, _x + 2, _y + 5, matrix.Color333(3,3,0));    
    matrix.drawPixel(_x + 1, _y, matrix.Color333(3,3,0));    
    matrix.drawPixel(_x + 1, _y + 6, matrix.Color333(3,3,0));    
    matrix.drawLine(_x + 1, _y + 3, _x + 3, _y + 3, matrix.Color333(1,1,1));    
    matrix.drawPixel(_x, _y + 3, matrix.Color333(4,4,4));    
  }
  else if(_item == LEATHER)
  {
    matrix.drawPixel(_x + 1, _y, matrix.Color333(3,1,0));
    matrix.drawLine(_x, _y + 1, _x, _y + 2, matrix.Color333(1,2,0));
    matrix.drawLine(_x + 2, _y + 1, _x + 2, _y + 2, matrix.Color333(1,2,0));
    matrix.drawLine(_x + 1, _y + 1, _x + 1, _y + 3, matrix.Color333(1,2,1));
    matrix.drawLine(_x, _y + 3, _x, _y + 5, matrix.Color333(1,2,1));
    matrix.drawLine(_x + 2, _y + 3, _x + 2, _y + 5, matrix.Color333(1,2,1));
  }  
  else if(_item == HEAVY)
  {
    matrix.drawPixel(_x + 1, _y, matrix.Color333(3,1,0));
    matrix.drawLine(_x, _y + 1, _x, _y + 2, matrix.Color333(0,0,2));
    matrix.drawLine(_x + 2, _y + 1, _x + 2, _y + 2, matrix.Color333(0,0,2));
    matrix.drawLine(_x + 1, _y + 1, _x + 1, _y + 3, matrix.Color333(2,2,2));
    matrix.drawLine(_x, _y + 3, _x, _y + 5, matrix.Color333(2,2,2));
    matrix.drawLine(_x + 2, _y + 3, _x + 2, _y + 5, matrix.Color333(2,2,2));
  }  
  else if(_item == INVISIBILITY)
  {
    matrix.drawPixel(_x + 2, _y, matrix.Color333(3,1,0));
    matrix.drawLine(_x, _y, _x, _y + 5, matrix.Color333(0,0,1));
    matrix.drawLine(_x + 4, _y, _x + 4, _y + 5, matrix.Color333(0,0,1));
  }  
  else if(_item == MAGIC)
  {
    matrix.drawPixel(_x + 2, _y, matrix.Color333(3,1,0));
    matrix.drawLine(_x + 1, _y + 1, _x + 1, _y + 2, matrix.Color333(0,3,0));
    matrix.drawLine(_x + 3, _y + 1, _x + 3, _y + 2, matrix.Color333(0,3,0));
    matrix.drawLine(_x + 2, _y + 1, _x + 2, _y + 3, matrix.Color333(0,3,1));
    matrix.drawLine(_x + 1, _y + 3, _x + 1, _y + 5, matrix.Color333(0,3,1));
    matrix.drawLine(_x + 3, _y + 3, _x + 3, _y + 5, matrix.Color333(0,3,1));
    matrix.drawLine(_x, _y, _x, _y + 5, matrix.Color333(1,0,1));
    matrix.drawLine(_x + 4, _y, _x + 4, _y + 5, matrix.Color333(1,0,1));
  }  
  else if(_item == HEALING)
  {    
    // Healing Spell
    matrix.fillRect(_x + 2, _y, 2, 6, matrix.Color333(3,0,0));
    matrix.fillRect(_x, _y + 2, 6, 2, matrix.Color333(3,0,0));
  }
  else if(_item == IMMORTAL)
  {    
    // Immortality Spell
    matrix.drawCircle(_x + 2, _y + 2, 2, matrix.Color333(3,3,0));
    matrix.drawPixel(_x + 2, _y + 1, matrix.Color333(2,1,0));
    matrix.drawLine(_x + 2, _y + 2, _x + 2, _y + 4, matrix.Color333(0,0,1));
    matrix.drawLine(_x + 1, _y + 2, _x + 3, _y + 2, matrix.Color333(0,0,1));  
  }
  else if(_item == FIREBALL)
  {    
    // Fireball Spell
    matrix.fillRect(_x, _y + 1, 1, 2, matrix.Color333(3,1,0));
    matrix.fillRect(_x + 1, _y + 1, 1, 2, matrix.Color333(2,1,0));
    matrix.fillRect(_x + 2, _y, 2, 4, matrix.Color333(1,2,0));
    matrix.fillRect(_x + 3, _y + 1, 2, 2, matrix.Color333(4,4,0));    
  }
  else if(_item == LIGHTNING)
  {    
    // Lightning Spell
    matrix.drawLine(_x, _y, _x, _y + 3, matrix.Color333(1,1,4));
    matrix.drawLine(_x, _y + 3, _x + 2, _y + 1, matrix.Color333(1,1,4));
    matrix.drawLine(_x + 2, _y + 1, _x + 2, _y + 6, matrix.Color333(1,1,4));
    matrix.drawPixel(_x + 1, _y + 5, matrix.Color333(1,1,4));
    matrix.drawPixel(_x + 3, _y + 5, matrix.Color333(1,1,4));
  }
  else if(_item == HASTE)
  {    
    // Lightning Spell
    matrix.drawLine(_x, _y, _x, _y + 4, matrix.Color333(1,1,1));
    matrix.drawLine(_x, _y, _x + 2, _y + 2, matrix.Color333(1,1,1));
    matrix.drawLine(_x, _y + 4, _x + 2, _y + 2, matrix.Color333(1,1,1));
    matrix.drawLine(_x + 3, _y, _x + 3, _y + 4, matrix.Color333(4,4,4));
    matrix.drawLine(_x + 3, _y, _x + 5, _y + 2, matrix.Color333(4,4,4));
    matrix.drawLine(_x + 3, _y + 4, _x + 5, _y + 2, matrix.Color333(4,4,4));
  }
}

// Returns true if player has _weapon in his inventory
boolean hasWeapon(byte _weapon)
{
  byte encoding = playerWeapons;
  if(encoding >= 16)
  {
    if(_weapon == LONGBOW)
    {
      return true;
    }
    else
    {
      encoding = encoding - 16;
    }
  }
  if(encoding >= 8)
  {
    if(_weapon == CROSSBOW)
    {
      return true;
    }
    else
    {
      encoding = encoding - 8;
    }
  }
  if(encoding >= 4)
  {
    if(_weapon == CLAYMORE)
    {
      return true;
    }
    else
    {
      encoding = encoding - 4;
    }
  }
  if(encoding >= 2)
  {
    if(_weapon == SWORD)
    {
      return true;
    }
    else
    {
      encoding = encoding - 2;
    }
  }
  if(encoding >= 1)
  {
    if(_weapon == GLADIUS)
    {
      return true;
    }
    else
    {
      encoding = encoding - 1;
    }
  }
  return false;
}

// Adds _weapon to player´s inventory
void addWeapon(byte _weapon)
{
  if(!hasWeapon(_weapon))
  {
    if(_weapon == GLADIUS){ playerWeapons = playerWeapons + 1; }
    else if(_weapon == SWORD ){ playerWeapons = playerWeapons + 2; }
    else if(_weapon == CLAYMORE ){ playerWeapons = playerWeapons + 4; }
    else if(_weapon == CROSSBOW ){ playerWeapons = playerWeapons + 8; }
    else if(_weapon == LONGBOW ){ playerWeapons = playerWeapons + 16; }
  }
}

// Returns true if player has _armor in his inventory
boolean hasArmor(byte _armor)
{
  byte encoding = playerArmors;
  if(encoding >= 8)
  {
    if(_armor == MAGIC)
    {
      return true;
    }
    else
    {
      encoding = encoding - 8;
    }
  }
  if(encoding >= 4)
  {
    if(_armor == INVISIBILITY)
    {
      return true;
    }
    else
    {
      encoding = encoding - 4;
    }
  }
  if(encoding >= 2)
  {
    if(_armor == HEAVY)
    {
      return true;
    }
    else
    {
      encoding = encoding - 2;
    }
  }
  if(encoding >= 1)
  {
    if(_armor == LEATHER)
    {
      return true;
    }
    else
    {
      encoding = encoding - 1;
    }
  }
  return false;
}

// Adds _armor to player´s inventory
void addArmor(byte _armor)
{
  if(!hasArmor(_armor))
  {
    if(_armor == LEATHER){ playerArmors = playerArmors + 1; }
    else if(_armor == HEAVY ){ playerArmors = playerArmors + 2; }
    else if(_armor == INVISIBILITY ){ playerArmors = playerArmors + 4; }
    else if(_armor == MAGIC ){ playerArmors = playerArmors + 8; }
  }
}

// Returns true if player has _spell in his inventory
boolean hasSpell(byte _spell)
{
  byte encoding = playerSpells;
  if(encoding >= 16)
  {
    if(_spell == HASTE)
    {
      return true;
    }
    else
    {
      encoding = encoding - 16;
    }
  }
  if(encoding >= 8)
  {
    if(_spell == LIGHTNING)
    {
      return true;
    }
    else
    {
      encoding = encoding - 8;
    }
  }
  if(encoding >= 4)
  {
    if(_spell == FIREBALL)
    {
      return true;
    }
    else
    {
      encoding = encoding - 4;
    }
  }
  if(encoding >= 2)
  {
    if(_spell == IMMORTAL)
    {
      return true;
    }
    else
    {
      encoding = encoding - 2;
    }
  }
  if(encoding >= 1)
  {
    if(_spell == HEALING)
    {
      return true;
    }
    else
    {
      encoding = encoding - 1;
    }
  }
  return false;
}

// Adds _spell to player´s inventory
void addSpell(byte _spell)
{
  if(!hasSpell(_spell))
  {
    if(_spell == HEALING){ playerSpells = playerSpells + 1; }
    else if(_spell == IMMORTAL ){ playerSpells = playerSpells + 2; }
    else if(_spell == FIREBALL ){ playerSpells = playerSpells + 4; }
    else if(_spell == LIGHTNING ){ playerSpells = playerSpells + 8; }
    else if(_spell == HASTE ){ playerSpells = playerSpells + 16; }
  }
}

// Show the game menu (to select weapons, armors, spells etc.)
void showMenu()
{
  int selectorX = 1;
  int selectorY = 3;
  boolean redraw;
  boolean exitMenu = false;

  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));

  do
  {
    redraw = false;
    
    // Show weapon
    matrix.setTextColor(matrix.Color333(3,0,0));
    matrix.setCursor(3, 0);
    matrix.print("W");    
    // Gladius
    if(currentWeapon == GLADIUS)
    {
      drawItem(GLADIUS, 10, 2);
    }
    // Sword
    else if(currentWeapon == SWORD)
    {
      drawItem(SWORD, 10, 1);
    }
    // Claymore
    else if(currentWeapon == CLAYMORE)
    {
      drawItem(CLAYMORE, 10, 0);
    }
    // Crossbow
    else if(currentWeapon == CROSSBOW)
    {
      drawItem(CROSSBOW, 9, 1);
      for(byte i = 0; i < playerBolts; i++)
      {
        matrix.drawPixel(14, 6 - i, matrix.Color333(0, 0, 1));
      }
    }
    // Longbow
    else if(currentWeapon == LONGBOW)
    {
      drawItem(LONGBOW, 9, 0);
      for(byte i = 0; i < playerArrows; i++)
      {
        matrix.drawPixel(14, 6 - i, matrix.Color333(0, 0, 1));
      }
    }
  
    // Show armor
    matrix.setTextColor(matrix.Color333(0,3,0));
    matrix.setCursor(19, 0);
    matrix.print("A");
    if(playerArmors == 0)
    {
      matrix.drawLine(25, 3, 29, 3, matrix.Color333(1, 1, 1));
    }
    else
    {
      // Leather Armor
      if(currentArmor == LEATHER)
      {
        drawItem(LEATHER, 26, 1);
      }
      // Heavy Armor
      else if(currentArmor == HEAVY)
      {
        drawItem(HEAVY, 26, 1);
      }
      // Invisibility Armor
      else if(currentArmor == INVISIBILITY)
      {
        drawItem(INVISIBILITY, 25, 1);
      }
      // Magic Armor
      else if(currentArmor == MAGIC)
      {
        drawItem(MAGIC, 25, 1);
      }
    }
      
    // Show spell
    matrix.setTextColor(matrix.Color333(0,0,3));
    matrix.setCursor(3, 8);
    matrix.print("S");
    if(playerSpells == 0)
    {
      matrix.drawLine(9, 11, 13, 11, matrix.Color333(1, 1, 1));
    }
    else
     {
      // Healing spell
      if(currentSpell == HEALING)
      {
        drawItem(HEALING, 9, 9);
      }
      // Immortality spell
      else if(currentSpell == IMMORTAL)
      {
        drawItem(IMMORTAL, 9, 9);
      }
      // Fireball spell
      else if(currentSpell == FIREBALL)
      {
        drawItem(FIREBALL, 9, 9);
      }
      // Lightning spell
      else if(currentSpell == LIGHTNING)
      {
        drawItem(LIGHTNING, 10, 8);
      }
      // Haste spell
      else if(currentSpell == HASTE)
      {
        drawItem(HASTE, 9, 9);
      }
    }
  
    // Show number of keys
    drawItem(KEY, 19, 8);
    matrix.setTextColor(matrix.Color333(3,3,0));
    matrix.setCursor(25, 8);
    matrix.print(playerKeys);
  
    // Show number of elexirs
    drawItem(ELIXIR, 4, 16);
    matrix.fillRect(9, 16, 5, 7, matrix.Color333(0,0,0));
    matrix.setTextColor(matrix.Color333(3,0,3));
    matrix.setCursor(9, 16);
    matrix.print(playerElixirs);
  
    // Show number of magic portions
    drawItem(MAGIC_POTION, 20, 16);
    matrix.fillRect(25, 16, 5, 7, matrix.Color333(0,0,0));
    matrix.setTextColor(matrix.Color333(0,3,3));
    matrix.setCursor(25, 16);
    matrix.print(playerMagicPotions);
  
    // Show map
    matrix.drawLine(3, 29, 3, 25, matrix.Color333(2,2,2));
    matrix.drawLine(5, 29, 5, 25, matrix.Color333(2,2,2));
    matrix.drawPixel(4, 26, matrix.Color333(2,2,2));

    matrix.drawLine(7, 29, 7, 26, matrix.Color333(2,2,2));
    matrix.drawLine(9, 29, 9, 26, matrix.Color333(2,2,2));
    matrix.drawPixel(8, 25, matrix.Color333(2,2,2));
    matrix.drawPixel(8, 27, matrix.Color333(2,2,2));

    matrix.drawLine(11, 29, 11, 25, matrix.Color333(2,2,2));
    matrix.drawPixel(12, 25, matrix.Color333(2,2,2));
    matrix.drawPixel(13, 26, matrix.Color333(2,2,2));
    matrix.drawPixel(12, 27, matrix.Color333(2,2,2));

    // Exit
    matrix.drawLine(17, 29, 17, 25, matrix.Color333(2,2,2));
    matrix.drawLine(18, 25, 19, 25, matrix.Color333(2,2,2));
    matrix.drawLine(18, 27, 19, 27, matrix.Color333(2,2,2));
    matrix.drawLine(18, 29, 19, 29, matrix.Color333(2,2,2));

    matrix.drawLine(21, 29, 21, 28, matrix.Color333(2,2,2));
    matrix.drawLine(21, 26, 21, 25, matrix.Color333(2,2,2));
    matrix.drawLine(23, 29, 23, 28, matrix.Color333(2,2,2));
    matrix.drawLine(23, 26, 23, 25, matrix.Color333(2,2,2));
    matrix.drawPixel(22, 27, matrix.Color333(2,2,2));

    matrix.drawLine(25, 29, 25, 25, matrix.Color333(2,2,2));

    matrix.drawLine(28, 29, 28, 26, matrix.Color333(2,2,2));
    matrix.drawLine(27, 25, 29, 25, matrix.Color333(2,2,2));

    matrix.drawLine(selectorX * 16, selectorY * 8, selectorX * 16, (selectorY * 8) + 7, matrix.Color333(3,3,3));
    matrix.drawLine(selectorX * 16, (selectorY * 8) + 7, (selectorX * 16) + 15, (selectorY * 8) + 7, matrix.Color333(3,3,3));
    matrix.drawLine((selectorX * 16) + 15, (selectorY * 8) + 7, (selectorX * 16) + 15, selectorY * 8, matrix.Color333(3,3,3));

    do
    {
    }
    while(joy1Fire());
    do
    {
      if(joy1Up() && selectorY > 0)
      {
        // SOUND: Menu
        tone(audio, 512, 10);
        matrix.drawLine(selectorX * 16, selectorY * 8, selectorX * 16, (selectorY * 8) + 7, matrix.Color333(0,0,0));
        matrix.drawLine(selectorX * 16, (selectorY * 8) + 7, (selectorX * 16) + 15, (selectorY * 8) + 7, matrix.Color333(0,0,0));
        matrix.drawLine((selectorX * 16) + 15, (selectorY * 8) + 7, (selectorX * 16) + 15, selectorY * 8, matrix.Color333(0,0,0));
        selectorY--;
        matrix.drawLine(selectorX * 16, selectorY * 8, selectorX * 16, (selectorY * 8) + 7, matrix.Color333(3,3,3));
        matrix.drawLine(selectorX * 16, (selectorY * 8) + 7, (selectorX * 16) + 15, (selectorY * 8) + 7, matrix.Color333(3,3,3));
        matrix.drawLine((selectorX * 16) + 15, (selectorY * 8) + 7, (selectorX * 16) + 15, selectorY * 8, matrix.Color333(3,3,3));
        delay(200);
  
      }
      else if(joy1Down() && selectorY < 3)
      {
        // SOUND: Menu
        tone(audio, 512, 10);
        matrix.drawLine(selectorX * 16, selectorY * 8, selectorX * 16, (selectorY * 8) + 7, matrix.Color333(0,0,0));
        matrix.drawLine(selectorX * 16, (selectorY * 8) + 7, (selectorX * 16) + 15, (selectorY * 8) + 7, matrix.Color333(0,0,0));
        matrix.drawLine((selectorX * 16) + 15, (selectorY * 8) + 7, (selectorX * 16) + 15, selectorY * 8, matrix.Color333(0,0,0));
        selectorY++;
        matrix.drawLine(selectorX * 16, selectorY * 8, selectorX * 16, (selectorY * 8) + 7, matrix.Color333(3,3,3));
        matrix.drawLine(selectorX * 16, (selectorY * 8) + 7, (selectorX * 16) + 15, (selectorY * 8) + 7, matrix.Color333(3,3,3));
        matrix.drawLine((selectorX * 16) + 15, (selectorY * 8) + 7, (selectorX * 16) + 15, selectorY * 8, matrix.Color333(3,3,3));
        delay(200);
      }
      else if(joy1Left() && selectorX == 1)
      {
        // SOUND: Menu
        tone(audio, 512, 10);
        matrix.drawLine(selectorX * 16, selectorY * 8, selectorX * 16, (selectorY * 8) + 7, matrix.Color333(0,0,0));
        matrix.drawLine(selectorX * 16, (selectorY * 8) + 7, (selectorX * 16) + 15, (selectorY * 8) + 7, matrix.Color333(0,0,0));
        matrix.drawLine((selectorX * 16) + 15, (selectorY * 8) + 7, (selectorX * 16) + 15, selectorY * 8, matrix.Color333(0,0,0));
        selectorX--;
        matrix.drawLine(selectorX * 16, selectorY * 8, selectorX * 16, (selectorY * 8) + 7, matrix.Color333(3,3,3));
        matrix.drawLine(selectorX * 16, (selectorY * 8) + 7, (selectorX * 16) + 15, (selectorY * 8) + 7, matrix.Color333(3,3,3));
        matrix.drawLine((selectorX * 16) + 15, (selectorY * 8) + 7, (selectorX * 16) + 15, selectorY * 8, matrix.Color333(3,3,3));
        delay(200);
      }
      else if(joy1Right() && selectorX == 0)
      {
        // SOUND: Menu
        tone(audio, 512, 10);
        matrix.drawLine(selectorX * 16, selectorY * 8, selectorX * 16, (selectorY * 8) + 7, matrix.Color333(0,0,0));
        matrix.drawLine(selectorX * 16, (selectorY * 8) + 7, (selectorX * 16) + 15, (selectorY * 8) + 7, matrix.Color333(0,0,0));
        matrix.drawLine((selectorX * 16) + 15, (selectorY * 8) + 7, (selectorX * 16) + 15, selectorY * 8, matrix.Color333(0,0,0));
        selectorX++;
        matrix.drawLine(selectorX * 16, selectorY * 8, selectorX * 16, (selectorY * 8) + 7, matrix.Color333(3,3,3));
        matrix.drawLine(selectorX * 16, (selectorY * 8) + 7, (selectorX * 16) + 15, (selectorY * 8) + 7, matrix.Color333(3,3,3));
        matrix.drawLine((selectorX * 16) + 15, (selectorY * 8) + 7, (selectorX * 16) + 15, selectorY * 8, matrix.Color333(3,3,3));
        delay(200);
      }
      // Selection
      else if(joy1FireA())
      {
        delay(100);
        do
        {          
        }
        while(joy1FireA());
        delay(100);
        // Select weapon
        if(selectorX == 0 && selectorY == 0)
        {
          // SOUND: Menu
          tone(audio, 1024, 10);
          do
          {
            currentWeapon++;
            if(currentWeapon == LONGBOW + 1)
            {
              currentWeapon = GLADIUS;
            }
          }
          while(!hasWeapon(currentWeapon));
          matrix.fillRect(8, 0, 7, 7, matrix.Color333(0,0,0));
          redraw = true;
        }
        // Select armor
        else if(selectorX == 1 && selectorY == 0)
        {
          // SOUND: Menu
          tone(audio, 1024, 10);
          do
          {
            currentArmor++;
            if(currentArmor == MAGIC + 1)
            {
              currentArmor = LEATHER;
            }
          }
          while(!hasArmor(currentArmor) && playerArmors != 0);
          matrix.fillRect(24, 0, 7, 7, matrix.Color333(0,0,0));
          redraw = true;
        }
        // Select spell
        else if(selectorX == 0 && selectorY == 1)
        {
          // SOUND: Menu
          tone(audio, 1024, 10);
          do
          {
            currentSpell++;
            if(currentSpell == HASTE + 1)
            {
              currentSpell = HEALING;
            }
          }
          while(!hasSpell(currentSpell) && playerSpells != 0);
          matrix.fillRect(8, 8, 7, 7, matrix.Color333(0,0,0));
          redraw = true;
        }
        // do nothing (position shows number of keys)
        else if(selectorX == 1 && selectorY == 1)
        {
          // SOUND: Menu
          tone(audio, 100, 50);
        }
        // Drink elixir
        else if(selectorX == 0 && selectorY == 2)
        {
          if(playerElixirs > 0 && hitPoints < 7)
          {
            // SOUND: Drink elixir
            for(byte i = 0; i < 100; i++)
            {
              tone(audio, 500 + i * 5, 5);
              delay(5);              
            }
            playerElixirs--;
            hitPoints = hitPoints + 4;
            if(hitPoints > 8)
            {
              hitPoints = 8;
            }
            redraw = true;
          }
          else
          {
            // SOUND: Menu
            tone(audio, 100, 50);
          }
        }
        // Drink magic potion
        else if(selectorX == 1 && selectorY == 2)
        {
          if(playerMagicPotions > 0 && magicPoints < 7)
          {
            // SOUND: Drink magic potion
            for(byte i = 0; i < 100; i++)
            {
              tone(audio, 1000 - i * 5, 5);
              delay(5);              
            }
            playerMagicPotions--;
            magicPoints = 8;
            redraw = true;
          }
          else
          {
            // SOUND: Menu
            tone(audio, 100, 50);         
          }
        }
        // Show map
        else if(selectorX == 0 && selectorY == 3)
        {
          // SOUND: Menu
          tone(audio, 1024, 10);
          drawPlayerMap();
          exitMenu = true;
        }
        // Leave menu
        else if(selectorX == 1 && selectorY == 3)
        {
          exitMenu = true;
        }
      }
      else if(joy1FireB())
      {
        exitMenu = true;              
      }
    }
    while(!redraw && !exitMenu);
  }
  while(!exitMenu);
}

// Set a specific element to the player´s auto map at position (_x, _y)
boolean setPlayerMapElement(byte _x, byte _y, byte _type)
{
  if(_x < 0 || _x >= mazeWidth || _y < 0 || _y >= mazeHeight || _type == 255)
  {
    return false; // Wrong access
  }
  playerMap[(_y * mazeWidth) + _x] = _type;
  return true;
}

// Set a specific element to the player´s auto map relative to player position
void setPlayerMapElementRelativeToPlayer(byte _distance, int _position)
{
  if(playerDirection == 1)
  {
    // North
    setPlayerMapElement(playerX + _position, playerY - _distance, getMazeElement(playerX + _position, playerY - _distance));
  }
  else if(playerDirection == 2)
  {
    // East
    setPlayerMapElement(playerX + _distance, playerY + _position, getMazeElement(playerX + _distance, playerY + _position));
  }
  else if(playerDirection == 3)
  {
    // South
    setPlayerMapElement(playerX - _position, playerY + _distance, getMazeElement(playerX - _position, playerY + _distance));
  }
  else
  {
    // West
    setPlayerMapElement(playerX - _distance, playerY - _position, getMazeElement(playerX - _distance, playerY - _position));
  }  
}

// Get element from player´s auto map at position (_x, _y)
byte getPlayerMapElement(byte _x, byte _y)
{
  if(_x < 0 || _x >= mazeWidth || _y < 0 || _y >= mazeHeight)
  {
    return 255; // Notifies wrong access
  }
  return playerMap[(_y * mazeWidth) + _x];
}

// Updates the auto map after player has moved
void updatePlayerMap()
{
  setPlayerMapElementRelativeToPlayer(0, LEFT);
  setPlayerMapElementRelativeToPlayer(0, MIDDLE);
  setPlayerMapElementRelativeToPlayer(0, RIGHT);

  setPlayerMapElementRelativeToPlayer(1, LEFT);
  setPlayerMapElementRelativeToPlayer(1, MIDDLE);
  setPlayerMapElementRelativeToPlayer(1, RIGHT);

  if(getMazeElementRelativeToPlayer(1, MIDDLE) == EMPTY)
  {
    setPlayerMapElementRelativeToPlayer(2, LEFT);
    setPlayerMapElementRelativeToPlayer(2, MIDDLE);
    setPlayerMapElementRelativeToPlayer(2, RIGHT);

    if(getMazeElementRelativeToPlayer(2, MIDDLE) == EMPTY)
    {
      setPlayerMapElementRelativeToPlayer(3, LEFT);
      setPlayerMapElementRelativeToPlayer(3, MIDDLE);
      setPlayerMapElementRelativeToPlayer(3, RIGHT);

      if(getMazeElementRelativeToPlayer(3, MIDDLE) == EMPTY)
      {
        setPlayerMapElementRelativeToPlayer(4, MIDDLE);
      }
    }
  }
}

// Draws the automap
void drawPlayerMap()
{
  // Delete screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
  
  matrix.drawRect(7, 7, 18, 18, matrix.Color333(1,1,1)); 
  matrix.drawRect(1, 1, 30, 30, matrix.Color333(5, 2, 0));

  // Draw map
  for(int i = 0; i < mazeWidth; i++)
  {
    for(int j = 0; j < mazeHeight; j++)
    {
      if(getPlayerMapElement(i, j) == WALL)
      {
        matrix.drawPixel(i + 8, j + 8, matrix.Color333(0,1,0));
      }
      if(getPlayerMapElement(i, j) == DOOR)
      {
        matrix.drawPixel(i + 8, j + 8, matrix.Color333(1,1,1));
      }
    }
  }
  
  // Draw player position
  matrix.drawPixel(playerX + 8, playerY + 8, matrix.Color333(0,0,3));
  
  do
  {
  }
  while(joy1Fire());
  do
  {
  }
  while(!joy1Fire());
}

// Set an element specified by _value to the maze position (_x, _y)
void setMazeElement(byte _x, byte _y, byte _value)
{
  if(!(_x < 0 || _x >= mazeWidth || _y < 0 || _y >= mazeHeight))
  {
    maze[(_y * mazeWidth) + _x] = _value;
  }
}

// Set an element specified by _value relativ to the player (depending on direction in a specified distance and position (left, middle, right))
void setMazeElementRelativeToPlayer(byte _distance, int _position, byte _value)
{
  if(playerDirection == 1)
  {
    // North
    setMazeElement(playerX + _position, playerY - _distance, _value);
  }
  else if(playerDirection == 2)
  {
    // East
    setMazeElement(playerX + _distance, playerY + _position, _value);
  }
  else if(playerDirection == 3)
  {
    // South
    setMazeElement(playerX - _position, playerY + _distance, _value);
  }
  else
  {
    // West
    setMazeElement(playerX - _distance, playerY - _position, _value);
  }
}

// Get maze element at position (_x, _y)
byte getMazeElement(byte _x, byte _y)
{
  if(_x < 0 || _x >= mazeWidth || _y < 0 || _y >= mazeHeight)
  {
    return 255; // Notifies wrong access
  }
  
  return maze[(_y * mazeWidth) + _x];
}

// Get maze element relative to player (depending on direction in a specified distance and position (left, middle, right))
byte getMazeElementRelativeToPlayer(int _distance, int _position)
{
  if(playerDirection == 1)
  {
    // North
    return getMazeElement(playerX + _position, playerY - _distance);
  }
  else if(playerDirection == 2)
  {
    // East
    return getMazeElement(playerX + _distance, playerY + _position);
  }
  else if(playerDirection == 3)
  {
    // South
    return getMazeElement(playerX - _position, playerY + _distance);
  }
  else
  {
    // West
    return getMazeElement(playerX - _distance, playerY - _position);
  }
}

// Get monster at maze position (_x, _y)
byte getMonster(byte _x, byte _y)
{
  for(int i = 0; i < 16; i++)
  {
    if(monsterX[i] == _x && monsterY[i] == _y)
    {
      return i + 1;
    }
  }  
  return 0;
}

// Get living monster at maze position (_x, _y)
byte getLivingMonster(byte _x, byte _y)
{
  for(int i = 0; i < 16; i++)
  {
    if(monsterX[i] == _x && monsterY[i] == _y && monsterStatus[i] == 1)
    {
      return i + 1;
    }
  }  
  return 0;
}

// Get dead monster at maze position (_x, _y)
byte getDeadMonster(byte _x, byte _y)
{
  for(int i = 0; i < 16; i++)
  {
    if(monsterX[i] == _x && monsterY[i] == _y && monsterStatus[i] == 2)
    {
      return i + 1;
    }
  }  
  return 0;
}

// Get living monster relative to player (depending on direction in a specified distance and position (left, middle, right))
byte getLivingMonsterRelativeToPlayer(int _distance, int _position)
{
  if(playerDirection == 1)
  {
    // North
    return getLivingMonster(playerX + _position, playerY - _distance);
  }
  else if(playerDirection == 2)
  {
    // East
    return getLivingMonster(playerX + _distance, playerY + _position);
  }
  else if(playerDirection == 3)
  {
    // South
    return getLivingMonster(playerX - _position, playerY + _distance);
  }
  else
  {
    // West
    return getLivingMonster(playerX - _distance, playerY - _position);
  }
}

// Get dead monster relative to player (depending on direction in a specified distance and position (left, middle, right))
byte getDeadMonsterRelativeToPlayer(int _distance, int _position)
{
  if(playerDirection == 1)
  {
    // North
    return getDeadMonster(playerX + _position, playerY - _distance);
  }
  else if(playerDirection == 2)
  {
    // East
    return getDeadMonster(playerX + _distance, playerY + _position);
  }
  else if(playerDirection == 3)
  {
    // South
    return getDeadMonster(playerX - _position, playerY + _distance);
  }
  else
  {
    // West
    return getDeadMonster(playerX - _distance, playerY - _position);
  }
}

// Get item at maze position (_x, _y)
byte getItem(byte _x, byte _y)
{
  for(int i = 0; i < 16; i++)
  {
    if(itemX[i] == _x && itemY[i] == _y && itemStatus[i] == 1)
    {
      return i + 1;
    }
  }  
  return 0;
}

// Get item relative to player (depending on direction in a specified distance and position (left, middle, right))
byte getItemRelativeToPlayer(byte _distance, int _position)
{
  if(playerDirection == 1)
  {
    // North
    return getItem(playerX + _position, playerY - _distance);
  }
  else if(playerDirection == 2)
  {
    // East
    return getItem(playerX + _distance, playerY + _position);
  }
  else if(playerDirection == 3)
  {
    // South
    return getItem(playerX - _position, playerY + _distance);
  }
  else
  {
    // West
    return getItem(playerX - _distance, playerY - _position);
  }
}

// Get the distance of monster _i
byte getMonsterDistance(byte _i)
{
  if(_i < 0 || _i > 15)
  {
    return 255;
  }
  return abs(playerX - monsterX[_i]) + abs(playerY - monsterY[_i]);
}

// Draw a specific maze element relative to player depending on _distance, _position (left, middle, right), and _type (empty, wall, door)
void drawMazeElement(byte _distance, int _position, byte _type)
{  
  if(_distance == 0)
  {
    if(_position == LEFT)
    {    
      matrix.drawLine(2, 2, 2, 29, matrix.Color333(red * 4, green * 4, blue * 4));
      if(_type == EMPTY && getMazeElementRelativeToPlayer(1, LEFT) == WALL)
      {
        matrix.drawLine(0, 2, 1, 2, matrix.Color333(red * 4, green * 4, blue * 4));
        matrix.drawLine(0, 29, 1, 29, matrix.Color333(red * 4, green * 4, blue * 4));
      }
      if(_type == WALL)
      {
        matrix.drawLine(0, 0, 1, 1, matrix.Color333(red * 4, green * 4, blue * 4));
        matrix.drawLine(0, 31, 1, 30, matrix.Color333(red * 4, green * 4, blue * 4));
      }    
    }
    if(_position == MIDDLE)
    {
      // Do nothing (it is the player position)
    }
    if(_position == RIGHT)
    {
      matrix.drawLine(29, 2, 29, 29, matrix.Color333(red * 4, green * 4, blue * 4));
      if(_type == EMPTY && getMazeElementRelativeToPlayer(1, RIGHT) == WALL)
      {
        matrix.drawLine(31, 2, 30, 2, matrix.Color333(red * 4, green * 4, blue * 4));
        matrix.drawLine(31, 29, 30, 29, matrix.Color333(red * 4, green * 4, blue * 4));
      }
      if(_type == WALL)
      {
        matrix.drawLine(31, 0, 30, 1, matrix.Color333(red * 4, green * 4, blue * 4));
        matrix.drawLine(31, 31, 30, 30, matrix.Color333(red * 4, green * 4, blue * 4));
      }    
    }
  }
  if(_distance == 1)
  {
    if(_position == LEFT)
    {
      matrix.drawLine(8, 8, 8, 23, matrix.Color333(red * 3, green * 3, blue * 3));
      if(_type == EMPTY && getMazeElementRelativeToPlayer(2, LEFT) == WALL)
      {
        matrix.drawLine(3, 8, 7, 8, matrix.Color333(red * 3, green * 3, blue * 3));
        matrix.drawLine(3, 23, 7, 23, matrix.Color333(red * 3, green * 3, blue * 3));
      }
      if(_type == WALL)
      {
        matrix.drawLine(3, 3, 7, 7, matrix.Color333(red * 3, green * 3, blue * 3));
        matrix.drawLine(3, 28, 7, 24, matrix.Color333(red * 3, green * 3, blue * 3));
      }    
    }
    if(_position == MIDDLE)
    {
      if(_type == EMPTY)
      {
        // nothing
      }
      if(_type == WALL)
      {
        matrix.drawLine(3, 2, 28, 2, matrix.Color333(red * 3, green * 3, blue * 3));
        matrix.drawLine(3, 29, 28, 29, matrix.Color333(red * 3, green * 3, blue * 3));
      }
      if(_type == DOOR)
      {
        // Door
        matrix.drawRect(2, 2, 28, 28, matrix.Color333(3, 3, 3));
        matrix.drawLine(5, 15, 8, 15, matrix.Color333(3, 3, 3));
      }
    }
    if(_position == RIGHT)
    {
      matrix.drawLine(23, 8, 23, 23, matrix.Color333(red * 3, green * 3, blue * 3));
      if(_type == EMPTY && getMazeElementRelativeToPlayer(2, RIGHT) == WALL)
      {
        matrix.drawLine(24, 8, 28, 8, matrix.Color333(red * 3, green * 3, blue * 3));
        matrix.drawLine(24, 23, 28, 23, matrix.Color333(red * 3, green * 3, blue * 3));
      }
      if(_type == WALL)
      {
        matrix.drawLine(24, 7, 28, 3, matrix.Color333(red * 3, green * 3, blue * 3));
        matrix.drawLine(24, 24, 28, 28, matrix.Color333(red * 3, green * 3, blue * 3));
      }    
    }
  }
  if(_distance == 2)
  {
    if(_position == LEFT)
    {
      matrix.drawLine(12, 12, 12, 19, matrix.Color333(red * 2, green * 2, blue * 2));
      if(_type == EMPTY && getMazeElementRelativeToPlayer(3, LEFT) == WALL)
      {
        matrix.drawLine(9, 12, 11, 12, matrix.Color333(red * 2, green * 2, blue * 2));
        matrix.drawLine(9, 19, 11, 19, matrix.Color333(red * 2, green * 2, blue * 2));
      }
      if(_type == WALL)
      {
        matrix.drawLine(9, 9, 11, 11, matrix.Color333(red * 2, green * 2, blue * 2));
        matrix.drawLine(9, 22, 11, 20, matrix.Color333(red * 2, green * 2, blue * 2));
      }    
    }
    if(_position == MIDDLE)
    {
      if(_type == EMPTY)
      {
        // nothing
      }
      if(_type == WALL)
      {
        matrix.drawLine(9, 8, 22, 8, matrix.Color333(red * 2, green * 2, blue * 2));
        matrix.drawLine(9, 23, 22, 23, matrix.Color333(red * 2, green * 2, blue * 2));
      }    
      if(_type == DOOR)
      {
        // Door
        matrix.drawRect(8, 8, 16, 16, matrix.Color333(2, 2, 2));
        matrix.drawLine(10, 15, 11, 15, matrix.Color333(2, 2, 2));
      }
    }
    if(_position == RIGHT)
    {
      matrix.drawLine(19, 12, 19, 19, matrix.Color333(red * 2, green * 2, blue * 2));
      if(_type == EMPTY && getMazeElementRelativeToPlayer(3, RIGHT) == WALL)
      {
        matrix.drawLine(22, 12, 20, 12, matrix.Color333(red * 2, green * 2, blue * 2));
        matrix.drawLine(22, 19, 20, 19, matrix.Color333(red * 2, green * 2, blue * 2));
      }
      if(_type == WALL)
      {
        matrix.drawLine(22, 9, 20, 11, matrix.Color333(red * 2, green * 2, blue * 2));
        matrix.drawLine(22, 22, 20, 20, matrix.Color333(red * 2, green * 2, blue * 2));
      }    
    }
  }
  if(_distance == 3)
  {
    if(_position == LEFT)
    {
      matrix.drawLine(14, 14, 14, 17, matrix.Color333(red * 1, green * 1, blue * 1));
      if(_type == EMPTY && getMazeElementRelativeToPlayer(4, LEFT) == WALL)
      {
        matrix.drawPixel(13, 14, matrix.Color333(red * 1, green * 1, blue * 1));
        matrix.drawPixel(13, 17, matrix.Color333(red * 1, green * 1, blue * 1));
      }
      if(_type == WALL)
      {
        matrix.drawPixel(13, 13, matrix.Color333(red * 1, green * 1, blue * 1));
        matrix.drawPixel(13, 18, matrix.Color333(red * 1, green * 1, blue * 1));
      }    
    }
    if(_position == MIDDLE)
    {
      if(_type == EMPTY)
      {
        // nothing
      }
      if(_type == WALL)
      {
        matrix.drawLine(13, 12, 18, 12, matrix.Color333(red * 1, green * 1, blue * 1));
        matrix.drawLine(13, 19, 18, 19, matrix.Color333(red * 1, green * 1, blue * 1));
      }    
      if(_type == DOOR)
      {
        // Door
        matrix.drawRect(12, 12, 8, 8, matrix.Color333(1, 1, 1));
      }
    }
    if(_position == RIGHT)
    {
      matrix.drawLine(17, 14, 17, 17, matrix.Color333(red * 1, green * 1, blue * 1));
      if(_type == EMPTY && getMazeElementRelativeToPlayer(4, RIGHT) == WALL)
      {
        matrix.drawPixel(18, 14, matrix.Color333(red * 1, green * 1, blue * 1));
        matrix.drawPixel(18, 17, matrix.Color333(red * 1, green * 1, blue * 1));
      }
      if(_type == WALL)
      {
        matrix.drawPixel(18, 13, matrix.Color333(red * 1, green * 1, blue * 1));
        matrix.drawPixel(18, 18, matrix.Color333(red * 1, green * 1, blue * 1));
      }    
    }
  }
  else if(_distance == 4)
  {
    if(_position == LEFT)
    {
      if(_type == EMPTY)
      {
        // nothing
      }
      if(_type == WALL)
      {
        matrix.drawLine(15, 15, 15, 16, matrix.Color333(red * 1, green * 1, blue * 1));
      }    
    }
    if(_position == MIDDLE)
    {
      if(_type == EMPTY)
      {
        // nothing
      }
      if(_type == WALL)
      {
        matrix.drawLine(15, 14, 16, 14, matrix.Color333(red * 1, green * 1, blue * 1));
        matrix.drawLine(15, 17, 16, 17, matrix.Color333(red * 1, green * 1, blue * 1));
      }    
      if(_type == DOOR)
      {
        // Door
        matrix.drawRect(15, 15, 2, 2, matrix.Color333(1, 1, 1));
      }
    }
    if(_position == RIGHT)
    {
      if(_type == EMPTY)
      {
        // nothing
      }
      if(_type == WALL)
      {
        matrix.drawLine(16, 15, 16, 16, matrix.Color333(red * 1, green * 1, blue * 1));
      }    
    }
  }
}

// Draw the orientation at the top of the screen
void drawOrientation()
{
  byte G = 0;
  byte B = 0;
  if(playerImmortal)
  {
    G = 3; B = 3;
    matrix.drawRect(0, 0, 32, 32, matrix.Color333(3,3,3));
  }
  if(playerHaste)
  {
    B = 3;
  }
  if(playerDirection == 1)
  {
    matrix.drawLine(13, 5, 13, 1, matrix.Color333(3,G,B));
    matrix.drawLine(13, 1, 17, 5, matrix.Color333(3,G,B));
    matrix.drawLine(17, 5, 17, 1, matrix.Color333(3,G,B));    
  }
  else if(playerDirection == 2)
  {
    matrix.drawLine(13, 1, 17, 1, matrix.Color333(3,G,B));
    matrix.drawLine(13, 3, 15, 3, matrix.Color333(3,G,B));
    matrix.drawLine(13, 5, 17, 5, matrix.Color333(3,G,B));
    matrix.drawLine(13, 1, 13, 5, matrix.Color333(3,G,B));
  }
  else if(playerDirection == 3)
  {
    matrix.drawLine(17, 1, 13, 1, matrix.Color333(3,G,B));
    matrix.drawLine(13, 1, 13, 3, matrix.Color333(3,G,B));
    matrix.drawLine(13, 3, 17, 3, matrix.Color333(3,G,B));
    matrix.drawLine(17, 3, 17, 5, matrix.Color333(3,G,B));
    matrix.drawLine(17, 5, 13, 5, matrix.Color333(3,G,B));
  }
  else
  {
    matrix.drawLine(13, 1, 13,5, matrix.Color333(3,G,B));
    matrix.drawLine(15, 3, 15, 5, matrix.Color333(3,G,B));
    matrix.drawLine(17, 1, 17, 5, matrix.Color333(3,G,B));
    matrix.drawLine(13, 5, 17, 5, matrix.Color333(3,G,B));
  }    
}

// Draw remaining hit points and magic points
void drawStatus()
{
  matrix.drawLine(4, 30, 27, 30, matrix.Color333(0,0,0));
  for(int i = 0; i < hitPoints; i++)
  {
    matrix.drawPixel(4 + i, 30, matrix.Color333(3,0,0));
  }
  for(int i = 0; i < magicPoints; i++)
  {
    matrix.drawPixel(27 - i, 30, matrix.Color333(0,3,0));
  }
}

// Player gets a hit with _damage hit points
void playerGetsHit(byte _damage)
{
  matrix.drawLine(1, 0, 30, 0, matrix.Color333(7,0,0));
  matrix.drawLine(1, 31, 30, 31, matrix.Color333(7,0,0));
  matrix.drawLine(0, 1, 0, 30, matrix.Color333(7,0,0));
  matrix.drawLine(31, 1, 31, 30, matrix.Color333(7,0,0));
  for(int i = 1; i < 50; i++)
  {
    // SOUND: Player gets a hit
    tone(audio, 1000 - (i * 10), 1);
    delay(1);
  }
  matrix.drawLine(1, 0, 30, 0, matrix.Color333(0,0,0));
  matrix.drawLine(1, 31, 30, 31, matrix.Color333(0,0,0));
  matrix.drawLine(0, 1, 0, 30, matrix.Color333(0,0,0));
  matrix.drawLine(31, 1, 31, 30, matrix.Color333(0,0,0));
  if(hitPoints < _damage)
  {
    hitPoints = 0;
  }
  else
  {
    hitPoints = hitPoints - _damage;
  }
  drawStatus();
}

// Draws a pixel in _color at screen position (_x, _y) (used to draw bitmaps)
void drawPixel(byte _x, byte _y, byte _color)
{
  if(_color == BL){ matrix.drawPixel(_x, _y, matrix.Color333(0, 0, 0)); }
  else if(_color == R1){ matrix.drawPixel(_x, _y, matrix.Color333(1, 0, 0)); }
  else if(_color == R2){ matrix.drawPixel(_x, _y, matrix.Color333(3, 0, 0)); }
  else if(_color == R3){ matrix.drawPixel(_x, _y, matrix.Color333(5, 0, 0)); }
  else if(_color == G1){ matrix.drawPixel(_x, _y, matrix.Color333(0, 1, 0)); }
  else if(_color == G2){ matrix.drawPixel(_x, _y, matrix.Color333(0, 3, 0)); }
  else if(_color == G3){ matrix.drawPixel(_x, _y, matrix.Color333(0, 5, 0)); }
  else if(_color == B1){ matrix.drawPixel(_x, _y, matrix.Color333(0, 0, 1)); }
  else if(_color == B2){ matrix.drawPixel(_x, _y, matrix.Color333(0, 0, 3)); }
  else if(_color == B3){ matrix.drawPixel(_x, _y, matrix.Color333(0, 0, 5)); }
  else if(_color == Y1){ matrix.drawPixel(_x, _y, matrix.Color333(1, 1, 0)); }
  else if(_color == Y2){ matrix.drawPixel(_x, _y, matrix.Color333(3, 3, 0)); }
  else if(_color == Y3){ matrix.drawPixel(_x, _y, matrix.Color333(5, 5, 0)); }
  else if(_color == P1){ matrix.drawPixel(_x, _y, matrix.Color333(1, 0, 1)); }
  else if(_color == P2){ matrix.drawPixel(_x, _y, matrix.Color333(3, 0, 3)); }
  else if(_color == P3){ matrix.drawPixel(_x, _y, matrix.Color333(5, 0, 5)); }
  else if(_color == T1){ matrix.drawPixel(_x, _y, matrix.Color333(0, 1, 1)); }
  else if(_color == T2){ matrix.drawPixel(_x, _y, matrix.Color333(0, 3, 3)); }
  else if(_color == T3){ matrix.drawPixel(_x, _y, matrix.Color333(0, 5, 5)); }
  else if(_color == W1){ matrix.drawPixel(_x, _y, matrix.Color333(1, 1, 1)); }
  else if(_color == W2){ matrix.drawPixel(_x, _y, matrix.Color333(3, 3, 3)); }
  else if(_color == W3){ matrix.drawPixel(_x, _y, matrix.Color333(5, 5, 5)); }
}

// Draws a chest in distance _distance
void drawChest(byte _distance)
{
  if(_distance == 1)
  {
    matrix.drawLine(13, 21, 18, 21, matrix.Color333(3,3,3));
    matrix.drawLine(12, 22, 19, 22, matrix.Color333(3,3,3));
    matrix.drawLine(12, 23, 12, 25, matrix.Color333(3,3,3));
    matrix.drawLine(19, 23, 19, 25, matrix.Color333(3,3,3));
    matrix.drawLine(12, 26, 19, 26, matrix.Color333(3,3,3));
    matrix.fillRect(13, 23, 6, 3, matrix.Color333(0,0,0));
    matrix.drawLine(15, 24, 16, 24, matrix.Color333(3,3,3));
  }
  else if(_distance == 2)
  {
    matrix.fillRect(14, 19, 4, 3, matrix.Color333(2,2,2));
    matrix.drawLine(15, 20, 16, 20, matrix.Color333(0,0,0));    
  }
  else if(_distance == 3)
  {
    matrix.fillRect(15, 17, 2, 2, matrix.Color333(1,1,1));    
  }
}

// Draws a specifc monster of type _type in distance _distance
// _attacking = true: Attack mode
// _draw = false: Monster is removed from the specified position
// offsetX and offsetY is the offset to the normal center position
void drawMonster(byte _distance, byte _type, boolean _attacking, boolean _draw, int offsetX, int offsetY)
{
  int arrayOffset = 0;
  byte monsterWidth = 0;
  byte monsterHeight = 0;

  if(_attacking){ _distance = 0; }

  if(_type == RAT)
  {
    arrayOffset = int(pgm_read_byte_near(ratPics + 3 - _distance));
    monsterWidth = byte(pgm_read_byte_near(ratPics + arrayOffset));
    monsterHeight = byte(pgm_read_byte_near(ratPics + arrayOffset + 1));
  }
  else if(_type == SPIDER)
  {
    arrayOffset = int(pgm_read_byte_near(spiderPics + 3 - _distance));
    monsterWidth = byte(pgm_read_byte_near(spiderPics + arrayOffset));
    monsterHeight = byte(pgm_read_byte_near(spiderPics + arrayOffset + 1));
  }
  else if(_type == GOBLIN)
  {
    arrayOffset = int(pgm_read_byte_near(goblinPics + 3 - _distance));
    monsterWidth = byte(pgm_read_byte_near(goblinPics + arrayOffset));
    monsterHeight = byte(pgm_read_byte_near(goblinPics + arrayOffset + 1));
  }
  else if(_type == MOTHMAN)
  {
    arrayOffset = int(pgm_read_byte_near(mothmanPics + 3 - _distance));
    monsterWidth = byte(pgm_read_byte_near(mothmanPics + arrayOffset));
    monsterHeight = byte(pgm_read_byte_near(mothmanPics + arrayOffset + 1));
  }
  else if(_type == GHOST)
  {
    arrayOffset = int(pgm_read_byte_near(ghostPics + 3 - _distance));
    monsterWidth = byte(pgm_read_byte_near(ghostPics + arrayOffset));
    monsterHeight = byte(pgm_read_byte_near(ghostPics + arrayOffset + 1));
  }
  else if(_type == WARRIOR)
  {
    arrayOffset = int(pgm_read_byte_near(warriorPics + 3 - _distance));
    monsterWidth = byte(pgm_read_byte_near(warriorPics + arrayOffset));
    monsterHeight = byte(pgm_read_byte_near(warriorPics + arrayOffset + 1));
  }
  else if(_type == BEHOLDER)
  {
    arrayOffset = int(pgm_read_byte_near(beholderPics + 3 - _distance));
    monsterWidth = byte(pgm_read_byte_near(beholderPics + arrayOffset));
    monsterHeight = byte(pgm_read_byte_near(beholderPics + arrayOffset + 1));
  }
  else if(_type == ENDBOSS)
  {
    arrayOffset = int(pgm_read_byte_near(endbossPics + 3 - _distance));
    // Hack for attacking endboss (returned value can not exceed 255)
    if(_attacking){ arrayOffset = arrayOffset + 256; }
    monsterWidth = byte(pgm_read_byte_near(endbossPics + arrayOffset));
    monsterHeight = byte(pgm_read_byte_near(endbossPics + arrayOffset + 1));
  }

  byte xShift = 16 - (monsterWidth / 2);
  byte yShift;

  if(_distance ==  0){ yShift = 27; }
  else if(_distance ==  1){ yShift = 26; }
  else if(_distance ==  2){ yShift = 21; }
  else if(_distance ==  3){ yShift = 18; }
  
  yShift = yShift - monsterHeight + 1;

  if(_draw)
  {
    // Draw monster
    for(byte i = 0; i < monsterHeight; i++)
    {
      for(byte j = 0; j < monsterWidth; j++)
      {
        if(_type == RAT){ drawPixel(offsetX + j + xShift, i + yShift, int(pgm_read_byte_near(ratPics + arrayOffset + 2 + (i * monsterWidth)+ j))); }
        else if(_type == SPIDER){ drawPixel(offsetX + j + xShift, offsetY + i + yShift, int(pgm_read_byte_near(spiderPics + arrayOffset + 2 + (i * monsterWidth)+ j))); }
        else if(_type == GOBLIN){ drawPixel(offsetX + j + xShift, offsetY + i + yShift, int(pgm_read_byte_near(goblinPics + arrayOffset + 2 + (i * monsterWidth)+ j))); }
        else if(_type == MOTHMAN){ drawPixel(offsetX + j + xShift, offsetY + i + yShift, int(pgm_read_byte_near(mothmanPics + arrayOffset + 2 + (i * monsterWidth)+ j))); }
        else if(_type == GHOST){ drawPixel(offsetX + j + xShift, offsetY + i + yShift, int(pgm_read_byte_near(ghostPics + arrayOffset + 2 + (i * monsterWidth)+ j))); }
        else if(_type == WARRIOR){ drawPixel(offsetX + j + xShift, offsetY + i + yShift, int(pgm_read_byte_near(warriorPics + arrayOffset + 2 + (i * monsterWidth)+ j))); }
        else if(_type == BEHOLDER){ drawPixel(offsetX + j + xShift, offsetY + i + yShift, int(pgm_read_byte_near(beholderPics + arrayOffset + 2 + (i * monsterWidth)+ j))); }
        else if(_type == ENDBOSS){ drawPixel(offsetX + j + xShift, offsetY + i + yShift, int(pgm_read_byte_near(endbossPics + arrayOffset + 2 + (i * monsterWidth)+ j))); }
      }
    }
  }
  else
  {
    // Remove monster
    for(byte i = 0; i < monsterHeight; i++)
    {
      for(byte j = 0; j < monsterWidth; j++)
      {
        if(_type == RAT)
        {
          if(byte(pgm_read_byte_near(ratPics + arrayOffset + 2 + (i * monsterWidth)+ j)) != TP)
          {
            drawPixel(offsetX + j + xShift, offsetY + i + yShift, BL);
          }
        }
        else if(_type == SPIDER)
        {
          if(byte(pgm_read_byte_near(spiderPics + arrayOffset + 2 + (i * monsterWidth)+ j)) != TP)
          {
            drawPixel(offsetX + j + xShift, offsetY + i + yShift, BL);
          }
        }
        else if(_type == GOBLIN)
        {
          if(byte(pgm_read_byte_near(goblinPics + arrayOffset + 2 + (i * monsterWidth)+ j)) != TP)
          {
            drawPixel(offsetX + j + xShift, offsetY + i + yShift, BL);
          }
        }
        else if(_type == MOTHMAN)
        {
          if(byte(pgm_read_byte_near(mothmanPics + arrayOffset + 2 + (i * monsterWidth)+ j)) != TP)
          {
            drawPixel(offsetX + j + xShift, offsetY + i + yShift, BL);
          }
        }
        else if(_type == GHOST)
        {
          if(byte(pgm_read_byte_near(ghostPics + arrayOffset + 2 + (i * monsterWidth)+ j)) != TP)
          {
            drawPixel(offsetX + j + xShift, offsetY + i + yShift, BL);
          }
        }
        else if(_type == WARRIOR)
        {
          if(byte(pgm_read_byte_near(warriorPics + arrayOffset + 2 + (i * monsterWidth)+ j)) != TP)
          {
            drawPixel(offsetX + j + xShift, offsetY + i + yShift, BL);
          }
        }
        else if(_type == BEHOLDER)
        {
          if(byte(pgm_read_byte_near(beholderPics + arrayOffset + 2 + (i * monsterWidth)+ j)) != TP)
          {
            drawPixel(offsetX + j + xShift, offsetY + i + yShift, BL);
          }
        }
        else if(_type == ENDBOSS)
        {
          if(byte(pgm_read_byte_near(endbossPics + arrayOffset + 2 + (i * monsterWidth)+ j)) != TP)
          {
            drawPixel(offsetX + j + xShift, offsetY + i + yShift, BL);
          }
        }
      }
    }
  }
}

// Draw blood at ground in a distance _distance
void drawHackepeter(byte _distance, byte _type)
{
  byte arrayOffset = 0;
  byte hackepeterWidth = 0;
  byte hackepeterHeight = 0;

  arrayOffset = byte(pgm_read_byte_near(hackepeter + 3 - _distance));
  hackepeterWidth = byte(pgm_read_byte_near(hackepeter + arrayOffset));
  hackepeterHeight = byte(pgm_read_byte_near(hackepeter + arrayOffset + 1));

  byte xShift = 16 - (hackepeterWidth / 2);
  byte yShift;

  if(_distance ==  1){ yShift = 28; }
  else if(_distance ==  2){ yShift = 22; }
  else if(_distance ==  3){ yShift = 18; }

  yShift = yShift - hackepeterHeight + 1;

  for(byte i = 0; i < hackepeterHeight; i++)
  {
    for(byte j = 0; j < hackepeterWidth; j++)
    {
      drawPixel(j + xShift, i + yShift, byte(pgm_read_byte_near(hackepeter + arrayOffset + 2 + (i * hackepeterWidth)+ j)));
    }
  }
}

// Draw blood fountain in distance _distance
// _draw = false: Removes blood fountain
void drawBloodFountain(byte _distance, boolean _draw)
{
  if(_distance == 1)
  {
    for(byte i = 1; i < 4; i++)
    {
      if(_draw)
      {
        matrix.drawCircle(15, 21 + i, i, matrix.Color333(i * 2, 0, 0));
        delay(50);
      }
      else
      {
        matrix.drawCircle(15, 21 + i, i, matrix.Color333(0, 0, 0));        
      }
    }
  }
  if(_distance == 2)
  {
    for(byte i = 1; i < 3; i++)
    {
      if(_draw)
      {
        matrix.drawCircle(16, 18 + i, i, matrix.Color333(i * 2, 0, 0));
        delay(100);
      }
      else
      {
        matrix.drawCircle(16, 18 + i, i, matrix.Color333(0, 0, 0));        
      }
    }
  }
  if(_distance == 3)
  {
    for(byte i = 1; i < 2; i++)
    {
      if(_draw)
      {
        matrix.drawCircle(16, 16 + i, i, matrix.Color333(i * 2, 0, 0));
        delay(150);
      }
      else
      {
        matrix.drawCircle(16, 16 + i, i, matrix.Color333(0, 0, 0));        
      }
    }
  }
}

// Draws the maze from the current view point of the player after player movement
void redrawMaze()
{
  // Delete screen (Fast fill)
  matrix.drawLine(15, 6, 15, 32, matrix.Color333(0,0,0));
  matrix.drawLine(16, 6, 16, 32, matrix.Color333(0,0,0));
  matrix.drawLine(14, 6, 14, 32, matrix.Color333(0,0,0));
  matrix.drawLine(17, 6, 17, 32, matrix.Color333(0,0,0));
  matrix.drawLine(13, 6, 13, 32, matrix.Color333(0,0,0));
  matrix.drawLine(18, 6, 18, 32, matrix.Color333(0,0,0));
  for (int16_t i = 0; i < 13; i++)
  {
    matrix.drawLine(12 - i, 0, 12 - i, 32, matrix.Color333(0,0,0));
    matrix.drawLine(19 + i, 0, 19 + i, 32, matrix.Color333(0,0,0));
  }
  matrix.drawLine(15, 0, 15, 5, matrix.Color333(0,0,0));
  matrix.drawLine(16, 0, 16, 5, matrix.Color333(0,0,0));
  matrix.drawLine(14, 0, 14, 5, matrix.Color333(0,0,0));
  matrix.drawLine(17, 0, 17, 5, matrix.Color333(0,0,0));
  matrix.drawLine(13, 0, 13, 5, matrix.Color333(0,0,0));
  matrix.drawLine(18, 0, 18, 5, matrix.Color333(0,0,0));
  
  // Draw maze  
  drawMaze();
}

// Draws the maze from the current view point of the player without deleting the current screen (used after monster movement or player actions)
void drawMaze()
{ 
  drawMazeElement(0, LEFT, getMazeElementRelativeToPlayer(0, LEFT));
  drawMazeElement(0, RIGHT, getMazeElementRelativeToPlayer(0, RIGHT));

  if(getMazeElementRelativeToPlayer(1, MIDDLE) == WALL)
  {
    drawMazeElement(1, MIDDLE, WALL);
  }
  else if(getMazeElementRelativeToPlayer(1, MIDDLE) == DOOR)
  {
    drawMazeElement(1, MIDDLE, DOOR);    
  }
  else
  {
    drawMazeElement(1, LEFT, getMazeElementRelativeToPlayer(1, LEFT));
    drawMazeElement(1, RIGHT, getMazeElementRelativeToPlayer(1, RIGHT));

    if(getMazeElementRelativeToPlayer(2, MIDDLE) == WALL)
    {
      drawMazeElement(2, MIDDLE, WALL);
    }
    else if(getMazeElementRelativeToPlayer(2, MIDDLE) == DOOR)
    {
      drawMazeElement(2, MIDDLE, DOOR);      
    }
    else
    {
      drawMazeElement(2, LEFT, getMazeElementRelativeToPlayer(2, LEFT));
      drawMazeElement(2, RIGHT, getMazeElementRelativeToPlayer(2, RIGHT));

      if(getMazeElementRelativeToPlayer(3, MIDDLE) == WALL)
      {
        drawMazeElement(3, MIDDLE, WALL);
      }
      else if(getMazeElementRelativeToPlayer(3, MIDDLE) == DOOR)
      {
        drawMazeElement(3, MIDDLE, DOOR);        
      }
      else
      {
        drawMazeElement(3, LEFT, getMazeElementRelativeToPlayer(3, LEFT));
        drawMazeElement(3, RIGHT, getMazeElementRelativeToPlayer(3, RIGHT));

        if(getMazeElementRelativeToPlayer(4, MIDDLE) == WALL)
        {
          drawMazeElement(4, MIDDLE, WALL);
        }
        else if(getMazeElementRelativeToPlayer(4, MIDDLE) == DOOR)
        {
          drawMazeElement(4, MIDDLE, DOOR);          
        }
        else
        {
          drawMazeElement(4, LEFT, getMazeElementRelativeToPlayer(4, LEFT));
          drawMazeElement(4, RIGHT, getMazeElementRelativeToPlayer(4, RIGHT));
        }  
      }  
    }  
  }

  byte item;
  byte monster;

  monster = getDeadMonsterRelativeToPlayer(3, MIDDLE);
  if(monster > 0 && getMazeElementRelativeToPlayer(2, MIDDLE) == EMPTY && getMazeElementRelativeToPlayer(1, MIDDLE) == EMPTY)
  {
    drawHackepeter(3, monsterType[monster - 1]);
  }
  item = getItemRelativeToPlayer(3, MIDDLE);
  if(item > 0 && getMazeElementRelativeToPlayer(2, MIDDLE) == EMPTY && getMazeElementRelativeToPlayer(1, MIDDLE) == EMPTY)
  {
    drawChest(3);
  }
  monster = getLivingMonsterRelativeToPlayer(3, MIDDLE);
  if(monster > 0 && getMazeElementRelativeToPlayer(2, MIDDLE) == EMPTY && getMazeElementRelativeToPlayer(1, MIDDLE) == EMPTY)
  {
    drawMonster(3, monsterType[monster - 1], false, true, 0, 0);
  }

  monster = getDeadMonsterRelativeToPlayer(2, MIDDLE);
  if(monster > 0 && getMazeElementRelativeToPlayer(1, MIDDLE) == EMPTY)
  {
    drawHackepeter(2, monsterType[monster - 1]);
  }
  item = getItemRelativeToPlayer(2, MIDDLE);
  if(item > 0 && getMazeElementRelativeToPlayer(1, MIDDLE) == EMPTY)
  {
    drawChest(2);
  }
  monster = getLivingMonsterRelativeToPlayer(2, MIDDLE);
  if(monster > 0 && getMazeElementRelativeToPlayer(1, MIDDLE) == EMPTY)
  {
    drawMonster(2, monsterType[monster - 1], false, true, 0, 0);
  }

  monster = getDeadMonsterRelativeToPlayer(1, MIDDLE);
  if(monster > 0)
  {
    drawHackepeter(1, monsterType[monster - 1]);
  }
  item = getItemRelativeToPlayer(1, MIDDLE);
  if(item > 0)
  {
    drawChest(1);
  }
  monster = getLivingMonsterRelativeToPlayer(1, MIDDLE);
  if(monster > 0)
  {
    drawMonster(1, monsterType[monster - 1], false, true, 0, 0);
  }

  drawOrientation();
  drawStatus();
}

// Draws the animation if a door is opened
void openDoorAnimation()
{
  // SOUND: Includes sounds when door is opened
  
  // Move handle
  matrix.drawLine(5, 15, 8, 15, matrix.Color333(0,0,0));
  matrix.drawLine(5, 15, 8, 16, matrix.Color333(4,4,4));
  tone(audio, 100, 20);
  delay(200);
  matrix.drawLine(5, 15, 8, 16, matrix.Color333(0,0,0));
  matrix.drawLine(5, 15, 7, 17, matrix.Color333(4,4,4));
  tone(audio, 80, 20);
  delay(200);
  matrix.drawLine(5, 15, 7, 17, matrix.Color333(0,0,0));
  matrix.drawLine(5, 15, 6, 18, matrix.Color333(4,4,4));
  tone(audio, 60, 20);
  delay(200);
  matrix.drawLine(5, 15, 6, 18, matrix.Color333(0,0,0));
  matrix.drawLine(5, 15, 5, 18, matrix.Color333(4,4,4));
  tone(audio, 40, 20);
  delay(500);
  
  // Open door
  matrix.drawLine(29, 2, 29, 29, matrix.Color333(4,4,4));
  for(int i = 0; i < 21; i++)
  {
    if(i == 1){ matrix.drawLine(2, 2, 2, 29, matrix.Color333(red * 4, green * 4, blue * 4)); };
    matrix.drawLine(2 + i, 2 + (i / 3), 29, 2, matrix.Color333(4,4,4));
    matrix.drawLine(2 + i, 29 - (i / 3), 29, 29, matrix.Color333(4,4,4));
    matrix.drawLine(2 + i, 2 + (i / 3), 2 + i, 29 - (i / 3), matrix.Color333(4,4,4));
    matrix.drawLine(5 + i - (i / 10), 15, 5 + i - (i / 10), 18 - (i / 10), matrix.Color333(4,4,4));
    drawOrientation();
    tone(audio, 400 + i * 10, 10);
    delay(50 + i * 3);
    matrix.drawLine(2 + i, 2 + (i / 3), 29, 2, matrix.Color333(0,0,0));
    matrix.drawLine(2 + i, 29 - (i / 3), 29, 29, matrix.Color333(0,0,0));
    matrix.drawLine(2 + i, 2 + (i / 3), 2 + i, 29 - (i / 3), matrix.Color333(0,0,0));
    matrix.drawLine(5 + i - (i / 10), 15, 5 + i - (i / 10), 18 - (i / 10), matrix.Color333(0,0,0));
  }
  matrix.drawLine(23, 8, 29, 2, matrix.Color333(4,4,4));
  matrix.drawLine(23, 23, 29, 29, matrix.Color333(4,4,4));
  matrix.drawLine(23, 8, 23, 23, matrix.Color333(4,4,4));
  matrix.drawLine(24, 15, 24, 16, matrix.Color333(4,4,4));
  drawOrientation();
  tone(audio, 610, 10);
  delay(250);
  matrix.drawLine(23, 8, 29, 2, matrix.Color333(0,0,0));
  matrix.drawLine(23, 23, 29, 29, matrix.Color333(0,0,0));
  matrix.drawLine(23, 8, 23, 23, matrix.Color333(0,0,0));
  matrix.drawLine(24, 15, 24, 16, matrix.Color333(0,0,0));
}

// Check whether player is moving (triggered by joystick)
void checkForPlayerMovement()
{
  playerMoved = false;

  if(joy1Left() && !joy1Fire())
  {
    playerDirection--;
    if(playerDirection < 1)
    {
      playerDirection = 4;
    }
    playerMoved = true;
  }
  else if(joy1Right() && !joy1Fire())
  {
    playerDirection++;
    if(playerDirection > 4)
    {
      playerDirection = 1;
    }
    playerMoved = true;
  }
  else if(joy1Up())
  {
    if(getMazeElementRelativeToPlayer(1, MIDDLE) != WALL && !joy1Fire())
    {
      if(getLivingMonsterRelativeToPlayer(1, MIDDLE) == 0)
      {
        if(getMazeElementRelativeToPlayer(1, MIDDLE) == EMPTY || getMazeElementRelativeToPlayer(1, MIDDLE) == EXIT)
        {
          if(getItemRelativeToPlayer(1, MIDDLE) > 0)
          {
            collectItem(getItemRelativeToPlayer(1, MIDDLE) - 1);
            delay(100);
            playerMoved = true;
          }
          else
          {
            if(playerDirection == 1){ playerY--; }
            if(playerDirection == 2){ playerX++; }
            if(playerDirection == 3){ playerY++; }
            if(playerDirection == 4){ playerX--; }
            playerMoved = true;
          }
        }
        else if(getMazeElementRelativeToPlayer(1, MIDDLE) == DOOR)
        {
          // Open door
          if(playerKeys > 0)
          {
            playerKeys--;
            setMazeElementRelativeToPlayer(1, MIDDLE, EMPTY);
            openDoorAnimation();
            playerMoved = true;
          }
          else
          {
            // SOUND: No key
            tone(audio, 100, 50);
          }
        }
      }
    }
  }
  else if(joy1Down())
  {
    if(getMazeElementRelativeToPlayer(-1, MIDDLE) == EMPTY && !joy1Fire())
    {
      if(getLivingMonsterRelativeToPlayer(-1, MIDDLE) == 0)
      {
        if(playerDirection == 1){ playerY++; }
        if(playerDirection == 2){ playerX--; }
        if(playerDirection == 3){ playerY--; }
        if(playerDirection == 4){ playerX++; }
        playerMoved = true;
      }
    }      
  }

  if(playerMoved)
  {
    // SOUND: Footsteps
    if(playerFootsteps == 0)
    {
      if(playerHaste)
      {
        tone(audio,400,50);
      }
      else
      {
        if(currentArmor != INVISIBILITY){ tone(audio,200,50); }
      }
      playerFootsteps = 1;
    }
    else
    {
      if(playerHaste)
      {
        tone(audio,200,50);
      }
      else
      {
        if(currentArmor != INVISIBILITY){ tone(audio,100,50); }
      }
      playerFootsteps = 0;
    }
    updatePlayerDelay();
  }
}

// Update player delay (Heavy weapons and armor increase player delay)
void updatePlayerDelay()
{
  playerDelayCounter = playerDelay;
  if(currentWeapon == SWORD){ playerDelayCounter = playerDelayCounter + 40; }
  if(currentWeapon == CLAYMORE){ playerDelayCounter = playerDelayCounter + 80; }
  if(currentWeapon == CROSSBOW){ playerDelayCounter = playerDelayCounter + 40; }
  if(currentWeapon == LONGBOW){ playerDelayCounter = playerDelayCounter + 40; }
  if(currentArmor == HEAVY){ playerDelayCounter = playerDelayCounter + 40; }
  if(currentArmor == MAGIC){ playerDelayCounter = playerDelayCounter - 80; }  
  if(playerHaste){ playerDelayCounter = playerDelayCounter / 2; }
  playerFireButtonCounterHoldTime = playerDelayCounter * 2;
}

// Check whether player is doing some action (triggered by joystick)
void checkForPlayerActions()
{
  // playerFireButtonStatus = 0: Initial status
  // playerFireButtonStatus = 1: Pressed: Counter started. If button is released before counter ends, the menu is opened with classic joystick. Otherwise return to initial status.
  // playerFireButtonStatus = 2: Pressed after counter has ended: Player can attack or fire spells. If button is released, status is set back to 0.
  // playerFireButtonStatus = 3: Pressed after an action. The counter is started again, but releasing it will not open the menu.

  playerAction = false;

  // Enter game menu with LEDmePlay Joypad
  if(joy1FireB())
  { 
    playerFireButtonStatus = 0;
    showMenu(); 
    playerMoved = true;      
    updatePlayerDelay();
  }   
  
if(playerFireButtonStatus == 0)
  {
    if(joy1Fire())
    {
      playerFireButtonStatus = 1;
      playerFireButtonCounter = 0;
    }
  }
  else if(playerFireButtonStatus == 1)
  {
    // Player is preparing for action
    if(joy1Fire())
    {
      playerFireButtonCounter++;
      if(playerFireButtonCounter > playerFireButtonCounterHoldTime)
      {
        playerFireButtonStatus = 2;
        playerFireButtonCounter = 0;
      }
    }
    // Player wants to enter game menu
    else
    {
      playerFireButtonStatus = 0;
      if(playerFireButtonCounter < 200)
      { 
        // Enter game menu with classic joystick
        if(!joy1IsLEDmePlayJoypad())
        {
          showMenu(); 
          playerMoved = true;      
        }
      }
      updatePlayerDelay();
    }
  }
  // Player is ready for action
  else if(playerFireButtonStatus == 2)
  {
    if(joy1Fire())
    {
      matrix.drawLine(13, 31, 15, 29, matrix.Color333(3, 3, 3)); 
      matrix.drawLine(16, 29, 18, 31, matrix.Color333(3, 3, 3));
      drawCurrentWeaponAndSpell();

      // Attack
      if(joy1Up())
      {
        attack();
        playerFireButtonStatus = 3;
        playerFireButtonCounter = 0;
      }

      // Spell
      if(joy1Down())
      {
        spell();
        playerFireButtonStatus = 3;
        playerFireButtonCounter = 0;
      }
    }
    else
    {
      playerFireButtonStatus = 0;
      playerFireButtonCounter = 0;
      matrix.drawLine(13, 31, 15, 29, matrix.Color333(0, 0, 0)); 
      matrix.drawLine(16, 29, 18, 31, matrix.Color333(0, 0, 0));
      matrix.fillRect(0, 22, 6, 7, matrix.Color333(0, 0, 0));
      matrix.fillRect(26, 22, 6, 7, matrix.Color333(0, 0, 0));
      playerAction = true;      
    }  
  }
  // Player prepares for another action
  else if(playerFireButtonStatus == 3)
  {
    if(joy1Fire())
    {
      playerFireButtonCounter++;
      if(playerFireButtonCounter > playerFireButtonCounterHoldTime)
      {
        playerFireButtonStatus = 2;
        playerFireButtonCounter = 0;
      }
    }
    else
    {
      playerFireButtonStatus = 0;
      updatePlayerDelay();
    }
  }
}

// Draws the current weapon and spell to the screen
void drawCurrentWeaponAndSpell()
{
  if(currentWeapon == GLADIUS){ drawItem(GLADIUS, 1, 24); }
  else if(currentWeapon == SWORD){ drawItem(SWORD, 1, 23); }
  else if(currentWeapon == CLAYMORE){ drawItem(CLAYMORE, 1, 22); }
  else if(currentWeapon == CROSSBOW)
  { 
    drawItem(CROSSBOW, 0, 23);
    for(byte i = 0; i < playerBolts; i++)
    {
      matrix.drawPixel(5, 28 - i, matrix.Color333(0, 0, 1));
    }
  }
  else if(currentWeapon == LONGBOW)
  { 
    drawItem(LONGBOW, 0, 22); 
    for(byte i = 0; i < playerArrows; i++)
    {
      matrix.drawPixel(5, 28 - i, matrix.Color333(0, 0, 1));
    }
  }

  if(playerSpells != 0)
  {
    if(currentSpell == HEALING){ drawItem(HEALING, 26, 23); }
    else if(currentSpell == IMMORTAL){ drawItem(IMMORTAL, 26, 23); }
    else if(currentSpell == FIREBALL){ drawItem(FIREBALL, 26, 24); }
    else if(currentSpell == LIGHTNING){ drawItem(LIGHTNING, 27, 22); }
    else if(currentSpell == HASTE){ drawItem(HASTE, 26, 23); }
  }
}

// Player attacks
void attack()
{  
  matrix.drawLine(13, 31, 15, 29, matrix.Color333(0, 0, 0)); 
  matrix.drawLine(16, 29, 18, 31, matrix.Color333(0, 0, 0));
  
  if(currentWeapon == GLADIUS)
  {
    matrix.drawLine(14, 31, 15, 24, matrix.Color333(2, 2, 2)); 
    matrix.drawLine(15, 24, 18, 31, matrix.Color333(2, 2, 2));
    delay(500);
    hitMonster(1, 1);
    matrix.drawLine(14, 31, 15, 24, matrix.Color333(0, 0, 0)); 
    matrix.drawLine(15, 24, 18, 31, matrix.Color333(0, 0, 0));
  }
  else if(currentWeapon == SWORD)
  {
    matrix.drawLine(14, 31, 15, 23, matrix.Color333(3, 5, 4)); 
    matrix.drawLine(15, 23, 18, 31, matrix.Color333(3, 5, 4));
    delay(500);
    hitMonster(1, 2);
    matrix.drawLine(14, 31, 15, 23, matrix.Color333(0, 0, 0)); 
    matrix.drawLine(15, 23, 18, 31, matrix.Color333(0, 0, 0));
  }
  else if(currentWeapon == CLAYMORE)
  {
    matrix.drawLine(14, 31, 15, 22, matrix.Color333(4, 4, 7)); 
    matrix.drawLine(15, 22, 18, 31, matrix.Color333(4, 4, 7));
    delay(500);
    hitMonster(1, 3);
    matrix.drawLine(14, 31, 15, 22, matrix.Color333(0, 0, 0)); 
    matrix.drawLine(15, 22, 18, 31, matrix.Color333(0, 0, 0));
  }
  else if(currentWeapon == CROSSBOW)
  {
    playerBolts--;

    // Get the next suitable weapon if running out of bolts
    if(playerBolts == 0)
    { 
      playerWeapons = playerWeapons - 8;
      if(hasWeapon(CLAYMORE)){ currentWeapon = CLAYMORE; }
      else if(hasWeapon(SWORD)){ currentWeapon = SWORD; }
      else { currentWeapon = GLADIUS; }
    }
    for(int i = 31; i > 15; i--)
    {
      // Sound: Crossbow
      tone(audio, 400, 20);
      matrix.drawPixel(16, i, matrix.Color333(1, 1, 1));
      delay(pow(2,(31 - i + 2) / 2) / 2);
      matrix.drawPixel(16, i, matrix.Color333(0, 0, 0));
      if((getLivingMonsterRelativeToPlayer(1, MIDDLE) > 0 || getMazeElementRelativeToPlayer(1, MIDDLE) != 0) && i == 24)
      { 
        if(getLivingMonsterRelativeToPlayer(1, MIDDLE) > 0)
        {
          hitMonster(1, 2);
          break;
        }
        if(getMazeElementRelativeToPlayer(1, MIDDLE) != 0){ break; }
      }
      if((getLivingMonsterRelativeToPlayer(2, MIDDLE) > 0 || getMazeElementRelativeToPlayer(2, MIDDLE) != 0) && i == 20)
      { 
        if(getLivingMonsterRelativeToPlayer(2, MIDDLE) > 0)
        {
          hitMonster(2, 2);
          break;
        }
        if(getMazeElementRelativeToPlayer(2, MIDDLE) != 0){ break; }
      }
      if((getLivingMonsterRelativeToPlayer(3, MIDDLE) > 0 || getMazeElementRelativeToPlayer(3, MIDDLE) != 0) && i == 18)
      { 
        if(getLivingMonsterRelativeToPlayer(3, MIDDLE) > 0)
        {
          hitMonster(3, 2);
          break;
        }
        if(getMazeElementRelativeToPlayer(3, MIDDLE) != 0){ break; }
      }
    }
  }
  else if(currentWeapon == LONGBOW)
  {
    playerArrows--;

    // Get the next suitable weapon if running out of arrows
    if(playerArrows == 0)
    { 
      playerWeapons = playerWeapons - 16; 
      if(hasWeapon(CROSSBOW)){ currentWeapon = CROSSBOW; }
      else if(hasWeapon(CLAYMORE)){ currentWeapon = CLAYMORE; }
      else if(hasWeapon(SWORD)){ currentWeapon = SWORD; }
      else { currentWeapon = GLADIUS; }
    }
    for(int i = 31; i > 15; i--)
    {
      // Sound: Longbow
      tone(audio, 600, 20);
      matrix.drawPixel(12 + i / 4, i, matrix.Color333(2, 2, 1));
      delay(pow(2,(31 - i + 2) / 2) / 3);
      matrix.drawPixel(12 + i / 4, i, matrix.Color333(0, 0, 0));
      if((getLivingMonsterRelativeToPlayer(1, MIDDLE) > 0 || getMazeElementRelativeToPlayer(1, MIDDLE) != 0) && i == 24)
      { 
        if(getLivingMonsterRelativeToPlayer(1, MIDDLE) > 0)
        {
          hitMonster(1, 4);
          break;
        }
        if(getMazeElementRelativeToPlayer(1, MIDDLE) != 0){ break; }
      }
      if((getLivingMonsterRelativeToPlayer(2, MIDDLE) > 0 || getMazeElementRelativeToPlayer(2, MIDDLE) != 0) && i == 20)
      { 
        if(getLivingMonsterRelativeToPlayer(2, MIDDLE) > 0)
        {
          hitMonster(2, 4);
          break;
        }
        if(getMazeElementRelativeToPlayer(2, MIDDLE) != 0){ break; }
      }
      if((getLivingMonsterRelativeToPlayer(3, MIDDLE) > 0 || getMazeElementRelativeToPlayer(3, MIDDLE) != 0) && i == 18)
      { 
        if(getLivingMonsterRelativeToPlayer(3, MIDDLE) > 0)
        {
          hitMonster(3, 4);
          break;
        }
        if(getMazeElementRelativeToPlayer(3, MIDDLE) != 0){ break; }
      }
    }
  }

  matrix.fillRect(0, 22, 6, 7, matrix.Color333(0, 0, 0));
  matrix.fillRect(26, 22, 6, 7, matrix.Color333(0, 0, 0));
  playerAction = true;      
}

// Player uses a spell
void spell()
{
  matrix.drawLine(13, 31, 15, 29, matrix.Color333(0, 0, 0)); 
  matrix.drawLine(16, 29, 18, 31, matrix.Color333(0, 0, 0));

  if(currentSpell == HEALING)
  {
    if(magicPoints > 2)
    {
      magicPoints = magicPoints - 3;
      playerImmortal = false;
      playerHaste = false;
      updatePlayerDelay();
      playerMagicCounter = 0;      
      hitPoints = 8;
      // SOUND: Healing (spell)
      for(byte i = 0; i < 100; i++)
      {
        tone(audio, 500 + i * 5, 5);
        delay(10);              
      }
    }
    else
    {
      showRequiredMagicPoints(3);     
    }
  }
  else if(currentSpell == IMMORTAL)
  {    
    if(magicPoints > 2)
    {
      magicPoints = magicPoints - 3;
      playerImmortal = true;
      playerHaste = false;
      updatePlayerDelay();
      playerMagicCounter = 10000;
      drawOrientation();
      // SOUND: Immortal
      for(byte i = 0; i < 100; i++)
      {
        tone(audio, 2000 - i * 10, 5);
        delay(5);              
      }
    }
    else
    {
      showRequiredMagicPoints(3);     
    }
  }
  else if(currentSpell == FIREBALL)
  {
    if(magicPoints > 0)
    {    
      magicPoints = magicPoints - 1;
      playerImmortal = false;
      playerHaste = false;
      updatePlayerDelay();
      playerMagicCounter = 0;      
      for(int i = 31; i > 15; i--)
      {
        // Sound: Fireball
        tone(audio, 150 + random(100), 20);
        if(i % 2 == 0){ matrix.drawCircle(16, i, i / 10, matrix.Color333(random(4) + 3, random(4) + 1, 0)); }
        delay(pow(2,(31 - i + 2) / 2) / 2);
        matrix.drawCircle(16, i, i / 10, matrix.Color333(0, 0, 0));
        if((getLivingMonsterRelativeToPlayer(1, MIDDLE) > 0 || getMazeElementRelativeToPlayer(1, MIDDLE) != 0) && i == 24)
        { 
          if(getLivingMonsterRelativeToPlayer(1, MIDDLE) > 0)
          {
            hitMonster(1, 3);
            break;
          }
          if(getMazeElementRelativeToPlayer(1, MIDDLE) != 0){ break; }
        }
        if((getLivingMonsterRelativeToPlayer(2, MIDDLE) > 0 || getMazeElementRelativeToPlayer(2, MIDDLE) != 0) && i == 20)
        { 
          if(getLivingMonsterRelativeToPlayer(2, MIDDLE) > 0)
          {
            hitMonster(2, 3);
            break;
          }
          if(getMazeElementRelativeToPlayer(2, MIDDLE) != 0){ break; }
        }
        if((getLivingMonsterRelativeToPlayer(3, MIDDLE) > 0 || getMazeElementRelativeToPlayer(3, MIDDLE) != 0) && i == 18)
        { 
          if(getLivingMonsterRelativeToPlayer(3, MIDDLE) > 0)
          {
            hitMonster(3, 3);
            break;
          }
          if(getMazeElementRelativeToPlayer(3, MIDDLE) != 0){ break; }
        }
      }
    }
    else
    {
      showRequiredMagicPoints(1);     
    }
  }
  else if(currentSpell == LIGHTNING)
  {
    if(magicPoints > 1)
    {    
      magicPoints = magicPoints - 2;
      playerImmortal = false;
      playerHaste = false;
      updatePlayerDelay();
      playerMagicCounter = 0;      
      int x0, x1, y1, x2, y2, x3, y3;
      for(int i = 0; i < 10; i++)
      {
        x0 = random(2) + 15;
        x1 = random(4) + 14;
        y1 = random(4) + 27;
        x2 = x1 + random(9) - 4;
        y2 = y1 - random(6);
        x3 = x2 + random(9) - 4;
        y3 = y2 - random(6);
        matrix.drawLine(x0, 31, x1, y1, matrix.Color333(7, 7, 7)); 
        matrix.drawLine(x1, y1, x2, y2, matrix.Color333(7, 7, 7)); 
        matrix.drawLine(x2, y2, x3, y3, matrix.Color333(7, 7, 7));
        // Sound: Lightning
        tone(audio, 40 + random(20), 10);
        delay(20);
        matrix.drawLine(x0, 31, x1, y1, matrix.Color333(0, 0, 0)); 
        matrix.drawLine(x1, y1, x2, y2, matrix.Color333(0, 0, 0)); 
        matrix.drawLine(x2, y2, x3, y3, matrix.Color333(0, 0, 0)); 
      }
      hitMonster(1, 6);
    }
    else
    {
      showRequiredMagicPoints(2);     
    }
  }
  else if(currentSpell == HASTE)
  {
    if(magicPoints > 1)
    {
      magicPoints = magicPoints - 2;
      playerImmortal = false;
      playerHaste = true;
      updatePlayerDelay();
      playerMagicCounter = 10000;
      drawOrientation();
      // SOUND: Haste
      for(byte i = 0; i < 100; i++)
      {
        tone(audio, 1000 + i * 10, 5);
        delay(5);              
      }
    }
    else
    {
      showRequiredMagicPoints(2);     
    }
  }

  matrix.fillRect(0, 22, 6, 7, matrix.Color333(0, 0, 0));
  matrix.fillRect(26, 22, 6, 7, matrix.Color333(0, 0, 0));
  playerAction = true;      
}

// Show required magic points in case that there are not enough for a spell
void showRequiredMagicPoints(byte _requiredMagicPoints)
{
  // SOUND: Not enough magic points     
  tone(audio, 100, 500);            
  for(byte i = 0; i < 10; i++)
  {
    matrix.drawLine(20, 30, 27, 30, matrix.Color333(0, 0, 0));
    delay(25);
    for(byte i = 0; i < _requiredMagicPoints; i++)
    {
      matrix.drawPixel(27 - i, 30, matrix.Color333(0,3,0));
    }
    delay(25);
  }
  drawOrientation();
}

// Player hits a monster
void hitMonster(byte _distance, byte _attackDices)
{
  int i = getLivingMonsterRelativeToPlayer(_distance, MIDDLE);
  if(i > 0)
  {
    byte hitValue = calculateDamage(_attackDices, monsterDefend[i - 1]);
    if(hitValue > 0)
    {
      // SOUND: Monster hit
      tone(audio, 800, 20);
      drawBloodFountain(_distance, true);
      if((monsterHitPoints[i - 1] - hitValue) <= 0)
      {
        monsterStatus[i - 1] = 2;
        // SOUND: Monster killed
        for (int i = 0; i < 500; i++)
        {
          tone(audio, 1000 - i * 2, 1);
        }
      }
      else
      {
        monsterHitPoints[i - 1] = monsterHitPoints[i - 1] - hitValue;        
      }
      // Remove killed monster
      drawBloodFountain(_distance, false);
      drawMonster(_distance, monsterType[i - 1], false, false, 0, 0);
    }
    else
    {
      // SOUND: Monster missed
      tone(audio, 100, 20);  
    }
  }
}

// Monster is doing a close combat attack
void monsterAttacksCloseCombat(byte _monster)
{
  if(getLivingMonsterRelativeToPlayer(1, MIDDLE) - 1 == _monster)
  {
    drawMonster(1, monsterType[_monster], false, false, 0, 0);
    drawMonster(1, monsterType[_monster], true, true, 0, 0);
    delay(500);
    monsterMovedWithinEyeshot = true;
    drawMonster(1, monsterType[_monster], true, false, 0, 0);
    drawMonster(1, monsterType[_monster], false, true, 0, 0);    
  }

  // Calculate damage if player is not immortal (by a spell)
  if(!playerImmortal)
  {
    byte damage = 0;
    if(playerArmors == 0)
    {
      damage = calculateDamage(monsterAttack[_monster], 1);
    }
    else
    {
      if(currentArmor == LEATHER){ damage = calculateDamage(monsterAttack[_monster], 2); }
      else if(currentArmor == HEAVY){ damage = calculateDamage(monsterAttack[_monster], 4); }
      else if(currentArmor == INVISIBILITY){ damage = calculateDamage(monsterAttack[_monster], 2); }
      else if(currentArmor == MAGIC){ damage = calculateDamage(monsterAttack[_monster], 3); }
    }
    if(damage > 0){ playerGetsHit(damage); }
  }
}

// Monster is doing a long range attack
void monsterAttacksLongRange(byte _monster, byte _distance)
{
  // Long range attack animation
  if(getLivingMonsterRelativeToPlayer(2, MIDDLE) - 1 == _monster || getLivingMonsterRelativeToPlayer(3, MIDDLE) - 1 == _monster)
  {
    int i;
    byte power = monsterAttackLongRange[_monster];
    if(_distance == 2){ i = 20; }
    if(_distance == 3){ i = 18; }  
    for(i; i < 32; i++)
    {
      // Sound: Monster long-range attack
      tone(audio, (400 / power) + random(100), 20);
      if(i % 2 == 0){ matrix.drawCircle(16, i, (i * power) / 20, matrix.Color333(random(4) + power, random(4) + power, 0)); }
      delay(pow(2,(31 - i + 2) / 2) / 2);
      matrix.drawCircle(16, i, (i * power) / 20, matrix.Color333(0, 0, 0));
      drawMonster(_distance, monsterType[_monster], false, true, 0, 0);    
    }  
  }
  
  // Calculate damage if player is not immortal (by a spell)
  if(!playerImmortal)
  {
    byte damage = 0;
    if(playerArmors == 0)
    {
      damage = calculateDamage(monsterAttack[_monster], 1);
    }
    else
    {
      if(currentArmor == LEATHER){ damage = calculateDamage(monsterAttackLongRange[_monster], 2); }
      else if(currentArmor == HEAVY){ damage = calculateDamage(monsterAttackLongRange[_monster], 4); }
      else if(currentArmor == INVISIBILITY){ damage = calculateDamage(monsterAttackLongRange[_monster], 2); }
      else if(currentArmor == MAGIC){ damage = calculateDamage(monsterAttackLongRange[_monster], 5); }
    }
    if(damage > 0){ playerGetsHit(damage); }
  }  
}

// Calculate the damage
byte calculateDamage(byte _attackDices, byte _defendDices)
{
      byte attackAttempts = _attackDices;
      byte defendAttempts = _defendDices;
      do
      {
        byte attack = random(6) + 1;
        byte defend = random(6) + 1;
        if(attack <= defend)
        {
          attackAttempts--;
        }
        else
        {
          defendAttempts--;
        }
      }
      while(attackAttempts > 0 && defendAttempts > 0);
      return attackAttempts; 
}

// Monster checks for player
byte getDirectionToPlayer(byte _monsterX, byte _monsterY, byte& _distance)
{
  // If player wears Invisible Armor, monsters cannot get direction (except in direct contact)

  // Check North
  if(playerX == _monsterX && playerY == _monsterY - 1){ _distance = 1; return 1; }
  else if(currentArmor != INVISIBILITY && getMazeElement(_monsterX, _monsterY - 1) == EMPTY && playerX == _monsterX && playerY == _monsterY - 2){ _distance = 2; return 1; }
  else if(currentArmor != INVISIBILITY && getMazeElement(_monsterX, _monsterY - 1) == EMPTY && getMazeElement(_monsterX, _monsterY - 2) == EMPTY && playerX == _monsterX && playerY == _monsterY - 3){ _distance = 3; return 1; }

  // Check East
  if(playerX == _monsterX + 1 && playerY == _monsterY){ _distance = 1; return 2; }
  else if(currentArmor != INVISIBILITY && getMazeElement(_monsterX + 1, _monsterY) == EMPTY && playerX == _monsterX + 2 && playerY == _monsterY){ _distance = 2; return 2; }
  else if(currentArmor != INVISIBILITY && getMazeElement(_monsterX + 1, _monsterY) == EMPTY && getMazeElement(_monsterX + 2, _monsterY) == EMPTY && playerX == _monsterX + 3 && playerY == _monsterY){ _distance = 3; return 2; }

  // Check South
  if(playerX == _monsterX && playerY == _monsterY + 1){ _distance = 1; return 3; }
  else if(currentArmor != INVISIBILITY && getMazeElement(_monsterX, _monsterY + 1) == EMPTY && playerX == _monsterX && playerY == _monsterY + 2){ _distance = 2; return 3; }
  else if(currentArmor != INVISIBILITY && getMazeElement(_monsterX, _monsterY + 1) == EMPTY && getMazeElement(_monsterX, _monsterY + 2) == EMPTY && playerX == _monsterX && playerY == _monsterY + 3){ _distance = 3; return 3; }

  // Check West
  if(playerX == _monsterX - 1 && playerY == _monsterY){ _distance = 1; return 4; }
  else if(currentArmor != INVISIBILITY && getMazeElement(_monsterX - 1, _monsterY) == EMPTY && playerX == _monsterX - 2 && playerY == _monsterY){ _distance = 2; return 4; }
  else if(currentArmor != INVISIBILITY && getMazeElement(_monsterX - 1, _monsterY) == EMPTY && getMazeElement(_monsterX - 2, _monsterY) == EMPTY && playerX == _monsterX - 3 && playerY == _monsterY){ _distance = 3; return 4; }

  return 0;
}

// Returns true if there are no additional monsters between monster and player
boolean clearFieldOfFire(byte _monsterX, byte _monsterY, byte _directionToPlayer, byte _distance)
{
  if(_directionToPlayer == 1)
  {
    if(_distance == 2 && getMonster(_monsterX, _monsterY - 1) == 0){ return true; }
    if(_distance == 3 && getMonster(_monsterX, _monsterY - 1) == 0 && getMonster(_monsterX, _monsterY - 2) == 0){ return true; }
  }
  else if(_directionToPlayer == 2)
  {
    if(_distance == 2 && getMonster(_monsterX + 1, _monsterY) == 0){ return true; }
    if(_distance == 3 && getMonster(_monsterX + 1, _monsterY) == 0 && getMonster(_monsterX + 2, _monsterY) == 0){ return true; }    
  }
  else if(_directionToPlayer == 3)
  {
    if(_distance == 2 && getMonster(_monsterX, _monsterY + 1) == 0){ return true; }
    if(_distance == 3 && getMonster(_monsterX, _monsterY + 1) == 0 && getMonster(_monsterX, _monsterY + 2) == 0){ return true; }   
  }
  else if(_directionToPlayer == 4)
  {
    if(_distance == 2 && getMonster(_monsterX - 1, _monsterY) == 0){ return true; }
    if(_distance == 3 && getMonster(_monsterX - 1, _monsterY) == 0 && getMonster(_monsterX - 2, _monsterY) == 0){ return true; }    
  }
  return false;
}

// Checks whether monster can move to a specific position (_targetPositionX, _targetPositionY)
// Returns true if the specified field contains no wall, no other monster and not the player
boolean canMonsterMoveHere(byte _targetPositionX, byte _targetPositionY)
{
  // Walls?
  if(getMazeElement(_targetPositionX, _targetPositionY) == EMPTY)
  {
    // Other monster?
    byte neighboringMonster = getMonster(_targetPositionX, _targetPositionY);
    if(neighboringMonster == 0)
    {
      // The player?
      if(!(_targetPositionX == playerX && _targetPositionY == playerY))
      {
        return true;
      }
    }
    else if(monsterStatus[neighboringMonster - 1] == 2)
    {
      // The player?
      if(!(_targetPositionX == playerX && _targetPositionY == playerY))
      {
        return true;
      }          
    }
  }
  return false;
}

// Move monsters
void moveMonsters()
{
  byte monsterDistance = 255;

  // Check movement within eyeshot
  byte dist1 = getLivingMonsterRelativeToPlayer(1, MIDDLE);
  byte dist2 = getLivingMonsterRelativeToPlayer(2, MIDDLE);
  byte dist3 = getLivingMonsterRelativeToPlayer(3, MIDDLE);

  for(int i = 0; i < 16; i++)
  {
    if(monsterStatus[i] == 1)
    {      
      if(monsterDelayCounter[i] == 0)
      {        
        // Move strategy: Random (also used for strategy "Guided" if player wears Invisible Armor)
        if(monsterStrategy[i] == 1 || (monsterStrategy[i] == 2 && currentArmor == INVISIBILITY))
        {
          byte distance = 0;
          byte directionToPlayer = getDirectionToPlayer(monsterX[i], monsterY[i], distance);
          if(monsterAttackLongRange[i] > 0 && directionToPlayer > 0 && distance > 1 && random(10) < 5)
          { 
            if(clearFieldOfFire(monsterX[i], monsterY[i], directionToPlayer, distance))
            {
              monsterAttacksLongRange(i, distance);
            }
          }
          else
          {
            int j = random(4);
            if(j == 0 && (monsterY[i] - 1) > 0)
            {
              if(canMonsterMoveHere(monsterX[i], monsterY[i] - 1))
              { 
                monsterY[i]--; 
              }
              else if(monsterX[i] == playerX && monsterY[i] - 1 == playerY)
              {
                monsterAttacksCloseCombat(i);
              }  
            }
            if(j == 1 && (monsterX[i] + 1) < mazeWidth)
            {
              if(canMonsterMoveHere(monsterX[i] + 1, monsterY[i]))
              { 
                monsterX[i]++; 
              }
              else if(monsterX[i] + 1 == playerX && monsterY[i] == playerY)
              {
                monsterAttacksCloseCombat(i);
              }  
            }
            if(j == 2 && (monsterY[i] + 1) < mazeHeight)
            {
              if(canMonsterMoveHere(monsterX[i], monsterY[i] + 1))
              { 
                monsterY[i]++; 
              }
              else if(monsterX[i] == playerX && monsterY[i] + 1 == playerY)
              {
                monsterAttacksCloseCombat(i);
              }  
            }
            if(j == 3 && (monsterX[i] - 1) > 0)
            {
              if(canMonsterMoveHere(monsterX[i] - 1, monsterY[i]))
              { 
                monsterX[i]--; 
              }
              else if(monsterX[i] - 1 == playerX && monsterY[i] == playerY)
              {
                monsterAttacksCloseCombat(i);
              }  
            }
          }
        }
        
        // Move strategy: Guided (only if player is not invisible)
        if(monsterStrategy[i] == 2 && currentArmor != INVISIBILITY)
        {
          byte distance = 0;
          byte directionToPlayer = getDirectionToPlayer(monsterX[i], monsterY[i], distance);
          if(monsterAttackLongRange[i] > 0 && directionToPlayer > 0 && distance > 1 && random(10) < 5)
          { 
            if(clearFieldOfFire(monsterX[i], monsterY[i], directionToPlayer, distance))
            {
              monsterAttacksLongRange(i, distance);
            }
          }
          else
          {
            int j = random(2); // 0 = Move on x-axis, 1 = Move on y-axis
            if(j == 0 && playerX <= monsterX[i])
            {
              if(canMonsterMoveHere(monsterX[i] - 1, monsterY[i]))
              { 
                monsterX[i]--; 
              }
              else if(monsterX[i] - 1 == playerX && monsterY[i] == playerY)
              {
                monsterAttacksCloseCombat(i);
              }              
            }
            else if(j == 0 && playerX >= monsterX[i])
            {
              if(canMonsterMoveHere(monsterX[i] + 1, monsterY[i]))
              { 
                monsterX[i]++; 
              }
              else if(monsterX[i] + 1 == playerX && monsterY[i] == playerY)
              {
                monsterAttacksCloseCombat(i);
              }         
            }
            else if(j == 1 && playerY <= monsterY[i])
            {
              if(canMonsterMoveHere(monsterX[i], monsterY[i] - 1))
              { 
                monsterY[i]--; 
              }
              else if(monsterX[i] == playerX && monsterY[i] - 1 == playerY)
              {
                monsterAttacksCloseCombat(i);
              }  
            }
            else if(j == 1 && playerY >= monsterY[i])
            {
              if(canMonsterMoveHere(monsterX[i], monsterY[i] + 1))
              { 
                monsterY[i]++; 
              }
              else if(monsterX[i] == playerX && monsterY[i] + 1 == playerY)
              {
                monsterAttacksCloseCombat(i);
              }  
            }
          }          
        }

        // Move strategy: Patrol
        if(monsterStrategy[i] == 3)
        {
          byte distance = 0;
          byte directionToPlayer = getDirectionToPlayer(monsterX[i], monsterY[i], distance);
          bool newDirectionRequired = false;
          if(directionToPlayer > 0){ monsterDirection[i] = directionToPlayer; }
          if(monsterAttackLongRange[i] > 0 && directionToPlayer > 0 && distance > 1 && random(10) < 5)
          { 
            if(clearFieldOfFire(monsterX[i], monsterY[i], directionToPlayer, distance))
            {
              monsterAttacksLongRange(i, distance);
            }
          }
          else
          {
            if(monsterDirection[i] == 1)
            {
              if(canMonsterMoveHere(monsterX[i], monsterY[i] - 1))
              { 
                monsterY[i]--; 
              }
              else if(monsterX[i] == playerX && monsterY[i] - 1 == playerY)
              {
                monsterAttacksCloseCombat(i);
              }
              else
              {
                 newDirectionRequired = true;              
              }
            }
            else if(monsterDirection[i] == 2)
            {
              if(canMonsterMoveHere(monsterX[i] + 1, monsterY[i]))
              { 
                monsterX[i]++; 
              }
              else if(monsterX[i] + 1 == playerX && monsterY[i] == playerY)
              {
                monsterAttacksCloseCombat(i);
              }                     
              else
              {
                newDirectionRequired = true;              
              }
            }
            else if(monsterDirection[i] == 3)
            {
              if(canMonsterMoveHere(monsterX[i], monsterY[i] + 1))
              { 
                monsterY[i]++; 
              }
              else if(monsterX[i] == playerX && monsterY[i] + 1 == playerY)
              {
                monsterAttacksCloseCombat(i);
              }              
              else
              {
                newDirectionRequired = true;              
              }
            }
            else if(monsterDirection[i] == 4)
            {
              if(canMonsterMoveHere(monsterX[i] - 1, monsterY[i]))
              { 
                monsterX[i]--; 
              }
              else if(monsterX[i] - 1 == playerX && monsterY[i] == playerY)
              {
                monsterAttacksCloseCombat(i);
              }                          
              else
              {
                newDirectionRequired = true;              
              }
            }
            if(newDirectionRequired)
            {
              monsterDirection[i] = random(4) + 1;
            }
          }
        }

        monsterDelayCounter[i] = monsterDelay[i];
        
        // HACK FOR THE MONSTER MOVEMENT SOUND
        if(getMonsterDistance(i) < 6 && getMonsterDistance(i) < monsterDistance)
        {
          monsterDistance = getMonsterDistance(i);
        }
      }
      if(monsterDelayCounter[i] > 0)
      {
        monsterDelayCounter[i]--;
      }
    }
  }

  // Check movement within eyeshot: Anything changed?
  if(dist1 != getLivingMonsterRelativeToPlayer(1, MIDDLE) || dist2 != getLivingMonsterRelativeToPlayer(2, MIDDLE) || dist3 != getLivingMonsterRelativeToPlayer(3, MIDDLE))
  {
    monsterMovedWithinEyeshot = true;
  }
  if(dist3 != 0 && dist3 != getLivingMonsterRelativeToPlayer(3, MIDDLE))
  {
    drawMonster(3, monsterType[dist3 - 1], false, false, 0, 0);
  }
  if(dist2 != 0 && dist2 != getLivingMonsterRelativeToPlayer(2, MIDDLE))
  {
    drawMonster(2, monsterType[dist2 - 1], false, false, 0, 0);    
  }
  if(dist1 != 0 && dist1 != getLivingMonsterRelativeToPlayer(1, MIDDLE))
  {
    drawMonster(1, monsterType[dist1 - 1], false, false, 0, 0);
  }
  
  if(monsterDistance < 6)
  { 
    // SOUND: Moving monster in close distance 
    tone(audio, 100 - (monsterDistance * 10), 50);
  }
}

// Player collects an item
void collectItem(byte _item)
{ 
  byte newWeapon = 0;
  byte newArmor = 0;
  byte newSpell = 0;
  
  matrix.fillRect(8, 8, 16, 16, matrix.Color333(0,0,0));
  matrix.drawRect(8, 8, 16, 16, matrix.Color333(1,1,0));

  matrix.setCursor(10, 12);

  if(itemType[_item] == KEY)
  {
    // Key
    drawItem(KEY, 13, 12);
    playerKeys++;
  }
  else if(itemType[_item] == ELIXIR)
  {
    // Elixir
    drawItem(ELIXIR, 14, 12);
    playerElixirs++;    
  }
  else if(itemType[_item] == MAGIC_POTION)
  {
    // Magic Potion
    drawItem(MAGIC_POTION, 14, 12);
    playerMagicPotions++;
  }
  else if(itemType[_item] == SWORD)
  {
    // Sword
    matrix.setTextColor(matrix.Color333(3,0,0));
    matrix.print("W");    
    drawItem(SWORD, 18, 13);
    addWeapon(SWORD); 
    newWeapon = SWORD;   
  }
  else if(itemType[_item] == CLAYMORE)
  {
    // Claymore
    matrix.setTextColor(matrix.Color333(3,0,0));
    matrix.print("W");    
    drawItem(CLAYMORE, 18, 12);
    addWeapon(CLAYMORE);        
    newWeapon = CLAYMORE;   
  }
  else if(itemType[_item] == CROSSBOW)
  {
    // Crossbow
    matrix.setTextColor(matrix.Color333(3,0,0));
    matrix.print("W");    
    drawItem(CROSSBOW, 17, 13);            
    addWeapon(CROSSBOW);        
    newWeapon = CROSSBOW;
    playerBolts = 6;   
  }
  else if(itemType[_item] == LONGBOW)
  {
    // Longbow
    matrix.setTextColor(matrix.Color333(3,0,0));
    matrix.print("W");    
    drawItem(LONGBOW, 18, 12);        
    addWeapon(LONGBOW);        
    newWeapon = LONGBOW;   
    playerArrows = 6;   
  }
  else if(itemType[_item] == LEATHER)
  {
    // Leather Armor
    matrix.setTextColor(matrix.Color333(0,3,0));
    matrix.print("A");
    drawItem(LEATHER, 18, 13);
    addArmor(LEATHER);
    newArmor = LEATHER;        
  }
  else if(itemType[_item] == HEAVY)
  {
    // Heavy Armor
    matrix.setTextColor(matrix.Color333(0,3,0));
    matrix.print("A");
    drawItem(HEAVY, 18, 13);        
    addArmor(HEAVY);
    newArmor = HEAVY;        
  }
  else if(itemType[_item] == INVISIBILITY)
  {
    // Invisibility Armor
    matrix.setTextColor(matrix.Color333(0,3,0));
    matrix.print("A");
    drawItem(INVISIBILITY, 17, 13);        
    addArmor(INVISIBILITY);
    newArmor = INVISIBILITY;        
  }
  else if(itemType[_item] == MAGIC)
  {
    // Magic Armor
    matrix.setTextColor(matrix.Color333(0,3,0));
    matrix.print("A");
    drawItem(MAGIC, 17, 13);        
    addArmor(MAGIC);
    newArmor = MAGIC;        
  }  
  else if(itemType[_item] == HEALING)
  {
    // Healing Spell
    matrix.setTextColor(matrix.Color333(0,0,3));
    matrix.print("S");
    drawItem(HEALING, 16, 13);        
    addSpell(HEALING);        
    newSpell = HEALING;   
  }
  else if(itemType[_item] == IMMORTAL)
  {
    // Immortality Spell
    matrix.setTextColor(matrix.Color333(0,0,3));
    matrix.print("S");
    drawItem(IMMORTAL, 17, 13);        
    addSpell(IMMORTAL);        
    newSpell = IMMORTAL;   
  }
  else if(itemType[_item] == FIREBALL)
  {
    // Fireball Spell
    matrix.setTextColor(matrix.Color333(0,0,3));
    matrix.print("S");
    drawItem(FIREBALL, 17, 14);        
    addSpell(FIREBALL);        
    newSpell = FIREBALL;   
  }
  else if(itemType[_item] == LIGHTNING)
  {
    // Lightning Spell
    matrix.setTextColor(matrix.Color333(0,0,3));
    matrix.print("S");
    drawItem(LIGHTNING, 18, 12);            
    addSpell(LIGHTNING);        
    newSpell = LIGHTNING;   
  }
  else if(itemType[_item] == HASTE)
  {
    // Haste Spell
    matrix.setTextColor(matrix.Color333(0,0,3));
    matrix.print("S");
    drawItem(HASTE, 16, 13);            
    addSpell(HASTE);        
    newSpell = HASTE;   
  }

  itemStatus[_item] = 0;

  boolean firePressed = false;
  delay(200);
  do
  {
    if(joy1Fire()){ firePressed = true; }
  }
  while(!firePressed && !joy1Up());

  // Select the new item
  if(firePressed)
  {
    if(newWeapon != 0)
    { 
      currentWeapon = newWeapon;
      // SOUND: Select new item
      tone(audio, 1024, 10);
    }
    if(newArmor != 0)
    { 
      currentArmor = newArmor;
      // SOUND: Select new item
      tone(audio, 1024, 10);
    }
    if(newSpell != 0)
    { 
      currentSpell = newSpell;
      // SOUND: Select new item
      tone(audio, 1024, 10);
    }
  }
  if(currentArmor == 0 && newArmor != 0)
  { 
    currentArmor = newArmor; 
    // SOUND: Select new item
    tone(audio, 1024, 10);
  }
  if(currentSpell == 0 && newSpell != 0)
  { 
    currentSpell = newSpell; 
    // SOUND: Select new item
    tone(audio, 1024, 10);
  }

  // Update player delay (depending on equipment)
  updatePlayerDelay();
  
  delay(100);
  do
  {
  }
  while(joy1Fire());
  delay(100);
}

// Death animation if player dies
void death()
{
  float bloodPosition[32];
  float bloodSpeed[32];
  byte bloodColor[32];
  boolean redScreen = false;
  for(int i = 0; i < 32; i++)
  {
    bloodPosition[i] = 0.0;
    bloodSpeed[i] = ((float(random(10)) / 10.0) + 0.5) / 1.5;
    bloodColor[i] = random(7) + 1;
  }
  do
  {
    redScreen = true;
    for(int i = 0; i < 32; i++)
    {
      matrix.drawPixel(i, byte(bloodPosition[i]), matrix.Color333(bloodColor[i], 0, 0));
      bloodPosition[i] = bloodPosition[i] + bloodSpeed[i];
      if(byte(bloodPosition[i]) < 32)
      {
        redScreen = false;
      }
    }
    delay(40);
  }
  while(!redScreen);
  delay(1000);
  matrix.setTextColor(matrix.Color333(0,0,0));
  matrix.setCursor(7, 8);
  matrix.println("You");
  matrix.setCursor(4, 16);
  matrix.println("died");
  delay(1000);
  for(int i = 0; i < 100; i++)
  {
    matrix.setTextColor(matrix.Color333(7 * (i % 2),7 * (i % 2),7 * (i % 2)));
    matrix.setCursor(7, 8);
    matrix.println("You");
    matrix.setCursor(4, 16);
    matrix.println("died");
    // SOUND: Game over
    tone(audio,i * 10,5);
    delay(10);
  }
  delay(2000);
}

// Main game loop
void loop()
{
  mithotronic();
  ledMePlay();
   
  do
  {
    title();
    setupGame();

    do
    {
      showMaze();
      setupMaze();       
      redrawMaze();
      reset = false;
      
      do
      {
        if(playerDelayCounter == 0)
        {
          checkForPlayerMovement();
          checkForPlayerActions();
        }
      
        if(playerDelayCounter > 0)
        {
          playerDelayCounter--;
        }

        if(playerMagicCounter > 0)
        {
          playerMagicCounter--;
          if(playerMagicCounter <= 1000 && (playerMagicCounter % 100) == 0)
          {
            tone(audio, 400, 20);
          }
          if(playerMagicCounter == 0)
          {
            playerImmortal = false;
            playerHaste = false;
            updatePlayerDelay();
            playerMoved = true;
          }
        }
      
        moveMonsters();
      
        if(playerMoved)
        {           
          monsterMovedWithinEyeshot = false;
          updatePlayerMap();
          redrawMaze();
          playerMoved = false;
        }
        else if(playerAction || monsterMovedWithinEyeshot)
        {
          playerAction = false;
          monsterMovedWithinEyeshot = false;
          drawMaze();
        }
        delay(1);
      
        // Pause
        if(digitalRead(buttonPause) == LOW)
        {          
          // SOUND: Pause
          tone(audio,1024,20);
          delay(500);
          do
          {
          }
          while((!digitalRead(buttonPause) == LOW) && !joy1Fire());
          // SOUND: Pause off
          tone(audio,1024,20);
          delay(500);
        }

        // Reset
        if (digitalRead(buttonReset) == LOW)
        {
          reset = true;
          break;
        }

      }
      while(hitPoints > 0 && getMazeElement(playerX, playerY) != EXIT && !reset);

      // Next level
      if(getMazeElement(playerX, playerY) == EXIT)
      {
        // SOUND: Maze finished
        for(byte i = 0; i < 8; i++)
        {
          byte j = 0;  
          do
          {
            if(i == 0){ tone(audio,NOTE_D4 * ((j % 2) + 1), 30); }
            if(i == 1){ tone(audio,NOTE_E4 * ((j % 2) + 1), 30); }
            if(i == 2){ tone(audio,NOTE_F4 * ((j % 2) + 1), 30); }
            if(i == 3){ tone(audio,NOTE_G4 * ((j % 2) + 1), 30); }
            if(i == 4){ tone(audio,NOTE_F4 * ((j % 2) + 1), 30); }
            if(i == 5){ tone(audio,NOTE_E4 * ((j % 2) + 1), 30); }
            if(i == 6){ tone(audio,NOTE_D4 * ((j % 2) + 1), 30); }
            if(i == 7){ tone(audio,NOTE_D4 * ((j % 2) + 1), 30); }
            delay(30);
            j++;
          }
          while(j < 10);

          if(i != 6){ delay(100); }
        }
        delay(2800);        
        currentMaze++;
      }
    }
    while(hitPoints > 0 && currentMaze < numberOfMazes + 1 && !reset);

    if(!reset)
    {
      if(hitPoints == 0)
      {
        death();
      }
      else
      {
        endSequence();
      }
    }
  }
  while(true);
    
} // loop

// Check for available memory
int availableMemory()
{
  int size = 8192;
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);
  return size;
} 

