// Keen Kenny
// for the LEDmePlay
//
// www.mithotronic.de
//
// Version: 1.1.0
// Authors: Michael Rosskopf and Thomas Laubach (2015)
// 
// Release Notes:
// V1.1.0: Support for LEDmePlay Joypad and LEDmePlayBoy (2018)
// V1.0.1: Color problem with newer Adafruit libraries fixed
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

// Define maps
//
// 1st value: Number of tiles in x-direction
// 2nd value: Number of tiles in y-direction
// 3rd value: Level color: 0 == Invisible, 1 == Blue, 2 == Green, 3 == Turquoise, 4 == Red, 5 == Violet, 6 == Yellow, 7 == White
//
// The remaining numbers encode the tiles, gems, and enemies.
// Add tile type value (0 - 6)
// to values of
// Normal gem:               10
// Extralife gem:            20
// Bird:                     30
// Fireball flying left:     40
// Fireball flying right:    50
// Ufo:                      60
// Crab:                     70
// Fire:                     80
// Twister:                  90
// Devil:                   100
// Vulcano:                 110
// Hornet:                  120
// Laser floor:             130
// Laser left:              140
// Laset right:             150
// Vertical moving laser    160
// Horizontal moving laser  170
// Comet                    180
//
// Play start position      240
//
// Example: Tile 1 with crab (value 70) is encoded by 71.
//
const byte numberOfLevels = 16;
const uint8_t levels[] PROGMEM  = {
                                         // Level 1: The Introduction
                                         16, 4, 2,
                                         0,   0,   0,  60,   0,  11,   0,   0,   0,  60,   0,   0,  11,   0,   0,  10,
                                         0,   0,   0,   0,   0,   0,   0,  11,  11,   0,   0,  11,   0,   0,   0,   1,
                                         0,   1,  10,   0,   1,   1,   0,   0,   0,   0,   1,   1,   1,  71,   0,   0,
                                       241,   1,  11,   1,   3,   4,   1,   1,   1,  71,   3,   0,   0,   4,   1,  71,

                                         // Level 2: The Big Mac
                                         8, 5, 1,
                                        0,   0,   0,   0,   0,   0,   0,   0,
                                       51,  21,  81,  11,  11,   1,   1,   0,
                                        0,  41,   1,  11,  11,   1,   1,   1,
                                       51,   1,   1,  11,  11,   1,   1,   0,
                                      241,   1,   1,  11,  11,   1,   1,   1, 
                                         
                                         // Level 3: The Scaffold
                                         8, 8, 3,
                                        50,   0,   0,  11,  11,   0,   0,   0,  
                                         0,   0,   1,   1,  71,   1,   0,   0,  
                                        11,   0,   0,   0,   0,   0,   0,  11,  
                                         0,  60,  11,  71,   1,  11,  60,   0,  
                                        11,   1,   1,   0,   0,   1,   1,  11,  
                                         0,   0,   0,  11,  11,   0,   0,   0,  
                                         0,   0,   1,   1,   1,   1,   0,  40,  
                                       241,  11,   1,   1,   1,   1,  11,  71,  
                                         
                                         // Level 4: The Beach
                                        24, 4, 6,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 180,   0,   0,   0,   0,   0,   0,  11,  71,   0,  10,
                                         0,   0,   0,   0,  11,  71,   0,   0,   0,   0,  11,  71,   0,   0,   0,  11,  71,   0,   0,   1,  11,  71,   0,   1,
                                         0,   0,  11,  71,   0,   0,   1,  71,   0,   0,   0,  11,   1,   0,   0,   0,   0,   0,   1,   1,  11,   1,  60,   3,
                                       241,   1,   0,   0,  11,  71,   0,   0,  11,  71,   0,   0,   0,   0,   1,   0,  11,   1,   0,   0,   0,   0,   1,   3,
                                          
                                         // Level 5: The Laserdisco
                                         8, 8, 5,
                                       241,   1,   1,   1,   1,   1, 151,   0,
                                         1,  11,   1, 130, 130,   1,   1,  41,
                                        51,  11,   0,   1,   1,   0,  11,   1,
                                        11,   1, 151, 130, 130, 141,   1,  11,
                                        21,   1, 130,   1,   1, 130,   1,  11,
                                         0,   0,   0, 141, 151,   0,   0,  40,
                                         0,  11,  11,   0,   0,  11,  11,   0,
                                         0,   0,   0,   0,   0,   0,   0,   0,

                                         // Level 6: The Tower
                                         4, 16, 1,
                                         0,   0, 120,  20, 
                                         0,   0,  11,  60, 
                                         0,   0,   0,   0, 
                                         0,  11,   0,   0, 
                                         0, 120,  60,   0,
                                        81,  11,   0,   0,
                                         0,  60,   0,   0,
                                         0,   0,   1,  11,
                                         0,   0,  60,   0,
                                        11,   1,   0,   0,
                                         0,  60,  30,   0,
                                         0,   0,  11,   1,
                                         0,   0,   0,   0,
                                         1,   0,   1,   1,
                                        11,   1,   0,  11,
                                       241,   1,   1,  11,
                                         
                                         // Level 7: The Abyss
                                         12, 8, 4,
                                        51,  11,   1,  11,   1,  11,   1,  11,   1,  11,   1,  40,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 120,
                                         0,   0,   0,   1,   1,   0,   1,  71,   0,   1,   0,   1,
                                         0,   1,   0,   0,   5,  81,   1,   1,  81,   1,  81,   1, 
                                        11,  81,  81,  11,   1,  11,   1,  11,   1,  11,   1,  40,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 120,
                                       241,   1,   0,   1,  71,   0,   1,  71,   0,   1,   0,   1,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,

                                         // Level 8: The Arena
                                         8, 8, 7, 
                                         0,   0,   1,  11,  11,   1,   0,   0,
                                        11,   1,   1,   0,   0,   1,   1,  11,
                                         0,   0,   0,  11,  11,   0,   0,   0,
                                       100,   0,  11,   1,   1,  11,   0, 100,
                                        11,   1,   0,   0,   0,   0,   1,  11,
                                         0,   0,   1,   0,   0,   1,   0,   0,
                                         1,   1,   0,  11,  11,   0,   1,   1,
                                       241,   1,   1,   0,   0,   1,   1,   1,
 
                                         // Level 9: The Windpark
                                         24, 4, 2,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  11,  11,   0,   0,   0,   0,  11,  11,   0,   0,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  71,   1,   1,  91,   0,   0,   0,   0,   0,   0,   0,  10,
                                         0,   0,   0,   0,   1,  11,  11,  91,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,  91,   0,   0,   0,   0,
                                       241,  11,  11,  71,   0,   0,   0,   0,   1,  11,  11,  91,   0,   0,   0,   0,   1,  71,   0,   0,   0,   0,   1,  91,
                                         
                                         // Level 10: The Magic Mountain
                                         10, 10, 5,
                                         0,  60,   0,  60,   0,  60,   0,   0,   1, 160,              
                                         0,   0,   0,   0,   0,   0,   1,   2,   4,   0,              
                                         0,   0,   0,   0,   1,   2, 120,  20,   4,   0,              
                                         0,   0,   1,   2,   0,  60,  11,  11,   4,   0,              
                                       241,   2,   0,  60,  11,  11,  11,  11,   4,   0,              
                                       120,   0,   0,   0,   0,   0,   0,   0,   4,   0,              
                                         0,   1,  11,   1,   1,  11,   1,   0,   4,   0,              
                                       171,   1,  11,   1,   1,  11,   1, 130,   6,   0,              
                                         0, 171,  11,   1,   1,  11,   1,   1,   3,   1,              
                                       171,   1,  11,   1,   1,  11,   1,   1,   1, 171,              

                                         // Level 11: The Bad Lands
                                         20, 6, 6,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 180,   0,   0,   0,   0,   0, 180,
                                        30,   0,   0,   0,  11,  11,   0,   0,   0,   0,  30,   0,   0,  11,  11,   0,   0,   0,   0,  20,
                                         0,   0,   0,   0,   0,   0,   0,  11,  11,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                                         0,   0,   0,   0,   0, 180,   0,   0,   0,   0, 111,   1,   0,  11,   0,   0,   0, 180,  11,   0,
                                         0,   0,   0, 111,   1,   0,   0,   0,   1,   2,   0,   0,   0,   0,   2, 111,   0,   0,   0,   0,
                                       241,  11,   2,   0,   0,   0,  11,   0,   0,   0,   0,   0,   0,   0,   0,   0,  11, 111,   0,  10,

                                         // Level 12: The Spiral
                                         12, 8, 4,
                                        30,  0, 171,   1,   1,   1,  31,   1,   1,   1,   1, 241,
                                       160,   0,   3,   0, 171,   1,   1,   1,   1,   1,   0,   0,
                                         0,   1,   3,   1,   3,   0,   1, 171,   0,   4,   1,   0,
                                         0,   0,   3,   0,  13,  10,  10,  14,   0,   4,   0,   0,
                                         0,   1,   3,   1,  13,  10,  10,  14,   1,   4,   1,   0,
                                         0,   0,   3,   0,  15,  11,  11,  16,   0,   4,   0, 160,
                                         0,   1,   5,   1,   1,   1,   1,   1, 171,   6,   1,   0,
                                       171,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,

                                         // Level 13: The Descend
                                         4, 24, 5,
                                       241,   1,   1,   0,
                                         0,  20,   4,  30,
                                         0,   0,   4,   0,
                                         0,  81,   1,   1,
                                         0,   3,   0,   0,
                                         0,   3,   0,   0,
                                         1,   1,  81,   0,
                                         0,  10,   4,   0,
                                       120,   1,  81,   1,
                                         0,   3,  10,   0,
                                         1,  81,   1, 120,
                                        30,  10,  10,   0,
                                         0,  81,  81,   0,
                                         0,   1,   1,   1,
                                         0,   1,   1,   0,
                                         0,   0,   0,   1,
                                         0,   0,   0,   0,
                                         0,  10,  10,   1,
                                         0,  10,  10,   0,
                                         0,  10,  10,   1,
                                         0,  10,  10,   0,
                                         0,  10,  10,  31,
                                         0,   0,   0,   0,
                                         1,   0,   0,   1,
 
                                         // Level 14: The Twisted Valley
                                         12, 8, 3,
                                         0,   0,   0,   0, 180,   0,   0,   0,   0,   0,   0,  20,
                                         0,   0,   0,  11,   1,   1,   0,   1,  11,   0,   0,   0,
                                       241,   0,   0,   0,  11,   1, 181,  11,   0,   0,   0,   1,
                                         4,   1,   0,   0,   1,   0,   1,   1,   0,   0,   1,   3,
                                        10,   4,   1,   0,   0, 181,  91,   0,   0,   1,   3,  10,
                                        11,   0,   4,   1,   0,  11,  11,   0,   1,   3,   0,  11,
                                        91,   1,   0,   4,   0,   0,   0, 180,   3,   0,   1,  91,
                                         0,   0,   0,   1,  11,  91,   1,  11,   1,   0,   0,   0,

                                         // Level 15: The total Insanity
                                         12, 8, 1,
                                         0, 180,   0,   0,  60,   0,   0,   0,   0,   0, 180,  30,
                                        11,   0,   0,   0,   0,  11,  91,  11,   0,   0,   0,   0,
                                         0,   0,   1, 111,   0,   0,   0,   0,   0,  11,   0,   0,
                                        21,   0,   3,   4,   0,   0,   0,  11,  71,   0,   0,  11,
                                         0,   0,   5,   6,   1,   1,   1,   0,   0,   0,   0,   0,
                                         0,   0, 171, 171,   1,   1,   1,   0,   0,  11, 130,  10,
                                       241,   0,   0,   0,   0,   0, 151,   1,   0,   0,  40, 140,
                                         0,   0,   0,  11,  91,  11,   0,   0,   0,   0,   0, 141,

                                         // Level 16: The Devils Cove
                                         10, 10, 7,
                                       100,   0,   0, 100,  11,  11,   0,   0,   0, 100, 
                                         0,  11,   0,   0,   0,   0,   0,   0,  11,   0, 
                                         0,   0,   0,   0,   1,   1,   0,   0,   0,   0, 
                                         0,   0,  11,   0,   0,   0,   0,  11,   0,   0, 
                                        11,   0,   0,   0, 241,   1,   0,   0,   0,  11, 
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
                                         0,   0,  11,   0,  11,  11,   0,  11,   0,   0, 
                                        11,   0,   0,   0,   0,   0,   0,   0,   0,  11, 
                                         0,   1,   0,   0,  11,  11,   0,   0,   1,   0, 
                                       100,   0,   0,   0,   0,   0, 100,   0,   0, 100
                                         
                                  };
                                  
byte levelMap[128]; // Contains the current level after loading from PROGMEM

byte tileNumberX; // Number of tiles in X-direction of the current map
byte tileNumberY; // Number of tiles in Y-direction of the current map
byte mapWidth;    // Number of pixels in X-direction of the current map (tiles * 8)
byte mapHeight;   // Number of pixels in Y-direction of the current map (tiles * 8)

// Level color
byte levelColor;  // Level platform color: 0 == Invisible, 1 == Blue, 2 == Green, 3 == Turquoise, 4 == Red, 5 == Violet, 6 == Yellow, 7 == White
byte red;
byte green;
byte blue;

// Playfield
byte playfield[48][48]; // Visible buffer on screen

// Position of the screen (screenX and screenY define the upper left position of the complete map)
int screenX;
int screenY;
int screenXNew; // New X position after movement
int screenYNew; // New Y position after movement
boolean redraw; // Update of screen if true

// Positions of the upper left tile which is visible on the screen (all others tiles are drawn relative to it to realize the scrolling)
int upperLeftTileX;
int upperLeftTileXOffset; // Number of pixels of upper left tile in X-direction which are not visible (out of screen)
int upperLeftTileY;
int upperLeftTileYOffset; // Number of pixels of upper left tile in Y-direction which are not visible (out of screen)

// Player
byte lives;                 // Remaining lives
byte level;                 // Current level
int gems;                   // Counts the collected gems
int playerXScreen;          // X position on screen
int playerYScreen;          // Y position on screen
int playerXScreenNew;       // New X position after movement
int playerYScreenNew;       // New Y position after movement
float playerXMap;           // X position on map
float playerYMap;           // Y position on map
float playerXMapNew;        // New X position after movement
float playerYMapNew;        // New Y position after movement
float playerYVector;        // Counts down to define the maximal jump height
byte playerMovement;        // 0 == Standing, 1 == Moving up (after a jump), 2 == Moving down, 3 == Duck
byte playerAnimationPhase;  // Animation phase (0 or 1)
boolean playerDirection;    // 0 == looking right, 1 == looking left
boolean playerDirectionNew; // New direction after movement
// X and Y Positions of the body parts after death
float todesanimationX[5];
float todesanimationY[5];

// Enemies
int enemyXScreen[16];    // X position on screen
int enemyYScreen[16];    // Y position on screen
int enemyXScreenNew[16]; // New X position after movement
int enemyYScreenNew[16]; // New Y position after movement
float enemyXMap[16];     // X position on map
float enemyYMap[16];     // Y position on map
byte enemyType[16];      // Enemy type (1 - 16)
// Some enemies move between two points (X1, Y1) and (X2, Y2). The enemyMovement defines the direction (depends on enemy type).
byte enemyMovement[16];
int enemyX1[16];
int enemyY1[16];
int enemyX2[16];
int enemyY2[16];
byte enemyStatus[16]; // 0 == inactive, 1 == active
byte enemyCounter;    // Number of enemies in current level (16 is the maximum)

// Gems
int gemXScreen[16];              // X position on screen
int gemYScreen[16];              // Y position on screen
int gemXScreenNew[16];           // New X position after movement
int gemYScreenNew[16];           // New Y position after movement
float gemXMap[16];               // X position on map
float gemYMap[16];               // Y position on map
byte gemStatus[16];              // 0 == inactive, 1 == normal gem, 2 == extralife gem, 3 == just collected
byte gemCounter;                 // Number of gems in current level (16 is the maximum)
byte gemsToFinishLevel;          // Jump to next level if 0
boolean initializeLevelWithGems; // true, if a new level is initialized

// Game synchronization
unsigned long engineLoopStartPoint; // Contains millisconds since start of LEDmePlay at begin of each engine loop (every 20ms)

// Some helpers
int i, j, k, x1, y1, x2, y2;
float x, y, z;
boolean changed;       // Helper used in the status screen

byte animationCounter; // Counts up till 255 and starts again with 0 (animations are synchronized to it)
byte audioOffUntil;    // If a gem is collected, other sounds are muted in order to hear a longer "collected" sound.
boolean reset;         // true if reset button has been set

// Program setup  
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
  i = -10;
  do
  {
    drawM(7, i);
    drawT(19, 22 - i);
    i++;
    delay(50);
  }
  while(i <= 11);

  // Jingle on start screen
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

  i = 0;
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

    j = 0;
    do
    {
      j++;
      delay(1);
    }
    while(j < 250 && !joy1Fire() && !joy2Fire());
  }
  while(!joy1Fire() && !joy2Fire());
  tone(audio,1024,20);
  delay(200);
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
}

// Keen Kenny title screen
void title()
{
  // Clear screen
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // Write 'Keen Kenny'
  matrix.setTextColor(matrix.Color333(0,3,0));
  matrix.setCursor(5, 2);
  matrix.println("Keen");
  matrix.setCursor(1, 8);
  matrix.println("Kenny");

  i = 0;
  do
  {
    i++;
    if(i == 1)
    {
      // Draw diamond
      matrix.drawLine(5, 20, 8, 17, matrix.Color333(1,1,4));
      matrix.drawLine(9, 17, 12, 20, matrix.Color333(1,1,7));
      matrix.drawLine(12, 20, 15, 17, matrix.Color333(1,1,7));
      matrix.drawLine(16, 17, 19, 20, matrix.Color333(1,1,7));
      matrix.drawLine(19, 20, 22, 17, matrix.Color333(1,1,7));
      matrix.drawLine(23, 17, 26, 20, matrix.Color333(1,1,4));
      matrix.drawLine(9, 17, 21, 17, matrix.Color333(1,1,4));
      matrix.drawLine(5, 20, 26, 20, matrix.Color333(1,1,4));
      matrix.drawLine(12, 20, 15, 29, matrix.Color333(1,1,7));
      matrix.drawLine(19, 20, 16, 29, matrix.Color333(1,1,7));
      matrix.drawLine(5, 20, 15, 29, matrix.Color333(1,1,4));
      matrix.drawLine(26, 20, 16, 29, matrix.Color333(1,1,4));
      x1 = random(12) + 10;
      y1 = random(8) + 18;
    }
    if(i == 750)
    {
      matrix.drawPixel(x1, y1, matrix.Color333(7,7,7));
    }
    if(i == 850)
    {
      matrix.drawLine(x1 - 1, y1, x1 + 1, y1, matrix.Color333(7,7,7));
      matrix.drawLine(x1, y1 - 1, x1, y1 + 1, matrix.Color333(7,7,7));
    }
    if(i == 1000)
    {
      matrix.drawLine(x1 - 1, y1, x1 + 1, y1, matrix.Color333(0,0,0));
      matrix.drawLine(x1, y1 - 1, x1, y1 + 1, matrix.Color333(0,0,0));
      i = 0;
    }
    delay(1);
  }
  while(!joy1Fire()); 
  tone(audio,1024,20);
  delay(200);
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
}

// Setup a new game
void setupGame()
{
  lives = 3;
  level = 1;
  gems = 0;
  initializeLevelWithGems = true;
}

// Setup a new level
void setupLevel()
{
  // If a live has been lost, the level is not initialized with gems (only on level start).
  if(initializeLevelWithGems)
  {
    for(i = 0; i < 16; i++)
    {
      gemXScreen[i] = 0;
      gemYScreen[i] = 0;
      gemXScreenNew[i] = 0;
      gemYScreenNew[i] = 0;
      gemXMap[i] = 0;
      gemYMap[i] = 0;
      gemStatus[i] = 0;
    }
    gemCounter = 0;
    gemsToFinishLevel = 0;
  }
  
  enemyCounter = 0;
  
  // Reset enemies
  for(i = 0; i < 16; i++)
  {
    enemyXScreen[i] = 0;
    enemyYScreen[i] = 0;
    enemyXScreenNew[i] = 0;
    enemyYScreenNew[i] = 0;
    enemyXMap[i] = 0;
    enemyYMap[i] = 0;
    enemyType[i] = 0;
    enemyMovement[i] = 0;
    enemyX1[i] = 0;
    enemyY1[i] = 0;
    enemyX2[i] = 0;
    enemyY2[i] = 0;
    enemyStatus[i] = 0;
  }
  
  // Copy level map and enemies from progmem
  k = 0;
  for(i = 0; i < level; i++)
  {
    tileNumberX = byte(pgm_read_byte_near(levels + k));
    tileNumberY = byte(pgm_read_byte_near(levels + 1 + k));
    levelColor = byte(pgm_read_byte_near(levels + 2 + k));
    updateLevelColors();
    
    if(i < level - 1)
    {
      k = k + ((tileNumberX * tileNumberY) + 3);
    }
  }

  mapWidth = tileNumberX * 8;
  mapHeight = tileNumberY * 8;

  for(i = 0; i < (tileNumberX * tileNumberY); i++)
  {
    j = byte(pgm_read_byte_near(levels + 3 + i + k));
    levelMap[i] = j % 10; // Returns tile type
    j = j / 10; // Returns start position, gems or enemy
    
    // Set starting position of player
    if(j == 24)
    {
      playerXMap = ((i % tileNumberX) * 8) + 3;
      playerYMap = ((i / tileNumberX) * 8) + 3;
      playerXScreen = 15;
      playerYScreen = 14;

      if(playerXMap > 14)
      {
        screenX = playerXMap - 15;
        if(screenX > mapWidth - 32)
        {
          screenX = mapWidth - 32;
          playerXScreen = 32 - (mapWidth - playerXMap);
        }
      }
      else
      {
        screenX = 0;
        playerXScreen = playerXMap;
      }
      
      if(playerYMap > 13)
      {
        screenY = playerYMap - 14;
        if(screenY > mapHeight - 32)
        {
          screenY = mapHeight - 32;
          playerYScreen = 32 - (mapHeight - playerYMap);
        }
      }
      else
      {
        screenY = 0;
        playerYScreen = playerYMap;
      }
      
      playerXMapNew = playerXMap;
      playerYMapNew = playerYMap;
      playerXScreenNew = playerXScreen;
      playerYScreenNew = playerYScreen;
      playerYVector = 0.0;
      playerMovement = 0;
      playerDirection = 1;
      playerDirectionNew = 0;
      screenXNew = screenX;
      screenYNew = screenY;
      redraw = false;
    }
        
    // Set normal gem
    if(j == 1 && initializeLevelWithGems)
    {
      gemXMap[gemCounter] = ((i % tileNumberX) * 8) + 3;
      gemYMap[gemCounter] = ((i / tileNumberX) * 8) + 3;
      gemStatus[gemCounter] = 1;
      gemsToFinishLevel++;
      if(gemCounter < 15)
      {
        gemCounter++;
      }
    }

    // Set extralife gem
    if(j == 2 && initializeLevelWithGems)
    {
      gemXMap[gemCounter] = ((i % tileNumberX) * 8) + 3;
      gemYMap[gemCounter] = ((i / tileNumberX) * 8) + 3;
      gemStatus[gemCounter] = 2;
      if(gemCounter < 15)
      {
        gemCounter++;
      }
    }

    // Set enemy: Bird
    if(j == 3)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8) + 2;
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8) + 3;
      enemyType[enemyCounter] = 1;
      enemyMovement[enemyCounter] = 1;
      enemyStatus[enemyCounter] = 1;
    }

    // Set enemy: Fireball flying left
    if(j == 4)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8) + 2;
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8) + 3;
      enemyType[enemyCounter] = 2;
      enemyMovement[enemyCounter] = 1;
      enemyStatus[enemyCounter] = 1;
    }

    // Set enemy: Fireball flying right
    if(j == 5)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8) + 2;
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8) + 3;
      enemyType[enemyCounter] = 3;
      enemyMovement[enemyCounter] = 1;
      enemyStatus[enemyCounter] = 1;
    }

    // Set enemy: Ufo
    if(j == 6)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8) + 2;
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8) + 3;
      enemyType[enemyCounter] = 4;
      enemyMovement[enemyCounter] = 1;
      enemyX1[enemyCounter] = i % tileNumberX;
      enemyY1[enemyCounter] = i / tileNumberX;
      enemyX2[enemyCounter] = i % tileNumberX;
      enemyY2[enemyCounter] = i / tileNumberX;
      enemyStatus[enemyCounter] = 1;
    }
    
    // Set enemy: Crab
    if(j == 7)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8) + 1;
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8) + 4;
      enemyType[enemyCounter] = 5;
      enemyMovement[enemyCounter] = 1;
      enemyX1[enemyCounter] = i % tileNumberX;
      enemyY1[enemyCounter] = i / tileNumberX;
      enemyX2[enemyCounter] = i % tileNumberX;
      enemyY2[enemyCounter] = i / tileNumberX;
      enemyStatus[enemyCounter] = 1;
    }

    // Set enemy: Fire
    if(j == 8)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8) + 2;
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8) + 4;
      enemyType[enemyCounter] = 6;
      enemyMovement[enemyCounter] = 1;
      enemyStatus[enemyCounter] = 1;
    }

    // Set enemy: Twister
    if(j == 9)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8) + 3;
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8) + 3;
      enemyType[enemyCounter] = 7;
      enemyMovement[enemyCounter] = 1;
      enemyX1[enemyCounter] = i % tileNumberX;
      enemyY1[enemyCounter] = i / tileNumberX;
      enemyX2[enemyCounter] = i % tileNumberX;
      enemyY2[enemyCounter] = i / tileNumberX;
      enemyStatus[enemyCounter] = 1;
    }

    // Set enemy: Devil
    if(j == 10)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8) + 2;
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8) + 2;
      enemyType[enemyCounter] = 8;
      enemyMovement[enemyCounter] = 1;
      enemyStatus[enemyCounter] = 1;
    }

    // Set enemy: Vulcano
    if(j == 11)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8) + 2;
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8) + 5;
      enemyType[enemyCounter] = 9;
      enemyMovement[enemyCounter] = 1;
      enemyStatus[enemyCounter] = 1;
    }

    // Set enemy: Hornet
    if(j == 12)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8) + 2;
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8) + 3;
      enemyType[enemyCounter] = 10;
      enemyMovement[enemyCounter] = 1;
      enemyX1[enemyCounter] = i % tileNumberX;
      enemyY1[enemyCounter] = i / tileNumberX;
      enemyX2[enemyCounter] = i % tileNumberX;
      enemyY2[enemyCounter] = i / tileNumberX;
      enemyStatus[enemyCounter] = 1;
    }
    
    // Set enemy: Laser floor
    if(j == 13)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8);
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8) + 7;
      enemyType[enemyCounter] = 11;
      enemyMovement[enemyCounter] = 1;
      enemyStatus[enemyCounter] = 1;
    }
            
    // Set enemy: Laser left
    if(j == 14)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8);
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8);
      enemyType[enemyCounter] = 12;
      enemyMovement[enemyCounter] = 1;
      enemyStatus[enemyCounter] = 1;
    }

    // Set enemy: Laser right
    if(j == 15)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8) + 7;
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8);
      enemyType[enemyCounter] = 13;
      enemyMovement[enemyCounter] = 1;
      enemyStatus[enemyCounter] = 1;
    }

    // Set enemy: Vertical moving laser
    if(j == 16)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8);
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8);
      enemyType[enemyCounter] = 14;
      enemyMovement[enemyCounter] = 1;
      enemyX1[enemyCounter] = i % tileNumberX;
      enemyY1[enemyCounter] = i / tileNumberX;
      enemyX2[enemyCounter] = i % tileNumberX;
      enemyY2[enemyCounter] = i / tileNumberX;
      enemyStatus[enemyCounter] = 1;
    }
    // Set enemy: Horizontal moving laser

    if(j == 17)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8);
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8);
      enemyType[enemyCounter] = 15;
      enemyMovement[enemyCounter] = 1;
      enemyX1[enemyCounter] = i % tileNumberX;
      enemyY1[enemyCounter] = i / tileNumberX;
      enemyX2[enemyCounter] = i % tileNumberX;
      enemyY2[enemyCounter] = i / tileNumberX;
      enemyStatus[enemyCounter] = 1;
    }

    // Set enemy: Comet
    if(j == 18)
    {
      enemyXMap[enemyCounter] = ((i % tileNumberX) * 8) + 3;
      enemyYMap[enemyCounter] = ((i / tileNumberX) * 8);
      enemyType[enemyCounter] = 16;
      enemyMovement[enemyCounter] = 1;
      enemyStatus[enemyCounter] = 1;
    }

    if(j > 2 && enemyCounter < 15)
    {
      enemyCounter++;
    }
  }

  // Initialize enemy movement
  initializeEnemyMovement();  

  // Clear screen
  matrix.fillScreen(matrix.Color333(0,0,0));
  
  // Initialize playfield
  for(i = 0; i < 48; i++)
  {
    for(j = 0; j < 48; j++)
    {
      playfield[i][j] = 0;
    }
  }
  
  animationCounter = 0;
  audioOffUntil = 0;
}

// Set RGB-values depending on level color
// 0 == Invisible
// 1 == Blue
// 2 == Green
// 3 == Turquoise
// 4 == Red
// 5 == Violet
// 6 == Yellow
// 7 == White
void updateLevelColors()
{
  switch(levelColor)
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

// Draw the screen according to the current x and y position
void drawScreen()
{
  if(redraw)
  {
    // Clear all tiles at old positions
    upperLeftTileX = screenX / 8;
    upperLeftTileXOffset = screenX % 8;
    upperLeftTileY = screenY / 8;
    upperLeftTileYOffset = screenY % 8;

    for(j = 0; j < 5; j++)
    {
      for(i = 0; i < 5; i++)
      {
        clearTile(levelMap[((upperLeftTileY + j ) * tileNumberX) + upperLeftTileX + i], (i * 8) - upperLeftTileXOffset, (j * 8) - upperLeftTileYOffset);
      }
    }
    screenX = screenXNew;
    screenY = screenYNew;
  }
  // Draw all tiles at new positions
  upperLeftTileX = screenX / 8;
  upperLeftTileXOffset = screenX % 8;
  upperLeftTileY = screenY / 8;
  upperLeftTileYOffset = screenY % 8;
    
  for(j = 0; j < 5; j++)
  {
    for(i = 0; i < 5; i++)
    {
      drawTile(levelMap[((upperLeftTileY + j ) * tileNumberX) + upperLeftTileX + i], (i * 8) - upperLeftTileXOffset, (j * 8) - upperLeftTileYOffset);
    }
  }
  redraw = false;
}

// Draws a tile (8*8) at position x, y
//
//   type 0     type 1     type 2     type 3     type 4     type 5     type 6
//   --------   --------   --------   O-------   -------O   O-------   -------O
//   --------   --------   --------   O-------   -------O   O-------   -------O
//   --------   --------   --------   O-------   -------O   O-------   -------O
//   --------   --------   OOOOOOOO   O-------   -------O   O-------   -------O
//   --------   --------   --------   O-------   -------O   O-------   -------O
//   --------   --------   --------   O-------   -------O   O-------   -------O
//   --------   --------   --------   O-------   -------O   O-------   -------O
//   --------   OOOOOOOO   --------   O-------   -------O   OOOOOOOO   OOOOOOOO
//
void drawTile(byte _type, int _x, int _y)
{
  if(_type == 0)
  {
  }
  if(_type == 1)
  {
    matrix.drawPixel(_x, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 1, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 2, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 3, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 4, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 5, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 6, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(red, green, blue));
    playfield[_x + 8][_y + 15] = 1;
    playfield[_x + 9][_y + 15] = 1;
    playfield[_x + 10][_y + 15] = 1;
    playfield[_x + 11][_y + 15] = 1;
    playfield[_x + 12][_y + 15] = 1;
    playfield[_x + 13][_y + 15] = 1;
    playfield[_x + 14][_y + 15] = 1;
    playfield[_x + 15][_y + 15] = 1;
  }
  if(_type == 2)
  {
    matrix.drawPixel(_x, _y + 3, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 1, _y + 3, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 2, _y + 3, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 3, _y + 3, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 4, _y + 3, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 5, _y + 3, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 6, _y + 3, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 3, matrix.Color333(red, green, blue));
    playfield[_x + 8][_y + 11] = 1;
    playfield[_x + 9][_y + 11] = 1;
    playfield[_x + 10][_y + 11] = 1;
    playfield[_x + 11][_y + 11] = 1;
    playfield[_x + 12][_y + 11] = 1;
    playfield[_x + 13][_y + 11] = 1;
    playfield[_x + 14][_y + 11] = 1;
    playfield[_x + 15][_y + 11] = 1;
  }
  if(_type == 3)
  {
    matrix.drawPixel(_x, _y, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 1, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 2, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 3, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 4, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 5, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 6, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 7, matrix.Color333(red, green, blue));
    playfield[_x + 8][_y + 8] = 1;
    playfield[_x + 8][_y + 9] = 1;
    playfield[_x + 8][_y + 10] = 1;
    playfield[_x + 8][_y + 11] = 1;
    playfield[_x + 8][_y + 12] = 1;
    playfield[_x + 8][_y + 13] = 1;
    playfield[_x + 8][_y + 14] = 1;
    playfield[_x + 8][_y + 15] = 1;
  }
  if(_type == 4)
  {
    matrix.drawPixel(_x + 7, _y, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 1, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 2, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 3, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 4, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 5, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 6, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(red, green, blue));
    playfield[_x + 15][_y + 8] = 1;
    playfield[_x + 15][_y + 9] = 1;
    playfield[_x + 15][_y + 10] = 1;
    playfield[_x + 15][_y + 11] = 1;
    playfield[_x + 15][_y + 12] = 1;
    playfield[_x + 15][_y + 13] = 1;
    playfield[_x + 15][_y + 14] = 1;
    playfield[_x + 15][_y + 15] = 1;
  }
  if(_type == 5)
  {
    matrix.drawPixel(_x, _y, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 1, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 2, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 3, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 4, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 5, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 6, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 1, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 2, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 3, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 4, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 5, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 6, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(red, green, blue));
    playfield[_x + 8][_y + 8] = 1;
    playfield[_x + 8][_y + 9] = 1;
    playfield[_x + 8][_y + 10] = 1;
    playfield[_x + 8][_y + 11] = 1;
    playfield[_x + 8][_y + 12] = 1;
    playfield[_x + 8][_y + 13] = 1;
    playfield[_x + 8][_y + 14] = 1;
    playfield[_x + 8][_y + 15] = 1;
    playfield[_x + 9][_y + 15] = 1;
    playfield[_x + 10][_y + 15] = 1;
    playfield[_x + 11][_y + 15] = 1;
    playfield[_x + 12][_y + 15] = 1;
    playfield[_x + 13][_y + 15] = 1;
    playfield[_x + 14][_y + 15] = 1;
    playfield[_x + 15][_y + 15] = 1;
  }
  if(_type == 6)
  {
    matrix.drawPixel(_x + 7, _y, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 1, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 2, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 3, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 4, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 5, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 6, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 1, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 2, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 3, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 4, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 5, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 6, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(red, green, blue));
    playfield[_x + 15][_y + 8] = 1;
    playfield[_x + 15][_y + 9] = 1;
    playfield[_x + 15][_y + 10] = 1;
    playfield[_x + 15][_y + 11] = 1;
    playfield[_x + 15][_y + 12] = 1;
    playfield[_x + 15][_y + 13] = 1;
    playfield[_x + 15][_y + 14] = 1;
    playfield[_x + 8][_y + 15] = 1;
    playfield[_x + 9][_y + 15] = 1;
    playfield[_x + 10][_y + 15] = 1;
    playfield[_x + 11][_y + 15] = 1;
    playfield[_x + 12][_y + 15] = 1;
    playfield[_x + 13][_y + 15] = 1;
    playfield[_x + 14][_y + 15] = 1;
    playfield[_x + 15][_y + 15] = 1;
  }
}

// Clear a tile (8*8) at position x, y
void clearTile(byte _type, int _x, int _y)
{
  if(_type == 0)
  {
  }
  if(_type == 1)
  {
    matrix.drawPixel(_x, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 1, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 2, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 3, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 4, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 5, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 6, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(0, 0, 0));
    playfield[_x + 8][_y + 15] = 0;
    playfield[_x + 9][_y + 15] = 0;
    playfield[_x + 10][_y + 15] = 0;
    playfield[_x + 11][_y + 15] = 0;
    playfield[_x + 12][_y + 15] = 0;
    playfield[_x + 13][_y + 15] = 0;
    playfield[_x + 14][_y + 15] = 0;
    playfield[_x + 15][_y + 15] = 0;
  }
  if(_type == 2)
  {
    matrix.drawPixel(_x, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 1, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 2, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 3, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 4, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 5, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 6, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 3, matrix.Color333(0, 0, 0));
    playfield[_x + 8][_y + 11] = 0;
    playfield[_x + 9][_y + 11] = 0;
    playfield[_x + 10][_y + 11] = 0;
    playfield[_x + 11][_y + 11] = 0;
    playfield[_x + 12][_y + 11] = 0;
    playfield[_x + 13][_y + 11] = 0;
    playfield[_x + 14][_y + 11] = 0;
    playfield[_x + 15][_y + 11] = 0;
  }
  if(_type == 3)
  {
    matrix.drawPixel(_x, _y, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 4, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 5, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 6, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 7, matrix.Color333(0, 0, 0));
    playfield[_x + 8][_y + 8] = 0;
    playfield[_x + 8][_y + 9] = 0;
    playfield[_x + 8][_y + 10] = 0;
    playfield[_x + 8][_y + 11] = 0;
    playfield[_x + 8][_y + 12] = 0;
    playfield[_x + 8][_y + 13] = 0;
    playfield[_x + 8][_y + 14] = 0;
    playfield[_x + 8][_y + 15] = 0;
  }
  if(_type == 4)
  {
    matrix.drawPixel(_x + 7, _y, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 4, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 5, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 6, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(0, 0, 0));
    playfield[_x + 15][_y + 8] = 0;
    playfield[_x + 15][_y + 9] = 0;
    playfield[_x + 15][_y + 10] = 0;
    playfield[_x + 15][_y + 11] = 0;
    playfield[_x + 15][_y + 12] = 0;
    playfield[_x + 15][_y + 13] = 0;
    playfield[_x + 15][_y + 14] = 0;
    playfield[_x + 15][_y + 15] = 0;
  }
  if(_type == 5)
  {
    matrix.drawPixel(_x, _y, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 4, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 5, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 6, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 1, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 2, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 3, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 4, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 5, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 6, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(0, 0, 0));
    playfield[_x + 8][_y + 8] = 0;
    playfield[_x + 8][_y + 9] = 0;
    playfield[_x + 8][_y + 10] = 0;
    playfield[_x + 8][_y + 11] = 0;
    playfield[_x + 8][_y + 12] = 0;
    playfield[_x + 8][_y + 13] = 0;
    playfield[_x + 8][_y + 14] = 0;
    playfield[_x + 8][_y + 15] = 0;
    playfield[_x + 9][_y + 15] = 0;
    playfield[_x + 10][_y + 15] = 0;
    playfield[_x + 11][_y + 15] = 0;
    playfield[_x + 12][_y + 15] = 0;
    playfield[_x + 13][_y + 15] = 0;
    playfield[_x + 14][_y + 15] = 0;
    playfield[_x + 15][_y + 15] = 0;
  }
  if(_type == 6)
  {
    matrix.drawPixel(_x + 7, _y, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 4, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 5, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 6, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 1, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 2, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 3, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 4, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 5, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 6, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(0, 0, 0));
    playfield[_x + 15][_y + 8] = 0;
    playfield[_x + 15][_y + 9] = 0;
    playfield[_x + 15][_y + 10] = 0;
    playfield[_x + 15][_y + 11] = 0;
    playfield[_x + 15][_y + 12] = 0;
    playfield[_x + 15][_y + 13] = 0;
    playfield[_x + 15][_y + 14] = 0;
    playfield[_x + 8][_y + 15] = 0;
    playfield[_x + 9][_y + 15] = 0;
    playfield[_x + 10][_y + 15] = 0;
    playfield[_x + 11][_y + 15] = 0;
    playfield[_x + 12][_y + 15] = 0;
    playfield[_x + 13][_y + 15] = 0;
    playfield[_x + 14][_y + 15] = 0;
    playfield[_x + 15][_y + 15] = 0;
  }
}

// Draw enemy i
void drawEnemies(byte i)
{
  x1 = enemyXScreen[i];
  y1 = enemyYScreen[i];
  x2 = enemyXScreenNew[i];
  y2 = enemyYScreenNew[i];

  // Bird
  if(enemyType[i] == 1)
  {
    // Remove enemy at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 10][y1 + 8] = 0;
      playfield[x1 + 8][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
    }
  
    // Draw enemy at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {   
      if(animationCounter % 16 < 8)
      {
        matrix.drawPixel(x2, y2, matrix.Color333(0, 3, 0));
        matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(0, 0, int((animationCounter % 32) / 4)));
        matrix.drawPixel(x2 + 2, y2, matrix.Color333(0, 3, 0));
        playfield[x2 + 8][y2 + 8] = 2;
        playfield[x2 + 9][y2 + 9] = 2;
        playfield[x2 + 10][y2 + 8] = 2;
      }
      else
      {
        matrix.drawPixel(x2, y2 + 1, matrix.Color333(0, 3, 0));
        matrix.drawPixel(x2 + 1, y2, matrix.Color333(0, 0, int((animationCounter % 32) / 4)));
        matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(0, 3, 0));
        playfield[x2 + 8][y2 + 9] = 2;
        playfield[x2 + 9][y2 + 8] = 2;
        playfield[x2 + 10][y2 + 9] = 2;
      }
    }  
  }

  // Fireball flying left
  if(enemyType[i] == 2)
  {
    // Remove enemy at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 10][y1 + 8] = 0;
      playfield[x1 + 11][y1 + 8] = 0;
    }

    // Draw enemy at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      matrix.drawPixel(x2, y2, matrix.Color333(7, 5, 0));
      if(animationCounter % 2 == 1){ matrix.drawPixel(x2 + 1, y2, matrix.Color333(6, 4, 0)); }
      if(animationCounter % 4 == 3){ matrix.drawPixel(x2 + 2, y2, matrix.Color333(5, 3, 0)); }
      if(animationCounter % 8 == 5){ matrix.drawPixel(x2 + 3, y2, matrix.Color333(4, 2, 0)); }
      playfield[x2 + 8][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 10][y2 + 8] = 2;
      playfield[x2 + 11][y2 + 8] = 0;
    }
  }

  // Fireball flying right
  if(enemyType[i] == 3)
  {
    // Remove enemy at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 10][y1 + 8] = 0;
      playfield[x1 + 11][y1 + 8] = 0;
    }

    // Draw enemy at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      if(animationCounter % 8 == 5){ matrix.drawPixel(x2, y2, matrix.Color333(4, 2, 0)); }
      if(animationCounter % 4 == 3){ matrix.drawPixel(x2 + 1, y2, matrix.Color333(5, 3, 0)); }
      if(animationCounter % 2 == 1){ matrix.drawPixel(x2 + 2, y2, matrix.Color333(6, 4, 0));
      matrix.drawPixel(x2 + 3, y2, matrix.Color333(7, 5, 0)); }
      playfield[x2 + 8][y2 + 8] = 0;
      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 10][y2 + 8] = 2;
      playfield[x2 + 11][y2 + 8] = 2;
    }
  }

  // Ufo
  if(enemyType[i] == 4)
  {
    // Remove enemy at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 1, matrix.Color333(0, 0, 0));
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 10][y1 + 8] = 0;
      playfield[x1 + 8][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
      playfield[x1 + 11][y1 + 9] = 0;
    }

    // Draw enemy at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      matrix.drawPixel(x2 + 1, y2, matrix.Color333(0, 1, 1));
      matrix.drawPixel(x2 + 2, y2, matrix.Color333(0, 1, 1));
      matrix.drawPixel(x2, y2 + 1, matrix.Color333(0, 1, 1));
      matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(0, 3, 3));
      matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(0, 3, 3));
      matrix.drawPixel(x2 + 3, y2 + 1, matrix.Color333(0, 1, 1));
      matrix.drawPixel(x2 + (animationCounter % 16) / 4, y2 + 1, matrix.Color333(3, 3, 0));
      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 10][y2 + 8] = 2;
      playfield[x2 + 8][y2 + 9] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 9] = 2;
      playfield[x2 + 11][y2 + 9] = 2;
    }
  }
  
  // Crab
  if(enemyType[i] == 5)
  {
    // Remove enemy at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 4, y1 + 2, matrix.Color333(0, 0, 0));
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 11][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
      playfield[x1 + 11][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 10] = 0;
      playfield[x1 + 11][y1 + 10] = 0;
    }

    // Draw enemy at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      matrix.drawPixel(x2 + 1, y2, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x2 + 3, y2, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(1, 0, 0));
      matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(5, 0, 0));
      matrix.drawPixel(x2 + 3, y2 + 1, matrix.Color333(1, 0, 0));
      if(animationCounter % 32 < 24){ matrix.drawPixel(x2, y2 + 2, matrix.Color333(3, 0, 0)); }
      else { matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(3, 0, 0)); }
      if(animationCounter % 32 > 8){ matrix.drawPixel(x2 + 4, y2 + 2, matrix.Color333(3, 0, 0)); }
      else { matrix.drawPixel(x2 + 3, y2 + 2, matrix.Color333(3, 0, 0)); }
      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 11][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 9] = 2;
      playfield[x2 + 11][y2 + 9] = 2;
      playfield[x2 + 9][y2 + 10] = 2;
      playfield[x2 + 11][y2 + 10] = 2;
    }
  }

  // Fire
  if(enemyType[i] == 6)
  {
    // Remove enemy at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 2, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 10][y1 + 8] = 0;
      playfield[x1 + 11][y1 + 8] = 0;
      playfield[x1 + 8][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
      playfield[x1 + 11][y1 + 9] = 0;
      playfield[x1 + 8][y1 + 10] = 0;
      playfield[x1 + 9][y1 + 10] = 0;
      playfield[x1 + 10][y1 + 10] = 0;
      playfield[x1 + 11][y1 + 10] = 0;
    }

    // Draw enemy at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      if(animationCounter % 16 == 11){ matrix.drawPixel(x2, y2, matrix.Color333(1, 1, 0)); }
      if(animationCounter % 8 == 3){ matrix.drawPixel(x2 + 1, y2, matrix.Color333(3, 3, 0)); }
      if(animationCounter % 8 == 5){ matrix.drawPixel(x2 + 2, y2, matrix.Color333(3, 3, 0)); }
      if(animationCounter % 16 == 7){ matrix.drawPixel(x2 + 3, y2, matrix.Color333(1, 1, 0)); }
      if(animationCounter % 8 == 5){ matrix.drawPixel(x2, y2 + 1, matrix.Color333(3, 3, 0)); }
      if(animationCounter % 4 == 1){ matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(5, 5, 0)); }
      if(animationCounter % 4 == 3){ matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(5, 5, 0)); }
      if(animationCounter % 8 == 3){ matrix.drawPixel(x2 + 3, y2 + 1, matrix.Color333(3, 3, 0)); }
      if(animationCounter % 4 == 0){ matrix.drawPixel(x2, y2 + 2, matrix.Color333(5, 5, 0)); }
      if(animationCounter % 4 == 1){ matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(7, 7, 0)); }
      if(animationCounter % 4 == 3){ matrix.drawPixel(x2 + 2, y2 + 2, matrix.Color333(7, 7, 0)); }
      if(animationCounter % 4 == 0){ matrix.drawPixel(x2 + 3, y2 + 2, matrix.Color333(5, 5, 0)); }
      playfield[x2 + 8][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 10][y2 + 8] = 2;
      playfield[x2 + 11][y2 + 8] = 2;
      playfield[x2 + 8][y2 + 9] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 9] = 2;
      playfield[x2 + 11][y2 + 9] = 2;
      playfield[x2 + 8][y2 + 10] = 2;
      playfield[x2 + 9][y2 + 10] = 2;
      playfield[x2 + 10][y2 + 10] = 2;
      playfield[x2 + 11][y2 + 10] = 2;
    }
  }

  // Twister  
  if(enemyType[i] == 7)
  {
    // Remove enemy at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 3, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 3, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 8][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 8][y1 + 10] = 0;
      playfield[x1 + 9][y1 + 10] = 0;
      playfield[x1 + 8][y1 + 11] = 0;
      playfield[x1 + 9][y1 + 11] = 0;
    }

    // Draw enemy at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      if(animationCounter % 16 < 8){ matrix.drawPixel(x2, y2, matrix.Color333(0, 0, 7)); }
      else { matrix.drawPixel(x2 + 1, y2, matrix.Color333(0, 0, 7)); }
      if(animationCounter % 16 > 7){ matrix.drawPixel(x2, y2 + 1, matrix.Color333(0, 0, 5)); }
      else { matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(0, 0, 5)); }
      if(animationCounter % 16 > 7){ matrix.drawPixel(x2, y2 + 2, matrix.Color333(0, 0, 3)); }
      else { matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(0, 0, 3)); }
      if(animationCounter % 16 < 8){ matrix.drawPixel(x2, y2 + 3, matrix.Color333(0, 0, 1)); }
      else { matrix.drawPixel(x2 + 1, y2 + 3, matrix.Color333(0, 0, 1)); }
      playfield[x2 + 8][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 8][y2 + 9] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 8][y2 + 10] = 2;
      playfield[x2 + 9][y2 + 10] = 2;
      playfield[x2 + 8][y2 + 11] = 2;
      playfield[x2 + 9][y2 + 11] = 2;
    }
  }

  // Devil
  if(enemyType[i] == 8)
  {
    // Remove enemy at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 2, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 11][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 10] = 0;
      playfield[x1 + 10][y1 + 10] = 0;
    }

    // Draw enemy at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      matrix.drawPixel(x2, y2, matrix.Color333(int((animationCounter % 32) / 4), 0, 0));
      matrix.drawPixel(x2 + 3, y2, matrix.Color333(int((animationCounter % 32) / 4), 0, 0));
      matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(5, 0, 5));
      matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(5, 0, 5));
      matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(1, 0, 1));
      matrix.drawPixel(x2 + 2, y2 + 2, matrix.Color333(1, 0, 1));
      playfield[x2 + 8][y2 + 8] = 2;
      playfield[x2 + 11][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 9] = 2;
      playfield[x2 + 9][y2 + 10] = 2;
      playfield[x2 + 10][y2 + 10] = 2;
      if(playerMovement == 0 && audioOffUntil == 0)
      // Sound: Devil on screen
      {
        if(playerAnimationPhase % 4 < 2 && audioOffUntil == 0) 
        {
          tone(audio, 32 + (animationCounter % 8), 50);
        }
      }
    }
  }

  // Vulcano
  if(enemyType[i] == 9)
  {
    // Remove enemy at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1 + 1, y1 - enemyMovement[i], matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 - enemyMovement[i], matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1 - enemyMovement[i], matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1 - enemyMovement[i], matrix.Color333(0, 0, 0));

      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 1, matrix.Color333(0, 0, 0));

      playfield[x1 + 9][y1 + 8 - enemyMovement[i]] = 0;
      playfield[x1 + 10][y1 + 8 - enemyMovement[i]] = 0;
      playfield[x1 + 9][y1 + 9 - enemyMovement[i]] = 0;
      playfield[x1 + 10][y1 + 9 - enemyMovement[i]] = 0;

      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 10][y1 + 8] = 0;
      playfield[x1 + 8][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
      playfield[x1 + 11][y1 + 9] = 0;
    }

    if(animationCounter % 4 == 0)
    {
      enemyMovement[i]++;
      if(enemyMovement[i] == 12)
      {
        enemyMovement[i] = 0;
      }
      // Sound: Vulcano erupts
      if(enemyMovement[i] == 2)
      {
        tone(audio, 100, 25);
      }
    }

    // Draw enemy at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      if(animationCounter % 2 == 1)
      {
        matrix.drawPixel(x2 + 1, y2 - enemyMovement[i], matrix.Color333(3, 3, 0));
        matrix.drawPixel(x2 + 2, y2 - enemyMovement[i], matrix.Color333(3, 3, 0));
      }
      if(animationCounter % 4 == 3)
      {
        matrix.drawPixel(x2 + 1, y2 + 1 - enemyMovement[i], matrix.Color333(3, 3, 0));
        matrix.drawPixel(x2 + 2, y2 + 1 - enemyMovement[i], matrix.Color333(3, 3, 0));
      }

      matrix.drawPixel(x2 + 1, y2, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2 + 2, y2, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2, y2 + 1, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(1, 0, 0));
      matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(1, 0, 0));
      matrix.drawPixel(x2 + 3, y2 + 1, matrix.Color333(3, 0, 0));

      if((y2 + 8 - enemyMovement[i]) >= 0 && (y + 8 - enemyMovement[i]) <= 47)
      {
        playfield[x2 + 9][y2 + 8 - enemyMovement[i]] = 2;
        playfield[x2 + 10][y2 + 8 - enemyMovement[i]] = 2;
      }
      if((y2 + 9 - enemyMovement[i]) >= 0 && (y2 + 9 - enemyMovement[i]) <= 47)
      {
        playfield[x2 + 9][y2 + 9 - enemyMovement[i]] = 2;
        playfield[x2 + 10][y2 + 9 - enemyMovement[i]] = 2;
      }

      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 10][y2 + 8] = 2;
      playfield[x2 + 8][y2 + 9] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 9] = 2;
      playfield[x2 + 11][y2 + 9] = 2;
    }
  }

  // Hornet
  if(enemyType[i] == 10)
  {
    // Remove enemy at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 1, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 11][y1 + 8] = 0;
      playfield[x1 + 8][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
      playfield[x1 + 11][y1 + 9] = 0;
    }

    // Draw enemy at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      if(animationCounter % 16 < 8)
      {
        matrix.drawPixel(x2, y2, matrix.Color333(3, 3, 0));
        matrix.drawPixel(x2 + 3, y2, matrix.Color333(3, 3, 0));
        playfield[x2 + 8][y2 + 8] = 2;
        playfield[x2 + 11][y2 + 8] = 2;
      }
      else
      {
        matrix.drawPixel(x2, y2 + 1, matrix.Color333(3, 3, 0));
        matrix.drawPixel(x2 + 3, y2 + 1, matrix.Color333(3, 3, 0));
        playfield[x2 + 8][y2 + 9] = 2;
        playfield[x2 + 11][y2 + 9] = 2;
      }
      matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(0, 3, 3));
      matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(0, 3, 3));
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 9] = 2;
    }
  }

  // Laser floor
  if(enemyType[i] == 11)
  {
    // Remove enemy at old position
    if(x1 > -8 && x1 < 32 && y1 > -8 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 4, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 5, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 6, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 7, y1, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 10][y1 + 8] = 0;
      playfield[x1 + 11][y1 + 8] = 0;
      playfield[x1 + 12][y1 + 8] = 0;
      playfield[x1 + 13][y1 + 8] = 0;
      playfield[x1 + 14][y1 + 8] = 0;
      playfield[x1 + 15][y1 + 8] = 0;
    }

    // Draw enemy at new position
    if(x2 > -8 && x2 < 32 && y2 > -8 && y2 < 32)
    {
      if(animationCounter < 64 || (animationCounter > 127 && animationCounter < 192))
      {
        if(animationCounter % 4 == 0)
        {
          matrix.drawPixel(x2, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 1, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 2, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 3, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 4, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 5, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 6, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 7, y2, matrix.Color333(3, 3, 3));
          // Sound: Laser on screen
          tone(audio, 100, 5);
        }
        playfield[x2 + 8][y2 + 8] = 2;
        playfield[x2 + 9][y2 + 8] = 2;
        playfield[x2 + 10][y2 + 8] = 2;
        playfield[x2 + 11][y2 + 8] = 2;
        playfield[x2 + 12][y2 + 8] = 2;
        playfield[x2 + 13][y2 + 8] = 2;
        playfield[x2 + 14][y2 + 8] = 2;
        playfield[x2 + 15][y2 + 8] = 2;
      }
    }
  }
  
  // Laser left
  if(enemyType[i] == 12)
  {
    // Remove enemy at old position
    if(x1 > -8 && x1 < 32 && y1 > -8 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 3, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 4, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 5, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 6, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 8][y1 + 9] = 0;
      playfield[x1 + 8][y1 + 10] = 0;
      playfield[x1 + 8][y1 + 11] = 0;
      playfield[x1 + 8][y1 + 12] = 0;
      playfield[x1 + 8][y1 + 13] = 0;
      playfield[x1 + 8][y1 + 14] = 0;
    }

    // Draw enemy at new position
    if(x2 > -8 && x2 < 32 && y2 > -8 && y2 < 32)
    {
      if(animationCounter < 64 || (animationCounter > 127 && animationCounter < 192))
      {
        if(animationCounter % 4 == 0)
        {
          matrix.drawPixel(x2, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2, y2 + 1, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2, y2 + 2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2, y2 + 3, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2, y2 + 4, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2, y2 + 5, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2, y2 + 6, matrix.Color333(3, 3, 3));
          // Sound: Laser on screen
          tone(audio, 100, 5);
        }
        playfield[x2 + 8][y2 + 8] = 2;
        playfield[x2 + 8][y2 + 9] = 2;
        playfield[x2 + 8][y2 + 10] = 2;
        playfield[x2 + 8][y2 + 11] = 2;
        playfield[x2 + 8][y2 + 12] = 2;
        playfield[x2 + 8][y2 + 13] = 2;
        playfield[x2 + 8][y2 + 14] = 2;
      }
    }
  }
  
  // Laser right
  if(enemyType[i] == 13)
  {
    // Remove enemy at old position
    if(x1 > -8 && x1 < 32 && y1 > -8 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 3, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 4, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 5, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 6, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 8][y1 + 9] = 0;
      playfield[x1 + 8][y1 + 10] = 0;
      playfield[x1 + 8][y1 + 11] = 0;
      playfield[x1 + 8][y1 + 12] = 0;
      playfield[x1 + 8][y1 + 13] = 0;
      playfield[x1 + 8][y1 + 14] = 0;
    }

    // Draw enemy at new position
    if(x2 > -8 && x2 < 32 && y2 > -8 && y2 < 32)
    {
      if(animationCounter < 64 || (animationCounter > 127 && animationCounter < 192))
      {
        if(animationCounter % 4 == 0)
        {
          matrix.drawPixel(x2, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2, y2 + 1, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2, y2 + 2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2, y2 + 3, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2, y2 + 4, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2, y2 + 5, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2, y2 + 6, matrix.Color333(3, 3, 3));
          // Sound: Laser on screen
          tone(audio, 100, 5);
        }
        playfield[x2 + 8][y2 + 8] = 2;
        playfield[x2 + 8][y2 + 9] = 2;
        playfield[x2 + 8][y2 + 10] = 2;
        playfield[x2 + 8][y2 + 11] = 2;
        playfield[x2 + 8][y2 + 12] = 2;
        playfield[x2 + 8][y2 + 13] = 2;
        playfield[x2 + 8][y2 + 14] = 2;
      }
    }
  }
  
  // Vertical moving laser
  if(enemyType[i] == 14)
  {
    // Remove enemy at old position
    if(x1 > -8 && x1 < 32 && y1 > -8 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 4, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 5, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 6, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 7, y1, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 10][y1 + 8] = 0;
      playfield[x1 + 11][y1 + 8] = 0;
      playfield[x1 + 12][y1 + 8] = 0;
      playfield[x1 + 13][y1 + 8] = 0;
      playfield[x1 + 14][y1 + 8] = 0;
      playfield[x1 + 15][y1 + 8] = 0;
    }

    // Draw enemy at new position
    if(x2 > -8 && x2 < 32 && y2 > -8 && y2 < 32)
    {
      matrix.drawPixel(x2, y2, matrix.Color333(3, 0, 3));
      matrix.drawPixel(x2 + 7, y2, matrix.Color333(3, 0, 3));
      playfield[x2 + 8][y2 + 8] = 2;
      playfield[x2 + 15][y2 + 8] = 2;
      if(animationCounter < 64 || (animationCounter > 127 && animationCounter < 192))
      {
        if(animationCounter % 4 == 0)
        {
          matrix.drawPixel(x2 + 1, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 2, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 3, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 4, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 5, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 6, y2, matrix.Color333(3, 3, 3));
          // Sound: Laser on screen
          tone(audio, 100, 5);
        }
        playfield[x2 + 9][y2 + 8] = 2;
        playfield[x2 + 10][y2 + 8] = 2;
        playfield[x2 + 11][y2 + 8] = 2;
        playfield[x2 + 12][y2 + 8] = 2;
        playfield[x2 + 13][y2 + 8] = 2;
        playfield[x2 + 14][y2 + 8] = 2;
      }
    }
  }
  
  // Horizontal moving laser
  if(enemyType[i] == 15)
  {
    // Remove enemy at old position
    if(x1 > -8 && x1 < 32 && y1 > -8 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 3, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 4, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 5, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 6, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 10][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 10] = 0;
      playfield[x1 + 9][y1 + 11] = 0;
      playfield[x1 + 9][y1 + 12] = 0;
      playfield[x1 + 9][y1 + 13] = 0;
      playfield[x1 + 9][y1 + 14] = 0;
    }

    // Draw enemy at new position
    if(x2 > -8 && x2 < 32 && y2 > -8 && y2 < 32)
    {
      matrix.drawPixel(x2, y2, matrix.Color333(3, 0, 3));
      matrix.drawPixel(x2 + 1, y2, matrix.Color333(3, 0, 3));
      matrix.drawPixel(x2 + 2, y2, matrix.Color333(3, 0, 3));
      playfield[x2 + 8][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 10][y2 + 8] = 2;
      if(animationCounter < 64 || (animationCounter > 127 && animationCounter < 192))
      {
        if(animationCounter % 4 == 0)
        {
          matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 1, y2 + 3, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 1, y2 + 4, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 1, y2 + 5, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 1, y2 + 6, matrix.Color333(3, 0, 3));
          // Sound: Laser on screen
          tone(audio, 100, 5);
        }
        playfield[x2 + 9][y2 + 9] = 2;
        playfield[x2 + 9][y2 + 10] = 2;
        playfield[x2 + 9][y2 + 11] = 2;
        playfield[x2 + 9][y2 + 12] = 2;
        playfield[x2 + 9][y2 + 13] = 2;
        playfield[x2 + 9][y2 + 14] = 2;
      }
    }
  }
  
  // Comet
  if(enemyType[i] == 16)
  {
    // Remove enemy at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 10][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
    }

    // Draw enemy at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      if(animationCounter % 4 == 3)
      {
        matrix.drawPixel(x2 + 1, y2, matrix.Color333(3, 3, 0));
        matrix.drawPixel(x2 + 2, y2, matrix.Color333(3, 3, 0));
      }
      if(animationCounter % 2 == 1)
      {
        matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(3, 3, 0));
        matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(3, 3, 0));
      }
      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 10][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 9] = 2;
    }
  }
  
  enemyXScreen[i] = x2;
  enemyYScreen[i] = y2;
}

// Move all active enemies
void moveEnemies()
{
  for(i = 0; i < 16; i++)
  {
    if(enemyStatus[i] > 0)
    {
      // Bird
      if(enemyType[i] == 1)
      {  
        if(enemyMovement[i] == 1)
        {
          if(enemyXMap[i] < (mapWidth - 2) && enemyYMap[i] > 0)
          {
            enemyXMap[i] = enemyXMap[i] + 0.25;
            enemyYMap[i] = enemyYMap[i] - 0.25;
          }
          else
          {
            enemyMovement[i] = byte(random(4)) + 1;
          }
        }
        else if(enemyMovement[i] == 2)
        {
          if(enemyXMap[i] < (mapWidth - 2) && enemyYMap[i] < (mapHeight - 1))
          {
            enemyXMap[i] = enemyXMap[i] + 0.25;
            enemyYMap[i] = enemyYMap[i] + 0.25;
          }
          else
          {
            enemyMovement[i] = byte(random(4)) + 1;
          }
        }
        else if(enemyMovement[i] == 3)
        {
          if(enemyXMap[i] > 0 && enemyYMap[i] < (mapHeight - 1))
          {
            enemyXMap[i] = enemyXMap[i] - 0.25;
            enemyYMap[i] = enemyYMap[i] + 0.25;
          }
          else
          {
            enemyMovement[i] = byte(random(4)) + 1;
          }
        }
        else if(enemyMovement[i] == 4)
        {
          if(enemyXMap[i] > 0 && enemyYMap[i] > 0)
          {
            enemyXMap[i] = enemyXMap[i] - 0.25;
            enemyYMap[i] = enemyYMap[i] - 0.25;
          }
          else
          {
            enemyMovement[i] = byte(random(4)) + 1;
          }
        }
      }
        
      // Fireball flying left
      if(enemyType[i] == 2)
      { 
        if(enemyXMap[i] > -4)
        {
          enemyXMap[i] = enemyXMap[i] - 0.5;
        }
        else
        {
          enemyXMap[i] = mapWidth - 1;
        }      
      }

      // Fireball flying right
      if(enemyType[i] == 3)
      { 
        if(enemyXMap[i] < mapWidth)
        {
          enemyXMap[i] = enemyXMap[i] + 0.5;
        }
        else
        {
          enemyXMap[i] = -3;
        }      
      }
      
      // Ufo
      if(enemyType[i] == 4)
      { 
        // Moving up
        if(enemyMovement[i] == 0)
        {
          if(enemyYMap[i] > enemyY1[i])
          {
            enemyYMap[i] = enemyYMap[i] - 0.25;
          }
          else
          {
            enemyMovement[i] = 1;
          }
        }
        // Moving down
        else
        {
          if(enemyYMap[i] < enemyY2[i])
          {
            enemyYMap[i] = enemyYMap[i] + 0.25;
          }
          else
          {
            enemyMovement[i] = 0;
          }
        }  
      }    

      // Crab
      if(enemyType[i] == 5)
      { 
        // Moving left
        if(enemyMovement[i] == 0)
        {
          if(enemyXMap[i] > enemyX1[i])
          {
            enemyXMap[i] = enemyXMap[i] - 0.15;
          }
          else
          {
            enemyMovement[i] = 1;
          }
        }
        else
        {
          if(enemyXMap[i] < enemyX2[i])
          {
            enemyXMap[i] = enemyXMap[i] + 0.15;
          }
          else
          {
            enemyMovement[i] = 0;
          }
        }      
      }

      // Fire
      if(enemyType[i] == 6)
      {
        // Do nothing
      } 

      // Twister
      if(enemyType[i] == 7)
      {
        // Moving down
        if(enemyMovement[i] > 7)
        {
          enemyYMap[i]++;
          enemyMovement[i] = enemyMovement[i] + 2;
        }
        // Moving up
        if(enemyMovement[i] > 1 && enemyMovement[i] < 8)
        {
          enemyYMap[i]--;
          enemyMovement[i] = enemyMovement[i] + 2;
        }
        // Jump ends
        if(enemyMovement[i] > 13)
        {
          enemyMovement[i] = enemyMovement[i] - 14;
        }
        // Moving left
        if(enemyMovement[i] % 2 == 0)
        {
          if(enemyXMap[i] > enemyX1[i])
          {
            enemyXMap[i] = enemyXMap[i] - 0.4;
          }
          else
          {
            enemyMovement[i] = enemyMovement[i] + 1;
          }
        }
        else
        {
          if(enemyXMap[i] < enemyX2[i])
          {
            enemyXMap[i] = enemyXMap[i] + 0.4;
          }
          else
          {
            enemyMovement[i] = enemyMovement[i] - 1;
          }
        }
        // Start a jump
        if(enemyMovement[i] < 2 && random(100) < 5)
        {
          enemyMovement[i] = enemyMovement[i] + 2;
        }
      } 

      // Devil
      if(enemyType[i] == 8)
      {
        // Move (turns towards player´s ship)
        // Calculate directional vector of the shot
        x = (double(playerXMap + 1.0) - (enemyXMap[i] + 2.0));
        y = (double(playerYMap + 2.0) - (enemyYMap[i] + 1.5));
        // Calculate length of directional vector to normalize the shot speed
        z = sqrt(sq(x) + sq(y));
        enemyXMap[i] = enemyXMap[i] + ((x / z) / 10.0);
        enemyYMap[i] = enemyYMap[i] + ((y / z) / 10.0);
        if(enemyXMap[i] < 0)
        {
          enemyXMap[i] = 0;
        }
        if(enemyXMap[i] > (mapWidth - 4))
        {
          enemyXMap[i] = (mapWidth - 4);
        }
        if(enemyYMap[i] < 0)
        {
          enemyYMap[i] = 0;
        }
        if(enemyYMap[i] > (mapHeight - 4))
        {
          enemyYMap[i] = (mapHeight - 4);
        }
      } 

      // Volcano
      if(enemyType[i] == 9)
      {
        // Do nothing
      } 

      // Hornet
      if(enemyType[i] == 10)
      {
        // Moving left
        if(enemyMovement[i] == 0)
        {
          if(enemyXMap[i] > enemyX1[i])
          {
            enemyXMap[i] = enemyXMap[i] - 0.75;
          }
          else
          {
            enemyMovement[i] = 1;
          }
        }
        else
        {
          if(enemyXMap[i] < enemyX2[i])
          {
            enemyXMap[i] = enemyXMap[i] + 0.75;
          }
          else
          {
            enemyMovement[i] = 0;
          }
        }      
      } 

      // Laser floor
      if(enemyType[i] == 11)
      {
        // Do nothing
      } 

      // Laser left
      if(enemyType[i] == 12)
      {
        // Do nothing
      } 

      // Laser right
      if(enemyType[i] == 13)
      {
        // Do nothing
      } 

      // Vertical moving laser
      if(enemyType[i] == 14)
      {
        // Moving up
        if(enemyMovement[i] == 0)
        {
          if(enemyYMap[i] > enemyY1[i])
          {
            enemyYMap[i] = enemyYMap[i] - 0.25;
          }
          else
          {
            enemyMovement[i] = 1;
          }
        }
        // Moving down
        else
        {
          if(enemyYMap[i] < enemyY2[i])
          {
            enemyYMap[i] = enemyYMap[i] + 0.25;
          }
          else
          {
            enemyMovement[i] = 0;
          }
        }  
      } 

      // Horizontal moving laser
      if(enemyType[i] == 15)
      {
        // Moving left
        if(enemyMovement[i] == 0)
        {
          if(enemyXMap[i] > enemyX1[i])
          {
            enemyXMap[i] = enemyXMap[i] - 0.25;
          }
          else
          {
            enemyMovement[i] = 1;
          }
        }
        else
        {
          if(enemyXMap[i] < enemyX2[i])
          {
            enemyXMap[i] = enemyXMap[i] + 0.25;
          }
          else
          {
            enemyMovement[i] = 0;
          }
        }      
      } 

      // Comet
      if(enemyType[i] == 16)
      {
        if(random(100) < 50)
        {
          if(enemyXMap[i] > -1)
          {
            enemyXMap[i] = enemyXMap[i] - double(random(5)) / 10;
          }
        }
        else
        {
          if(enemyXMap[i] < mapWidth - 1)
          {
            enemyXMap[i] = enemyXMap[i] + double(random(5)) / 10;
          }
        }
        if(enemyYMap[i] < mapHeight)
        {
          enemyYMap[i] = enemyYMap[i] + 0.5;
        }        
        else
        {
          enemyYMap[i] = -1;
        }      
      }

      enemyXScreenNew[i] = enemyXMap[i] - screenXNew;
      enemyYScreenNew[i] = enemyYMap[i] - screenYNew;
      drawEnemies(i);
    }
  }  
}

// Initialize enemy movement (for those enemies moving between two points)
void initializeEnemyMovement()
{
  for(i = 0; i < 16; i++)
  {
    if(enemyStatus[i] > 0)
    {
      // Bird
      if(enemyType[i] == 1)
      {  
        // Nothing to do
      }
      
      // Fireball flying left
      if(enemyType[i] == 2)
      {  
        // Nothing to do
      }
      
      // Fireball flying right
      if(enemyType[i] == 3)
      {  
        // Nothing to do
      }
      
      // Ufo
      if(enemyType[i] == 4)
      {
        j = enemyX1[i];
        k = enemyY1[i];
        while(k > 0 && levelMap[(tileNumberX * k) + j] == 0)
        {
          k--;
        }
        if(k == 0)
        {
          enemyY1[i] = k * 8;
        }
        else
        {
          enemyY1[i] = (k + 1) * 8;
        }
        j = enemyX2[i];
        k = enemyY2[i];
        while(k < tileNumberY && levelMap[(tileNumberX * k) + j] == 0)
        {
          k++;
        }
        if(levelMap[(tileNumberX * k) + j] == 1)
        {
          enemyY2[i] = (k * 8) + 5;
        }
        else if(levelMap[(tileNumberX * k) + j] == 2)
        {
          enemyY2[i] = (k * 8) + 1;
        }
        else
        {
          enemyY2[i] = ((k - 1) * 8) + 6;
        }
      }
      
      // Crab
      if(enemyType[i] == 5)
      {  
        j = enemyX1[i];
        k = enemyY1[i];
        while(j > 0 && levelMap[(tileNumberX * k) + j] == 1)
        {
          j--;
        }
        if(j == 0 && levelMap[(tileNumberX * k) + j] == 1)
        {
          enemyX1[i] = (j * 8) + 1;
        }
        else
        {
          enemyX1[i] = ((j + 1) * 8) + 1;
        }
        j = enemyX2[i];
        k = enemyY2[i];
        while(j < tileNumberX && levelMap[(tileNumberX * k) + j] == 1)
        {
          j++;
        }
        enemyX2[i] = ((j - 1) * 8) + 3;
      }
      
      // Fire
      if(enemyType[i] == 6)
      {  
        // Nothing to do
      }
      
      // Twister
      if(enemyType[i] == 7)
      {  
        j = enemyX1[i];
        k = enemyY1[i];
        while(j > 0 && levelMap[(tileNumberX * k) + j] == 1)
        {
          j--;
        }
        if(j == 0 && levelMap[(tileNumberX * k) + j] == 1)
        {
          enemyX1[i] = j * 8;
        }
        else
        {
          enemyX1[i] = (j + 1) * 8;
        }
        j = enemyX2[i];
        k = enemyY2[i];
        while(j < tileNumberX && levelMap[(tileNumberX * k) + j] == 1)
        {
          j++;
        }
        if(j == 1)
        {
          enemyX2[i] = (j * 8) + 6;
        }
        else
        {
          enemyX2[i] = ((j - 1) * 8) + 6;
        }
      }
      
      // Devil
      if(enemyType[i] == 8)
      {  
        // Nothing to do
      }
      
      // Vulcano
      if(enemyType[i] == 9)
      {  
      }
      
      // Hornet
      if(enemyType[i] == 10)
      {  
        j = enemyX1[i];
        k = enemyY1[i];
        while(j > 0 && (levelMap[(tileNumberX * k) + j] == 0 || levelMap[(tileNumberX * k) + j] == 1))
        {
          j--;
        }
        if(j == 0 && (levelMap[(tileNumberX * k) + j] == 0 || levelMap[(tileNumberX * k) + j] == 1))
        {
          enemyX1[i] = j * 8;
        }
        else
        {
          enemyX1[i] = (j + 1) * 8;
        }
        j = enemyX2[i];
        k = enemyY2[i];
        while(j < tileNumberX && (levelMap[(tileNumberX * k) + j] == 0 || levelMap[(tileNumberX * k) + j] == 1))
        {
          j++;
        }
        if(levelMap[(tileNumberX * k) + j] == 4)
        {
          enemyX2[i] = (j * 8) + 3;
        }
        else
        {
          enemyX2[i] = ((j - 1) * 8) + 4;
        }
      }
      
      // Vertical moving laser
      if(enemyType[i] == 14)
      {  
        j = enemyX1[i];
        k = enemyY1[i];
        while(k > 0 && levelMap[(tileNumberX * k) + j] == 0)
        {
          k--;
        }
        if(levelMap[(tileNumberX * k) + j] == 0)
        {
          enemyY1[i] = k * 8;
        }
        else
        {
          enemyY1[i] = (k + 1) * 8;
        }
        j = enemyX2[i];
        k = enemyY2[i];
        while(k < tileNumberY && levelMap[(tileNumberX * k) + j] == 0)
        {
          k++;
        }
        if(levelMap[(tileNumberX * k) + j] == 1)
        {
          enemyY2[i] = (k * 8) + 6;
        }
        else
        {
          enemyY2[i] = ((k - 1) * 8) + 7;
        }
      }      
      
      // Horizontal moving laser
      if(enemyType[i] == 15)
      {  
        j = enemyX1[i];
        k = enemyY1[i];
        while(j > 0 && levelMap[(tileNumberX * k) + j] == 1)
        {
          j--;
        }
        if(j == 0 && levelMap[(tileNumberX * k) + j] == 1)
        {
          enemyX1[i] = j * 8;
        }
        else if(levelMap[(tileNumberX * k) + j] == 5)
        {
          enemyX1[i] = (j * 8) + 1;
        }
        else
        {
          enemyX1[i] = (j + 1) * 8;
        }
        j = enemyX2[i];
        k = enemyY2[i];
        while(j < tileNumberX && levelMap[(tileNumberX * k) + j] == 1)
        {
          j++;
        }
        if(levelMap[(tileNumberX * k) + j] == 6)
        {
          enemyX2[i] = (j * 8) + 5;
        }
        else
        {
          enemyX2[i] = ((j - 1) * 8) + 6;
        }
      }      
      
    }
  }
}

// Draw all not collected gems
void drawGems()
{
  for(i = 0; i < 16; i++)
  {
    if(gemStatus[i] > 0)
    {
      gemXScreenNew[i] = gemXMap[i] - screenXNew;
      gemYScreenNew[i] = gemYMap[i] - screenYNew;

      if(gemXScreen[i] > -9 && gemXScreen[i] < 39 && gemYScreen[i] > -9 && gemYScreen[i] < 39)
      {     
        x1 = gemXScreen[i];
        y1 = gemYScreen[i];
        x2 = gemXScreenNew[i];
        y2 = gemYScreenNew[i];
                
        // Remove gems at old position
        matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
                
        playfield[x1 + 8][y1 + 8] = 0;
        playfield[x1 + 9][y1 + 8] = 0;
        playfield[x1 + 8][y1 + 9] = 0;
        playfield[x1 + 9][y1 + 9] = 0;
        
        if(gemStatus[i] == 3)
        {
          gemStatus[i] = 0;
          gemCounter--;
        }

        // Draw gems at new position
        if(gemStatus[i] == 1 || gemStatus[i] == 2)
        {
          if(gemXScreenNew[i] > -9 && gemXScreenNew[i] < 39 && gemYScreenNew[i] > -9 && gemYScreenNew[i] < 39)
          {
            if(gemStatus[i] == 1)
            {
              // Normal gem
              if(gemsToFinishLevel > 1)
              {
                matrix.drawPixel(x2, y2, matrix.Color333(1, 1, 1));
                matrix.drawPixel(x2, y2 + 1, matrix.Color333(1, 1, 1));
                matrix.drawPixel(x2 + 1, y2, matrix.Color333(1, 1, 1));
                matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(1, 1, 1));
                if(animationCounter % 8 < 2) { matrix.drawPixel(x2, y2, matrix.Color333(5, 5, 5)); }
                else if(animationCounter % 8 < 4) { matrix.drawPixel(x2, y2 + 1, matrix.Color333(5, 5, 5)); }
                else if(animationCounter % 8 < 6) { matrix.drawPixel(x2 + 1, y2, matrix.Color333(5, 5, 5)); }
                else if(animationCounter % 8 < 8) { matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(5, 5, 5)); }
              }
              else
              {
                matrix.drawPixel(x2, y2, matrix.Color333(random(2), random(2), random(2)));
                matrix.drawPixel(x2, y2 + 1, matrix.Color333(random(2), random(2), random(2)));
                matrix.drawPixel(x2 + 1, y2, matrix.Color333(random(2), random(2), random(2)));
                matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(random(2), random(2), random(2)));
                if(animationCounter % 8 < 2) { matrix.drawPixel(x2, y2, matrix.Color333(random(2) * 5, random(2) * 5, random(2) * 5)); }
                else if(animationCounter % 8 < 4) { matrix.drawPixel(x2, y2 + 1, matrix.Color333(random(2) * 5, random(2) * 5, random(2) * 5)); }
                else if(animationCounter % 8 < 6) { matrix.drawPixel(x2 + 1, y2, matrix.Color333(random(2) * 5, random(2) * 5, random(2) * 5)); }
                else if(animationCounter % 8 < 8) { matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(random(2) * 5, random(2) * 5, random(2) * 5)); }
              }
            }
            else
            {
              // Extralife gem
              matrix.drawPixel(x2, y2, matrix.Color333(1, 0, 0));
              matrix.drawPixel(x2, y2 + 1, matrix.Color333(1, 0, 0));
              matrix.drawPixel(x2 + 1, y2, matrix.Color333(1, 0, 0));
              matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(1, 0, 0));
              if(animationCounter % 8 < 2) { matrix.drawPixel(x2, y2, matrix.Color333(5, 0, 0)); }
              else if(animationCounter % 8 < 4) { matrix.drawPixel(x2, y2 + 1, matrix.Color333(5, 0, 0)); }
              else if(animationCounter % 8 < 6) { matrix.drawPixel(x2 + 1, y2, matrix.Color333(5, 0, 0)); }
              else if(animationCounter % 8 < 8) { matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(5, 0, 0)); }
            }
            playfield[x2 + 8][y2 + 8] = i + 3;
            playfield[x2 + 9][y2 + 8] = i + 3;
            playfield[x2 + 8][y2 + 9] = i + 3;
            playfield[x2 + 9][y2 + 9] = i + 3;
          }
        }
      }  
      gemXScreen[i] = gemXScreenNew[i];
      gemYScreen[i] = gemYScreenNew[i];
    }
  }  
}

// Draw player
void drawPlayer()
{
  // Remove player at old position
  if(playerDirection == 0)
  {
    matrix.drawPixel(playerXScreen, playerYScreen, matrix.Color333(0, 0, 0));
    matrix.drawPixel(playerXScreen + 1, playerYScreen, matrix.Color333(0, 0, 0));
    matrix.drawPixel(playerXScreen, playerYScreen + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(playerXScreen + 1, playerYScreen + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(playerXScreen, playerYScreen + 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(playerXScreen + 1, playerYScreen + 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(playerXScreen, playerYScreen + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(playerXScreen + 1, playerYScreen + 3, matrix.Color333(0, 0, 0));
  }
  else
  {
    matrix.drawPixel(playerXScreen + 1, playerYScreen, matrix.Color333(0, 0, 0));
    matrix.drawPixel(playerXScreen, playerYScreen, matrix.Color333(0, 0, 0));
    matrix.drawPixel(playerXScreen + 1, playerYScreen + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(playerXScreen, playerYScreen + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(playerXScreen + 1, playerYScreen + 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(playerXScreen, playerYScreen + 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(playerXScreen + 1, playerYScreen + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(playerXScreen, playerYScreen + 3, matrix.Color333(0, 0, 0));
  }
  
  // Draw player at new position
  if(playerDirectionNew == 0)
  {
    if(playerMovement == 3)
    {
      matrix.drawPixel(playerXScreenNew + 1, playerYScreenNew + 2, matrix.Color333(5, 2, 0));
      matrix.drawPixel(playerXScreenNew, playerYScreenNew + 2, matrix.Color333(3, 0, 0));
      matrix.drawPixel(playerXScreenNew + 1, playerYScreenNew + 3, matrix.Color333(5, 0, 0));
      matrix.drawPixel(playerXScreenNew, playerYScreenNew + 3, matrix.Color333(0, 3, 0));
    }
    else
    {
      matrix.drawPixel(playerXScreenNew, playerYScreenNew, matrix.Color333(5, 2, 0));
      matrix.drawPixel(playerXScreenNew, playerYScreenNew + 1, matrix.Color333(3, 0, 0));
      if(playerMovement == 0 && playerAnimationPhase % 4 < 2)
      {
        matrix.drawPixel(playerXScreenNew + 1, playerYScreenNew + 1, matrix.Color333(5, 0, 0));
      }
      else if(playerMovement == 1)
      {
        matrix.drawPixel(playerXScreenNew + 1, playerYScreenNew, matrix.Color333(5, 0, 0));
      }
      else if(playerMovement == 2)
      {
        matrix.drawPixel(playerXScreenNew + 1, playerYScreenNew + 2, matrix.Color333(5, 0, 0));
      }
      matrix.drawPixel(playerXScreenNew, playerYScreenNew + 2, matrix.Color333(3, 0, 0));
      if(playerAnimationPhase % 4 < 2)
      {
        matrix.drawPixel(playerXScreenNew, playerYScreenNew + 3, matrix.Color333(0, 3, 0));
      }
      else
      {
        matrix.drawPixel(playerXScreenNew + 1, playerYScreenNew + 3, matrix.Color333(0, 3, 0));
      }
    }
  }
  else
  {
    if(playerMovement == 3)
    {
      matrix.drawPixel(playerXScreenNew, playerYScreenNew + 2, matrix.Color333(5, 2, 0));
      matrix.drawPixel(playerXScreenNew + 1, playerYScreenNew + 2, matrix.Color333(3, 0, 0));
      matrix.drawPixel(playerXScreenNew, playerYScreenNew + 3, matrix.Color333(5, 0, 0));
      matrix.drawPixel(playerXScreenNew + 1, playerYScreenNew + 3, matrix.Color333(0, 3, 0));
    }
    else
    {
      matrix.drawPixel(playerXScreenNew + 1, playerYScreenNew, matrix.Color333(5, 2, 0));
      matrix.drawPixel(playerXScreenNew + 1, playerYScreenNew + 1, matrix.Color333(3, 0, 0));
      if(playerMovement == 0 && playerAnimationPhase % 4 < 2)
      {
        matrix.drawPixel(playerXScreenNew, playerYScreenNew + 1, matrix.Color333(5, 0, 0));
      }
      else if(playerMovement == 1)
      {
        matrix.drawPixel(playerXScreenNew, playerYScreenNew, matrix.Color333(5, 0, 0));
      }
      else if(playerMovement == 2)
      {
        matrix.drawPixel(playerXScreenNew, playerYScreenNew + 2, matrix.Color333(5, 0, 0));
      }
      matrix.drawPixel(playerXScreenNew + 1, playerYScreenNew + 2, matrix.Color333(3, 0, 0));
      if(playerAnimationPhase % 4 < 2)
      {
        matrix.drawPixel(playerXScreenNew + 1, playerYScreenNew + 3, matrix.Color333(0, 3, 0));
      }
      else
      {
        matrix.drawPixel(playerXScreenNew, playerYScreenNew + 3, matrix.Color333(0, 3, 0));
      }
    }
  }
}

// Check whether movement to the left is possible
boolean directionLeftFree()
{
  if(playfield[playerXScreenNew + 8][playerYScreenNew + 8] == 1){ return false; }
  if(playfield[playerXScreenNew + 7][playerYScreenNew + 9] == 1){ return false; }
  if(playfield[playerXScreenNew + 8][playerYScreenNew + 10] == 1){ return false; }
  if(playfield[playerXScreenNew + 8][playerYScreenNew + 11] == 1){ return false; }
  return true;
}

// Check whether movement to the right is possible
boolean directionRightFree()
{
  if(playfield[playerXScreenNew + 9][playerYScreenNew + 8] == 1){ return false; }
  if(playfield[playerXScreenNew + 10][playerYScreenNew + 9] == 1){ return false; }
  if(playfield[playerXScreenNew + 9][playerYScreenNew + 10] == 1){ return false; }
  if(playfield[playerXScreenNew + 9][playerYScreenNew + 11] == 1){ return false; }
  return true;
}

// Check whether proceeding of jump is possible
boolean directionUpFree()
{
  if(playerDirection == 0)
  {
    if(playfield[playerXScreenNew + 8][playerYScreenNew + 7] == 1){ return false; }
  }
  else
  {
    if(playfield[playerXScreenNew + 9][playerYScreenNew + 7] == 1){ return false; }
  }
  return true;
}

// Check whether proceeding of falling is possible
boolean directionDownFree()
{
  if(playerDirection == 0)
  {
    if(playfield[playerXScreen + 8][playerYScreen + 12] == 1 || playfield[playerXScreenNew + 8][playerYScreenNew + 12] == 1){ return false; }
  }
  else
  {
    if(playfield[playerXScreen + 9][playerYScreen + 12] == 1 || playfield[playerXScreenNew + 9][playerYScreenNew + 12] == 1){ return false; }
  }
  return true;
}

// Moves player (checks also the joystick)
void movePlayer()
{
  // Moving to the left side
  if(playerMovement != 3 && joy1Left() && int(playerXMap) > 0)
  {
    playerDirectionNew = 1;      
    if(directionLeftFree())
    {
      if(playerMovement == 0)
      {
        playerAnimationPhase++;
      }
      else
      {
        playerAnimationPhase = 0;
      }
      // Sound: Steps
      if(playerMovement == 0 && audioOffUntil == 0)
      {
        if(playerAnimationPhase % 4 < 2) 
        {
          tone(audio, 1024, 10);
        }
        else
        {
          tone(audio, 512, 10);
        }  
      }
      playerXMapNew = playerXMap - 1;
      if(screenX > 0 && playerXScreen < 16)
      {
        screenXNew = screenX - 1;
        redraw = true;
      }
      else
      {
        playerXScreenNew = playerXScreen - 1;
      }
    }
  }
  // Moving to the right side
  else if(playerMovement != 3 && joy1Right() && int(playerXMap) < (mapWidth - 2))
  {
    playerDirectionNew = 0;
    if(directionRightFree())
    {
      if(playerMovement == 0)
      {
        playerAnimationPhase++;
      }
      else
      {
        playerAnimationPhase = 0;
      }
      // Sound: Steps
      if(playerMovement == 0 && audioOffUntil == 0)
      {
        if(playerAnimationPhase % 4 < 2) 
        {
          tone(audio, 1024, 10);
        }
        else
        {
          tone(audio, 512, 10);
        }  
      }
      playerXMapNew = playerXMap + 1;
      if(screenX < (mapWidth - 32) && playerXScreen > 14)
      {
        screenXNew = screenX + 1;
        redraw = true;
      }
      else
      {
        playerXScreenNew = playerXScreen + 1;
      }
    }
  }
  else
  {
    if(playerAnimationPhase % 4 > 1)
    {
      playerAnimationPhase = 0;
      redraw = true;
    }
  }
    
  // Falling
  if(playerMovement == 1)
  {
    playerAnimationPhase = 0;
  }
  if(playerMovement == 0)
  {
    // Check if player is on a surface if it is not flying upwards. If not, falling starts
    if(directionDownFree() && int(playerYMap) < (mapHeight - 4)) // DOWN
    {
      playerMovement = 2;
      playerYVector = 0;
    }
  }
  if(playerMovement == 2)
  {
    // Sound: Falling
    if(audioOffUntil == 0)
    {
      tone(audio, 1024 + 64 * (20 - playerYVector), 50);
    }
    if(directionDownFree() && int(playerYMap) < (mapHeight - 4))
    {
      if(playerYVector < 32)
      {
        playerYVector++;
      }
      playerYMapNew = playerYMap + 1;
      if(screenY < (mapHeight - 32) && playerYScreen > 13)
      {      
        screenYNew = screenY + 1;
        redraw = true;
      }
      else
      {
        playerYScreenNew = playerYScreen + 1;
      }
    }
    else
    {
      playerMovement = 0;
      redraw = true;
    }
  }

  // Start a jump
  if(playerMovement == 0 && joy1Fire() && int(playerYMap) > 0) // UP
  {
    playerMovement = 1;
    playerYVector = 19;
  }
    
  // Flying upwards after jump
  if(playerMovement == 1)
  {
    // Sound: Jump
    if(audioOffUntil == 0)
    {
      tone(audio, 1024 + 64 * (20 - playerYVector), 50);
    }
    if(int(playerYMap) > 0 && directionUpFree())
    {
      // Higher jumps if fire button is pressed
      if(joy1Fire())
      {
        playerYVector = playerYVector - 1;
      }
      else
      {
        playerYVector = playerYVector - 5;
      }
      playerYMapNew = playerYMap - 1;
      if(screenY > 0 && playerYScreen < 15)
      {
        screenYNew = screenY - 1;
        redraw = true;
      }
      else
      {
        playerYScreenNew = playerYScreen - 1;
      }

      if(playerYVector < 1)
      {
        playerMovement = 2;
      }
    }
    else
    {
      playerMovement = 2;
    }
  }
    
  // Duck
  if(playerMovement == 0 && joy1Down())
  {
    // Sound: Jump
    if(audioOffUntil == 0)
    {
      tone(audio, 220, 50);
    }
    playerMovement = 3;
    redraw = true;
  }
  else if(playerMovement == 3)
  {
    if(!joy1Down())
    {
      playerMovement = 0;
      redraw = true;
    }
  }

  // Redraw player if position has changed or animation is incomplete   
  if(redraw || ((playerXScreen != playerXScreenNew) || (playerYScreen != playerYScreenNew) || (playerXMap != playerXMapNew) || (playerYMap != playerYMapNew) || playerDirection != playerDirectionNew))
  {
    drawPlayer();  
    playerXScreen = playerXScreenNew;
    playerYScreen = playerYScreenNew;
    playerXMap = playerXMapNew;
    playerYMap = playerYMapNew;
    playerDirection = playerDirectionNew;
  }
}

// Checks whether new player position collides with enemy or gem
byte collisionDetection()
{
  if(playerMovement == 3)
  {
    if(playfield[playerXScreenNew + 8][playerYScreenNew + 10] > 1){ return playfield[playerXScreenNew + 8][playerYScreenNew + 10]; }
    if(playfield[playerXScreenNew + 9][playerYScreenNew + 10] > 1){ return playfield[playerXScreenNew + 9][playerYScreenNew + 10]; }
    if(playfield[playerXScreenNew + 8][playerYScreenNew + 11] > 1){ return playfield[playerXScreenNew + 8][playerYScreenNew + 11]; }
    if(playfield[playerXScreenNew + 9][playerYScreenNew + 11] > 1){ return playfield[playerXScreenNew + 9][playerYScreenNew + 11]; }
  }
  else
  {
    if(playerDirection == 0)
    {
      if(playfield[playerXScreenNew + 8][playerYScreenNew + 8] > 1){ return playfield[playerXScreenNew + 8][playerYScreenNew + 8]; }
      if(playfield[playerXScreenNew + 8][playerYScreenNew + 9] > 1){ return playfield[playerXScreenNew + 8][playerYScreenNew + 9]; }
      if(playfield[playerXScreenNew + 9][playerYScreenNew + 9] > 1){ return playfield[playerXScreenNew + 9][playerYScreenNew + 9]; }
      if(playfield[playerXScreenNew + 8][playerYScreenNew + 10] > 1){ return playfield[playerXScreenNew + 8][playerYScreenNew + 10]; }
      if(playfield[playerXScreenNew + 8][playerYScreenNew + 11] > 1){ return playfield[playerXScreenNew + 8][playerYScreenNew + 11]; }
    }
    else
    {
      if(playfield[playerXScreenNew + 9][playerYScreenNew + 8] > 1){ return playfield[playerXScreenNew + 9][playerYScreenNew + 8]; }
      if(playfield[playerXScreenNew + 9][playerYScreenNew + 9] > 1){ return playfield[playerXScreenNew + 9][playerYScreenNew + 9]; }
      if(playfield[playerXScreenNew + 8][playerYScreenNew + 9] > 1){ return playfield[playerXScreenNew + 8][playerYScreenNew + 9]; }
      if(playfield[playerXScreenNew + 9][playerYScreenNew + 10] > 1){ return playfield[playerXScreenNew + 9][playerYScreenNew + 10]; }
      if(playfield[playerXScreenNew + 9][playerYScreenNew + 11] > 1){ return playfield[playerXScreenNew + 9][playerYScreenNew + 11]; }
    }
  }
  return 0;  
}

// Check if collision with enemy is true
boolean checkForLosingLive()
{
  // Collision with enemy
  if(collisionDetection() == 2)
  {
    lives--;
    return true;
  }
  // Player falls out of screen
  if(playerYMap == mapHeight - 4)
  {
    lives--;
    return true;
  }
  return false;  
}

// Check if collision with gem is true (gem collected)
void checkForGems()
{
  i = collisionDetection();
  if(i > 2)
  {
    // Normal gem collected
    if(gemStatus[i - 3] == 1)
    {
      gems++;
      gemsToFinishLevel--;
      // Sound: Gem collected
      tone(audio, 440, 200);
    }
    // Extralife gem collected
    else if(gemStatus[i - 3] == 2)
    {
      if(lives < 9)
      {
        lives++;
      }
      // Sound: Gem collected
      tone(audio, 220, 400);
    }
    gemStatus[i - 3] = 3;
    // Blocks all movement sounds
    audioOffUntil = animationCounter + 4;
    if(audioOffUntil == 0)
    {
      audioOffUntil = 1;
    }
  }
}

// Show status screen
void showStatus()
{
  changed = true;
  do
  {
    if(changed)
    {
      // Frame
      matrix.setTextColor(matrix.Color333(0,3,0));
      // Game ends because of no more lives or last level completed (in this case a full screen frame is drawn)
      if(lives < 1 || level == numberOfLevels + 1)
      {
        matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
        matrix.drawRect(0, 0, 32, 32, matrix.Color333(1,1,1));
      }
      // Status after loosing live
      else
      {
        matrix.fillRect(2, 6, 28, 19, matrix.Color333(0,0,0));
        matrix.drawRect(2, 6, 28, 19, matrix.Color333(1,0,0));
      }
      // Lives
      matrix.drawPixel(4, 9, matrix.Color333(5, 2, 0));
      matrix.drawPixel(4, 10, matrix.Color333(3, 0, 0));
      matrix.drawPixel(5, 10, matrix.Color333(5, 0, 0));
      matrix.drawPixel(4, 11, matrix.Color333(3, 0, 0));
      matrix.drawPixel(4, 12, matrix.Color333(0, 3, 0));
      matrix.setCursor(7, 8);
      matrix.print(lives);
      // Level
      matrix.drawLine(13, 9, 13, 13, matrix.Color333(3, 3, 3));
      matrix.drawLine(13, 13, 15, 13, matrix.Color333(3, 3, 3));
      matrix.setCursor(17, 8);
      if(level <= numberOfLevels)
      {
        matrix.print(level);
      }
      else
      {
        matrix.print("-");
      }
      // Gems
      matrix.drawLine(6, 17, 8, 17, matrix.Color333(3, 3, 3));
      matrix.drawLine(5, 18, 7, 20, matrix.Color333(3, 3, 3));
      matrix.drawLine(9, 18, 7, 20, matrix.Color333(3, 3, 3));
      matrix.setCursor(11, 16);
      matrix.print(gems);
      changed = false;
    }
    
    if(joy1Fire() && joy1Left() && level > 1 && lives > 0)
    {
      level--;
      initializeLevelWithGems = true;
      changed = true;
      tone(audio,1024,20);
      delay(100);  
    }
    else if(joy1Fire() && joy1Right() && level < numberOfLevels && lives > 0)
    {
      level++;
      initializeLevelWithGems = true;
      changed = true;
      tone(audio,1024,20);
      delay(200);  
    }
  }
  while(!(joy1Fire() && !joy1Left() && !joy1Right())); 
  tone(audio,1024,20);
  delay(200);  
}

// Death animation with flying body parts
void realistischeTodesanimation()
{
  todesanimationX[0] = playerXScreen + playerDirection;
  todesanimationY[0] = playerYScreen;
  todesanimationX[1] = playerXScreen + playerDirection;
  todesanimationY[1] = playerYScreen + 1;
  todesanimationX[2] = playerXScreen + 1 - playerDirection;
  todesanimationY[2] = playerYScreen + 1;
  todesanimationX[3] = playerXScreen + playerDirection;
  todesanimationY[3] = playerYScreen + 2;
  todesanimationX[4] = playerXScreen + playerDirection;
  todesanimationY[4] = playerYScreen + 3;
  for(i = 0; i < 32; i++)
  {
    // Sound: realistischeTodesanimation
    tone(audio, 2048 / pow(2, (i / 3)),40);
    matrix.drawPixel(todesanimationX[0], todesanimationY[0], matrix.Color333(5, 2, 0));
    matrix.drawPixel(todesanimationX[1], todesanimationY[1], matrix.Color333(3, 0, 0));
    matrix.drawPixel(todesanimationX[2], todesanimationY[2], matrix.Color333(5, 0, 0));
    matrix.drawPixel(todesanimationX[3], todesanimationY[3], matrix.Color333(3, 0, 0));
    matrix.drawPixel(todesanimationX[4], todesanimationY[4], matrix.Color333(0, 3, 0));
    delay(20);    
    matrix.drawPixel(todesanimationX[0], todesanimationY[0], matrix.Color333(0, 0, 0));
    matrix.drawPixel(todesanimationX[1], todesanimationY[1], matrix.Color333(0, 0, 0));
    matrix.drawPixel(todesanimationX[2], todesanimationY[2], matrix.Color333(0, 0, 0));
    matrix.drawPixel(todesanimationX[3], todesanimationY[3], matrix.Color333(0, 0, 0));
    matrix.drawPixel(todesanimationX[4], todesanimationY[4], matrix.Color333(0, 0, 0));
    todesanimationY[0]--;
    todesanimationX[1] = todesanimationX[1] + 0.95;
    todesanimationY[1] = todesanimationY[1] - 0.3;
    todesanimationX[2] = todesanimationX[2] - 0.95;
    todesanimationY[2] = todesanimationY[2] - 0.3;
    todesanimationX[3] = todesanimationX[3] + 0.575;
    todesanimationY[3] = todesanimationY[3] + 0.8;
    todesanimationX[4] = todesanimationX[4] - 0.575;
    todesanimationY[4] = todesanimationY[4] + 0.8;
  }
}

// End sequence after completion of all levels
void endSequence()
{
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
  matrix.setTextColor(matrix.Color333(3,0,0));
  matrix.setCursor(2, 0);
  matrix.println("Done!");
  matrix.setTextColor(matrix.Color333(0,3,0));
  matrix.setCursor(7, 8);
  matrix.println("She");
  matrix.setTextColor(matrix.Color333(0,0,3));
  matrix.setCursor(4, 16);
  matrix.println("said");
  matrix.setTextColor(matrix.Color333(3,3,3));
  matrix.setCursor(5, 24);
  matrix.println("YES!");
  for(i = 0; i < 3; i++)
  {
    tone(audio, NOTE_C5, 100);
    delay(100);
    tone(audio, NOTE_E5, 100);
    delay(100);
    tone(audio, NOTE_G5, 100);
    delay(100);        
  }
  tone(audio,1047,400);
  delay(4000);
  
  // End screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
  matrix.drawRect(0, 0, 32, 32, matrix.Color333(1,1,1));
  
  // Heart
  matrix.drawCircle(12,6,3,matrix.Color333(3,0,0));  
  matrix.drawCircle(19,6,3,matrix.Color333(3,0,0));  
  matrix.fillRect(9,7,16,4,matrix.Color333(0,0,0));
  matrix.drawLine(9,7,15,13,matrix.Color333(3,0,0));
  matrix.drawLine(22,7,16,13,matrix.Color333(3,0,0));
  
  // Kenny
  matrix.drawPixel(9, 15, matrix.Color333(5, 2, 0));
  matrix.drawPixel(9, 16, matrix.Color333(3, 0, 0));
  matrix.drawPixel(10, 16, matrix.Color333(5, 0, 0));
  matrix.drawPixel(9, 17, matrix.Color333(3, 0, 0));
  matrix.drawPixel(9, 18, matrix.Color333(0, 3, 0));
  
  // Lucy
  matrix.drawPixel(22, 15, matrix.Color333(5, 2, 0));
  matrix.drawPixel(21, 16, matrix.Color333(5, 0, 5));
  matrix.drawPixel(22, 16, matrix.Color333(3, 0, 3));
  //matrix.drawPixel(23, 16, matrix.Color333(5, 2, 0));
  matrix.drawPixel(22, 17, matrix.Color333(3, 0, 3));
  matrix.drawPixel(22, 18, matrix.Color333(0, 3, 0));
  
  // Rings
  matrix.drawCircle(13,24,4,matrix.Color333(1,1,4));  
  matrix.drawCircle(18,24,4,matrix.Color333(1,1,7));  
  
  delay(500);
  
  do
  {
  }
  while(!joy1Fire());  
  delay(200);

  showStatus();
  initializeLevelWithGems = true; // Important for a new start of the game (to have a correct gem setup)
}

// Main loop
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
      showStatus();
      setupLevel();
      reset = false; // Set reset indicator to false
      matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
      do
      {
        // Get time of engine loop start point (important for a constant game speed)
        engineLoopStartPoint = millis();
        
        // Release audio after collection sound (this blocks all movement sounds)
        if(audioOffUntil == animationCounter)
        {
          audioOffUntil = 0;
        }

        // Draw screen
        drawScreen();
        
        // Move player (draws also the player)
        if(animationCounter % 2 == 0)
        {
          movePlayer();
        }
        
        // Draw gems
        drawGems();
        
        // Move enemies
        moveEnemies();
        
        // Check whether gem has been collected
        checkForGems();
        
        animationCounter++;
    
        // Pause
        if(digitalRead(buttonPause) == LOW)
        {          
          tone(audio,1024,20);
          delay(500);
          do
          {
          }
          while((!digitalRead(buttonPause) == LOW) && (!joy1Fire()));
          tone(audio,1024,20);
          delay(500);
        }

        // Reset
        if(digitalRead(buttonReset) == LOW)
        {
          reset = true;
          lives = 0;
          break;
        }    

        // Loop until 20 milliseconds has passed since begin of engine loop
        do
          {
          }
        while(abs(millis() - engineLoopStartPoint) < 20);

      }
      while (!checkForLosingLive() && gemsToFinishLevel > 0);

      // Live lost
      if(gemsToFinishLevel > 0)
      {
        realistischeTodesanimation();
        delay(1000);
        initializeLevelWithGems = false;
      }
      // Level complete (beam player away)
      else
      {
        j = playerXScreen;
        k = playerYScreen;
        for(i = 0; i < 20; i++)
        {
          playerXScreenNew = j;
          playerYScreenNew = k;
          drawPlayer();
          tone(audio, 256 + i * 10, 100);
          delay(100 - (i * 5));
          playerXScreenNew = -4;
          playerYScreenNew = -4;
          drawPlayer();
          tone(audio, 128 + i * 10, 100);
          delay(i * 5);
        }
        level++;
        initializeLevelWithGems = true;
        matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
      }
    }
    while(!reset && level < (numberOfLevels + 1) && lives > 0);

    // All lives lost (Game over sequence)
    if(lives < 1)
    {
      matrix.setTextColor(matrix.Color333(3,0,0));
      matrix.setCursor(4, 8);
      matrix.println("GAME");
      matrix.setCursor(4, 16);
      matrix.println("OVER");
      tone(audio, NOTE_C5, 200);
      delay(200);
      tone(audio, NOTE_G4, 200);
      delay(200);
      tone(audio, NOTE_E4, 200);
      delay(200);
      tone(audio, NOTE_C4, 400);
      delay(4000);
      showStatus();
    }
    
    // Game completed (End sequence)
    if(level == (numberOfLevels + 1))
    {
      endSequence();
    }
  }
  while(true);
    
} // loop

