// Fire & Fury
// for the LEDmePlay
//
// www.mithotronic.de
//
// Version: 1.0.0
// Author: Michael Rosskopf (2018)
// 
// Thanks to Thomas Laubach! 
// 
// Release Notes:
// V1.0.0: First release

// Include libraries for adafruit matrix
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <avr/pgmspace.h>   // Necessary to maintain data in program memory
#include <Timer.h>          // Enables timed events (used for reset and pause)

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

// Sensitivity of analog thumb joysticks (minimal difference from center position)
const int sensitivityAnalog = 64;

// Returns the value of maximal excursion already at 0 + maxValueDistance or 1024 - maxValueDistance
const int maxValueDistanceAnalog = 64;

// Helper
int denominatorAnalog = 512 - sensitivityAnalog - maxValueDistanceAnalog;

// Buffer for analog joystick values
double joy1X;
double joy1Y;
double joy2X;
double joy2Y;

// Joystick directions
#define UP          1
#define UP_RIGHT    2
#define RIGHT       3
#define DOWN_RIGHT  4
#define DOWN        5
#define DOWN_LEFT   6
#define LEFT        7
#define UP_LEFT     8
#define FIRE_A      9
#define FIRE_B     10

// Other buttons
int buttonReset = 42;
int buttonPause = 43;

byte numberOfPlayers = 1; // 1 or 2

// Units
#define SCOUT         1
#define LIGHTTANK     2
#define TANK          3
#define HEAVYTANK     4
#define HOWITZER      5
#define LAUNCHER      6
#define AA            7
#define SAM           8
#define ATTACKCOPTER  9
#define HEAVYCOPTER   10
#define HQ            11

// Weapon types (for visualization of battles)
#define AUTOGUN       1
#define CANNON        2
#define MISSILE       3
#define EXHAUST       4 // No weapon: It is used for missile exhaust plumes

// Actors
#define RED           1
#define BLUE          2

// Players
#define PLAYER1       1 // Controlled by joystick 1
#define PLAYER2       2 // Controlled by joystick 2
#define COMPUTER      3 // Controlled by the AI

// Colors (for visualization of units)
#define W             1 // Weapon of a unit
#define B             2 // Body of a unit (color depends on actor)
#define C             3 // Chassis (or rotor) of a unit

const uint8_t unitPics[] PROGMEM = {
                      3, 3,
                      B, B, 0,
                      B, B, B,
                      C, 0, C,

                      4, 3,
                      0, 0, B, W,
                      B, B, B, B,
                      C, C, C, C,

                      5, 3,
                      0, B, B, W, W,
                      B, B, B, B, 0,
                      C, C, C, C, 0,

                      5, 4,
                      0, 0, B, B, W,
                      0, B, B, B, 0,
                      B, B, B, B, B,
                      C, C, C, C, C,

                      4, 4,
                      0, 0, 0, W,
                      0, 0, W, 0,
                      0, B, B, 0,
                      C, B, B, C,

                      5, 4,
                      0, 0, W, 0, 0,
                      0, W, 0, B, B,
                      W, B, B, B, B,
                      C, C, 0, 0, C,

                      3, 4,
                      0, W, 0,
                      0, W, 0,
                      B, B, B,
                      C, B, C,

                      5, 4,
                      B, B, W, 0, 0,
                      0, B, 0, 0, 0,
                      B, B, B, B, B,
                      C, C, C, C, C,

                      5, 3,
                      B, 0, C, C, C,
                      0, B, B, B, B,
                      0, 0, 0, B, B,

                      5, 3,
                      B, 0, C, C, C,
                      C, B, B, B, B,
                      0, 0, B, B, B,

                      5, 4,
                      B, B, B, B, B,
                      B, C, 0, C, B,
                      B, C, C, C, B,
                      B, C, 0, C, B
};

struct Unit
{
  byte type;
  byte movement;
  byte armor;
  boolean groundUnit;
  byte groundAttackWeapon;
  byte groundAttackRange;
  byte groundAttackPower;
  byte airAttackWeapon;
  byte airAttackRange;
  byte airAttackPower;
  
  int hitPoints;
  byte experience;
  byte initiativeValue;
  
  byte actor;
  boolean active;
  boolean settled;
  byte fieldsMoved;
  byte xPosition;
  byte yPosition;
  byte orientation;
};
const byte numberOfUnits = 32;
Unit unit[numberOfUnits];
byte nextUnit = 0;
Unit bufferUnit; // Used to calculate hit probabilities

byte xRedHQ;
byte yRedHQ;
byte xBlueHQ;
byte yBlueHQ;

byte shooter[numberOfUnits];
byte target[numberOfUnits];
byte nextShot;

// Maps
#define WOOD              1
#define RUMBLE            2
#define RED_SCOUT         3
#define RED_LIGHTTANK     4
#define RED_TANK          5
#define RED_HEAVYTANK     6
#define RED_HOWITZER      7
#define RED_LAUNCHER      8
#define RED_AA            9
#define RED_SAM           10
#define RED_ATTACKCOPTER  11
#define RED_HEAVYCOPTER   12
#define RED_HQ            13
#define BLUE_SCOUT        14
#define BLUE_LIGHTTANK    15
#define BLUE_TANK         16
#define BLUE_HEAVYTANK    17
#define BLUE_HOWITZER     18
#define BLUE_LAUNCHER     19
#define BLUE_AA           20
#define BLUE_SAM          21
#define BLUE_ATTACKCOPTER 22
#define BLUE_HEAVYCOPTER  23
#define BLUE_HQ           24

const byte numberOfMaps = 8;
const uint8_t maps[] PROGMEM = {
  
// Skirmish (5)
                        8, 8,
                        RED_HQ, RED_LIGHTTANK, 0, WOOD, 0, 0, 0, 0,
                        RED_TANK, RED_SCOUT, 0, 0, 0, 0, WOOD, 0,
                        RED_LIGHTTANK, RED_SCOUT, 0, 0, WOOD, 0, 0, 0,
                        0, WOOD, 0, RUMBLE, RUMBLE, 0, 0, 0,
                        0, 0, 0, RUMBLE, RUMBLE, 0, WOOD, 0,
                        0, 0, 0, WOOD, 0, 0, BLUE_SCOUT, BLUE_LIGHTTANK,
                        0, WOOD, 0, 0, 0, 0, BLUE_SCOUT, BLUE_TANK,
                        0, 0, 0, 0, WOOD, 0, BLUE_LIGHTTANK, BLUE_HQ,

// Tanks (6)
                        8, 8,
                        0, 0, WOOD, 0, 0, WOOD, 0, 0,
                        0, 0, 0, RUMBLE, 0, 0, BLUE_HEAVYTANK, BLUE_HQ,
                        0, RED_LIGHTTANK, 0, 0, WOOD, BLUE_LIGHTTANK, BLUE_TANK, 0,
                        0, RUMBLE, RED_TANK, 0, 0, WOOD, BLUE_HEAVYTANK, 0,
                        0, RED_HEAVYTANK, WOOD, 0, 0, BLUE_TANK, RUMBLE, 0,
                        0, RED_TANK, RED_LIGHTTANK, WOOD, 0, 0, BLUE_LIGHTTANK, 0,
                        RED_HQ, RED_HEAVYTANK, 0, 0, RUMBLE, 0, 0, 0,
                        0, 0, WOOD, 0, 0, WOOD, 0, 0,

// Heavy Fire (7)
                        10, 10,
                        0, 0, 0, 0, WOOD, WOOD, 0, 0, 0, 0,
                        RED_SCOUT, 0, 0, 0, 0, 0, 0, 0, 0, BLUE_SCOUT,
                        RED_HQ, RED_TANK, 0, 0, 0, WOOD, 0, 0, BLUE_TANK, 0,
                        RED_LIGHTTANK, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        RED_HOWITZER, 0, WOOD, 0, 0, 0, RUMBLE, 0, 0, BLUE_HOWITZER,
                        RED_HOWITZER, 0, 0, RUMBLE, 0, 0, 0, WOOD, 0, BLUE_HOWITZER,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, BLUE_LIGHTTANK,
                        0, RED_TANK, 0, 0, WOOD, 0, 0, 0, BLUE_TANK, BLUE_HQ,
                        RED_SCOUT, 0, 0, 0, 0, 0, 0, 0, 0, BLUE_SCOUT,
                        0, 0, 0, 0, WOOD, WOOD, 0, 0, 0, 0,

// Deep in the Woods (8)
                        10, 10,
                        0, 0, WOOD, 0, 0, WOOD, 0, 0, WOOD, 0,
                        0, 0, 0, 0, WOOD, 0, WOOD, 0, 0, BLUE_HEAVYCOPTER,
                        0, WOOD, 0, WOOD, 0, WOOD, 0, 0, BLUE_SCOUT, BLUE_HQ,
                        RED_SCOUT, 0, 0, WOOD, 0, 0, 0, 0, WOOD, BLUE_AA,
                        RED_LIGHTTANK, 0, WOOD, 0, WOOD, 0, WOOD, 0, BLUE_LIGHTTANK, BLUE_HOWITZER,
                        RED_HOWITZER, RED_LIGHTTANK, 0, WOOD, 0, WOOD, 0, WOOD, 0, BLUE_LIGHTTANK,
                        RED_AA, WOOD, 0, 0, 0, 0, WOOD, 0, 0, BLUE_SCOUT,
                        RED_HQ, RED_SCOUT, 0, 0, WOOD, 0, WOOD, 0, WOOD, 0,
                        RED_HEAVYCOPTER, 0, 0, WOOD, 0, WOOD, 0, 0, 0, 0,
                        0, WOOD, 0, 0, WOOD, 0, 0, WOOD, 0, 0,

// Airstrike (9)
                        10, 10,
                        0, RED_LIGHTTANK, 0, 0, 0, 0, 0, 0, 0, 0,
                        RED_HQ, RED_AA, 0, WOOD, 0, WOOD, 0, 0, WOOD, 0,
                        RED_AA, RED_SAM, 0, 0, 0, 0, WOOD, 0, 0, 0,
                        RED_ATTACKCOPTER, 0, 0, 0, WOOD, 0, 0, 0, BLUE_SAM, BLUE_ATTACKCOPTER,
                        RED_HEAVYCOPTER, 0, 0, WOOD, RUMBLE, WOOD, 0, 0, 0, BLUE_HEAVYCOPTER,
                        RED_HEAVYCOPTER, 0, 0, 0, WOOD, RUMBLE, WOOD, 0, 0, BLUE_HEAVYCOPTER,
                        RED_ATTACKCOPTER, RED_SAM, 0, 0, 0, WOOD, 0, 0, 0, BLUE_ATTACKCOPTER,
                        0, 0, 0, WOOD, 0, 0, 0, 0, BLUE_SAM, BLUE_AA,
                        0, WOOD, 0, 0, WOOD, 0, WOOD, 0, BLUE_AA, BLUE_HQ,
                        0, 0, 0, 0, 0, 0, 0, 0, BLUE_LIGHTTANK, 0,

// The Canyon (10)
                        12, 8,
                        0, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, 0,
                        RED_ATTACKCOPTER, 0, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, 0, 0,
                        RED_HEAVYCOPTER, RED_LAUNCHER, 0, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, BLUE_TANK, 0, BLUE_ATTACKCOPTER,
                        RED_SAM, RED_HOWITZER, RED_TANK, 0, 0, WOOD, 0, WOOD, 0, BLUE_TANK, BLUE_AA, BLUE_HQ,
                        RED_HQ, RED_AA, RED_TANK, 0, WOOD, 0, WOOD, 0, 0, BLUE_TANK, BLUE_HOWITZER, BLUE_SAM,
                        RED_ATTACKCOPTER, 0, RED_TANK, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, 0, BLUE_LAUNCHER, BLUE_HEAVYCOPTER,
                        0, 0, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, 0, BLUE_ATTACKCOPTER,
                        0, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, RUMBLE, 0,

// Static Warfare (11)
                        8, 12,
                        RED_LAUNCHER, 0, 0, RUMBLE, 0, 0, BLUE_TANK, 0,
                        RED_HQ, RED_HOWITZER, 0, 0, 0, 0, 0, BLUE_HOWITZER,
                        0, 0, 0, 0, RUMBLE, 0, BLUE_HEAVYTANK, 0,
                        RED_LAUNCHER, RED_TANK, 0, 0, 0, 0, 0, BLUE_LAUNCHER,
                        0, 0, 0, RUMBLE, 0, 0, BLUE_TANK, 0,
                        0, RED_HEAVYTANK, 0, 0, 0, RUMBLE, 0, BLUE_HOWITZER,
                        RED_HOWITZER, 0, RUMBLE, 0, 0, 0, BLUE_HEAVYTANK, 0,
                        0, RED_TANK, 0, 0, RUMBLE, 0, 0, 0,
                        RED_LAUNCHER, 0, 0, 0, 0, 0, BLUE_TANK, BLUE_LAUNCHER,
                        0, RED_HEAVYTANK, 0, RUMBLE, 0, 0, 0, 0,
                        RED_HOWITZER, 0, 0, 0, 0, 0, BLUE_HOWITZER, BLUE_HQ,
                        0, RED_TANK, 0, 0, RUMBLE, 0, 0, BLUE_LAUNCHER,

// The Mother of All Battles (12)
                        12, 12,
                        0, 0, WOOD, 0, 0, 0, 0, RUMBLE, 0, 0, 0, 0,
                        RED_ATTACKCOPTER, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, BLUE_HEAVYCOPTER,
                        0, 0, RED_SCOUT, 0, WOOD, WOOD, 0, 0, 0, BLUE_LIGHTTANK, BLUE_LAUNCHER, 0,
                        0, RED_TANK, 0, 0, WOOD, WOOD, 0, 0, WOOD, WOOD, 0, BLUE_HQ,
                        RED_SAM, RED_HEAVYTANK, 0, 0, 0, RUMBLE, 0, 0, 0, 0, BLUE_HOWITZER, 0,
                        0, RED_HEAVYTANK, 0, 0, 0, WOOD, 0, WOOD, 0, 0, BLUE_TANK, BLUE_AA,
                        RED_AA, RED_TANK, 0, 0, WOOD, 0, WOOD, 0, 0, 0, BLUE_HEAVYTANK, 0,
                        0, RED_HOWITZER, 0, 0, 0, 0, RUMBLE, 0, 0, 0, BLUE_HEAVYTANK, BLUE_SAM,
                        RED_HQ, 0, WOOD, WOOD, 0, 0, WOOD, WOOD, 0, 0, BLUE_TANK, 0,
                        0, RED_LAUNCHER, RED_LIGHTTANK, 0, 0, 0, WOOD, WOOD, 0, BLUE_SCOUT, 0, 0,
                        RED_HEAVYCOPTER, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, BLUE_ATTACKCOPTER,
                        0, 0, 0, 0, RUMBLE, 0, 0, 0, 0, WOOD, 0, 0
};

byte currentMap[256]; // Contains the current map after loading from PROGMEM
byte mapNumber;
byte mapWidth;
byte mapHeight;
byte distanceMap[256]; // Contains the distances to other fields from current unit (calculated by kind of Dijkstra algorithm)
boolean outMap[256];   // Contains for which fields the closest distance has already been determined

byte cursorX;
byte cursorY;
byte upperLeftMapCornerX;
byte upperLeftMapCornerY;
byte cursorXNew;
byte cursorYNew;
byte upperLeftMapCornerXNew;
byte upperLeftMapCornerYNew;

byte overlayedUnit;
boolean unitOverlayed;
byte selectedUnit;
boolean unitSelected;

boolean overlaySelectedUnit;
boolean overlayMove;
boolean overlayTarget;
boolean overlaySettled;

// Game round types
#define MOVE 1
#define AIM  2
#define FIX  3 
#define FIRE 4

// Overlay types
#define SELECTEDUNIT 1
#define MOVEMENT     2
#define SETTLED      3
#define TARGET       4

// Game settings
byte redPlayer;
byte bluePlayer;

// Animations

byte playfield[32][32]; // Array to store screen content (e.g. used for collision detection of bullet with target)

byte mountainBackground[32];
byte treeX[8];
byte treeY[8];
byte treeD[8];
byte treeR[8];
byte treeG[8];
byte treeB[8];

struct Bullet
{
  byte type;
  byte power;
  float xPosition;
  float yPosition;
  float xVector;
  float yVector;
  boolean active;
};
const byte numberOfBullets = 16;
Bullet bullet[numberOfBullets];
byte nextBullet = 0;

byte bulletHitCounter;          // Counts how many bullets have hit a unit in target animation
byte bulletHitExplodeThreshold; // Counts how many bullets must hit before a unit with no more hit points explodes

struct Explosion
{
  byte phase;
  byte counter;
  byte power;
  float xPosition;
  float yPosition;
  boolean active;
};
const byte numberOfExplosions = 8;
Explosion explosion[numberOfExplosions];
byte nextExplosion = 0;

unsigned long synchPoint;
Timer resetAndPauseTimer; // Used to check cyclically the reset and pause button
boolean reset;

// Computer AI

// Order
#define ATTACK 0
#define DEFEND 1

byte order;

struct TargetField
{
  byte xPosition;
  byte yPosition;
  float expectedProvokedDamage;
  float expectedTakenDamage;
  byte enemyHQDistance;
  byte ownHQDistance;
  int assessment;
};
const byte numberOfTargetFields = 25;
TargetField targetField[numberOfTargetFields]; // Used to calculate the expected effect of entering or shooting at a specific field.
byte targetFieldCounter;

void setup() 
{
  // Initialize serial connection
  Serial.begin(9600);

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

  // Logos
  mithotronic();
  ledMePlay();

  // Start the timer for reset and pause
  resetAndPauseTimer.every(50, checkResetAndPause);

  redPlayer = PLAYER1;
  bluePlayer = PLAYER2;
  mapNumber = 1;

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
float joy1XValue()
{
  joy1X = analogRead(analogX1);
  if((joy1X > (512 - sensitivityAnalog)) && (joy1X < (512 + sensitivityAnalog))){ return 0.0; }
  else if(joy1X > (1024 - maxValueDistanceAnalog)){ return 1.0; }
  else if(joy1X < maxValueDistanceAnalog){ return -1.0; }
  else if(joy1X > 512){ return (1.0 * (joy1X - sensitivityAnalog - 512) / denominatorAnalog); }
  else{ return (-1.0 * (512 - joy1X - sensitivityAnalog) / denominatorAnalog); }
}
float joy1YValue()
{
  joy1Y = analogRead(analogY1);
  if((joy1Y > (512 - sensitivityAnalog)) && (joy1Y < (512 + sensitivityAnalog))){ return 0.0; }
  else if(joy1Y > (1024 - maxValueDistanceAnalog)){ return 1.0; }
  else if(joy1Y < maxValueDistanceAnalog){ return -1.0; }
  else if(joy1Y > 512){ return (1.0 * (joy1Y - sensitivityAnalog - 512) / denominatorAnalog); }
  else{ return (-1.0 * (512 - joy1Y - sensitivityAnalog) / denominatorAnalog); }
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
float joy2XValue()
{
  joy2X = analogRead(analogX2);
  if((joy2X > (512 - sensitivityAnalog)) && (joy2X < (512 + sensitivityAnalog))){ return 0.0; }
  else if(joy2X > (1024 - maxValueDistanceAnalog)){ return 1.0; }
  else if(joy2X < maxValueDistanceAnalog){ return -1.0; }
  else if(joy2X > 512){ return (1.0 * (joy2X - sensitivityAnalog - 512) / denominatorAnalog); }
  else{ return (-1.0 * (512 - joy2X - sensitivityAnalog) / denominatorAnalog); }
}
float joy2YValue()
{
  joy2Y = analogRead(analogY2);
  if((joy2Y > (512 - sensitivityAnalog)) && (joy2Y < (512 + sensitivityAnalog))){ return 0.0; }
  else if(joy2Y > (1024 - maxValueDistanceAnalog)){ return 1.0; }
  else if(joy2Y < maxValueDistanceAnalog){ return -1.0; }
  else if(joy2Y > 512){ return (1.0 * (joy2Y - sensitivityAnalog - 512) / denominatorAnalog); }
  else{ return (-1.0 * (512 - joy2Y - sensitivityAnalog) / denominatorAnalog); }
}
boolean joy2Fire()
{
  if(digitalRead(buttonFire2) == LOW || (digitalRead(buttonU2) == LOW && digitalRead(buttonD2) == LOW)){ return true; }
  return false;
}
boolean joy2FireA()
{
  if(digitalRead(buttonFire2) == LOW){ return true; }
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
    while(j < 250 && digitalRead(buttonFire1) != LOW && digitalRead(buttonFire2) != LOW);
  }
  while(digitalRead(buttonFire1) != LOW && digitalRead(buttonFire2) != LOW);
  tone(audio,1024,20);
  delay(200);
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
}

// buttonFire1Clicked: Used for menu selection (triggers a sound effect)
boolean buttonFire1Clicked()
{
  if(!joy1Fire())
  {
    return false;
  }
  else
  {
    do
    {
      delay(1);
    }
    while(joy1Fire());
    clickSound();
    delay(200);  
    return true;
  }
}

// checkResetAndPause: Called by resetAndPauseTimer to check whether reset or pause is triggered
void checkResetAndPause()
{
  // Reset
  if(digitalRead(buttonReset) == LOW)
  {
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
    reset = true;
  }
  
  // Pause
  if(digitalRead(buttonPause) == LOW)
  {          
    clickSound();
    delay(500);
    do
    {
    }
    while((digitalRead(buttonPause) != LOW) && !joy1Fire() && !joy2Fire());
    clickSound();
    delay(200);
  }    
}

// clickSound: Sound effect
void clickSound()
{
  // SOUND: Menu button clicked
  for(byte i = 0; i < 20; i++)
  {
    tone(audio, 40 + 40 * i, 2);
    delay(1);   
  }
}


// title: Title screen
void title()
{
  reset = false;
  
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Text
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.setCursor(4, 4);
  matrix.println("FIRE");
  matrix.setTextColor(matrix.Color333(2, 2, 0));
  matrix.setCursor(14, 12);
  matrix.println("&");
  matrix.setTextColor(matrix.Color333(0, 0, 3));
  matrix.setCursor(5, 20);
  matrix.println("FURY");
  delay(20);

  drawUnit(TANK, RED, RIGHT, true, false, 3, 14);
  drawUnit(TANK, BLUE, LEFT, true, false, 24, 14);

  do
  {
    random(100);
  }
  while(!buttonFire1Clicked());

  // Initialize random number generator
  randomSeed(analogRead(40));
}


// selectPlayers: Menu to select the number of players
void selectPlayers()
{    
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Text
  matrix.setCursor(1, 0);
  matrix.setTextColor(matrix.Color333(2, 2, 0));
  matrix.println("Plyrs");
  matrix.setCursor(7, 8);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.println("Red");
  matrix.setCursor(7, 16);
  matrix.setTextColor(matrix.Color333(0, 0, 3));
  matrix.println("Blu");
  matrix.setCursor(7, 24);
  matrix.setTextColor(matrix.Color333(2, 2, 0));
  matrix.println("Go");

  boolean updateMenu = true;
  byte selector = 3;

  // Standard player settings
  redPlayer = PLAYER1;
  bluePlayer = COMPUTER;
  
  do
  {
    if(updateMenu)
    {
      matrix.fillRect(1, 8, 4, 24, matrix.Color333(0, 0, 0));
      matrix.fillRect(26, 8, 5, 16, matrix.Color333(0, 0, 0));
      
      matrix.setCursor(0, selector * 8);
      matrix.setTextColor(matrix.Color333(2, 2, 0));
      matrix.println(">");

      matrix.setCursor(26, 8);
      matrix.setTextColor(matrix.Color333(2, 2, 0));
      if(redPlayer == PLAYER1){ matrix.println("1"); }
      if(redPlayer == PLAYER2){ matrix.println("2"); }
      if(redPlayer == COMPUTER){ matrix.println("C"); }
      matrix.setCursor(26, 16);
      matrix.setTextColor(matrix.Color333(2, 2, 0));
      if(bluePlayer == PLAYER1){ matrix.println("1"); }
      if(bluePlayer == PLAYER2){ matrix.println("2"); }
      if(bluePlayer == COMPUTER){ matrix.println("C"); }

      delay(200);  
      updateMenu = false;
    }
    if(joy1Up() && selector > 1)
    {
      selector--;
      updateMenu = true;
      clickSound();
    }
    if(joy1Down() && selector < 3)
    {
      selector++;
      updateMenu = true;        
      clickSound();
    } 
    if(joy1Fire())
    {
      if(selector == 1)
      {
        redPlayer++;
        if(redPlayer == 4){ redPlayer = 1; }        
      }
      else if(selector == 2)
      {
        bluePlayer++;
        if(bluePlayer == 4){ bluePlayer = 1; }                
      }
      else if(selector == 3)
      { 
        clickSound();
        break;
      }
      
      // Avoid that player plays against himself
      if(redPlayer == bluePlayer && redPlayer != COMPUTER)
      {
        if(selector == 1){ redPlayer++; }
        else if(selector == 2){ bluePlayer++; }
      }
      
      // Avoid that just player 2 plays (and joystick port needs to be changed --> player 1 uses always port 1)
      if(redPlayer == PLAYER2 && bluePlayer == COMPUTER){ redPlayer = COMPUTER; }
      else if(redPlayer == COMPUTER && bluePlayer == PLAYER2){ bluePlayer = COMPUTER; }

      updateMenu = true;        
      clickSound();      
    }
  }
  while(true);  
}

void printMapName(int scrollOffset, boolean draw)
{
  matrix.setCursor(scrollOffset, 20);

  if(draw){ matrix.setTextColor(matrix.Color333(2, 2, 0)); }
  else{ matrix.setTextColor(matrix.Color333(0, 0, 0)); }

  if(mapNumber == 1){ matrix.println("Skirmish"); }
  else if(mapNumber == 2){ matrix.println("Tanks"); }
  else if(mapNumber == 3){ matrix.println("Heavy Fire");}
  else if(mapNumber == 4){ matrix.println("Deep in the Woods");}
  else if(mapNumber == 5){ matrix.println("Airstrike");}
  else if(mapNumber == 6){ matrix.println("The Canyon");}
  else if(mapNumber == 7){ matrix.println("Static Warfare");}
  else if(mapNumber == 8){ matrix.println("The Mother of All Battles");}  

  // Set color back to yellow
  matrix.setTextColor(matrix.Color333(2, 2, 0));
}

// selectMap: Menu to select the map
void selectMap()
{ 
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  matrix.drawLine(0, 2, 32, 2, matrix.Color333(3, 0, 0));
  matrix.drawLine(0, 28, 32, 28, matrix.Color333(0, 0, 3));

  // Text
  matrix.setTextColor(matrix.Color333(2, 2, 0));
  matrix.setCursor(7, 4);
  matrix.println("Map");
  matrix.setCursor(0, 12);
  matrix.println("<");
  matrix.setCursor(26, 12);
  matrix.println(">");

  boolean updateMenu = true;
  int scrollOffset = 32;
  do
  {
    if(updateMenu)
    {
      matrix.fillRect(6, 12, 20, 8, matrix.Color333(0, 0, 0));
      matrix.setCursor(13, 12);
      matrix.println(mapNumber);
      delay(200);  
      updateMenu = false;
    }

    // Scroll map name
    // Remove old position
    printMapName(scrollOffset, false);

    // Update position
    scrollOffset--;
    if(scrollOffset < -160){ scrollOffset = 32; }

    // Draw new position 
    printMapName(scrollOffset, true);
    delay(40);
        
    if(joy1Left() && mapNumber > 1)
    {
      printMapName(scrollOffset, false);
      scrollOffset = 32;
      mapNumber--;
      updateMenu = true;
      clickSound();
    }
    if(joy1Right() && mapNumber < numberOfMaps)
    {
      printMapName(scrollOffset, false);
      scrollOffset = 32;
      mapNumber++;
      updateMenu = true;        
      clickSound();
    }    
  }
  while(!buttonFire1Clicked());  
}


// InitializeMap: Called before a new game. Loads the defined map from progmem and initializes the game. 
void initializeMap(byte _mapNumber)
{  
  // Reset units
  for(byte i = 0; i < numberOfUnits; i++)
  {
    unit[i].type = 0;
    unit[i].movement = 0;
    unit[i].armor = 0;
    unit[i].groundUnit = true;
    unit[i].groundAttackWeapon = 0;
    unit[i].groundAttackRange = 0;
    unit[i].groundAttackPower = 0;
    unit[i].airAttackWeapon = 0;
    unit[i].airAttackRange = 0;
    unit[i].airAttackPower = 0;  
    unit[i].hitPoints = 0;
    unit[i].experience = 0;  
    unit[i].actor = 0;
    unit[i].active = false;
    unit[i].xPosition = 0;
    unit[i].yPosition = 0;
    unit[i].orientation = RIGHT;
  }
  nextUnit = 0;

  // Reset map
  for(int i = 0; i < 256; i++)
  {
    currentMap[i] = 0;
  }
  mapNumber = _mapNumber;
  mapWidth = 0;
  mapHeight = 0;

  // Copy map and units from progmem
  int k = 0;
  for(int i = 0; i < mapNumber; i++)
  {
    mapWidth = byte(pgm_read_byte_near(maps + k));
    mapHeight = byte(pgm_read_byte_near(maps + 1 + k));

    if(i < mapNumber - 1)
    {
      k = k + ((mapWidth * mapHeight) + 2);
    }
  }

  byte j = 0;
  for(int i = 0; i < (mapWidth * mapHeight); i++)
  {
    j = byte(pgm_read_byte_near(maps + 2 + i + k));

    if(j == WOOD)
    {
      currentMap[i] = WOOD;
    }
    else if (j == RUMBLE)
    {
      currentMap[i] = RUMBLE;
    }
    else if(j == RED_SCOUT)
    {
      unit[nextUnit].type = SCOUT;
      unit[nextUnit].movement = 3;
      unit[nextUnit].armor = 3;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = AUTOGUN;
      unit[nextUnit].groundAttackRange = 1;
      unit[nextUnit].groundAttackPower = 2;
      unit[nextUnit].airAttackWeapon = AUTOGUN;
      unit[nextUnit].airAttackRange = 1;
      unit[nextUnit].airAttackPower = 2;
      unit[nextUnit].actor = RED;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == RED_LIGHTTANK)
    {
      unit[nextUnit].type = LIGHTTANK;
      unit[nextUnit].movement = 2;
      unit[nextUnit].armor = 5;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = AUTOGUN;
      unit[nextUnit].groundAttackRange = 2;
      unit[nextUnit].groundAttackPower = 3;
      unit[nextUnit].airAttackWeapon = AUTOGUN;
      unit[nextUnit].airAttackRange = 2;
      unit[nextUnit].airAttackPower = 3;
      unit[nextUnit].actor = RED;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == RED_TANK)
    {
      unit[nextUnit].type = TANK;
      unit[nextUnit].movement = 2;
      unit[nextUnit].armor = 7;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = CANNON;
      unit[nextUnit].groundAttackRange = 2;
      unit[nextUnit].groundAttackPower = 4;
      unit[nextUnit].airAttackWeapon = AUTOGUN;
      unit[nextUnit].airAttackRange = 1;
      unit[nextUnit].airAttackPower = 2;
      unit[nextUnit].actor = RED;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == RED_HEAVYTANK)
    {
      unit[nextUnit].type = HEAVYTANK;
      unit[nextUnit].movement = 2;
      unit[nextUnit].armor = 9;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = CANNON;
      unit[nextUnit].groundAttackRange = 2;
      unit[nextUnit].groundAttackPower = 5;
      unit[nextUnit].airAttackWeapon = AUTOGUN;
      unit[nextUnit].airAttackRange = 1;
      unit[nextUnit].airAttackPower = 2;
      unit[nextUnit].actor = RED;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == RED_HOWITZER)
    {
      unit[nextUnit].type = HOWITZER;
      unit[nextUnit].movement = 1;
      unit[nextUnit].armor = 4;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = CANNON;
      unit[nextUnit].groundAttackRange = 3;
      unit[nextUnit].groundAttackPower = 4;
      unit[nextUnit].airAttackWeapon = AUTOGUN;
      unit[nextUnit].airAttackRange = 1;
      unit[nextUnit].airAttackPower = 2;
      unit[nextUnit].actor = RED;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == RED_LAUNCHER)
    {
      unit[nextUnit].type = LAUNCHER;
      unit[nextUnit].movement = 2;
      unit[nextUnit].armor = 3;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = MISSILE;
      unit[nextUnit].groundAttackRange = 4;
      unit[nextUnit].groundAttackPower = 5;
      unit[nextUnit].airAttackWeapon = AUTOGUN;
      unit[nextUnit].airAttackRange = 1;
      unit[nextUnit].airAttackPower = 2;
      unit[nextUnit].actor = RED;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == RED_AA)
    {
      unit[nextUnit].type = AA;
      unit[nextUnit].movement = 1;
      unit[nextUnit].armor = 4;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = AUTOGUN;
      unit[nextUnit].groundAttackRange = 1;
      unit[nextUnit].groundAttackPower = 2;
      unit[nextUnit].airAttackWeapon = CANNON;
      unit[nextUnit].airAttackRange = 3;
      unit[nextUnit].airAttackPower = 4;
      unit[nextUnit].actor = RED;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == RED_SAM)
    {
      unit[nextUnit].type = SAM;
      unit[nextUnit].movement = 2;
      unit[nextUnit].armor = 5;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = AUTOGUN;
      unit[nextUnit].groundAttackRange = 1;
      unit[nextUnit].groundAttackPower = 2;
      unit[nextUnit].airAttackWeapon = MISSILE;
      unit[nextUnit].airAttackRange = 4;
      unit[nextUnit].airAttackPower = 4;
      unit[nextUnit].actor = RED;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == RED_ATTACKCOPTER)
    {
      unit[nextUnit].type = ATTACKCOPTER;
      unit[nextUnit].movement = 3;
      unit[nextUnit].armor = 5;
      unit[nextUnit].groundUnit = false;
      unit[nextUnit].groundAttackWeapon = AUTOGUN;
      unit[nextUnit].groundAttackRange = 2;
      unit[nextUnit].groundAttackPower = 3;
      unit[nextUnit].airAttackWeapon = MISSILE;
      unit[nextUnit].airAttackRange = 3;
      unit[nextUnit].airAttackPower = 4;
      unit[nextUnit].actor = RED;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == RED_HEAVYCOPTER)
    {
      unit[nextUnit].type = HEAVYCOPTER;
      unit[nextUnit].movement = 3;
      unit[nextUnit].armor = 5;
      unit[nextUnit].groundUnit = false;
      unit[nextUnit].groundAttackWeapon = MISSILE;
      unit[nextUnit].groundAttackRange = 3;
      unit[nextUnit].groundAttackPower = 5;
      unit[nextUnit].airAttackWeapon = AUTOGUN;
      unit[nextUnit].airAttackRange = 2;
      unit[nextUnit].airAttackPower = 3;
      unit[nextUnit].actor = RED;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == RED_HQ)
    {
      unit[nextUnit].type = HQ;
      unit[nextUnit].movement = 0;
      unit[nextUnit].armor = 9;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = 0;
      unit[nextUnit].groundAttackRange = 0;
      unit[nextUnit].groundAttackPower = 0;
      unit[nextUnit].airAttackWeapon = 0;
      unit[nextUnit].airAttackRange = 0;
      unit[nextUnit].airAttackPower = 0;
      unit[nextUnit].actor = RED;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      xRedHQ = unit[nextUnit].xPosition;
      yRedHQ = unit[nextUnit].yPosition;
      nextUnit++;
    }
    else if(j == BLUE_SCOUT)
    {
      unit[nextUnit].type = SCOUT;
      unit[nextUnit].movement = 3;
      unit[nextUnit].armor = 3;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = AUTOGUN;
      unit[nextUnit].groundAttackRange = 1;
      unit[nextUnit].groundAttackPower = 2;
      unit[nextUnit].airAttackWeapon = AUTOGUN;
      unit[nextUnit].airAttackRange = 1;
      unit[nextUnit].airAttackPower = 2;
      unit[nextUnit].actor = BLUE;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == BLUE_LIGHTTANK)
    {
      unit[nextUnit].type = LIGHTTANK;
      unit[nextUnit].movement = 2;
      unit[nextUnit].armor = 5;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = AUTOGUN;
      unit[nextUnit].groundAttackRange = 2;
      unit[nextUnit].groundAttackPower = 3;
      unit[nextUnit].airAttackWeapon = AUTOGUN;
      unit[nextUnit].airAttackRange = 2;
      unit[nextUnit].airAttackPower = 3;
      unit[nextUnit].actor = BLUE;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == BLUE_TANK)
    {
      unit[nextUnit].type = TANK;
      unit[nextUnit].movement = 2;
      unit[nextUnit].armor = 7;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = CANNON;
      unit[nextUnit].groundAttackRange = 2;
      unit[nextUnit].groundAttackPower = 4;
      unit[nextUnit].airAttackWeapon = AUTOGUN;
      unit[nextUnit].airAttackRange = 1;
      unit[nextUnit].airAttackPower = 2;
      unit[nextUnit].actor = BLUE;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == BLUE_HEAVYTANK)
    {
      unit[nextUnit].type = HEAVYTANK;
      unit[nextUnit].movement = 2;
      unit[nextUnit].armor = 9;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = CANNON;
      unit[nextUnit].groundAttackRange = 2;
      unit[nextUnit].groundAttackPower = 5;
      unit[nextUnit].airAttackWeapon = AUTOGUN;
      unit[nextUnit].airAttackRange = 1;
      unit[nextUnit].airAttackPower = 2;
      unit[nextUnit].actor = BLUE;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == BLUE_HOWITZER)
    {
      unit[nextUnit].type = HOWITZER;
      unit[nextUnit].movement = 1;
      unit[nextUnit].armor = 4;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = CANNON;
      unit[nextUnit].groundAttackRange = 3;
      unit[nextUnit].groundAttackPower = 4;
      unit[nextUnit].airAttackWeapon = AUTOGUN;
      unit[nextUnit].airAttackRange = 1;
      unit[nextUnit].airAttackPower = 2;
      unit[nextUnit].actor = BLUE;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == BLUE_LAUNCHER)
    {
      unit[nextUnit].type = LAUNCHER;
      unit[nextUnit].movement = 2;
      unit[nextUnit].armor = 3;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = MISSILE;
      unit[nextUnit].groundAttackRange = 4;
      unit[nextUnit].groundAttackPower = 5;
      unit[nextUnit].airAttackWeapon = AUTOGUN;
      unit[nextUnit].airAttackRange = 1;
      unit[nextUnit].airAttackPower = 2;
      unit[nextUnit].actor = BLUE;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == BLUE_AA)
    {
      unit[nextUnit].type = AA;
      unit[nextUnit].movement = 1;
      unit[nextUnit].armor = 4;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = AUTOGUN;
      unit[nextUnit].groundAttackRange = 1;
      unit[nextUnit].groundAttackPower = 2;
      unit[nextUnit].airAttackWeapon = CANNON;
      unit[nextUnit].airAttackRange = 3;
      unit[nextUnit].airAttackPower = 4;
      unit[nextUnit].actor = BLUE;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == BLUE_SAM)
    {
      unit[nextUnit].type = SAM;
      unit[nextUnit].movement = 2;
      unit[nextUnit].armor = 5;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = AUTOGUN;
      unit[nextUnit].groundAttackRange = 1;
      unit[nextUnit].groundAttackPower = 2;
      unit[nextUnit].airAttackWeapon = MISSILE;
      unit[nextUnit].airAttackRange = 4;
      unit[nextUnit].airAttackPower = 4;
      unit[nextUnit].actor = BLUE;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == BLUE_ATTACKCOPTER)
    {
      unit[nextUnit].type = ATTACKCOPTER;
      unit[nextUnit].movement = 3;
      unit[nextUnit].armor = 5;
      unit[nextUnit].groundUnit = false;
      unit[nextUnit].groundAttackWeapon = AUTOGUN;
      unit[nextUnit].groundAttackRange = 2;
      unit[nextUnit].groundAttackPower = 3;
      unit[nextUnit].airAttackWeapon = MISSILE;
      unit[nextUnit].airAttackRange = 3;
      unit[nextUnit].airAttackPower = 4;
      unit[nextUnit].actor = BLUE;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == BLUE_HEAVYCOPTER)
    {
      unit[nextUnit].type = HEAVYCOPTER;
      unit[nextUnit].movement = 3;
      unit[nextUnit].armor = 5;
      unit[nextUnit].groundUnit = false;
      unit[nextUnit].groundAttackWeapon = MISSILE;
      unit[nextUnit].groundAttackRange = 3;
      unit[nextUnit].groundAttackPower = 5;
      unit[nextUnit].airAttackWeapon = AUTOGUN;
      unit[nextUnit].airAttackRange = 2;
      unit[nextUnit].airAttackPower = 3;
      unit[nextUnit].actor = BLUE;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      nextUnit++;
    }
    else if(j == BLUE_HQ)
    {
      unit[nextUnit].type = HQ;
      unit[nextUnit].movement = 0;
      unit[nextUnit].armor = 9;
      unit[nextUnit].groundUnit = true;
      unit[nextUnit].groundAttackWeapon = 0;
      unit[nextUnit].groundAttackRange = 0;
      unit[nextUnit].groundAttackPower = 0;
      unit[nextUnit].airAttackWeapon = 0;
      unit[nextUnit].airAttackRange = 0;
      unit[nextUnit].airAttackPower = 0;
      unit[nextUnit].actor = BLUE;
      unit[nextUnit].active = true;
      unit[nextUnit].xPosition = i % mapWidth;
      unit[nextUnit].yPosition = i / mapWidth;
      if(unit[nextUnit].xPosition >= (mapWidth / 2)){ unit[nextUnit].orientation = LEFT; }
      unit[nextUnit].hitPoints = unit[nextUnit].armor;
      unit[nextUnit].experience = 0;
      xBlueHQ = unit[nextUnit].xPosition;
      yBlueHQ = unit[nextUnit].yPosition;
      nextUnit++;
    }
  }
}


// drawUnit: Draws a unit of a defined type, controlled by a defined actor at position (x, y). 
// If draw == false, the unit is erased. If drawOnPlayfield == true, the unit is also drawn on the virtual playfield (required for collision detection in battle animations).
void drawUnit(byte type, byte actor, byte orientation, boolean draw, boolean drawOnPlayfield, byte x, byte y)
{
  int arrayOffset = 0;
  byte unitWidth = 0;
  byte unitHeight = 0; 

  if(     type == SCOUT){        arrayOffset = 0; }
  else if(type == LIGHTTANK){    arrayOffset = 11; }
  else if(type == TANK){         arrayOffset = 25; }
  else if(type == HEAVYTANK){    arrayOffset = 42; }
  else if(type == HOWITZER){      arrayOffset = 64; }
  else if(type == LAUNCHER){     arrayOffset = 82; }
  else if(type == AA){           arrayOffset = 104; }
  else if(type == SAM){          arrayOffset = 118;}
  else if(type == ATTACKCOPTER){ arrayOffset = 140;}
  else if(type == HEAVYCOPTER){  arrayOffset = 157;}
  else if(type == HQ){           arrayOffset = 174;}
  unitWidth = byte(pgm_read_byte_near(unitPics + arrayOffset));
  unitHeight = byte(pgm_read_byte_near(unitPics + arrayOffset + 1));

  // Draw unit
  if(orientation == RIGHT)
  {
    for(byte i = 0; i < unitHeight; i++)
    {
      for(byte j = 0; j < unitWidth; j++)
      {
        if(draw)
        {
          switch(int(pgm_read_byte_near(unitPics + arrayOffset + 2 + (i * unitWidth) + j)))
          {
            case W:
              matrix.drawPixel(x + j, y + i, matrix.Color333(3, 3, 3));
              if(drawOnPlayfield){ playfield[x + j][y + i] = 1; }
              break;
            case B:
              if(actor == RED)
              { 
                matrix.drawPixel(x + j, y + i, matrix.Color333(2, 0, 0));
                if(drawOnPlayfield){ playfield[x + j][y + i] = 1; }
              }
              else if(actor == BLUE)
              { 
                matrix.drawPixel(x + j, y + i, matrix.Color333(0, 0, 2));
                if(drawOnPlayfield){ playfield[x + j][y + i] = 1; }
              }
              break;
            case C:
              matrix.drawPixel(x + j, y + i, matrix.Color333(1, 1, 1));
              if(drawOnPlayfield){ playfield[x + j][y + i] = 1; }
              break;
            default:
              break;
          }                  
        }
        else
        {
          matrix.drawPixel(x + j, y + i, matrix.Color333(0, 0, 0));
          if(drawOnPlayfield){ playfield[x + j][y + i] = 0; }
        }
      }
    }
  }
  else if(orientation == LEFT)
  {
    for(byte i = 0; i < unitHeight; i++)
    {
      for(byte j = 0; j < unitWidth; j++)
      for(byte j = unitWidth - 1; j != 255; j--)
      {
        if(draw)
        {
          switch(int(pgm_read_byte_near(unitPics + arrayOffset + 2 + (i * unitWidth) + j)))
          {
            case W:
              matrix.drawPixel(x + unitWidth - 1 - j, y + i, matrix.Color333(3, 3, 3));
              if(drawOnPlayfield){ playfield[x + unitWidth - 1 - j][y + i] = 1; }
              break;
            case B:
              if(actor == RED)
              { 
                matrix.drawPixel(x + unitWidth - 1 - j, y + i, matrix.Color333(2, 0, 0)); 
                if(drawOnPlayfield){ playfield[x + unitWidth - 1 - j][y + i] = 1; }
              }
              else if(actor == BLUE)
              { 
                matrix.drawPixel(x + unitWidth - 1 - j, y + i, matrix.Color333(0, 0, 2)); 
                if(drawOnPlayfield){ playfield[x + unitWidth - 1 - j][y + i] = 1; }
              }
              break;
            case C:
              matrix.drawPixel(x + unitWidth - 1 - j, y + i, matrix.Color333(1, 1, 1));
              if(drawOnPlayfield){ playfield[x + unitWidth - 1 - j][y + i] = 1; }
              break;
            default:
              break;
          }                  
        }
        else
        {
          matrix.drawPixel(x + unitWidth - 1 - j, y + i, matrix.Color333(0, 0, 0));
          if(drawOnPlayfield){ playfield[x + unitWidth - 1 - j][y + i] = 0; }
        }
      }
    }
  }
}


// getMapSurface: Get surface from current map at position (_x, _y) (e.g. WOOD)
byte getMapSurface(byte _x, byte _y)
{
  if(_x < 0 || _x >= mapWidth || _y < 0 || _y >= mapHeight)
  {
    return 255; // Notifies wrong access
  }
  return currentMap[(_y * mapWidth) + _x];
}


// getUnit: Get index unit from current map at position (_x, _y). Returns 255 if no valid and active unit can be found.
byte getUnit(byte _x, byte _y)
{
  if(_x < 0 || _x >= mapWidth || _y < 0 || _y >= mapHeight)
  {
    return 255; // Notifies wrong access
  }
  for(byte i = 0; i < numberOfUnits; i++)
  {
    if(unit[i].xPosition == _x && unit[i].yPosition == _y && unit[i].active)
    {
      return i;
    }
  }
  return 255;
}


// drawMapComplete: Draws the complete map with position (x, y) in the upper left corner. Removes map if draw == false.
void drawMapComplete(byte x, byte y, int offsetX, int offsetY, boolean draw)
{
  for(byte j = max(0, y - 1); j < min(mapHeight, y + 6); j++)
  {
    for(byte i = max(0, x - 1); i < min(mapWidth, x + 6); i++)
    {
      drawMapTile(x, y, i, j, offsetX, offsetY, draw, true);

      // Draw overlay if unit selected
      if(unitSelected && overlaySelectedUnit && i == unit[selectedUnit].xPosition && j == unit[selectedUnit].yPosition){ drawOverlay(x, y, i, j, offsetX, offsetY, SELECTEDUNIT, draw); }
      if(unitSelected && overlayMove && canMove(selectedUnit, i, j)){ drawOverlay(x, y, i, j, offsetX, offsetY, MOVEMENT, draw); }
      byte l = getUnit(i, j);
      if(l != 255 && unit[l].active)
      { 
        if(overlaySettled && unit[l].settled){ drawOverlay(x, y, i, j, offsetX, offsetY, SETTLED, draw); }          
      }
      if(unitSelected && overlayTarget && canHit(selectedUnit, i, j)){ drawOverlay(x, y, i, j, offsetX, offsetY, TARGET, draw); }
    }     
  }       
}

void drawWood(byte x, byte y, byte i, byte j, int offsetX, int offsetY, boolean draw)
{
//  if(draw){ matrix.drawRect(offsetX + 2 + (i - x) * 8, offsetY + 2 + (j - y) * 8, 4, 4, matrix.Color333(0, 1, 0)); }
//  else{ matrix.drawRect(offsetX + 2 + (i - x) * 8, offsetY + 2 + (j - y) * 8, 4, 4, matrix.Color333(0, 0, 0)); }  
  if(draw)
  {     
    matrix.drawPixel(offsetX + 3 + (i - x) * 8, offsetY + 2 + (j - y) * 8, matrix.Color333(0, 1, 0));
    matrix.drawPixel(offsetX + 4 + (i - x) * 8, offsetY + 2 + (j - y) * 8, matrix.Color333(0, 4, 0));
    matrix.drawPixel(offsetX + 2 + (i - x) * 8, offsetY + 3 + (j - y) * 8, matrix.Color333(0, 1, 0));
    matrix.drawPixel(offsetX + 3 + (i - x) * 8, offsetY + 3 + (j - y) * 8, matrix.Color333(0, 4, 0));
    matrix.drawPixel(offsetX + 4 + (i - x) * 8, offsetY + 3 + (j - y) * 8, matrix.Color333(0, 1, 0));
    matrix.drawPixel(offsetX + 5 + (i - x) * 8, offsetY + 3 + (j - y) * 8, matrix.Color333(0, 4, 0));
    matrix.drawPixel(offsetX + 2 + (i - x) * 8, offsetY + 4 + (j - y) * 8, matrix.Color333(0, 4, 0));
    matrix.drawPixel(offsetX + 3 + (i - x) * 8, offsetY + 4 + (j - y) * 8, matrix.Color333(0, 1, 0));
    matrix.drawPixel(offsetX + 4 + (i - x) * 8, offsetY + 4 + (j - y) * 8, matrix.Color333(0, 4, 0));
    matrix.drawPixel(offsetX + 5 + (i - x) * 8, offsetY + 4 + (j - y) * 8, matrix.Color333(0, 1, 0));
    matrix.drawPixel(offsetX + 3 + (i - x) * 8, offsetY + 5 + (j - y) * 8, matrix.Color333(0, 4, 0));
    matrix.drawPixel(offsetX + 4 + (i - x) * 8, offsetY + 5 + (j - y) * 8, matrix.Color333(0, 1, 0));
  }
  else{ matrix.fillRect(offsetX + 2 + (i - x) * 8, offsetY + 2 + (j - y) * 8, 4, 4, matrix.Color333(0, 0, 0)); }  
}

void drawRumble(byte x, byte y, byte i, byte j, int offsetX, int offsetY, boolean draw)
{
//  if(draw){ matrix.drawRect(offsetX + 2 + (i - x) * 8, offsetY + 2 + (j - y) * 8, 4, 4, matrix.Color333(2, 1, 0)); }
//  else{ matrix.drawRect(offsetX + 2 + (i - x) * 8, offsetY + 2 + (j - y) * 8, 4, 4, matrix.Color333(0, 0, 0)); }  
  if(draw)
  {     
    matrix.drawPixel(offsetX + 2 + (i - x) * 8, offsetY + 2 + (j - y) * 8, matrix.Color333(2, 1, 0));
    matrix.drawPixel(offsetX + 5 + (i - x) * 8, offsetY + 2 + (j - y) * 8, matrix.Color333(3, 2, 0));
    matrix.drawPixel(offsetX + 3 + (i - x) * 8, offsetY + 3 + (j - y) * 8, matrix.Color333(3, 2, 0));
    matrix.drawPixel(offsetX + 4 + (i - x) * 8, offsetY + 3 + (j - y) * 8, matrix.Color333(2, 1, 0));
    matrix.drawPixel(offsetX + 3 + (i - x) * 8, offsetY + 4 + (j - y) * 8, matrix.Color333(2, 1, 0));
    matrix.drawPixel(offsetX + 4 + (i - x) * 8, offsetY + 4 + (j - y) * 8, matrix.Color333(3, 2, 0));
    matrix.drawPixel(offsetX + 2 + (i - x) * 8, offsetY + 5 + (j - y) * 8, matrix.Color333(3, 2, 0));
    matrix.drawPixel(offsetX + 5 + (i - x) * 8, offsetY + 5 + (j - y) * 8, matrix.Color333(2, 1, 0));
  }
  else{ matrix.fillRect(offsetX + 2 + (i - x) * 8, offsetY + 2 + (j - y) * 8, 4, 4, matrix.Color333(0, 0, 0)); }  
}

// drawMapTile: Draws a single tile of the map with position (x, y) in the upper left corner. 
// (i, j) is the tile position on the map. Offset shifts by single pixels. Removes tile if draw == false. Draws unit if drawTileAndUnit == true.
void drawMapTile(byte x, byte y, byte i, byte j, int offsetX, int offsetY, boolean draw, boolean drawTileAndUnit)
{
  if(getMapSurface(i, j) == WOOD)
  {
    drawWood(x, y, i, j, offsetX, offsetY, draw);
  }
  if(getMapSurface(i, j) == RUMBLE)
  {
    drawRumble(x, y, i, j, offsetX, offsetY, draw);
  }
  byte k = getUnit(i, j);
  if(k != 255 && unit[k].active && drawTileAndUnit)
  {
    if(unit[k].orientation == RIGHT)
    {
      if(unit[k].type == SCOUT || unit[k].type == LIGHTTANK){ drawUnit(unit[k].type, unit[k].actor, unit[k].orientation, draw, false, offsetX + 2 + (i - x) * 8, offsetY + 3 + (j - y) * 8); }
      else if(unit[k].type == TANK || unit[k].type == ATTACKCOPTER || unit[k].type == HEAVYCOPTER){ drawUnit(unit[k].type, unit[k].actor, unit[k].orientation, draw, false, offsetX + 1 + (i - x) * 8, offsetY + 3 + (j - y) * 8); }
      else if(unit[k].type == HOWITZER || unit[k].type == AA || unit[k].type == HEAVYCOPTER){ drawUnit(unit[k].type, unit[k].actor, unit[k].orientation, draw, false, offsetX + 2 + (i - x) * 8, offsetY + 2 + (j - y) * 8); }
      else { drawUnit(unit[k].type, unit[k].actor, unit[k].orientation, draw, false, offsetX + 1 + (i - x) * 8, offsetY + 2 + (j - y) * 8); }
    }
    else
    {
      if(unit[k].type == SCOUT){ drawUnit(unit[k].type, unit[k].actor, unit[k].orientation, draw, false, offsetX + 3 + (i - x) * 8, offsetY + 3 + (j - y) * 8); }
      else if(unit[k].type == LIGHTTANK){ drawUnit(unit[k].type, unit[k].actor, unit[k].orientation, draw, false, offsetX + 2 + (i - x) * 8, offsetY + 3 + (j - y) * 8); }
      else if(unit[k].type == TANK || unit[k].type == ATTACKCOPTER || unit[k].type == HEAVYCOPTER){ drawUnit(unit[k].type, unit[k].actor, unit[k].orientation, draw, false, offsetX + 2 + (i - x) * 8, offsetY + 3 + (j - y) * 8); }
      else if(unit[k].type == HOWITZER || unit[k].type == AA || unit[k].type == HEAVYCOPTER){ drawUnit(unit[k].type, unit[k].actor, unit[k].orientation, draw, false, offsetX + 2 + (i - x) * 8, offsetY + 2 + (j - y) * 8); }
      else { drawUnit(unit[k].type, unit[k].actor, unit[k].orientation, draw, false, offsetX + 2 + (i - x) * 8, offsetY + 2 + (j - y) * 8); }
    }
  }
}


// drawOverlay: Draws an overlay with type = (MOVEMENT, SETTLED, TARGET) with position (x, y) in the upper left corner. 
// (i, j) is the tile position on the map. Offset shifts by single pixels. Removes tile if draw == false.
void drawOverlay(byte x, byte y, byte i, byte j, int offsetX, int offsetY, byte type, boolean draw)
{
  byte r = 0;
  byte g = 0;
  byte b = 0;

  if(draw && type == SELECTEDUNIT){ r = 3; g = 3; b = 3; }
  else if(draw && type == MOVEMENT){ r = 2; g = 2; }
  else if(draw && type == SETTLED){ g = 2; }
  else if(draw && type == TARGET){ r = 2; }

  if(type == SELECTEDUNIT || !draw)
  {
    matrix.drawLine(offsetX + 2 + (i - x) * 8, offsetY + (j - y) * 8, offsetX + 5 + (i - x) * 8, offsetY + (j - y) * 8, matrix.Color333(r, g, b));
    matrix.drawLine(offsetX + (i - x) * 8, offsetY + 2 + (j - y) * 8, offsetX + (i - x) * 8, offsetY + 5 + (j - y) * 8, matrix.Color333(r, g, b));
    matrix.drawLine(offsetX + 7 + (i - x) * 8, offsetY + 2 + (j - y) * 8, offsetX + 7 + (i - x) * 8, offsetY + 5 + (j - y) * 8, matrix.Color333(r, g, b));
    matrix.drawLine(offsetX + 2 + (i - x) * 8, offsetY + 7 + (j - y) * 8, offsetX + 5 + (i - x) * 8, offsetY + 7 + (j - y) * 8, matrix.Color333(r, g, b));
  }
  if(type != SELECTEDUNIT || !draw)
  {
    matrix.drawPixel(offsetX + (i - x) * 8, offsetY + (j - y) * 8, matrix.Color333(r, g, b));
    matrix.drawPixel(offsetX + 7 + (i - x) * 8, offsetY + (j - y) * 8, matrix.Color333(r, g, b));
    matrix.drawPixel(offsetX + 7 + (i - x) * 8, offsetY + 7 + (j - y) * 8, matrix.Color333(r, g, b));
    matrix.drawPixel(offsetX + (i - x) * 8, offsetY + 7 + (j - y) * 8, matrix.Color333(r, g, b));
  }
}


// removeAllOverlays: Removes all overlays without removing anything else.
void removeAllOverlays(byte x, byte y, int offsetX, int offsetY)
{
  for(byte j = max(0, y - 1); j < min(mapHeight, y + 6); j++)
  {
    for(byte i = max(0, x - 1); i < min(mapWidth, x + 6); i++)
    {
      drawOverlay(x, y, i, j, offsetX, offsetY, 0, false); 
    }     
  }       
}


// distance: Returns the Manhattan distance between (aX, aY) and (bX, bY).
byte distance(int aX, int aY, int bX, int bY)
{
  return (abs(aX - bX) + abs(aY - bY));
}


// getRequiredMovementPoints: Returns the number of required movement points. Prerequisite of correct values is a calculation of distanceMap for the current unit (with kind of Dijkstra algorithm).
byte getRequiredMovementPoints(byte x, byte y)
{
  if(x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
  {
    return 255; // Notifies wrong access
  }
  return distanceMap[(y * mapWidth) + x];
}


// setRequiredMovementPoints: Returns the number of required movement points. Prerequisite of correct values is a calculation of distanceMap for the current unit (with kind of Dijkstra algorithm).
boolean setRequiredMovementPoints(byte x, byte y, byte distance)
{
  if(x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
  {
    return false; // Notifies wrong access
  }
  distanceMap[(y * mapWidth) + x] = distance;
  return true;
}


// isOut: Returns true if closest distance to (x, y) has alreday been determined (with kind of Dijkstra algorithm).
boolean isOut(byte x, byte y)
{
  return outMap[(y * mapWidth) + x];
}
// setOut: Returns true if closest distance to (x, y) has alreday been determined (with kind of Dijkstra algorithm).
boolean setOut(byte x, byte y, boolean out)
{
  if(x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
  {
    return false; // Notifies wrong access
  }
  outMap[(y * mapWidth) + x] = out;
  return true;
}


// canMove: Returns whether _selectedUnit can move to field (i, j).
boolean canMove(byte _selectedUnit, byte i, byte j)
{
  // Already there? (return false)
  if(unit[_selectedUnit].xPosition == i && unit[_selectedUnit].yPosition == j){ return false; }

  // Field is rumble and unit cannot fly
//  if(unit[_selectedUnit].groundUnit && getMapSurface(i, j) == RUMBLE){ return false; }

  // Field is occupied by another unit (return false)
  for(byte k = 0; k < numberOfUnits; k++)
  {
    if(_selectedUnit != k)
    {
      if(unit[k].xPosition == i && unit[k].yPosition == j && unit[k].active){ return false; }
    }
  }

  // Field is wood (requires 1 extra movement point if not neighbored)
//  if(getMapSurface(i, j) == RUMBLE)
//  {
//    if(distance(i, j, unit[_selectedUnit].xPosition, unit[_selectedUnit].yPosition) == 1){ return true; }
//    if((distance(i, j, unit[_selectedUnit].xPosition, unit[_selectedUnit].yPosition) + 1) <= unit[_selectedUnit].movement){ return true; } 
//  }
//  else
//  {
//    if(distance(i, j, unit[_selectedUnit].xPosition, unit[_selectedUnit].yPosition) <= unit[_selectedUnit].movement){ return true; }
//  }

  // Field is in distance according to distanceMap (return true)
  if(getRequiredMovementPoints(i, j) <= unit[_selectedUnit].movement){ return true; }

  // Field has distance 1 to enable slow units to enter woods (return true)
  if(distance(i, j, unit[_selectedUnit].xPosition, unit[_selectedUnit].yPosition) == 1 && getMapSurface(i, j) == WOOD){ return true; }

  return false;
}


// canHit: Returns whether _selectedUnit can hit the unit in field (i, j). If (i, j) is empty, false is returned.
boolean canHit(byte _selectedUnit, byte i, byte j)
{
  byte k = getUnit(i, j);
  if(k == 255){ return false; }
  if(!unit[k].active){ return false; }
  if(_selectedUnit == k){ return false; }

  if(unit[_selectedUnit].actor == RED && unit[k].actor == RED ){ return false; }
  if(unit[_selectedUnit].actor == BLUE && unit[k].actor == BLUE){ return false; }

  if(unit[k].groundUnit)
  {
    if(distance(unit[_selectedUnit].xPosition, unit[_selectedUnit].yPosition, unit[k].xPosition, unit[k].yPosition) <= unit[_selectedUnit].groundAttackRange){ return true; }
  }
  else
  {
    if(distance(unit[_selectedUnit].xPosition, unit[_selectedUnit].yPosition, unit[k].xPosition, unit[k].yPosition) <= unit[_selectedUnit].airAttackRange){ return true; }        
  }
  return false;
}


// anyTargetInRange: Returns true, if there is any unit that is in fire range of _selectedUnit.
boolean anyTargetInRange(byte _selectedUnit)
{
  for(byte i = 0; i < numberOfUnits; i++)
  {
    if(canHit(_selectedUnit, unit[i].xPosition, unit[i].yPosition))
    {
      return true;
    }
  }
  return false;
}


// drawCursor: Draws cursor to select units to ( _cursorX, _cursorY). Offset shifts by single pixels. If draw == false, the cursor is erased.
void drawCursor(byte _cursorX, byte _cursorY, int offsetX, int offsetY, boolean draw)
{
  byte posX = _cursorX - upperLeftMapCornerX;
  byte posY = _cursorY - upperLeftMapCornerY;
  byte intensity = 0;

  if(draw){ intensity = 3; }

  matrix.drawPixel(offsetX + (posX * 8) + 1, offsetY + (posY * 8), matrix.Color333(intensity, intensity, intensity));
  matrix.drawPixel(offsetX + (posX * 8), offsetY + (posY * 8) + 1, matrix.Color333(intensity, intensity, intensity));

  matrix.drawPixel(offsetX + (posX * 8) + 6, offsetY + (posY * 8), matrix.Color333(intensity, intensity, intensity));
  matrix.drawPixel(offsetX + (posX * 8) + 7, offsetY + (posY * 8) + 1, matrix.Color333(intensity, intensity, intensity));

  matrix.drawPixel(offsetX + (posX * 8) + 7, offsetY + (posY * 8) + 6, matrix.Color333(intensity, intensity, intensity));
  matrix.drawPixel(offsetX + (posX * 8) + 6, offsetY + (posY * 8) + 7, matrix.Color333(intensity, intensity, intensity));

  matrix.drawPixel(offsetX + (posX * 8) + 1, offsetY + (posY * 8) + 7, matrix.Color333(intensity, intensity, intensity));
  matrix.drawPixel(offsetX + (posX * 8), offsetY + (posY * 8) + 6, matrix.Color333(intensity, intensity, intensity));
}


// scrollMap: Scrolls complete visible map to the new cursor position (defined by global variables)
void scrollMap(byte scrollSteps)
{
  byte x = upperLeftMapCornerX;
  byte y = upperLeftMapCornerY;
  int offsetX = 0;
  int offsetY = 0;
  int tileOffsetX = 0;
  int tileOffsetY = 0;

  for(byte k = 0; k < scrollSteps; k++)
  { 
    // Update resetAndPauseTimer   
    resetAndPauseTimer.update();
    if(reset){ break; }
    
    if(upperLeftMapCornerXNew > upperLeftMapCornerX){ tileOffsetX = offsetX - (8 / scrollSteps); }
    else if(upperLeftMapCornerXNew < upperLeftMapCornerX){ tileOffsetX = offsetX + (8 / scrollSteps); }
        
    if(upperLeftMapCornerYNew > upperLeftMapCornerY){ tileOffsetY = offsetY - (8 / scrollSteps); }
    else if(upperLeftMapCornerYNew < upperLeftMapCornerY){ tileOffsetY = offsetY + (8 / scrollSteps); }

    for(byte j = max(0, y - 1); j < min(mapHeight, y + 6); j++)
    {
      for(byte i = max(0, x - 1); i < min(mapWidth, x + 6); i++)
      {
        drawOverlay(x, y, i, j, offsetX, offsetY, 0, false);
        drawMapTile(x, y, i, j, offsetX, offsetY, false, true);

        drawMapTile(x, y, i, j, tileOffsetX, tileOffsetY, true, true);
        if(unitSelected && overlaySelectedUnit && i == unit[selectedUnit].xPosition && j == unit[selectedUnit].yPosition){ drawOverlay(x, y, i, j, tileOffsetX, tileOffsetY, SELECTEDUNIT, true); }
        if(unitSelected && overlayMove && canMove(selectedUnit, i, j)){ drawOverlay(x, y, i, j, tileOffsetX, tileOffsetY, MOVEMENT, true); }
        byte l = getUnit(i, j);
        if(l != 255 && unit[l].active)
        { 
          if(overlaySettled && unit[l].settled){ drawOverlay(x, y, i, j, tileOffsetX, tileOffsetY, SETTLED, true); }          
        }
        if(unitSelected && overlayTarget && canHit(selectedUnit, i, j)){ drawOverlay(x, y, i, j, tileOffsetX, tileOffsetY, TARGET, true); }
      }
    }
    // Remove cursor from old position    
    drawCursor(cursorXNew, cursorYNew, offsetX, offsetY, false);

    if(upperLeftMapCornerXNew > upperLeftMapCornerX){ offsetX = offsetX - (8 / scrollSteps); }
    else if(upperLeftMapCornerXNew < upperLeftMapCornerX){ offsetX = offsetX + (8 / scrollSteps);}
    
    if(upperLeftMapCornerYNew > upperLeftMapCornerY){ offsetY = offsetY - (8 / scrollSteps); }
    else if(upperLeftMapCornerYNew < upperLeftMapCornerY){ offsetY = offsetY + (8 / scrollSteps);}    

    // Draw cursor at new position
    drawCursor(cursorXNew, cursorYNew, offsetX, offsetY, true);  
  }
}


// initiativeDiceRoll: Makes an initiative roll for all units.
void initiativeDiceRoll()
{
  for(byte i = 0; i < numberOfUnits; i++)
  {
    unit[i].initiativeValue = random(256);
    unit[i].settled = false;
  }
}


// getNextUnit: Returns the index of the unit with the next best initiative. Returns 255 if no next unit available.
byte getNextUnit()
{
  byte nextUnit = 255;
  byte bestInitiativeValue = 0;
  
  for(byte i = 0; i < numberOfUnits; i++)
  {
    if(unit[i].initiativeValue >= bestInitiativeValue && !unit[i].settled && unit[i].active && unit[i].type != HQ)
    {
      bestInitiativeValue = unit[i].initiativeValue;
      nextUnit = i;
    }
  }

  return nextUnit;
}


// moveMapToUnit: Scrolls map to the next active unit (scrollSteps defines the number of pixels per scroll step).
void moveMapToUnit(byte _currentUnit, byte scrollSteps)
{
  boolean updateMap = true;
  boolean updateCursor = true;

  unitSelected = false;
  cursorXNew = cursorX;
  cursorYNew = cursorY;
  upperLeftMapCornerXNew = upperLeftMapCornerX;
  upperLeftMapCornerYNew = upperLeftMapCornerY;
  overlaySelectedUnit = false;
  updateMap = true;
    
  do
  {
    // Update resetAndPauseTimer   
    resetAndPauseTimer.update();
    if(reset){ break; }
    
    if(updateMap)
    {
      if(upperLeftMapCornerX != upperLeftMapCornerXNew || upperLeftMapCornerY != upperLeftMapCornerYNew)
      {
        drawCursor(cursorX, cursorY, 0, 0,false);
        scrollMap(scrollSteps);
      }
      drawMapComplete(upperLeftMapCornerXNew, upperLeftMapCornerYNew, 0, 0, true);
      upperLeftMapCornerX = upperLeftMapCornerXNew;
      upperLeftMapCornerY = upperLeftMapCornerYNew;
    }
    if(updateCursor)
    {
      drawCursor(cursorX, cursorY, 0, 0,false);
      drawCursor(cursorXNew, cursorYNew, 0, 0, true);
      updateCursor = false;
      cursorX = cursorXNew;
      cursorY = cursorYNew;
      
      if(!updateMap){ delay(250); }
    }
    updateMap = false;
    
    if(cursorY > unit[_currentUnit].yPosition)
    {
      cursorYNew = cursorY - 1;
      updateCursor = true;
      if(upperLeftMapCornerY > (cursorYNew - 1) && (upperLeftMapCornerY > 0))
      { 
        upperLeftMapCornerYNew = upperLeftMapCornerY - 1;
        updateMap = true;
      }
    }
    else if(cursorY < unit[_currentUnit].yPosition)
    {
      cursorYNew = cursorY + 1;
      updateCursor = true;
      if((upperLeftMapCornerY + 2 ) < cursorYNew && ((upperLeftMapCornerY + 4) < mapHeight))
      { 
        upperLeftMapCornerYNew = upperLeftMapCornerY + 1;
        updateMap = true;
      }
    }    
    
    if(cursorX > unit[_currentUnit].xPosition)
    {
      cursorXNew = cursorX - 1;
      updateCursor = true;
      if(upperLeftMapCornerX > (cursorXNew - 1) && (upperLeftMapCornerX > 0))
      { 
        upperLeftMapCornerXNew = upperLeftMapCornerX - 1;
        updateMap = true;
      }
    }        
    else if(cursorX < unit[_currentUnit].xPosition)
    {
      cursorXNew = cursorX + 1;
      updateCursor = true;
      if((upperLeftMapCornerX + 2 ) < cursorXNew && ((upperLeftMapCornerX + 4) < mapWidth))
      { 
        upperLeftMapCornerXNew = upperLeftMapCornerX + 1;
        updateMap = true;
      }
    }
  }
  while(!(cursorX == unit[_currentUnit].xPosition && cursorY == unit[_currentUnit].yPosition) && !reset);
  delay(500);
}


// moveMapToField: Scrolls map to a defined target field (scrollSteps defines the number of pixels per scroll step).
void moveMapToField(byte xPosition, byte yPosition, byte scrollSteps, byte type)
{
  boolean updateMap = true;
  boolean updateCursor = true;

  unitSelected = true;
  cursorXNew = cursorX;
  cursorYNew = cursorY;
  upperLeftMapCornerXNew = upperLeftMapCornerX;
  upperLeftMapCornerYNew = upperLeftMapCornerY;
  overlaySelectedUnit = true;
  if(type == MOVE)
  {
    overlayMove = true;
    overlayTarget = false;    
  }
  else if(type == AIM)
  {
    overlayMove = false;
    overlayTarget = true;        
  }  
  overlaySettled = true;
  updateMap = true;
    
  do
  {
    // Update resetAndPauseTimer   
    resetAndPauseTimer.update();
    if(reset){ break; }
    
    if(updateMap)
    {
      if(upperLeftMapCornerX != upperLeftMapCornerXNew || upperLeftMapCornerY != upperLeftMapCornerYNew)
      {
        drawCursor(cursorX, cursorY, 0, 0,false);
        scrollMap(scrollSteps);
      }
      drawMapComplete(upperLeftMapCornerXNew, upperLeftMapCornerYNew, 0, 0, true);
      upperLeftMapCornerX = upperLeftMapCornerXNew;
      upperLeftMapCornerY = upperLeftMapCornerYNew;
    }
    if(updateCursor)
    {
      drawCursor(cursorX, cursorY, 0, 0,false);
      drawCursor(cursorXNew, cursorYNew, 0, 0, true);
      updateCursor = false;
      cursorX = cursorXNew;
      cursorY = cursorYNew;
      
      if(!updateMap){ delay(250); }
    }
    updateMap = false;
    
    if(cursorY > yPosition)
    {
      cursorYNew = cursorY - 1;
      updateCursor = true;
      if(upperLeftMapCornerY > (cursorYNew - 1) && (upperLeftMapCornerY > 0))
      { 
        upperLeftMapCornerYNew = upperLeftMapCornerY - 1;
        updateMap = true;
      }
    }
    else if(cursorY < yPosition)
    {
      cursorYNew = cursorY + 1;
      updateCursor = true;
      if((upperLeftMapCornerY + 2 ) < cursorYNew && ((upperLeftMapCornerY + 4) < mapHeight))
      { 
        upperLeftMapCornerYNew = upperLeftMapCornerY + 1;
        updateMap = true;
      }
    }    
    
    if(cursorX > xPosition)
    {
      cursorXNew = cursorX - 1;
      updateCursor = true;
      if(upperLeftMapCornerX > (cursorXNew - 1) && (upperLeftMapCornerX > 0))
      { 
        upperLeftMapCornerXNew = upperLeftMapCornerX - 1;
        updateMap = true;
      }
    }        
    else if(cursorX < xPosition)
    {
      cursorXNew = cursorX + 1;
      updateCursor = true;
      if((upperLeftMapCornerX + 2 ) < cursorXNew && ((upperLeftMapCornerX + 4) < mapWidth))
      { 
        upperLeftMapCornerXNew = upperLeftMapCornerX + 1;
        updateMap = true;
      }
    }
  }
  while(!(cursorX == xPosition && cursorY == yPosition) && !reset);
  delay(500);
}


// joystickInquiry: Returns for joystick (1 or 2) the detected action.
byte joystickInquiry(byte joystick)
{
  if(joystick == 1)
  {
    if(joy1Up() && !joy1Left() && !joy1Right()){ return UP; }
    else if(joy1Up() && joy1Right()){ return UP_RIGHT; }
    else if(joy1Right() && !joy1Up() && !joy1Down()){ return RIGHT; }
    else if(joy1Down() && joy1Right()){ return DOWN_RIGHT; }
    else if(joy1Down() && !joy1Left() && !joy1Right()){ return DOWN; }
    else if(joy1Down() && joy1Left()){ return DOWN_LEFT; }
    else if(joy1Left() && !joy1Up() && !joy1Down()){ return LEFT; }
    else if(joy1Up() && joy1Left()){ return UP_LEFT; }
    else if(joy1FireA())
    {
      int counter = 0;
      do
      {          
        if(counter == 800 && !joy1IsLEDmePlayJoypad()){ return FIRE_B; }
        counter++;
        delay(1);          
      }
      while(joy1FireA());
      return FIRE_A;
    }
    else if(joy1FireB()){ return FIRE_B; }
  }
  else if(joystick == 2)
  {
    if(joy2Up() && !joy2Left() && !joy2Right()){ return UP; }
    else if(joy2Up() && joy2Right()){ return UP_RIGHT; }
    else if(joy2Right() && !joy2Up() && !joy2Down()){ return RIGHT; }
    else if(joy2Down() && joy2Right()){ return DOWN_RIGHT; }
    else if(joy2Down() && !joy2Left() && !joy2Right()){ return DOWN; }
    else if(joy2Down() && joy2Left()){ return DOWN_LEFT; }
    else if(joy2Left() && !joy2Up() && !joy2Down()){ return LEFT; }
    else if(joy2Up() && joy2Left()){ return UP_LEFT; }
    else if(joy2FireA())
    {
      int counter = 0;
      do
      {
        if(counter == 800 && !joy2IsLEDmePlayJoypad()){ return FIRE_B; }
        counter++;
        delay(1);          
      }
      while(joy2FireA());
      return FIRE_A;
    }    
    else if(joy2FireB()){ return FIRE_B; }
  }
}


float calculateExpectedProvokedDamage(Unit &bufferUnit, byte u)
{
  float expectedProvokedDamage = 0;
  float provokedDamageToOneUnit = 0;
  
  for(byte i = 0; i < numberOfUnits; i++)
  {
    provokedDamageToOneUnit = 0;
    if(unit[i].active && i != u)
    {
      if((bufferUnit.actor == RED && unit[i].actor != RED) || (bufferUnit.actor == BLUE && unit[i].actor != BLUE))
      {
        if(unit[i].groundUnit)
        {
          if(distance(bufferUnit.xPosition, bufferUnit.yPosition, unit[i].xPosition, unit[i].yPosition) <= bufferUnit.groundAttackRange)
          {
            provokedDamageToOneUnit = (1.0 - (float(calculateHitValue(bufferUnit, unit[i])) / 100.0)) * bufferUnit.groundAttackPower;
            if(provokedDamageToOneUnit >= unit[i].hitPoints){ provokedDamageToOneUnit = provokedDamageToOneUnit * 2; }
          }
        }
        else
        {
          if(distance(bufferUnit.xPosition, bufferUnit.yPosition, unit[i].xPosition, unit[i].yPosition) <= bufferUnit.airAttackRange)
          {
            provokedDamageToOneUnit = (1.0 - (float(calculateHitValue(bufferUnit, unit[i])) / 100.0)) * bufferUnit.airAttackPower;
            if(provokedDamageToOneUnit >= unit[i].hitPoints){ provokedDamageToOneUnit = provokedDamageToOneUnit * 2; }
          }        
        }
        if(provokedDamageToOneUnit > expectedProvokedDamage){ expectedProvokedDamage = provokedDamageToOneUnit; } 
      }      
    }      
  }  
  return expectedProvokedDamage;
}

float calculateExpectedTakenDamage(Unit &bufferUnit, byte u)
{
  float expectedTakenDamage = 0;
  float takenDamageByOneUnit = 0;
  
  for(byte i = 0; i < numberOfUnits; i++)
  {
    takenDamageByOneUnit = 0;
    if(unit[i].active && i != u)
    {
      if((bufferUnit.actor == RED && unit[i].actor != RED) || (bufferUnit.actor == BLUE && unit[i].actor != BLUE))
      {
        if(bufferUnit.groundUnit)
        {
          if(distance(unit[i].xPosition, unit[i].yPosition, bufferUnit.xPosition, bufferUnit.yPosition) <= unit[i].groundAttackRange)
          {
            takenDamageByOneUnit = (1.0 - (float(calculateHitValue(unit[i], bufferUnit)) / 100.0)) * unit[i].groundAttackPower;
          }
        }
        else
        {
          if(distance(unit[i].xPosition, unit[i].yPosition, bufferUnit.xPosition, bufferUnit.yPosition) <= unit[i].airAttackRange)
          {
            takenDamageByOneUnit = (1.0 - (float(calculateHitValue(unit[i], bufferUnit)) / 100.0)) * unit[i].airAttackPower;
          }        
        }
        expectedTakenDamage = expectedTakenDamage + takenDamageByOneUnit; 
      }      
    }      
  }  
  return expectedTakenDamage;
}

void copyUnitToBuffer(byte u)
{
  bufferUnit.type = unit[u].type;
  bufferUnit.movement = unit[u].movement;
  bufferUnit.armor = unit[u].armor;
  bufferUnit.groundUnit = unit[u].groundUnit;
  bufferUnit.groundAttackWeapon = unit[u].groundAttackWeapon;
  bufferUnit.groundAttackRange = unit[u].groundAttackRange;
  bufferUnit.groundAttackPower = unit[u].groundAttackPower;
  bufferUnit.airAttackWeapon = unit[u].airAttackWeapon;
  bufferUnit.airAttackRange = unit[u].airAttackRange;
  bufferUnit.airAttackPower = unit[u].airAttackPower;
  
  bufferUnit.hitPoints = unit[u].hitPoints;
  bufferUnit.experience = unit[u].experience;
  bufferUnit.initiativeValue = unit[u].initiativeValue;
  
  bufferUnit.actor = unit[u].actor;
  bufferUnit.active = unit[u].active;
  bufferUnit.settled = unit[u].settled;
  bufferUnit.fieldsMoved = unit[u].fieldsMoved;
  bufferUnit.xPosition = unit[u].xPosition;
  bufferUnit.yPosition = unit[u].yPosition;
  bufferUnit.orientation = unit[u].orientation;
}


// computerMove: The computer makes a move.
byte computerMove(byte u, byte type)
{
  for(byte i = 0; i < numberOfTargetFields; i++)
  {
    targetField[i].xPosition = 0;
    targetField[i].yPosition = 0;
    targetField[i].expectedProvokedDamage = 0;
    targetField[i].expectedTakenDamage = 0;
    targetField[i].enemyHQDistance = 0;
    targetField[i].ownHQDistance = 0;
    targetField[i].assessment = 0;
  }
  targetFieldCounter = 0;

  copyUnitToBuffer(u);

  // Check if own HQ is threatened
  boolean ownHQThreatened = false;
  byte distanceHQs = distance(xRedHQ, yRedHQ, xBlueHQ, yBlueHQ);
  if(bufferUnit.actor == RED)
  {
    for(byte i = 0; i < numberOfUnits; i++)
    {
      if(unit[i].actor == BLUE)
      {
        if(unit[i].active && distance(unit[i].xPosition, unit[i].yPosition, xRedHQ, yRedHQ) <= (distanceHQs / 3)){ ownHQThreatened = true; }
      }
    }
  }
  else if(bufferUnit.actor == BLUE)
  {
    for(byte i = 0; i < numberOfUnits; i++)
    {
      if(unit[i].actor == RED)
      {
        if(unit[i].active && distance(unit[i].xPosition, unit[i].yPosition, xBlueHQ, yBlueHQ) <= (distanceHQs / 3)){ ownHQThreatened = true; }
      }
    }    
  }

  if(type == MOVE)
  {
    for(byte i = max(0, int(unit[u].xPosition - unit[u].movement)); i < min(mapWidth, unit[u].xPosition + unit[u].movement + 1); i++)
    {
      for(byte j = max(0, int(unit[u].yPosition - unit[u].movement)); j < min(mapHeight, unit[u].yPosition + unit[u].movement + 1); j++)
      {
        if(canMove(u, i, j) || (unit[u].xPosition == i && unit[u].yPosition == j)) // canMove returns false for the current field. The OR statement anyway allows to step into the following block.
        {
          bufferUnit.fieldsMoved = distance(unit[u].xPosition, unit[u].yPosition, i, j); // Here the distance counts, not the number of used movement points
          bufferUnit.xPosition = i;
          bufferUnit.yPosition = j;
          targetField[targetFieldCounter].xPosition = i;
          targetField[targetFieldCounter].yPosition = j;
          targetField[targetFieldCounter].expectedProvokedDamage = calculateExpectedProvokedDamage(bufferUnit, u);
          targetField[targetFieldCounter].expectedTakenDamage = calculateExpectedTakenDamage(bufferUnit, u);
          if(bufferUnit.actor == RED)
          {
            targetField[targetFieldCounter].enemyHQDistance = distance(bufferUnit.xPosition, bufferUnit.yPosition, xBlueHQ, yBlueHQ);
            targetField[targetFieldCounter].ownHQDistance = distance(bufferUnit.xPosition, bufferUnit.yPosition, xRedHQ, yRedHQ);            
          }
          else if(bufferUnit.actor == BLUE)
          {            
            targetField[targetFieldCounter].enemyHQDistance = distance(bufferUnit.xPosition, bufferUnit.yPosition, xRedHQ, yRedHQ);
            targetField[targetFieldCounter].ownHQDistance = distance(bufferUnit.xPosition, bufferUnit.yPosition, xBlueHQ, yBlueHQ);            
          }              
          targetFieldCounter++;
        }
      }      
    }

    // Assessment
    float maxExpectedProvokedDamage = 0;
    float maxExpectedTakenDamage = 0;
    for(byte i = 0; i < targetFieldCounter; i++)
    {
      if(targetField[i].expectedProvokedDamage > maxExpectedProvokedDamage){ maxExpectedProvokedDamage = targetField[i].expectedProvokedDamage; }
      if(targetField[i].expectedTakenDamage > maxExpectedTakenDamage){ maxExpectedTakenDamage = targetField[i].expectedTakenDamage; }
    }
    for(byte i = 0; i < targetFieldCounter; i++)
    {
      if(targetField[i].expectedProvokedDamage > 0){ targetField[i].assessment++; } // Target field is in shot range of a target
      if(targetField[i].expectedTakenDamage > 0){ targetField[i].assessment--; } // Target field is in range of enemies
      if(targetField[i].expectedTakenDamage > bufferUnit.hitPoints){ targetField[i].assessment = targetField[i].assessment - 2; } // High chance that current unit will be destroyed

      if(targetField[i].expectedProvokedDamage == maxExpectedProvokedDamage){ targetField[i].assessment = targetField[i].assessment + 2; } // Field with best expected provoked damage
      if(targetField[i].expectedTakenDamage == maxExpectedTakenDamage){ targetField[i].assessment--; } // Field with highest expected taken damage

      // Weight distance to HQ
      // In general move forward. If own HQ is threatened, move back if own distance is closer to own HQ than to enemy HQ.
      if(bufferUnit.actor == RED)
      {
        if(!ownHQThreatened || (ownHQThreatened && (distance(unit[u].xPosition, unit[u].yPosition, xBlueHQ, yBlueHQ) < distance(unit[u].xPosition, unit[u].yPosition, xRedHQ, yRedHQ))))
        {
          if(targetField[i].enemyHQDistance < distance(unit[u].xPosition, unit[u].yPosition, xBlueHQ, yBlueHQ)){ targetField[i].assessment++; } // Target field is closer to enemy HQ
        }
        else
        {
          if(targetField[i].ownHQDistance < distance(unit[u].xPosition, unit[u].yPosition, xRedHQ, yRedHQ)){ targetField[i].assessment++; } // Target field is closer to own HQ          
        }
      }
      else if(bufferUnit.actor == BLUE)
      {
        if(!ownHQThreatened || (ownHQThreatened && (distance(unit[u].xPosition, unit[u].yPosition, xRedHQ, yRedHQ) < distance(unit[u].xPosition, unit[u].yPosition, xBlueHQ, yBlueHQ))))
        {
          if(targetField[i].enemyHQDistance < distance(unit[u].xPosition, unit[u].yPosition, xRedHQ, yRedHQ)){ targetField[i].assessment++; } // Target field is closer to enemy HQ
        }
        else
        {
          if(targetField[i].ownHQDistance < distance(unit[u].xPosition, unit[u].yPosition, xBlueHQ, yBlueHQ)){ targetField[i].assessment++; } // Target field is closer to own HQ                          
        }
      }
    }

    int assessmentBuffer = -127;
    byte targetFieldIndex = 0;
    byte numberOfMaxAssessment = 0;
    byte selectedTargetFieldWithMaxAssessment;
    byte maxAssessmentCounter = 0;
    for(byte i = 0; i < targetFieldCounter; i++)
    {      
      if(targetField[i].assessment > assessmentBuffer)
      { 
        assessmentBuffer = targetField[i].assessment; 
      }
    }

    for(byte i = 0; i < targetFieldCounter; i++)
    {
      if(targetField[i].assessment == assessmentBuffer){ numberOfMaxAssessment++; }
    }
    selectedTargetFieldWithMaxAssessment = random(numberOfMaxAssessment);

    for(byte i = 0; i < targetFieldCounter; i++)
    {
      if(targetField[i].assessment == assessmentBuffer)
      {
        if(maxAssessmentCounter == selectedTargetFieldWithMaxAssessment)
        {
          targetFieldIndex = i;
        }
        maxAssessmentCounter++;
      }
    }

    overlayMove = true;
    overlayTarget = false;      
    moveMapToField(targetField[targetFieldIndex].xPosition, targetField[targetFieldIndex].yPosition, 4, MOVE);

  }
  else if(type == AIM)
  {       
    for(byte i = 0; i < numberOfUnits; i++)
    {
      if(unit[i].active && u != i && ((unit[u].actor == RED && unit[i].actor == BLUE) || (unit[u].actor == BLUE && unit[i].actor == RED)))
      {
        if(canHit(u, unit[i].xPosition, unit[i].yPosition))
        {
          float provokedDamageToOneUnit = 0;
          targetField[targetFieldCounter].xPosition = unit[i].xPosition;
          targetField[targetFieldCounter].yPosition = unit[i].yPosition;
          if(unit[i].groundUnit)
          {
            if(distance(unit[u].xPosition, unit[u].yPosition, unit[i].xPosition, unit[i].yPosition) <= unit[u].groundAttackRange)
            {
              provokedDamageToOneUnit = (1.0 - (float(calculateHitValue(unit[u], unit[i])) / 100.0)) * unit[u].groundAttackPower;
              if(provokedDamageToOneUnit >= unit[i].hitPoints){ provokedDamageToOneUnit = provokedDamageToOneUnit * 2; }
            }
          }
          else
          {
            if(distance(unit[u].xPosition, unit[u].yPosition, unit[i].xPosition, unit[i].yPosition) <= unit[u].airAttackRange)
            {
              provokedDamageToOneUnit = (1.0 - (float(calculateHitValue(unit[u], unit[i])) / 100.0)) * unit[u].airAttackPower;
              if(provokedDamageToOneUnit >= unit[i].hitPoints){ provokedDamageToOneUnit = provokedDamageToOneUnit * 2; }
            }        
          }

          targetField[i].expectedProvokedDamage = provokedDamageToOneUnit;
          targetFieldCounter++;          
        }
      }
    }

    float maxExpectedProvokedDamage = 0;
    byte targetFieldIndex = 0;
    byte numberOfMaxExpectedProvokedDamage = 0;
    byte selectedTargetFieldWithMaxExpectedProvokedDamage;
    byte maxExpectedProvokedDamageCounter = 0;

    for(byte i = 0; i < targetFieldCounter; i++)
    {
      if(targetField[i].expectedProvokedDamage > maxExpectedProvokedDamage){ maxExpectedProvokedDamage = targetField[i].expectedProvokedDamage; }
    }

    for(byte i = 0; i < targetFieldCounter; i++)
    {
      if(targetField[i].expectedProvokedDamage == maxExpectedProvokedDamage){ numberOfMaxExpectedProvokedDamage++; }
    }

    selectedTargetFieldWithMaxExpectedProvokedDamage = random(numberOfMaxExpectedProvokedDamage);

    for(byte i = 0; i < targetFieldCounter; i++)
    {
      if(targetField[i].expectedProvokedDamage == maxExpectedProvokedDamage)
      {
        if(maxExpectedProvokedDamageCounter == selectedTargetFieldWithMaxExpectedProvokedDamage)
        {
          targetFieldIndex = i;
        }
        maxExpectedProvokedDamageCounter++;
      }
    }

    overlayMove = false;
    overlayTarget = true;        
    // If target field counter == 0, no target can be hit -> Current field ist selected
    if(targetFieldCounter > 0){ moveMapToField(targetField[targetFieldIndex].xPosition, targetField[targetFieldIndex].yPosition, 4, AIM); }
  }

  delay(20);      
  return FIRE_A;
}


// makeMove: Main procedure to make a move (move map and select a field or a target).
void makeMove(byte _currentUnit, byte type)
{
  boolean updateMap = true;
  boolean updateCursor = true;
  boolean moveFinished = false;
  byte action = 0;

  unitSelected = true;
  selectedUnit = _currentUnit;
  cursorXNew = cursorX;
  cursorYNew = cursorY;
  upperLeftMapCornerXNew = upperLeftMapCornerX;
  upperLeftMapCornerYNew = upperLeftMapCornerY;
  overlaySelectedUnit = true;
  if(type == MOVE)
  {
    overlayMove = true;
    overlayTarget = false;    
  }
  else if(type == AIM)
  {
    overlayMove = false;
    overlayTarget = true;        
  }  
  overlaySettled = true;
  updateMap = true;

  removeAllOverlays(upperLeftMapCornerXNew, upperLeftMapCornerYNew, 0, 0);
    
  do
  {
    // Update resetAndPauseTimer   
    resetAndPauseTimer.update();
    if(reset){ break; }
    
    if(updateMap)
    {
      if(upperLeftMapCornerX != upperLeftMapCornerXNew || upperLeftMapCornerY != upperLeftMapCornerYNew)
      {
        drawCursor(cursorX, cursorY, 0, 0,false);
        scrollMap(4);
      }
      drawMapComplete(upperLeftMapCornerXNew, upperLeftMapCornerYNew, 0, 0, true);
      upperLeftMapCornerX = upperLeftMapCornerXNew;
      upperLeftMapCornerY = upperLeftMapCornerYNew;
    }
    if(updateCursor)
    {
      drawCursor(cursorX, cursorY, 0, 0,false);
      drawCursor(cursorXNew, cursorYNew, 0, 0, true);
      updateCursor = false;
      cursorX = cursorXNew;
      cursorY = cursorYNew;

      // Currently overlayed unit
      overlayedUnit = getUnit(cursorX, cursorY);
      if(overlayedUnit != 255 && unit[overlayedUnit].active){ unitOverlayed = true; }
      else{ unitOverlayed = false; }
      
      if(!updateMap){ delay(250); }
    }
    updateMap = false;

    // Check joysticks
    boolean wait;
    do
    { 
      // Update resetAndPauseTimer   
      resetAndPauseTimer.update();
      if(reset){ break; }

      wait = false;
      if(redPlayer == PLAYER1 && unit[_currentUnit].actor == RED && (joy1FireA() || joy1FireB())){ wait = true; };    
      if(redPlayer == PLAYER2 && unit[_currentUnit].actor == RED && (joy2FireA() || joy2FireB())){ wait = true; };    
      if(bluePlayer == PLAYER1 && unit[_currentUnit].actor == BLUE && (joy1FireA() || joy1FireB())){ wait = true; };    
      if(bluePlayer == PLAYER2 && unit[_currentUnit].actor == BLUE && (joy2FireA() || joy2FireB())){ wait = true; };    
    }
    while(wait && !reset);
            
    if(redPlayer == PLAYER1 && unit[_currentUnit].actor == RED){ action = joystickInquiry(1); }
    if(redPlayer == PLAYER2 && unit[_currentUnit].actor == RED){ action = joystickInquiry(2); }
    if(redPlayer == COMPUTER && unit[_currentUnit].actor == RED){ action = computerMove(_currentUnit, type); }
    if(bluePlayer == PLAYER1 && unit[_currentUnit].actor == BLUE){ action = joystickInquiry(1); }
    if(bluePlayer == PLAYER2 && unit[_currentUnit].actor == BLUE){ action = joystickInquiry(2); }
    if(bluePlayer == COMPUTER && unit[_currentUnit].actor == BLUE){ action = computerMove(_currentUnit, type); }
        
    if((action == UP || action == UP_LEFT || action == UP_RIGHT) && cursorY > 0)
    {
      cursorYNew = cursorY - 1;
      updateCursor = true;
      if(upperLeftMapCornerY > (cursorYNew - 1) && (upperLeftMapCornerY > 0))
      { 
        upperLeftMapCornerYNew = upperLeftMapCornerY - 1;
        updateMap = true;
      }
    }
    else if((action == DOWN || action == DOWN_LEFT || action == DOWN_RIGHT) && cursorY < (mapHeight - 1))
    {
      cursorYNew = cursorY + 1;
      updateCursor = true;
      if((upperLeftMapCornerY + 2 ) < cursorYNew && ((upperLeftMapCornerY + 4) < mapHeight))
      { 
        upperLeftMapCornerYNew = upperLeftMapCornerY + 1;
        updateMap = true;
      }
    }    
    
    if((action == LEFT || action == UP_LEFT || action == DOWN_LEFT) && cursorX > 0)
    {
      cursorXNew = cursorX - 1;
      updateCursor = true;
      if(upperLeftMapCornerX > (cursorXNew - 1) && (upperLeftMapCornerX > 0))
      { 
        upperLeftMapCornerXNew = upperLeftMapCornerX - 1;
        updateMap = true;
      }
    }        
    else if((action == RIGHT || action == UP_RIGHT || action == DOWN_RIGHT) && cursorX < (mapWidth - 1))
    {
      cursorXNew = cursorX + 1;
      updateCursor = true;
      if((upperLeftMapCornerX + 2 ) < cursorXNew && ((upperLeftMapCornerX + 4) < mapWidth))
      { 
        upperLeftMapCornerXNew = upperLeftMapCornerX + 1;
        updateMap = true;
      }
    }

    if(action == FIRE_A && cursorX == cursorXNew && cursorY == cursorYNew)
    {
      if(type == MOVE)
      {
        if(canMove(_currentUnit, cursorX, cursorY)) // Move to a specified field
        {
          // SOUND: Move
          for(byte i = 0; i < 5; i++){ tone(audio, 100, 20); delay(100); }

          drawMapTile(upperLeftMapCornerX, upperLeftMapCornerY, unit[_currentUnit].xPosition, unit[_currentUnit].yPosition, 0, 0, false, true);
          drawMapTile(upperLeftMapCornerX, upperLeftMapCornerY, unit[_currentUnit].xPosition, unit[_currentUnit].yPosition, 0, 0, true, false);
          if(unit[_currentUnit].xPosition < cursorX){ unit[_currentUnit].orientation = RIGHT; }
          if(unit[_currentUnit].xPosition > cursorX){ unit[_currentUnit].orientation = LEFT; }
          unit[_currentUnit].fieldsMoved = distance(unit[_currentUnit].xPosition, unit[_currentUnit].yPosition, cursorX, cursorY);
          unit[_currentUnit].xPosition = cursorX;
          unit[_currentUnit].yPosition = cursorY;
          unit[_currentUnit].settled = true;
          drawMapTile(upperLeftMapCornerX, upperLeftMapCornerY, unit[_currentUnit].xPosition, unit[_currentUnit].yPosition, 0, 0, true, true);
          delay(500);
          moveFinished = true;
        }
        else if(distance(unit[_currentUnit].xPosition, unit[_currentUnit].yPosition, cursorX, cursorY) == 0) // Keep position (pre-requisite to fix unit)
        {
          unit[_currentUnit].fieldsMoved = 0;
          unit[_currentUnit].settled = true;
          delay(500);
          moveFinished = true;
                  
          // SOUND: Stay
          clickSound();          
        }
        else
        {
          // SOUND: Cannot move to field
          tone(audio, 100, 50);          
        }
      }
      else if(type == AIM)
      {
        if(canHit(_currentUnit, cursorX, cursorY)) // Select a target
        {
          // SOUND: Aim
          for(byte i = 0; i < 10; i++){ tone(audio, 200 + (i * 20), 9); delay(10); }

          unit[_currentUnit].settled = true;
          delay(500);
          moveFinished = true;

          // Store shooter and target in array for later battle
          shooter[nextShot] = _currentUnit;
          target[nextShot] = getUnit(cursorX, cursorY);
          if(target[nextShot] != 255 && unit[target[nextShot]].active){ nextShot++; }
        }
        else if(distance(unit[_currentUnit].xPosition, unit[_currentUnit].yPosition, cursorX, cursorY) == 0) // Select own unit (pre-requisite to fix unit)
        {
          unit[_currentUnit].settled = true;
          delay(500);
          moveFinished = true;

          // SOUND: Do nothing or fix
          clickSound();          

          if(unit[_currentUnit].hitPoints < unit[_currentUnit].armor && unit[_currentUnit].fieldsMoved == 0) // If unit is damaged and has not been moved in the same round, it is selected for fixing
          {
            shooter[nextShot] = _currentUnit;
            target[nextShot] = _currentUnit;
            if(target[nextShot] != 255 && unit[target[nextShot]].active){ nextShot++; }

            // Show screw-wrench as symbol for fixing
            matrix.fillRect((cursorX - upperLeftMapCornerX) * 8, (cursorY - upperLeftMapCornerY) * 8, 8, 8, matrix.Color333(0, 0, 0));
            matrix.drawRect((cursorX - upperLeftMapCornerX) * 8, (cursorY - upperLeftMapCornerY) * 8, 8, 8, matrix.Color333(0, 0, 2));
            matrix.drawLine(2 + (cursorX - upperLeftMapCornerX) * 8, 1 + (cursorY - upperLeftMapCornerY) * 8, 2 + (cursorX - upperLeftMapCornerX) * 8, 3 + (cursorY - upperLeftMapCornerY) * 8, matrix.Color333(2, 2, 2));
            matrix.drawLine(5 + (cursorX - upperLeftMapCornerX) * 8, 1 + (cursorY - upperLeftMapCornerY) * 8, 5 + (cursorX - upperLeftMapCornerX) * 8, 3 + (cursorY - upperLeftMapCornerY) * 8, matrix.Color333(2, 2, 2));
            matrix.fillRect(3 + (cursorX - upperLeftMapCornerX) * 8, 3 + (cursorY - upperLeftMapCornerY) * 8, 2, 4, matrix.Color333(2, 2, 2));
            delay(500);       
            matrix.fillRect((cursorX - upperLeftMapCornerX) * 8, (cursorY - upperLeftMapCornerY) * 8, 8, 8, matrix.Color333(0, 0, 0));                        
          }
        }
        else
        {
          // SOUND: Cannot aim to field
          tone(audio, 100, 50);          
        }                
      }
    }

    if(action == FIRE_B)
    {
      byte unitSelectedForInformation;
      unitSelectedForInformation = getUnit(cursorX, cursorY);
      if(unitSelectedForInformation != 255 && unit[unitSelectedForInformation].active)
      { 
        // SOUND: Show unit information
        clickSound();
        showUnitInformation(_currentUnit, unitSelectedForInformation); 
        updateCursor = true;
        updateMap = true;
      }
    }  
  }  
  while(!moveFinished && !reset);
 
  unitSelected = false;
  overlaySelectedUnit = false;
  removeAllOverlays(upperLeftMapCornerXNew, upperLeftMapCornerYNew, 0, 0);
}


// drawKillMarking: Draws a kill marking at (x, y) on the screen. Used for unit information or in battle animations.
void drawKillMarking(byte x, byte y)
{
  matrix.drawLine(x, y, x + 1, y, matrix.Color333(3, 3, 0));
  matrix.drawLine(x, y + 1, x + 1, y + 1, matrix.Color333(2, 2, 0));
  matrix.drawLine(x, y + 2, x + 1, y + 2, matrix.Color333(1, 1, 0));
}


// showUnitInformation: Shows unit information for unit u. _currentUnit is the currently active unit.
void showUnitInformation(byte _currentUnit, byte u)
{
  boolean wait;
  byte action = 0;

  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Show unit
  if(unit[u].type == SCOUT || unit[u].type == LIGHTTANK){ drawUnit(unit[u].type, unit[u].actor, RIGHT, true, false, 2, 3); }
  else if(unit[u].type == TANK || unit[u].type == ATTACKCOPTER || unit[u].type == HEAVYCOPTER){ drawUnit(unit[u].type, unit[u].actor, RIGHT, true, false, 2, 3); }
  else if(unit[u].type == HOWITZER || unit[u].type == AA || unit[u].type == HEAVYCOPTER){ drawUnit(unit[u].type, unit[u].actor, RIGHT, true, false, 2, 2); }
  else { drawUnit(unit[u].type, unit[u].actor, RIGHT, true, false, 2, 2); }
  
  // Show experience
  if(unit[u].experience > 0){ drawKillMarking(9, 0); }
  if(unit[u].experience > 1){ drawKillMarking(12, 0); }
  if(unit[u].experience > 2){ drawKillMarking(9, 4); }
  if(unit[u].experience > 3){ drawKillMarking(12, 4); }

  // Show movement points
  matrix.drawLine(16, 0, 16, 6, matrix.Color333(0, 3, 0));
  matrix.drawLine(22, 0, 22, 6, matrix.Color333(0, 3, 0));

  if(unit[u].fieldsMoved > 0)
  {
    matrix.drawLine(17, 6, 19, 4, matrix.Color333(0, 3, 0));
    matrix.drawLine(21, 6, 20, 5, matrix.Color333(0, 3, 0));
  }
  if(unit[u].fieldsMoved > 1)
  {
    matrix.drawLine(17, 4, 19, 2, matrix.Color333(0, 3, 0));
    matrix.drawLine(21, 4, 20, 3, matrix.Color333(0, 3, 0));
  }
  if(unit[u].fieldsMoved > 2)
  {
    matrix.drawLine(17, 2, 19, 0, matrix.Color333(0, 3, 0));
    matrix.drawLine(21, 2, 20, 1, matrix.Color333(0, 3, 0));
  }
  
  matrix.setTextColor(matrix.Color333(0, 3, 0));
  matrix.setCursor(25, 0);
  matrix.print(unit[u].movement);

  // Show damage
  matrix.drawLine(2, 8, 6, 8, matrix.Color333(2, 2, 2));
  matrix.drawLine(2, 9, 2, 12, matrix.Color333(2, 2, 2));
  matrix.drawLine(6, 9, 6, 12, matrix.Color333(2, 2, 2));
  matrix.drawLine(3, 13, 5, 13, matrix.Color333(2, 2, 2));
  if(unit[_currentUnit].actor == RED)
  {
    matrix.drawLine(3, 11, 5, 9, matrix.Color333(2, 0, 0));
  }
  else
  {
    matrix.drawLine(3, 11, 5, 9, matrix.Color333(0, 0, 2));    
  }

  matrix.setCursor(9, 8);
  if(unit[u].hitPoints < 3)
  {
    matrix.setTextColor(matrix.Color333(3, 0, 0));    
  }
  else
  {
    matrix.setTextColor(matrix.Color333(2, 2, 2));        
  }
  matrix.print(unit[u].hitPoints);
  matrix.setTextColor(matrix.Color333(2, 2, 2));
  matrix.setCursor(17, 8);
  matrix.print("/");
  matrix.setCursor(25, 8);
  matrix.print(unit[u].armor);

  // Show ability to attack ground targets
  if(unit[u].actor == RED)
  {
    drawUnit(TANK, BLUE, RIGHT, true, false, 2, 16);
  }
  else
  {
    drawUnit(TANK, RED, RIGHT, true, false, 2, 16);    
  }
  matrix.drawLine(2, 22, 4, 20, matrix.Color333(2, 2, 0));
  matrix.drawLine(4, 20, 6, 22, matrix.Color333(2, 2, 0));
  matrix.setTextColor(matrix.Color333(2, 2, 0));
  matrix.setCursor(9, 16);
  matrix.print(unit[u].groundAttackRange);
  matrix.fillCircle(19, 19, 3, matrix.Color333(4, 1, 0));
  matrix.fillCircle(19, 19, 2, matrix.Color333(4, 4, 0));
  matrix.fillCircle(19, 19, 1, matrix.Color333(5, 5, 1));
  matrix.fillCircle(19, 19, 0, matrix.Color333(5, 5, 5));
  matrix.setTextColor(matrix.Color333(2,2,0));
  matrix.setCursor(25, 16);
  matrix.print(unit[u].groundAttackPower);
 
  // Show ability to attack air targets
  if(unit[u].actor == RED)
  {
    drawUnit(HEAVYCOPTER, BLUE, RIGHT, true, false, 2, 24);
  }
  else
  {
    drawUnit(HEAVYCOPTER, RED, RIGHT, true, false, 2, 24);    
  }
  matrix.drawLine(2, 30, 4, 28, matrix.Color333(2, 2, 0));
  matrix.drawLine(4, 28, 6, 30, matrix.Color333(2, 2, 0));
  matrix.setTextColor(matrix.Color333(2, 2, 0));
  matrix.setCursor(9, 24);
  matrix.print(unit[u].airAttackRange);
  matrix.fillCircle(19, 27, 3, matrix.Color333(4, 1, 0));
  matrix.fillCircle(19, 27, 2, matrix.Color333(4, 4, 0));
  matrix.fillCircle(19, 27, 1, matrix.Color333(5, 5, 1));
  matrix.fillCircle(19, 27, 0, matrix.Color333(5, 5, 5));
  matrix.setTextColor(matrix.Color333(2,2,0));
  matrix.setCursor(25, 24);
  matrix.print(unit[u].airAttackPower);

  do
  { 
    // Update resetAndPauseTimer   
    resetAndPauseTimer.update();
    if(reset){ break; }

    wait = false;
    if(redPlayer == PLAYER1 && unit[_currentUnit].actor == RED && (joy1FireA() || joy1FireB())){ wait = true; };    
    if(redPlayer == PLAYER2 && unit[_currentUnit].actor == RED && (joy2FireA() || joy2FireB())){ wait = true; };    
    if(bluePlayer == PLAYER1 && unit[_currentUnit].actor == BLUE && (joy1FireA() || joy1FireB())){ wait = true; };    
    if(bluePlayer == PLAYER2 && unit[_currentUnit].actor == BLUE && (joy2FireA() || joy2FireB())){ wait = true; };    
  }
  while(wait && !reset);
  delay(250);
      
  do
  {
    // Update resetAndPauseTimer   
    resetAndPauseTimer.update();
    if(reset){ break; }
    
    // Check joysticks
    if(redPlayer == PLAYER1 && unit[_currentUnit].actor == RED){ action = joystickInquiry(1); }
    if(redPlayer == PLAYER2 && unit[_currentUnit].actor == RED){ action = joystickInquiry(2); }
    if(bluePlayer == PLAYER1 && unit[_currentUnit].actor == BLUE){ action = joystickInquiry(1); }
    if(bluePlayer == PLAYER2 && unit[_currentUnit].actor == BLUE){ action = joystickInquiry(2); }
  }
  while(!((!joy1IsLEDmePlayJoypad() && action == FIRE_A) || (!joy2IsLEDmePlayJoypad() && action == FIRE_A) || (joy1IsLEDmePlayJoypad() && action == FIRE_B) || (joy2IsLEDmePlayJoypad() && action == FIRE_B)) && !reset);

  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
}


// showRound: Shows next round and type of round (MOVE, AIM, FIX, FIRE).
void showRound(byte gameRound, byte type)
{  
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  if(type == MOVE)
  {
    matrix.setTextColor(matrix.Color333(0, 3, 0));    
    matrix.drawLine(0, 1, 31, 1, matrix.Color333(0, 1, 0));
    matrix.drawLine(0, 30, 31, 30, matrix.Color333(0, 1, 0));
  }
  else if(type == AIM)
  {
    matrix.setTextColor(matrix.Color333(2, 2, 0));        
    matrix.drawLine(0, 1, 31, 1, matrix.Color333(1, 1, 0));
    matrix.drawLine(0, 30, 31, 30, matrix.Color333(1, 1, 0));
  }
  else if(type == FIX)
  {
    matrix.setTextColor(matrix.Color333(0, 0, 3));        
    matrix.drawLine(0, 1, 31, 1, matrix.Color333(0, 0, 1));
    matrix.drawLine(0, 30, 31, 30, matrix.Color333(0, 0, 1));
  }
  else if(type == FIRE)
  {
    matrix.setTextColor(matrix.Color333(3, 0, 0));        
    matrix.drawLine(0, 1, 31, 1, matrix.Color333(1, 0, 0));
    matrix.drawLine(0, 30, 31, 30, matrix.Color333(1, 0, 0));
  }
  matrix.setCursor(1, 3);
  matrix.println("Round");
  if(gameRound < 10){ matrix.setCursor(13, 12); }
  else{ matrix.setCursor(10, 12); }
  matrix.println(gameRound);
  if(type == MOVE){ matrix.setCursor(4, 21); matrix.println("Move"); }
  else if(type == AIM){ matrix.setCursor(7, 21); matrix.println("Aim"); }
  else if(type == FIX){ matrix.setCursor(7, 21); matrix.println("Fix"); }
  else if(type == FIRE){ matrix.setCursor(4, 21); matrix.println("Fire"); }

  delay(2000);
}


// calculateHitValue: Calculate percentage that shooter hits target
byte calculateHitValue(byte shooter, byte target)
{
  int hitValue = 30;

  // Shooter modificator
  
  // Movement
  if(unit[shooter].fieldsMoved > 0)
  { 
    hitValue = hitValue + 10;
  }

  // Experience
  if(unit[shooter].experience > 0)
  { 
    hitValue = hitValue - (unit[shooter].experience * 5); 
  }

  // Target modificator
  
  // Movement
  if(unit[target].fieldsMoved > 0)
  { 
    hitValue = hitValue + (unit[target].fieldsMoved * 10);
  }

  // Wood
  if(getMapSurface(unit[target].xPosition, unit[target].yPosition) == WOOD && unit[target].groundUnit)
  { 
    hitValue = hitValue + 20;
  }

  // Normalalize hit value
  if(hitValue < 0){ hitValue = 0; }
  if(hitValue > 100){ hitValue = 100; }

  return hitValue;
}


// calculateHitValue: Calculate percentage that shooter hits target
byte calculateHitValue(Unit &shooter, Unit &target)
{
  byte hitValue = 30;

  // Shooter modificator
  
  // Movement
  if(shooter.fieldsMoved > 0)
  { 
    hitValue = hitValue + 10;
  }

  // Experience
  if(shooter.experience > 0)
  { 
    hitValue = hitValue - (shooter.experience * 5); 
  }

  // Target modificator
  
  // Movement
  if(target.fieldsMoved > 0)
  { 
    hitValue = hitValue + (target.fieldsMoved * 10);
  }

  // Wood
  if(getMapSurface(target.xPosition, target.yPosition) == WOOD && target.groundUnit)
  { 
    hitValue = hitValue + 20;
  }

  // Normalalize hit value
  if(hitValue < 0){ hitValue = 0; }
  if(hitValue > 100){ hitValue = 100; }

  return hitValue;
}


// battle: Processes a battle between shooter and target with a defined hit value including animations
boolean battle(byte shooter, byte target, byte hitValue)
{
  byte hitDice;
  byte damageDice;
  byte damage;

  // Fire
  hitDice = random(100);

  // Shooter animation
  shooterAnimation(shooter, target, hitValue);
    
  if(hitDice < hitValue)
  {
    // Miss

    // Target animation (Miss)
    targetAnimation(shooter, target, 0);
  }
  else
  {
    // Hit
    if(unit[target].groundUnit)
    {
      damageDice = unit[shooter].groundAttackPower;
    }
    else
    {
      damageDice = unit[shooter].airAttackPower;      
    }
    damage = 1; // At least damage of 1
    for(byte i = 0; i < damageDice - 1; i++)
    {
      if(50 > random(100)){ damage++; }
    }
    unit[target].hitPoints = unit[target].hitPoints - damage;
    if(unit[target].hitPoints < 0){ unit[target].hitPoints = 0; }

    // Target animation (Hit)
    targetAnimation(shooter, target, damage);
    
    if(unit[target].hitPoints == 0)
    { 
      if(unit[shooter].experience < 4){ unit[shooter].experience++; }
      experienceAnimation(shooter);
      return true;      
    }
  }
  return false;
}


// fix: Performs fixing of the shooter
void fix(byte shooter)
{
  byte fixedHitPoints = random(unit[shooter].armor) + 1;
  if((unit[shooter].hitPoints + fixedHitPoints) > unit[shooter].armor){ fixedHitPoints = unit[shooter].armor - unit[shooter].hitPoints; }
  
  unit[shooter].hitPoints = unit[shooter].hitPoints + fixedHitPoints;
  if(unit[shooter].hitPoints > unit[shooter].armor){ unit[shooter].hitPoints = unit[shooter].armor; }

  // Fix animation
  fixAnimation(shooter, fixedHitPoints);
}


// initializeBulletsAndExplosions: Should be called before every battle.
void initializeBulletsAndExplosions()
{
  for(byte i = 0; i < numberOfBullets; i++)
  {
    bullet[i].active = false;
  }
  nextBullet = 0;
  for(byte i = 0; i < numberOfExplosions; i++)
  {
    explosion[i].active = false;
  }
  nextExplosion = 0;
}

void fireBullet(byte type, byte power, float xPosition, float yPosition, float xVector, float yVector)
{
  if(!bullet[nextBullet].active)
  {
    bullet[nextBullet].type = type;
    bullet[nextBullet].power = power;
    bullet[nextBullet].xPosition = xPosition;
    bullet[nextBullet].yPosition = yPosition;
    bullet[nextBullet].xVector = xVector;
    bullet[nextBullet].yVector = yVector;
    bullet[nextBullet].active = true;

    if(bullet[nextBullet].type == AUTOGUN){ matrix.drawPixel(xPosition, yPosition, matrix.Color333(3, 2, 0)); }
    if(bullet[nextBullet].type == CANNON){ matrix.drawPixel(xPosition, yPosition, matrix.Color333(3, 2, 0)); }
    if(bullet[nextBullet].type == MISSILE){ matrix.drawPixel(xPosition, yPosition, matrix.Color333(1, 1, 1)); }
    if(bullet[nextBullet].type == EXHAUST){ matrix.drawPixel(xPosition, yPosition, matrix.Color333(7, 7, 0)); }

    nextBullet++;
    if(nextBullet == numberOfBullets){ nextBullet = 0; }
  }
}

void moveBullets()
{
  float xBuffer;
  float yBuffer;
  for(byte i = 0; i < numberOfBullets; i++)
  {
    if(bullet[i].active)
    {
      // Remove bullet from old position
      matrix.drawPixel(byte(bullet[i].xPosition), byte(bullet[i].yPosition), matrix.Color333(0, 0, 0));

      // Calculate new position
      xBuffer = bullet[i].xPosition + bullet[i].xVector;
      yBuffer = bullet[i].yPosition + bullet[i].yVector;

      // Bullet still visible?
      if(int(xBuffer) < -32 || int(xBuffer) > 63 || int(yBuffer) < -32 || int(yBuffer) > 63)
      {
        bullet[i].active = false;
      }

      // Hit something?
      if(bullet[i].active && int(xBuffer) >= 0 && int(xBuffer) <= 31 && int(yBuffer) >= 0 && int(yBuffer) <= 31 && playfield[byte(xBuffer)][byte(yBuffer)] == 1)
      {
        bullet[i].active = false;
        initializeExplosion(bullet[i].power, 1, byte(bullet[i].xPosition), byte(bullet[i].yPosition));
        bulletHitCounter++;
        if(bullet[i].power > 2)
        {
          initializeExplosion(bullet[i].power, bullet[i].power - 2, byte(bullet[i].xPosition - 1), byte(bullet[i].yPosition));
          initializeExplosion(bullet[i].power, bullet[i].power - 2, byte(bullet[i].xPosition + 1), byte(bullet[i].yPosition));          
        }
      }

      // Draw bullet at new position
      if(bullet[i].active)
      {
        bullet[i].xPosition = xBuffer;
        bullet[i].yPosition = yBuffer;
        if(bullet[i].type == AUTOGUN){ matrix.drawPixel(byte(bullet[i].xPosition), byte(bullet[i].yPosition), matrix.Color333(1, 1, 1)); }
        if(bullet[i].type == CANNON){ matrix.drawPixel(byte(bullet[i].xPosition), byte(bullet[i].yPosition), matrix.Color333(3, 3, 3)); }
        if(bullet[i].type == MISSILE){ matrix.drawPixel(byte(bullet[i].xPosition), byte(bullet[i].yPosition), matrix.Color333(1, 1, 1)); }
        if(bullet[i].type == EXHAUST && random(10) > 4){ matrix.drawPixel(byte(bullet[i].xPosition), byte(bullet[i].yPosition), matrix.Color333(7, 7, 0)); tone(audio,196 + random(128),10); }
      }
    }
  }
}

void initializeExplosion(byte power, byte phase, byte xPosition, byte yPosition)
{
  if(!explosion[nextExplosion].active)
  {
    explosion[nextExplosion].phase = phase;
    explosion[nextExplosion].counter = 0;
    explosion[nextExplosion].power = power;
    explosion[nextExplosion].xPosition = xPosition;
    explosion[nextExplosion].yPosition = yPosition;
    explosion[nextExplosion].active = true;

    matrix.fillCircle(explosion[nextExplosion].xPosition, explosion[nextExplosion].yPosition, 0, matrix.Color333(7, 7, 7));

    nextExplosion++;
    if(nextExplosion == numberOfExplosions){ nextExplosion = 0; }  
  }
}

void updateExplosions()
{
  for(byte i = 0; i < numberOfExplosions; i++)
  {
    if(explosion[i].active)
    {
      if(explosion[i].counter <= explosion[i].phase){ explosion[i].counter++; }

      if(explosion[i].phase <= explosion[i].power)
      {
        //SOUND: Explosion
        tone(audio, 2048 / pow(2, (explosion[i].counter * explosion[i].phase)), 40);
        if((explosion[i].yPosition + explosion[i].counter) < 23)
        {
          matrix.fillCircle(explosion[i].xPosition, explosion[i].yPosition, explosion[i].counter, matrix.Color333(7, 3 + 4 * (1.0 - float(explosion[i].phase / explosion[i].power)), 7 * (1.0 - float(explosion[i].counter / explosion[i].power))));
        }
        else
        {
          matrix.fillCircle(explosion[i].xPosition, explosion[i].yPosition - ((explosion[i].yPosition + explosion[i].counter + 1) - 23), explosion[i].counter, matrix.Color333(7, 3 + 4 * (1.0 - float(explosion[i].phase / explosion[i].power)), 7 * (1.0 - float(explosion[i].counter / explosion[i].power))));
        }
      }
      else if(explosion[i].phase == (explosion[i].power + 1))
      {
        if((explosion[i].yPosition + explosion[i].counter) < 23)
        {
          matrix.fillCircle(explosion[i].xPosition, explosion[i].yPosition, explosion[i].counter, matrix.Color333(0, 0, 0));
        }
        else
        {
          matrix.fillCircle(explosion[i].xPosition,  explosion[i].yPosition - ((explosion[i].yPosition + explosion[i].counter + 1) - 23), explosion[i].counter, matrix.Color333(0, 0, 0));
        }
      }
      else if(explosion[i].phase == (explosion[i].power + 2))
      {
        explosion[i].active = false;
      }
      
      if(explosion[i].counter == explosion[i].phase || explosion[i].counter == explosion[i].power)
      {
        explosion[i].phase++;
        explosion[i].counter = 0;
      }
    }
  }  
}

void rotorBladeAnimation(byte i, byte counter)
{
  if(unit[i].orientation == RIGHT)
  {
    if(counter % 2 == 0){ matrix.drawPixel(8, 13, matrix.Color333(3, 3, 3)); matrix.drawPixel(9, 13, matrix.Color333(1, 1, 1)); matrix.drawPixel(10, 13, matrix.Color333(1, 1, 1)); };
    if(counter % 4 == 1){ matrix.drawPixel(8, 13, matrix.Color333(1, 1, 1)); matrix.drawPixel(9, 13, matrix.Color333(5, 5, 5)); matrix.drawPixel(10, 13, matrix.Color333(1, 1, 1));};
    if(counter % 4 == 3){ matrix.drawPixel(8, 13, matrix.Color333(1, 1, 1)); matrix.drawPixel(9, 13, matrix.Color333(1, 1, 1)); matrix.drawPixel(10, 13, matrix.Color333(3, 3, 3));};
  }
  else if(unit[i].orientation == LEFT)
  {
    if(counter % 2 == 0){ matrix.drawPixel(21, 13, matrix.Color333(3, 3, 3)); matrix.drawPixel(22, 13, matrix.Color333(1, 1, 1)); matrix.drawPixel(23, 13, matrix.Color333(1, 1, 1)); };
    if(counter % 4 == 1){ matrix.drawPixel(21, 13, matrix.Color333(1, 1, 1)); matrix.drawPixel(22, 13, matrix.Color333(5, 5, 5)); matrix.drawPixel(23, 13, matrix.Color333(1, 1, 1));};
    if(counter % 4 == 3){ matrix.drawPixel(21, 13, matrix.Color333(1, 1, 1)); matrix.drawPixel(22, 13, matrix.Color333(1, 1, 1)); matrix.drawPixel(23, 13, matrix.Color333(3, 3, 3));};
  }  
}

void shooterAnimation(byte shooter, byte target, byte hitValue)
{
  int counter = 0;
  byte firedBullets = 0;
  
  // Clean playfield
  for(byte i = 0; i < 32; i++)
  {
    for(byte j = 0; j < 32; j++)
    {
      playfield[i][j] = 0;
    }
  }

  initializeBulletsAndExplosions();
  drawScene(shooter, true, true);
  
  do
  {
    // Get time of engine loop start point (important for a constant game speed)
    synchPoint = millis();

    // Update resetAndPauseTimer   
    resetAndPauseTimer.update();
    if(reset){ break; }
    
    // Update scene
    if(counter < 150){ drawScene(shooter, false, false); }

    // Update animated rotor blades for air units
    if(!unit[shooter].groundUnit){ rotorBladeAnimation(shooter, counter); }

    // Move bullets
    moveBullets();

    // Update explosions();
    updateExplosions();

    // Fire bullets
    if(counter > 50 && unit[shooter].type == SCOUT)
    {
      if(counter % 5 == 0)
      {
        if(unit[shooter].orientation == RIGHT)
        {
          if(unit[target].groundUnit && firedBullets < 8){ fireBullet(unit[shooter].groundAttackWeapon, 2, 9, 20, 1.0, 0.0); tone(audio,128,20); firedBullets++; }
          else if(!unit[target].groundUnit && firedBullets < 8){ fireBullet(unit[shooter].airAttackWeapon, 2, 9, 20, 1.0, -1.0); tone(audio,128,20); firedBullets++; }
        }
        else if(unit[shooter].orientation == LEFT)
        {
          if(unit[target].groundUnit && firedBullets < 8){ fireBullet(unit[shooter].groundAttackWeapon, 2, 22, 20, -1.0, 0.0); tone(audio,128,20); firedBullets++; }
          else if(!unit[target].groundUnit && firedBullets < 8){ fireBullet(unit[shooter].airAttackWeapon, 2, 22, 20, -1.0, -1.0); tone(audio,128,20); firedBullets++; }
        }
      }
    }
    else if(counter > 50 && unit[shooter].type == LIGHTTANK)
    {
      if(counter % 3 == 0)
      {
        if(unit[shooter].orientation == RIGHT)
        {
          if(unit[target].groundUnit && firedBullets < 16){ fireBullet(unit[shooter].groundAttackWeapon, 2, 11, 20, 1.0, 0.0); tone(audio,128,20); firedBullets++; }
          else if(!unit[target].groundUnit && firedBullets < 16){ fireBullet(unit[shooter].airAttackWeapon, 2, 11, 19, 1.0, -1.0); tone(audio,128,20); firedBullets++; }
        }
        else if(unit[shooter].orientation == LEFT)
        {
          if(unit[target].groundUnit && firedBullets < 16){ fireBullet(unit[shooter].groundAttackWeapon, 2, 20, 20, -1.0, 0.0); tone(audio,128,20); firedBullets++; }
          else if(!unit[target].groundUnit && firedBullets < 16){ fireBullet(unit[shooter].airAttackWeapon, 2, 20, 19, -1.0, -1.0); tone(audio,128,20); firedBullets++; }
        }
      }
    }
    else if(counter > 50 && unit[shooter].type == TANK)
    {
      if(unit[shooter].orientation == RIGHT)
      {
        if(unit[target].groundUnit && counter % 5 == 0 && firedBullets < 1){ fireBullet(unit[shooter].groundAttackWeapon, 2, 11, 20, 1.0, 0.0); tone(audio,64,40); firedBullets++; }
        else if(!unit[target].groundUnit && counter % 5 == 0 &&firedBullets < 8){ fireBullet(unit[shooter].airAttackWeapon, 4, 9, 19, 1.0, -1.0); tone(audio,128,20); firedBullets++; }
        if(unit[target].groundUnit && counter == 52){ initializeExplosion(1, 1, 13, 20); }
        if(unit[target].groundUnit && counter == 54){ initializeExplosion(2, 2, 15, 20); }
        if(unit[target].groundUnit && counter == 56){ initializeExplosion(2, 2, 17, 20); }
        if(unit[target].groundUnit && counter == 58){ initializeExplosion(2, 2, 19, 20); }
      }
      else if(unit[shooter].orientation == LEFT)
      {
        if(unit[target].groundUnit && counter % 5 == 0 &&firedBullets < 1){ fireBullet(unit[shooter].groundAttackWeapon, 2, 20, 20, -1.0, 0.0); tone(audio,64,40); firedBullets++; }
        else if(!unit[target].groundUnit && counter % 5 == 0 &&firedBullets < 8){ fireBullet(unit[shooter].airAttackWeapon, 4, 22, 19, -1.0, -1.0); tone(audio,128,20); firedBullets++; }
        if(unit[target].groundUnit && counter == 52){ initializeExplosion(1, 1, 18, 20); }
        if(unit[target].groundUnit && counter == 54){ initializeExplosion(2, 2, 16, 20); }
        if(unit[target].groundUnit && counter == 56){ initializeExplosion(2, 2, 14, 20); }
        if(unit[target].groundUnit && counter == 58){ initializeExplosion(2, 2, 12, 20); }
      }
    }
    else if(counter > 50 && unit[shooter].type == HEAVYTANK)
    {
      if(unit[shooter].orientation == RIGHT)
      {
        if(unit[target].groundUnit && counter % 5 == 0 && firedBullets < 1){ fireBullet(unit[shooter].groundAttackWeapon, 2, 11, 19, 1.0, 0.0); tone(audio,64,40); firedBullets++; }
        else if(!unit[target].groundUnit && counter % 5 == 0 && firedBullets < 8){ fireBullet(unit[shooter].airAttackWeapon, 5, 9, 18, 1.0, -1.0); tone(audio,128,20); firedBullets++; }
        if(unit[target].groundUnit && counter == 52){ initializeExplosion(1, 1, 13, 19); }
        if(unit[target].groundUnit && counter == 54){ initializeExplosion(2, 2, 15, 19); }
        if(unit[target].groundUnit && counter == 56){ initializeExplosion(3, 3, 17, 19); }
        if(unit[target].groundUnit && counter == 58){ initializeExplosion(2, 2, 19, 19); }
      }
      else if(unit[shooter].orientation == LEFT)
      {
        if(unit[target].groundUnit && counter % 5 == 0 && firedBullets < 1){ fireBullet(unit[shooter].groundAttackWeapon, 2, 20, 19, -1.0, 0.0); tone(audio,64,40); firedBullets++; }
        else if(!unit[target].groundUnit && counter % 5 == 0 && firedBullets < 8){ fireBullet(unit[shooter].airAttackWeapon, 5, 22, 18, -1.0, -1.0); tone(audio,128,20); firedBullets++; }
        if(unit[target].groundUnit && counter == 52){ initializeExplosion(1, 1, 18, 19); }
        if(unit[target].groundUnit && counter == 54){ initializeExplosion(2, 2, 16, 19); }
        if(unit[target].groundUnit && counter == 56){ initializeExplosion(3, 3, 14, 19); }
        if(unit[target].groundUnit && counter == 58){ initializeExplosion(2, 2, 12, 19); }
      }
    }
    else if(counter > 50 && unit[shooter].type == HOWITZER)
    {
      if(unit[shooter].orientation == RIGHT)
      {
        if(unit[target].groundUnit && counter % 5 == 0 && firedBullets < 1){ fireBullet(unit[shooter].groundAttackWeapon, 2, 11, 18, 1.0, -1.0); tone(audio,64,40); firedBullets++; }
        else if(!unit[target].groundUnit && counter % 5 == 0 && firedBullets < 8){ fireBullet(unit[shooter].airAttackWeapon, 5, 10, 21, 1.0, -1.0); tone(audio,128,20); firedBullets++; }
        if(unit[target].groundUnit && counter == 52){ initializeExplosion(1, 1, 12, 17); }
        if(unit[target].groundUnit && counter == 54){ initializeExplosion(2, 2, 14, 15); }
        if(unit[target].groundUnit && counter == 56){ initializeExplosion(3, 3, 16, 13); }
        if(unit[target].groundUnit && counter == 58){ initializeExplosion(2, 2, 18, 11); }
      }
      else if(unit[shooter].orientation == LEFT)
      {
        if(unit[target].groundUnit && counter % 5 == 0 && firedBullets < 1){ fireBullet(unit[shooter].groundAttackWeapon, 2, 20, 18, -1.0, -1.0); tone(audio,64,40); firedBullets++; }
        else if(!unit[target].groundUnit && counter % 5 == 0 && firedBullets < 8){ fireBullet(unit[shooter].airAttackWeapon, 5, 21, 21, -1.0, -1.0); tone(audio,128,20); firedBullets++; }
        if(unit[target].groundUnit && counter == 52){ initializeExplosion(1, 1, 18, 17); }
        if(unit[target].groundUnit && counter == 54){ initializeExplosion(2, 2, 16, 15); }
        if(unit[target].groundUnit && counter == 56){ initializeExplosion(3, 3, 14, 13); }
        if(unit[target].groundUnit && counter == 58){ initializeExplosion(2, 2, 12, 11); }
      }
    }
    else if(counter > 50 && unit[shooter].type == LAUNCHER)
    {
      if(unit[target].groundUnit)
      {
        if(counter == 51 || counter == 52 || counter == 71 || counter == 72 || counter == 91 || counter == 92)
        {
          if(unit[shooter].orientation == RIGHT)
          {
            fireBullet(unit[shooter].groundAttackWeapon, 4, 9, 18, 1.0, -1.0); tone(audio,128,80); firedBullets++;
          }
          else if(unit[shooter].orientation == LEFT)
          {
            fireBullet(unit[shooter].groundAttackWeapon, 4, 22, 18, -1.0, -1.0); tone(audio,128,80); firedBullets++;
          }
        }        
        if(counter == 53 || counter == 73 || counter == 93)
        {
          if(unit[shooter].orientation == RIGHT)
          {
            fireBullet(EXHAUST, 4, 9, 18, 1.0, -1.0); tone(audio,256,80); firedBullets++;
          }
          else if(unit[shooter].orientation == LEFT)
          {
            fireBullet(EXHAUST, 4, 22, 18, -1.0, -1.0); tone(audio,256,80); firedBullets++;
          }
        }        
      }
      else if(!unit[target].groundUnit)
      {
        if(counter % 5 == 0 && firedBullets < 8)
        {
          if(unit[shooter].orientation == RIGHT)
          {
            fireBullet(unit[shooter].airAttackWeapon, 2, 11, 20, 1.0, -1.0); tone(audio,128,20);firedBullets++;
          }
          else if(unit[shooter].orientation == LEFT)
          {
            fireBullet(unit[shooter].airAttackWeapon, 2, 20, 20, -1.0, -1.0); tone(audio,128,20);firedBullets++;
          }
        }        
      }
    }
    else if(counter > 50 && unit[shooter].type == AA)
    {
      if(unit[target].groundUnit)
      {
        if(unit[shooter].orientation == RIGHT && counter % 5 == 0 && firedBullets < 8)
        {
          fireBullet(unit[shooter].groundAttackWeapon, 2, 10, 21, 1.0, 0.0);
          tone(audio,128,20); firedBullets++;
        }
        else if(unit[shooter].orientation == LEFT && counter % 5 == 0 && firedBullets < 8)
        {
          fireBullet(unit[shooter].groundAttackWeapon, 2, 20, 21, -1.0, 0.0);
          tone(audio,128,20); firedBullets++;
        }        
      }
      else if(!unit[target].groundUnit)
      {
        // Move cannon if target is an air target
        if(counter == 51)
        {
          if(!unit[target].groundUnit && unit[shooter].orientation == RIGHT)
          {
            matrix.drawPixel(8, 19, matrix.Color333(0, 0, 0));
            matrix.drawPixel(9, 19, matrix.Color333(3, 3, 3));
          }
          else if(!unit[target].groundUnit && unit[shooter].orientation == LEFT)
          {
            matrix.drawPixel(22, 19, matrix.Color333(0, 0, 0));
            matrix.drawPixel(21, 19, matrix.Color333(3, 3, 3));          
          }        
        }
        if(unit[shooter].orientation == RIGHT)
        {
          if(!unit[target].groundUnit && (counter == 100 || counter == 125 || counter == 150)){ fireBullet(unit[shooter].airAttackWeapon, 3, 10, 18, 1.0, -1.0); tone(audio,96,30);firedBullets++; }
          if(!unit[target].groundUnit && (counter == 102 || counter == 127 || counter == 152)){ initializeExplosion(1, 1, 11, 17); }
          if(!unit[target].groundUnit && (counter == 104 || counter == 129 || counter == 154)){ initializeExplosion(2, 2, 13, 15); }
          if(!unit[target].groundUnit && (counter == 106 || counter == 131 || counter == 156)){ initializeExplosion(1, 1, 15, 13); }
        }
        else if(unit[shooter].orientation == LEFT)
        {
          if(!unit[target].groundUnit && (counter == 100 || counter == 125 || counter == 150)){ fireBullet(unit[shooter].airAttackWeapon, 3, 20, 18, -1.0, -1.0); tone(audio,96,30);firedBullets++; }
          if(!unit[target].groundUnit && (counter == 102 || counter == 127 || counter == 152)){ initializeExplosion(1, 1, 19, 17); }
          if(!unit[target].groundUnit && (counter == 104 || counter == 129 || counter == 154)){ initializeExplosion(2, 2, 17, 15); }
          if(!unit[target].groundUnit && (counter == 106 || counter == 131 || counter == 156)){ initializeExplosion(1, 1, 15, 13); }
        }        
      }
    }
    else if(counter > 50 && unit[shooter].type == SAM)
    {
      if(unit[target].groundUnit)
      {
        if(unit[shooter].orientation == RIGHT && counter % 5 == 0 && firedBullets < 8)
        {
          fireBullet(unit[shooter].groundAttackWeapon, 2, 11, 21, 1.0, 0.0); tone(audio,128,20); firedBullets++;
        }
        else if(unit[shooter].orientation == LEFT & counter % 5 == 0 && firedBullets < 8)
        {
          fireBullet(unit[shooter].groundAttackWeapon, 2, 20, 21, -1.0, 0.0); tone(audio,128,20); firedBullets++;
        }        
      }
      else if(!unit[target].groundUnit)
      {
        // Move launcher if target is an air target
        if(counter == 51)
        {
          if(!unit[target].groundUnit && unit[shooter].orientation == RIGHT)
          {
            matrix.drawPixel(6, 19, matrix.Color333(0, 0, 0));
            matrix.drawPixel(8, 19, matrix.Color333(0, 0, 0));
            matrix.drawPixel(8, 18, matrix.Color333(3, 3, 3));
            if(unit[shooter].actor == RED){ matrix.drawPixel(6, 20, matrix.Color333(2, 0, 0)); }
            else{ matrix.drawPixel(6, 20, matrix.Color333(0, 0, 2)); }
          }
          else if(!unit[target].groundUnit && unit[shooter].orientation == LEFT)
          {
            matrix.drawPixel(23, 19, matrix.Color333(0, 0, 0));
            matrix.drawPixel(25, 19, matrix.Color333(0, 0, 0));
            matrix.drawPixel(23, 18, matrix.Color333(3, 3, 3));
            if(unit[shooter].actor == RED){ matrix.drawPixel(25, 20, matrix.Color333(2, 0, 0)); }
            else{ matrix.drawPixel(25, 20, matrix.Color333(0, 0, 2)); }
          }
        }
        if(counter == 100 || counter == 101)
        {
          if(unit[shooter].orientation == RIGHT)
          {
            fireBullet(unit[shooter].airAttackWeapon, 3, 9, 17, 1.0, -1.0); tone(audio,128,80); firedBullets++;
          }
          else if(unit[shooter].orientation == LEFT)
          {
            fireBullet(unit[shooter].airAttackWeapon, 3, 22, 17, -1.0, -1.0); tone(audio,128,80); firedBullets++;
          }
        }                
        if(counter == 102)
        {
          if(unit[shooter].orientation == RIGHT)
          {
            fireBullet(EXHAUST, 3, 9, 17, 1.0, -1.0); tone(audio,256,80); firedBullets++;
          }
          else if(unit[shooter].orientation == LEFT)
          {
            fireBullet(EXHAUST, 3, 22, 17, -1.0, -1.0); tone(audio,256,80); firedBullets++;
          }
        }                
      }
    }
    else if(counter > 50 && unit[shooter].type == ATTACKCOPTER)
    {
      if(unit[target].groundUnit)
      {
        if(counter % 3 == 0)
        {
          if(unit[shooter].orientation == RIGHT)
          {
            if(firedBullets < 16){ fireBullet(unit[shooter].groundAttackWeapon, 2, 11, 15, 1.0, 0.5); tone(audio,128,20); firedBullets++; }
          }
          else if(unit[shooter].orientation == LEFT)
          {
            if(firedBullets < 16){ fireBullet(unit[shooter].groundAttackWeapon, 2, 20, 15, -1.0, 0.5); tone(audio,128,20); firedBullets++; }
          }
        }       
      }
      else if(!unit[target].groundUnit)
      {
        if(counter == 50 || counter == 51)
        {
          if(unit[shooter].orientation == RIGHT)
          {
            fireBullet(unit[shooter].airAttackWeapon, 3, 11, 15, 1.0, 0.0); tone(audio,128,80); firedBullets++;
          }
          else if(unit[shooter].orientation == LEFT)
          {
            fireBullet(unit[shooter].airAttackWeapon, 3, 20, 15, -1.0, 0.0); tone(audio,128,80); firedBullets++;
          }
        }        
        if(counter == 52)
        {
          if(unit[shooter].orientation == RIGHT)
          {
            fireBullet(EXHAUST, 3, 11, 15, 1.0, 0.0); tone(audio,256,80); firedBullets++;
          }
          else if(unit[shooter].orientation == LEFT)
          {
            fireBullet(EXHAUST, 3, 20, 15, -1.0, 0.0); tone(audio,256,80); firedBullets++;
          }
        }        
      }      
    }
    else if(counter > 50 && unit[shooter].type == HEAVYCOPTER)
    {
      if(unit[target].groundUnit)
      {
        if(counter == 50 || counter == 51)
        {
          if(unit[shooter].orientation == RIGHT)
          {
            fireBullet(unit[shooter].airAttackWeapon, 4, 11, 15, 1.0, 0.5); tone(audio,128,80); firedBullets++;
          }
          else if(unit[shooter].orientation == LEFT)
          {
            fireBullet(unit[shooter].airAttackWeapon, 4, 20, 15, -1.0, 0.5); tone(audio,128,80); firedBullets++;
          }
        }        
        if(counter == 52)
        {
          if(unit[shooter].orientation == RIGHT)
          {
            fireBullet(EXHAUST, 4, 11, 15, 1.0, 0.5); tone(audio,256,80); firedBullets++;
          }
          else if(unit[shooter].orientation == LEFT)
          {
            fireBullet(EXHAUST, 4, 20, 15, -1.0, 0.5); tone(audio,256,80); firedBullets++;
          }
        }        
      }
      else if(!unit[target].groundUnit)
      {
        if(counter % 3 == 0)
        {
          if(unit[shooter].orientation == RIGHT)
          {
            if(firedBullets < 16){ fireBullet(unit[shooter].groundAttackWeapon, 2, 11, 15, 1.0, 0.0); tone(audio,128,20); firedBullets++; }
          }
          else if(unit[shooter].orientation == LEFT)
          {
            if(firedBullets < 16){ fireBullet(unit[shooter].groundAttackWeapon, 2, 20, 15, -1.0, 0.0); tone(audio,128,20); firedBullets++; }
          }
        }       
      }      
    }

    if(counter == 150)
    {
      // Show percentage to hit
      matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
      matrix.setTextColor(matrix.Color333(2, 2, 0));
      matrix.setCursor(7, 24);
      matrix.println(100 - hitValue);
      matrix.setCursor(19, 24);
      matrix.print("%");
    }
    
    delay(1);
    do
    {
    }
    while(abs(millis() - synchPoint) < 20);
    counter++;
  }
  while(counter < 250 && !reset);  
}

void shotVector(int xShooter, int yShooter, int xTarget, int yTarget, float &xVector, float &yVector)
{
  // Calculate directional vector of the shot (pointing to target)
  float x = xTarget - xShooter;
  float y = yTarget - yShooter;

  // Calculate length of directional vector to normalize the shot speed to 1.0
  float z = sqrt(sq(x) + sq(y));
  xVector = (x / z);
  yVector = (y / z);
}

void targetAnimation(byte shooter, byte target, byte damage)
{
  int counter = 0;
  byte firedBullets = 0;

  float xVector;
  float yVector;

  int missileOffset;
  boolean tooShort;

  // Clean playfield
  for(byte i = 0; i < 32; i++)
  {
    for(byte j = 0; j < 32; j++)
    {
      playfield[i][j] = 0;
    }
  }

  bulletHitCounter = 0;
  bulletHitExplodeThreshold = 255;
  boolean explosion = false;

  initializeBulletsAndExplosions();
  drawScene(target, true, true);  
  
  do
  {
    // Get time of engine loop start point (important for a constant game speed)
    synchPoint = millis();

    // Update resetAndPauseTimer   
    resetAndPauseTimer.update();
    if(reset){ break; }
    
    // Update scene
    if(counter < 200)
    { 
      if(explosion){ drawScene(target, false, false); }
      else{ drawScene(target, false, true); }
    }

    // Update animated rotor blades for air units
    if(!unit[target].groundUnit && !explosion){ rotorBladeAnimation(target, counter); }

    // Move bullets
    moveBullets();

    // Update explosions();
    updateExplosions();

    if(counter > 50 && damage == 0)
    {
      // Miss
      if(unit[target].orientation == RIGHT)
      {
        if(unit[target].groundUnit)
        {
          if((unit[shooter].type == SCOUT || unit[shooter].type == AA || unit[shooter].type == SAM) && counter % 5 == 0 && firedBullets < 8)
          {
            if(counter == 51){ if(random(10) < 5){ tooShort = true; } else { tooShort = false; }}
            if(tooShort){ shotVector(63.0, 21.0, 14.0 + random(18), 23, xVector, yVector); }
            else{ shotVector(63.0, 21.0, 8.0, 12 + random(7), xVector, yVector); }
            fireBullet(unit[shooter].groundAttackWeapon, 2, 63.0, 21.0, xVector, yVector);
            firedBullets++;             
          }
          if(unit[shooter].type == LIGHTTANK && counter % 3 == 0 && firedBullets < 16)
          {
            if(counter == 51){ if(random(10) < 5){ tooShort = true; } else { tooShort = false; }}
            if(tooShort){ shotVector(63.0, 21.0, 14.0 + random(18), 23, xVector, yVector); }
            else{ shotVector(63.0, 21.0, 8.0, 12 + random(7), xVector, yVector); }
            fireBullet(unit[shooter].groundAttackWeapon, 2, 63.0, 21.0, xVector, yVector);
            firedBullets++;                         
          }
          if(unit[shooter].type == TANK && firedBullets < 1)
          {
            if(random(10) < 5){ shotVector(63.0, 21.0, 16.0 + random(16), 23, xVector, yVector); }
            else{ shotVector(63.0, 21.0, 8.0, 12 + random(7), xVector, yVector); }
            fireBullet(unit[shooter].groundAttackWeapon, 5, 63.0, 21.0, xVector, yVector);
            firedBullets++;                                     
          }
          if(unit[shooter].type == HEAVYTANK && firedBullets < 1)
          {
            if(random(10) < 5){ shotVector(63.0, 21.0, 16.0 + random(16), 23, xVector, yVector); }
            else{ shotVector(63.0, 21.0, 8.0, 12 + random(7), xVector, yVector); }
            fireBullet(unit[shooter].groundAttackWeapon, 6, 63.0, 21.0, xVector, yVector);
            firedBullets++;                         
          }
          if(unit[shooter].type == HOWITZER && firedBullets < 1)
          {
            fireBullet(unit[shooter].groundAttackWeapon, 6, 36.0 + random(16), 0.0, -1.0, 1.0);
            firedBullets++;                                     
          }
          if(unit[shooter].type == LAUNCHER)
          {
            if(counter == 51 || counter == 71 || counter == 91){ missileOffset = 8 + random(16); }
            if(counter == 51 || counter == 52 || counter == 71 || counter == 72 || counter == 91 || counter == 92)
            {
              fireBullet(unit[shooter].groundAttackWeapon, 5, 28.0 + missileOffset, 0.0, -1.0, 1.0);
              firedBullets++;                                                   
            }
            if(counter == 53 || counter == 73 || counter == 93)
            {
              fireBullet(EXHAUST, 4, 28.0 + missileOffset, 0.0, -1.0, 1.0);
              firedBullets++;                                                                 
            }            
          }
          if(unit[shooter].type == ATTACKCOPTER && counter % 3 == 0 && firedBullets < 16)
          {
            shotVector(50.0, 0.0, 12.0 + random(20), 21.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, 50.0, 0.0, xVector, yVector);
            firedBullets++;                                     
          }
          if(unit[shooter].type == HEAVYCOPTER)
          {
            if(counter == 51){ missileOffset = 8 + random(16); }
            if(counter == 51 || counter == 52)
            {
              shotVector(50.0, 0.0, 8.0 + missileOffset, 21.0, xVector, yVector);
              fireBullet(unit[shooter].groundAttackWeapon, 5, 50.0, 0.0, xVector, yVector);
              firedBullets++;                                     
            }
            if(counter == 53)
            {
              shotVector(50.0, 0.0, 8.0 + missileOffset, 21.0, xVector, yVector);
              fireBullet(EXHAUST, 4, 50.0, 0.0, xVector, yVector);
              firedBullets++;                                     
            }                        
          }
        }
        else if(!unit[target].groundUnit)
        {
          if((unit[shooter].type == SCOUT || unit[shooter].type == TANK || unit[shooter].type == HEAVYTANK || unit[shooter].type == HOWITZER || unit[shooter].type == LAUNCHER) && counter % 5 == 0 && firedBullets < 8)
          {
            shotVector(57.0, 63.0, 12.0 + random(16), 14.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, 57.0, 63.0, xVector, yVector);
            firedBullets++;                                  
          }
          if(unit[shooter].type == LIGHTTANK && counter % 3 == 0 && firedBullets < 16)
          {
            shotVector(57.0, 63.0, 12.0 + random(16), 14.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, 57.0, 63.0, xVector, yVector);
            firedBullets++;                                              
          }
          if(unit[shooter].type == AA && (counter == 51 || counter == 76 || counter == 101))
          {
            shotVector(57.0, 63.0, 12.0 + random(16), 14.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, 57.0, 63.0, xVector, yVector);
            firedBullets++;                                                          
          }
          if(unit[shooter].type == SAM)
          {
            if(counter == 51){ missileOffset = 4 + random(16); }
            if(counter == 51 || counter == 52)
            {
              shotVector(57.0, 63.0, 6.0 + missileOffset, 14.0, xVector, yVector);
              fireBullet(unit[shooter].groundAttackWeapon, 4, 57.0, 63.0, xVector, yVector);
              firedBullets++;                                     
            }
            if(counter == 53)
            {
              shotVector(57.0, 63.0, 6.0 + missileOffset, 14.0, xVector, yVector);
              fireBullet(EXHAUST, 4, 57.0, 63.0, xVector, yVector);
              firedBullets++;                                     
            }                                    
          }
          if(unit[shooter].type == ATTACKCOPTER)
          {
            if(counter == 51 || counter == 52)
            {
              if(counter == 51){ missileOffset = 5 + random(5); }
              shotVector(63.0, 14.0, 8.0, 13.0 + missileOffset, xVector, yVector);
              fireBullet(unit[shooter].groundAttackWeapon, 4, 63.0, 14.0, xVector, yVector);
              firedBullets++;                                     
            }
            if(counter == 53)
            {
              shotVector(63.0, 14.0, 8.0, 13.0 + missileOffset, xVector, yVector);
              fireBullet(EXHAUST, 3, 63.0, 14.0, xVector, yVector);
              firedBullets++;                                     
            }                                                
          }
          if(unit[shooter].type == HEAVYCOPTER && counter % 3 == 0 && firedBullets < 16)
          {
            shotVector(63.0, 14.0, 8.0, 17.0 + random(5), xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, 63.0, 14.0, xVector, yVector);
            firedBullets++;                                                          
          }
        }
      }
      else if(unit[target].orientation == LEFT)
      {        
        if(unit[target].groundUnit)
        {
          if((unit[shooter].type == SCOUT || unit[shooter].type == AA || unit[shooter].type == SAM) && counter % 5 == 0 && firedBullets < 8)
          {
            if(counter == 50){ if(random(10) < 5){ tooShort = true; } else { tooShort = false; }}
            if(tooShort){ shotVector(-32.0, 21.0, 17 - random(18), 23, xVector, yVector); }
            else{ shotVector(-32.0, 21.0, 23.0, 12 + random(7), xVector, yVector); }
            fireBullet(unit[shooter].groundAttackWeapon, 2, -32.0, 21.0, xVector, yVector);
            firedBullets++;             
          }
          if(unit[shooter].type == LIGHTTANK && counter % 3 == 0 && firedBullets < 16)
          {
            if(counter == 50){ if(random(10) < 5){ tooShort = true; } else { tooShort = false; }}
            if(tooShort){ shotVector(-32.0, 21.0, 17 - random(18), 23, xVector, yVector); }
            else{ shotVector(-32.0, 21.0, 23.0, 12 + random(7), xVector, yVector); }
            fireBullet(unit[shooter].groundAttackWeapon, 2, -32.0, 21.0, xVector, yVector);
            firedBullets++;                         
          }
          if(unit[shooter].type == TANK && firedBullets < 1)
          {
            if(random(10) < 5){ shotVector(-32.0, 21.0, 17 - random(18), 23, xVector, yVector); }
            else{ shotVector(-32.0, 21.0, 23.0, 12 + random(7), xVector, yVector); }
            fireBullet(unit[shooter].groundAttackWeapon, 5, -32.0, 21.0, xVector, yVector);
            firedBullets++;                                     
          }
          if(unit[shooter].type == HEAVYTANK && firedBullets < 1)
          {
            if(random(10) < 5){ shotVector(-32.0, 21.0, 17 - random(18), 23, xVector, yVector); }
            else{ shotVector(-32.0, 21.0, 23.0, 12 + random(7), xVector, yVector); }
            fireBullet(unit[shooter].groundAttackWeapon, 6, -32.0, 21.0, xVector, yVector);
            firedBullets++;                         
          }
          if(unit[shooter].type == HOWITZER && firedBullets < 1)
          {
            fireBullet(unit[shooter].groundAttackWeapon, 6, -5.0 - random(16), 0.0, 1.0, 1.0);
            firedBullets++;                                     
          }
          if(unit[shooter].type == LAUNCHER)
          {
            if(counter == 51 || counter == 71 || counter == 91){ missileOffset = random(16); }
            if(counter == 51 || counter == 52 || counter == 71 || counter == 72 || counter == 91 || counter == 92)
            {
              fireBullet(unit[shooter].groundAttackWeapon, 5, -5.0 - missileOffset, 0.0, 1.0, 1.0);
              firedBullets++;                                                   
            }
            if(counter == 53 || counter == 73 || counter == 93)
            {
              fireBullet(EXHAUST, 4, -5.0 - missileOffset, 0.0, 1.0, 1.0);
              firedBullets++;                                                                 
            }                        
          }
          if(unit[shooter].type == ATTACKCOPTER && counter % 3 == 0 && firedBullets < 16)
          {
            shotVector(-32.0, 1.0, 19.0 - random(20), 21.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, -32.0, 1.0, xVector, yVector);
            firedBullets++;                                     
          }
          if(unit[shooter].type == HEAVYCOPTER)
          {
            if(counter == 51){ missileOffset = random(16); }
            if(counter == 51 || counter == 52)
            {
              shotVector(-32.0, 1.0, 15.0 - missileOffset, 21.0, xVector, yVector);
              fireBullet(unit[shooter].groundAttackWeapon, 5, -32.0, 1.0, xVector, yVector);
              firedBullets++;                                     
            }
            if(counter == 53)
            {
              shotVector(-32.0, 1.0, 15.0 - missileOffset, 21.0, xVector, yVector);
              fireBullet(EXHAUST, 4, -32.0, 1.0, xVector, yVector);
              firedBullets++;                                     
            }                        
          }
        }
        else if(!unit[target].groundUnit)
        {
          if((unit[shooter].type == SCOUT || unit[shooter].type == TANK || unit[shooter].type == HEAVYTANK || unit[shooter].type == HOWITZER || unit[shooter].type == LAUNCHER) && counter % 5 == 0 && firedBullets < 8)
          {
            shotVector(-26.0, 63.0, 15.0 - random(16), 14.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, -26.0, 63.0, xVector, yVector);
            firedBullets++;                                  
          }
          if(unit[shooter].type == LIGHTTANK && counter % 3 == 0 && firedBullets < 16)
          {
            shotVector(-26.0, 63.0, 15.0 - random(16), 14.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, -26.0, 63.0, xVector, yVector);
            firedBullets++;                                              
          }
          if(unit[shooter].type == AA && (counter == 51 || counter == 76 || counter == 101))
          {
            shotVector(-26.0, 63.0, 15.0 - random(16), 14.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 4, -26.0, 63.0, xVector, yVector);
            firedBullets++;                                                          
          }
          if(unit[shooter].type == SAM)
          {
            if(counter == 51){ missileOffset = random(16); }
            if(counter == 51 || counter == 52)
            {
              shotVector(-26.0, 63.0, 15.0 - missileOffset, 14.0, xVector, yVector);
              fireBullet(unit[shooter].groundAttackWeapon, 4, -26.0, 63.0, xVector, yVector);
              firedBullets++;                                     
            }
            if(counter == 53)
            {
              shotVector(-26.0, 63.0, 15.0 - missileOffset, 14.0, xVector, yVector);
              fireBullet(EXHAUST, 3, -26.0, 63.0, xVector, yVector);
              firedBullets++;                                     
            }                                    
          }
          if(unit[shooter].type == ATTACKCOPTER)
          {
            if(counter == 51){ missileOffset = 5 + random(5); }
            if(counter == 51 || counter == 52)
            {
              shotVector(-32.0, 14.0, 23.0, 13.0 + missileOffset, xVector, yVector);
              fireBullet(unit[shooter].groundAttackWeapon, 4, -32.0, 14.0, xVector, yVector);
              firedBullets++;                                     
            }
            if(counter == 53)
            {
              shotVector(-32.0, 14.0, 23.0, 13.0 + missileOffset, xVector, yVector);
              fireBullet(EXHAUST, 3, -32.0, 14.0, xVector, yVector);
              firedBullets++;                                     
            }                                                
          }
          if(unit[shooter].type == HEAVYCOPTER && counter % 3 == 0 && firedBullets < 16)
          {
            shotVector(-32.0, 14.0, 23.0, 17.0 + random(5), xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, -32.0, 14.0, xVector, yVector);
            firedBullets++;                                                          
          }          
        }
      }      
    }
    
    else if(counter > 50 && damage > 0)
    {
      // Hit
      if(unit[target].orientation == RIGHT)
      {
        if(unit[target].groundUnit)
        {
          if((unit[shooter].type == SCOUT || unit[shooter].type == AA || unit[shooter].type == SAM) && counter % 5 == 0 && firedBullets < 8)
          {
            shotVector(63.0, 21.0, 8.0, 19.0 + random(5), xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, 63.0, 21.0, xVector, yVector);
            firedBullets++;
            bulletHitExplodeThreshold = 4;             
          }
          if(unit[shooter].type == LIGHTTANK && counter % 3 == 0 && firedBullets < 16)
          {
            shotVector(63.0, 21.0, 8.0, 19.0 + random(5), xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, 63.0, 21.0, xVector, yVector);
            firedBullets++;                         
            bulletHitExplodeThreshold = 8;             
          }
          if(unit[shooter].type == TANK && firedBullets < 1)
          {
            shotVector(63.0, 21.0, 8.0, 20.0 + random(3), xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 5, 63.0, 21.0, xVector, yVector);
            firedBullets++;                                     
            bulletHitExplodeThreshold = 1;             
          }
          if(unit[shooter].type == HEAVYTANK && firedBullets < 1)
          {
            shotVector(63.0, 21.0, 8.0, 20.0 + random(3), xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 6, 63.0, 21.0, xVector, yVector);
            firedBullets++;                         
            bulletHitExplodeThreshold = 1;             
          }
          if(unit[shooter].type == HOWITZER && firedBullets < 1)
          {
            fireBullet(unit[shooter].groundAttackWeapon, 6, 27.0 + random(5), 0.0, -1.0, 1.0);
            firedBullets++;                                     
            bulletHitExplodeThreshold = 1;             
          }
          if(unit[shooter].type == LAUNCHER)
          {
            if(counter == 51 || counter == 71 || counter == 91){ missileOffset = random(7); }
            if(counter == 51 || counter == 52 || counter == 71 || counter == 72 || counter == 91 || counter == 92)
            {
              fireBullet(unit[shooter].groundAttackWeapon, 5, 26.0 + missileOffset, 0.0, -1.0, 1.0);
              firedBullets++;                                                   
            }
            if(counter == 53 || counter == 73 || counter == 93)
            {
              fireBullet(EXHAUST, 4, 26.0 + missileOffset, 0.0, -1.0, 1.0);
              firedBullets++;                                                                 
              bulletHitExplodeThreshold = 2;             
            }            
          }
          if(unit[shooter].type == ATTACKCOPTER && counter % 3 == 0 && firedBullets < 16)
          {
            shotVector(50.0, 0.0, 5.0 + random(7), 21.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, 50.0, 0.0, xVector, yVector);
            firedBullets++;                                     
            bulletHitExplodeThreshold = 8;             
          }
          if(unit[shooter].type == HEAVYCOPTER)
          {
            if(counter == 51){ missileOffset = random(5); }
            if(counter == 51 || counter == 52)
            {
              shotVector(50.0, 0.0, 6.0 + missileOffset, 21.0, xVector, yVector);
              fireBullet(unit[shooter].groundAttackWeapon, 5, 50.0, 0.0, xVector, yVector);
              firedBullets++;                                     
            }
            if(counter == 53)
            {
              shotVector(50.0, 0.0, 6.0 + missileOffset, 21.0, xVector, yVector);
              fireBullet(EXHAUST, 4, 50.0, 0.0, xVector, yVector);
              firedBullets++;                                     
              bulletHitExplodeThreshold = 1;             
            }                        
          }
        }
        else if(!unit[target].groundUnit)
        {
          if((unit[shooter].type == SCOUT || unit[shooter].type == TANK || unit[shooter].type == HEAVYTANK || unit[shooter].type == HOWITZER || unit[shooter].type == LAUNCHER) && counter % 5 == 0 && firedBullets < 8)
          {
            shotVector(57.0, 63.0, 5.0 + random(7), 14.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, 57.0, 63.0, xVector, yVector);
            firedBullets++;                                  
            bulletHitExplodeThreshold = 4;             
          }
          if(unit[shooter].type == LIGHTTANK && counter % 3 == 0 && firedBullets < 16)
          {
            shotVector(57.0, 63.0, 5.0 + random(7), 14.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, 57.0, 63.0, xVector, yVector);
            firedBullets++;                                              
            bulletHitExplodeThreshold = 8;             
          }
          if(unit[shooter].type == AA && (counter == 51 || counter == 76 || counter == 101))
          {
            shotVector(57.0, 63.0, 6.0 + random(5), 14.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, 57.0, 63.0, xVector, yVector);
            firedBullets++;                                                          
            bulletHitExplodeThreshold = 2;             
          }
          if(unit[shooter].type == SAM)
          {
            if(counter == 51){ missileOffset = random(3); }
            if(counter == 51 || counter == 52)
            {
              shotVector(57.0, 63.0, 7.0 + missileOffset, 14.0, xVector, yVector);
              fireBullet(unit[shooter].groundAttackWeapon, 4, 57.0, 63.0, xVector, yVector);
              firedBullets++;                                     
            }
            if(counter == 53)
            {
              shotVector(57.0, 63.0, 6.0 + missileOffset, 14.0, xVector, yVector);
              fireBullet(EXHAUST, 4, 57.0, 63.0, xVector, yVector);
              firedBullets++;                                     
              bulletHitExplodeThreshold = 1;             
            }                                    
          }
          if(unit[shooter].type == ATTACKCOPTER)
          {
            if(counter == 51 || counter == 52)
            {
              if(counter == 51){ missileOffset = random(3); }
              shotVector(63.0, 14.0, 8.0, 13.0 + missileOffset, xVector, yVector);
              fireBullet(unit[shooter].groundAttackWeapon, 4, 63.0, 14.0, xVector, yVector);
              firedBullets++;                                     
            }
            if(counter == 53)
            {
              shotVector(63.0, 14.0, 8.0, 13.0 + missileOffset, xVector, yVector);
              fireBullet(EXHAUST, 3, 63.0, 14.0, xVector, yVector);
              firedBullets++;                                     
              bulletHitExplodeThreshold = 1;             
            }                                                
          }
          if(unit[shooter].type == HEAVYCOPTER && counter % 3 == 0 && firedBullets < 16)
          {
            shotVector(63.0, 14.0, 8.0, 12.0 + random(5), xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, 63.0, 14.0, xVector, yVector);
            firedBullets++;                                                          
            bulletHitExplodeThreshold = 8;             
          }
        }
      }
      else if(unit[target].orientation == LEFT)
      {        
        if(unit[target].groundUnit)
        {
          if((unit[shooter].type == SCOUT || unit[shooter].type == AA || unit[shooter].type == SAM) && counter % 5 == 0 && firedBullets < 8)
          {
            shotVector(-32.0, 21.0, 23.0, 19.0 + random(5), xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, -32.0, 21.0, xVector, yVector);
            firedBullets++;             
            bulletHitExplodeThreshold = 4;             
          }
          if(unit[shooter].type == LIGHTTANK && counter % 3 == 0 && firedBullets < 16)
          {
            shotVector(-32.0, 21.0, 23.0, 19.0 + random(5), xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, -32.0, 21.0, xVector, yVector);
            firedBullets++;                         
            bulletHitExplodeThreshold = 8;             
          }
          if(unit[shooter].type == TANK && firedBullets < 1)
          {
            shotVector(-32.0, 21.0, 23.0, 20.0 + random(3), xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 5, -32.0, 21.0, xVector, yVector);
            firedBullets++;                                     
            bulletHitExplodeThreshold = 1;             
          }
          if(unit[shooter].type == HEAVYTANK && firedBullets < 1)
          {
            shotVector(-32.0, 21.0, 23.0, 20.0 + random(3), xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 6, -32.0, 21.0, xVector, yVector);
            firedBullets++;                         
            bulletHitExplodeThreshold = 1;             
          }
          if(unit[shooter].type == HOWITZER && firedBullets < 1)
          {
            fireBullet(unit[shooter].groundAttackWeapon, 6, 0.0 + random(5), 0.0, 1.0, 1.0);
            firedBullets++;                                     
            bulletHitExplodeThreshold = 1;             
          }
          if(unit[shooter].type == LAUNCHER)
          {
            if(counter == 51 || counter == 71 || counter == 91){ missileOffset = random(7); }
            if(counter == 51 || counter == 52 || counter == 71 || counter == 72 || counter == 91 || counter == 92)
            {
              fireBullet(unit[shooter].groundAttackWeapon, 5, -1.0 + missileOffset, 0.0, 1.0, 1.0);
              firedBullets++;                                                   
            }
            if(counter == 53 || counter == 73 || counter == 93)
            {
              fireBullet(EXHAUST, 4, -1.0 + missileOffset, 0.0, 1.0, 1.0);
              firedBullets++;                                                                 
              bulletHitExplodeThreshold = 2;             
            }                        
          }
          if(unit[shooter].type == ATTACKCOPTER && counter % 3 == 0 && firedBullets < 16)
          {
            shotVector(-32.0, 1.0, 20.0 + random(7), 21.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, -32.0, 1.0, xVector, yVector);
            firedBullets++;                                     
            bulletHitExplodeThreshold = 8;             
          }
          if(unit[shooter].type == HEAVYCOPTER)
          {
            if(counter == 51){ missileOffset = random(5); }
            if(counter == 51 || counter == 52)
            {
              shotVector(-32.0, 1.0, 21.0 + missileOffset, 21.0, xVector, yVector);
              fireBullet(unit[shooter].groundAttackWeapon, 5, -32.0, 1.0, xVector, yVector);
              firedBullets++;                                     
            }
            if(counter == 53)
            {
              shotVector(-32.0, 1.0, 21.0 + missileOffset, 21.0, xVector, yVector);
              fireBullet(EXHAUST, 4, -32.0, 1.0, xVector, yVector);
              firedBullets++;                                     
              bulletHitExplodeThreshold = 1;             
            }                        
          }
        }
        else if(!unit[target].groundUnit)
        {
          if((unit[shooter].type == SCOUT || unit[shooter].type == TANK || unit[shooter].type == HEAVYTANK || unit[shooter].type == HOWITZER || unit[shooter].type == LAUNCHER) && counter % 5 == 0 && firedBullets < 8)
          {
            shotVector(-26.0, 63.0, 20.0 + random(7), 14.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, -26.0, 63.0, xVector, yVector);
            firedBullets++;                                  
            bulletHitExplodeThreshold = 4;             
          }
          if(unit[shooter].type == LIGHTTANK && counter % 3 == 0 && firedBullets < 16)
          {
            shotVector(-26.0, 63.0, 20.0 + random(7), 14.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, -26.0, 63.0, xVector, yVector);
            firedBullets++;                                              
            bulletHitExplodeThreshold = 8;             
          }
          if(unit[shooter].type == AA && (counter == 51 || counter == 76 || counter == 101))
          {
            shotVector(-26.0, 63.0, 21.0 + random(5), 14.0, xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 4, -26.0, 63.0, xVector, yVector);
            firedBullets++;                                                          
            bulletHitExplodeThreshold = 2;             
          }
          if(unit[shooter].type == SAM)
          {
            if(counter == 51){ missileOffset = random(3); }
            if(counter == 51 || counter == 52)
            {
              shotVector(-26.0, 63.0, 22.0 + missileOffset, 14.0, xVector, yVector);
              fireBullet(unit[shooter].groundAttackWeapon, 4, -26.0, 63.0, xVector, yVector);
              firedBullets++;                                     
            }
            if(counter == 53)
            {
              shotVector(-26.0, 63.0, 21.0 + missileOffset, 14.0, xVector, yVector);
              fireBullet(EXHAUST, 3, -26.0, 63.0, xVector, yVector);
              firedBullets++;                                     
              bulletHitExplodeThreshold = 1;             
            }                                    
          }
          if(unit[shooter].type == ATTACKCOPTER)
          {
            if(counter == 51){ missileOffset = random(3); }
            if(counter == 51 || counter == 52)
            {
              shotVector(-32.0, 14.0, 23.0, 13.0 + missileOffset, xVector, yVector);
              fireBullet(unit[shooter].groundAttackWeapon, 4, -32.0, 14.0, xVector, yVector);
              firedBullets++;                                     
            }
            if(counter == 53)
            {
              shotVector(-32.0, 14.0, 23.0, 13.0 + missileOffset, xVector, yVector);
              fireBullet(EXHAUST, 3, -32.0, 14.0, xVector, yVector);
              firedBullets++;                                     
              bulletHitExplodeThreshold = 1;             
            }                                                
          }
          if(unit[shooter].type == HEAVYCOPTER && counter % 3 == 0 && firedBullets < 16)
          {
            shotVector(-32.0, 14.0, 23.0, 12.0 + random(5), xVector, yVector);
            fireBullet(unit[shooter].groundAttackWeapon, 2, -32.0, 14.0, xVector, yVector);
            firedBullets++;                                                          
            bulletHitExplodeThreshold = 8;             
          }          
        }
      }      
    }

    // Unit explodes
    if(bulletHitCounter == bulletHitExplodeThreshold && unit[target].hitPoints < 1 && !explosion)
    {
      explosion = true;
      if(unit[target].orientation == RIGHT)
      {
        if(unit[target].groundUnit)
        {
          initializeExplosion(min(unit[target].armor + 4, 11), 1, 8, 21);            
          initializeExplosion(min(unit[target].armor + 2, 9), 1, 6, 21);            
          initializeExplosion(min(unit[target].armor + 2, 9), 1, 10, 21);                                
        }
        else if(!unit[target].groundUnit)
        {
          initializeExplosion(unit[target].armor + 2, 1, 8, 14);                      
        }
      }
      else if(unit[target].orientation == LEFT)
      {
        if(unit[target].groundUnit)
        {
          initializeExplosion(min(unit[target].armor + 4, 11), 1, 23, 21);            
          initializeExplosion(min(unit[target].armor + 2, 9), 1, 21, 21);            
          initializeExplosion(min(unit[target].armor + 2, 9), 1, 25, 21);            
        }
        else if(!unit[target].groundUnit)
        {
          initializeExplosion(unit[target].armor + 2, 1, 23, 14);            
        }
      }
    }
    
    if(counter == 200)
    {
      // Show caused damage
      matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
      if(damage == 0)
      {
        matrix.setTextColor(matrix.Color333(0, 2, 0));
        matrix.setCursor(4, 24);
        matrix.print("Miss");    
      }
      else
      {
        matrix.setTextColor(matrix.Color333(2, 0, 0));
        matrix.setCursor(13, 24);
        matrix.print(damage);        
      }
    }
    
    if(counter == 250)
    {
      // Show damage of unit
      matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
      matrix.drawLine(2, 24, 6, 24, matrix.Color333(2, 2, 2));
      matrix.drawLine(2, 25, 2, 28, matrix.Color333(2, 2, 2));
      matrix.drawLine(6, 25, 6, 28, matrix.Color333(2, 2, 2));
      matrix.drawLine(3, 29, 5, 29, matrix.Color333(2, 2, 2));
      if(unit[target].actor == RED)
      {
        matrix.drawLine(3, 27, 5, 25, matrix.Color333(2, 0, 0));
      }
      else
      {
        matrix.drawLine(3, 27, 5, 25, matrix.Color333(0, 0, 2));    
      }
    
      matrix.setCursor(9, 24);
      if(unit[target].hitPoints < 3)
      {
        matrix.setTextColor(matrix.Color333(3, 0, 0));    
      }
      else
      {
        matrix.setTextColor(matrix.Color333(2, 2, 2));        
      }
      matrix.print(unit[target].hitPoints);
      matrix.setTextColor(matrix.Color333(2, 2, 2));
      matrix.setCursor(17, 24);
      matrix.print("/");
      matrix.setCursor(25, 24);
      matrix.print(unit[target].armor);
    }
    
    delay(1);
    do
    {
    }
    while(abs(millis() - synchPoint) < 20);
    counter++;
  }
  while(counter < 300 && !reset);
}

void experienceAnimation(byte shooter)
{
  int counter = 0;

  drawScene(shooter, true, true);  

  do
  {
    // Get time of engine loop start point (important for a constant game speed)
    synchPoint = millis();

    // Update resetAndPauseTimer   
    resetAndPauseTimer.update();
    if(reset){ break; }
    
    // Update animated rotor blades for air units
    if(!unit[shooter].groundUnit){ rotorBladeAnimation(shooter, counter); }

    if(counter == 50)
    {  
      matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
      matrix.setTextColor(matrix.Color333(2, 0, 0));
      matrix.setCursor(4, 24);
      matrix.print("Kill");    
    }

    // SOUND: Kill
    if(counter == 50){ tone(audio, NOTE_C4, 250); };
    if(counter == 68){ tone(audio, NOTE_C4, 250); };
    if(counter == 86){ tone(audio, NOTE_C4, 250); };
    if(counter == 104){ tone(audio, NOTE_G4, 600); };

    if(counter == 100)
    {  
      // Show experience
      matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
      if(unit[shooter].experience > 0){ drawKillMarking(9, 26); }
      if(unit[shooter].experience > 1){ drawKillMarking(13, 26); }
      if(unit[shooter].experience > 2){ drawKillMarking(17, 26); }
      if(unit[shooter].experience > 3){ drawKillMarking(21, 26); }
    }
    
    delay(1);
    do
    {
    }
    while(abs(millis() - synchPoint) < 20);
    counter++;
  }
  while(counter < 200 && !reset);
}

void fixAnimation(byte shooter, byte fixedHitPoints)
{
  int counter = 0;

  drawScene(shooter, true, true);  

  do
  {
    // Get time of engine loop start point (important for a constant game speed)
    synchPoint = millis();

    // Update resetAndPauseTimer   
    resetAndPauseTimer.update();
    if(reset){ break; }
    
    // Update animated rotor blades for air units
    if(!unit[shooter].groundUnit){ rotorBladeAnimation(shooter, counter); }

    if(counter == 50)
    {  
      // Show screw-wrench as symbol for fixing
      matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
      matrix.drawLine(14, 25, 14, 27, matrix.Color333(2, 2, 2));
      matrix.drawLine(17, 25, 17, 27, matrix.Color333(2, 2, 2));
      matrix.fillRect(15, 27, 2, 4, matrix.Color333(2, 2, 2));
    }
      
    // SOUND: Fix
    if(counter >= 50 && counter < 100)
    {
      tone(audio, 500 + ((counter - 50) * 10), 5);
      delay(5);
    }

    if(counter == 100)
    {  
      // Show fixed hit points
      matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
      matrix.setTextColor(matrix.Color333(0, 2, 0));
      matrix.setCursor(13, 24);
      matrix.print(fixedHitPoints);  
    }      

    if(counter == 150)
    {
      // Show damage of unit
      matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
      matrix.drawLine(2, 24, 6, 24, matrix.Color333(2, 2, 2));
      matrix.drawLine(2, 25, 2, 28, matrix.Color333(2, 2, 2));
      matrix.drawLine(6, 25, 6, 28, matrix.Color333(2, 2, 2));
      matrix.drawLine(3, 29, 5, 29, matrix.Color333(2, 2, 2));
      if(unit[shooter].actor == RED)
      {
        matrix.drawLine(3, 27, 5, 25, matrix.Color333(2, 0, 0));
      }
      else
      {
        matrix.drawLine(3, 27, 5, 25, matrix.Color333(0, 0, 2));    
      }
    
      matrix.setCursor(9, 24);
      if(unit[shooter].hitPoints < 3)
      {
        matrix.setTextColor(matrix.Color333(3, 0, 0));    
      }
      else
      {
        matrix.setTextColor(matrix.Color333(2, 2, 2));        
      }
      matrix.print(unit[shooter].hitPoints);
      matrix.setTextColor(matrix.Color333(2, 2, 2));
      matrix.setCursor(17, 24);
      matrix.print("/");
      matrix.setCursor(25, 24);
      matrix.print(unit[shooter].armor);
    }

    delay(1);
    do
    {
    }
    while(abs(millis() - synchPoint) < 20);
    counter++;
  }
  while(counter < 250 && !reset);
}

// drawScene: Renders the unit shownUnit within the landscape. If complete == false, only the area which can be overwritten by bullets and explosions is updated.
void drawScene(byte shownUnit, boolean completeLandscape, boolean visualizeUnit)
{
  // Clear screen
  if(completeLandscape){ matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0)); }

  if(unit[shownUnit].groundUnit)
  {
    // Ground unit
      
    // Clouds
    for(byte i = 0; i < 3; i++)
    {
      for(byte j = 0; j < 32; j++)
      {
        if((i + j) % 2 == 0){ matrix.drawPixel(j, i, matrix.Color333(0, 0, 1)); }
      }
    }

    // Mountains
    if(completeLandscape)
    {
      mountainBackground[0] = random(5) + 6;
      matrix.drawPixel(0, mountainBackground[0], matrix.Color333(1, 1, 1));
      for(byte i = 1; i < 32; i++)
      {
        mountainBackground[i] = mountainBackground[i - 1] + random(3) - 1;
        if(mountainBackground[i] < 6){ mountainBackground[i] = 6; }
        if(mountainBackground[i] > 10){ mountainBackground[i] = 10; }
      }      
    }
    for(byte i = 1; i < 32; i++)
    {
      matrix.drawPixel(i, mountainBackground[i], matrix.Color333(1, 1, 1));
    }
    
    // Unit in wood
    if(completeLandscape && getMapSurface(unit[shownUnit].xPosition, unit[shownUnit].yPosition) == WOOD)
    {
      for(byte i = 0; i < 8; i++)
      {
        do
        {
          treeX[i] = random(32);
        }
        while((unit[shownUnit].orientation == RIGHT && (treeX[i] > 3 && treeX[i] < 12)) || (unit[shownUnit].orientation == LEFT && (treeX[i] > 19 && treeX[i] < 28)));
        treeY[i] = random(3) + 14;
        treeD[i] = random(2) + 2;
        treeR[i] = random(2);
        treeG[i] = random(4) + 4;
        treeB[i] = random(2);
      }
    }
    if(getMapSurface(unit[shownUnit].xPosition, unit[shownUnit].yPosition) == WOOD)
    {
      for(byte i = 0; i < 8; i++)
      {
        matrix.drawCircle(treeX[i], treeY[i], treeD[i], matrix.Color444(treeR[i], treeG[i], treeB[i]));
        matrix.drawLine(treeX[i], treeY[i] + 3, treeX[i], 22, matrix.Color333(2, 1, 0));        
      }
    }

    // Surface
    if(completeLandscape)
    {
      matrix.drawLine(0, 23, 31, 23, matrix.Color333(0, 2, 0));
      for(byte i = 0; i < 32; i++)
      {
        playfield[i][23] = 1;
      }
      for(byte i = 24; i < 32; i++)
      {
        for(byte j = 0; j < 32; j++)
        {
          if((i + j) % 2 == 0){ matrix.drawPixel(j, i, matrix.Color333(0, 1, 0)); }
        }
      }
    }

    // Draw unit
    if(visualizeUnit && unit[shownUnit].orientation == RIGHT)
    {
      if(unit[shownUnit].type == SCOUT || unit[shownUnit].type == LIGHTTANK){ drawUnit(unit[shownUnit].type, unit[shownUnit].actor, unit[shownUnit].orientation, true, true, 7, 20); }
      else if(unit[shownUnit].type == TANK){ drawUnit(unit[shownUnit].type, unit[shownUnit].actor, unit[shownUnit].orientation, true, true, 6, 20); }
      else if(unit[shownUnit].type == HOWITZER || unit[shownUnit].type == AA || unit[shownUnit].type == HEAVYCOPTER){ drawUnit(unit[shownUnit].type, unit[shownUnit].actor, unit[shownUnit].orientation, true, true, 7, 19); }
      else { drawUnit(unit[shownUnit].type, unit[shownUnit].actor, unit[shownUnit].orientation, true, true, 6, 19); }
    }
    else if(visualizeUnit && unit[shownUnit].orientation == LEFT)
    {
      if(unit[shownUnit].type == SCOUT){ drawUnit(unit[shownUnit].type, unit[shownUnit].actor, unit[shownUnit].orientation, true, true, 22, 20); }
      else if(unit[shownUnit].type == LIGHTTANK){ drawUnit(unit[shownUnit].type, unit[shownUnit].actor, unit[shownUnit].orientation, true, true, 21, 20); }
      else if(unit[shownUnit].type == TANK){ drawUnit(unit[shownUnit].type, unit[shownUnit].actor, unit[shownUnit].orientation, true, true, 21, 20); }
      else if(unit[shownUnit].type == HOWITZER || unit[shownUnit].type == AA || unit[shownUnit].type == HEAVYCOPTER){ drawUnit(unit[shownUnit].type, unit[shownUnit].actor, unit[shownUnit].orientation, true, true, 21, 19); }
      else { drawUnit(unit[shownUnit].type, unit[shownUnit].actor, unit[shownUnit].orientation, true, true, 21, 19); }
    }    
    
  }
  else
  {
    // Air unit

    // Clouds
    for(byte i = 0; i < 7; i++)
    {
      for(byte j = 0; j < 32; j++)
      {
        if((i + j) % 2 == 0){ matrix.drawPixel(j, i, matrix.Color333(0, 0, 1)); }
      }
    }

    // Mountains
    if(completeLandscape)
    {
      mountainBackground[0] = random(5) + 21;
      matrix.drawPixel(0, mountainBackground[0], matrix.Color333(1, 1, 1));
      for(byte i = 1; i < 32; i++)
      {
        mountainBackground[i] = mountainBackground[i - 1] + random(3) - 1;
        if(mountainBackground[i] < 21){ mountainBackground[i] = 21; }
        if(mountainBackground[i] > 25){ mountainBackground[i] = 25; }
      }
    }
    for(byte i = 1; i < 32; i++)
    {
      matrix.drawPixel(i, mountainBackground[i], matrix.Color333(1, 1, 1));
    }

    // Surface
    for(byte i = 29; i < 32; i++)
    {
      for(byte j = 0; j < 32; j++)
      {
        if((i + j) % 2 == 0){ matrix.drawPixel(j, i, matrix.Color333(0, 1, 0)); }
      }
    }

    // Draw unit
    if(visualizeUnit && unit[shownUnit].orientation == RIGHT)
    {
      drawUnit(unit[shownUnit].type, unit[shownUnit].actor, unit[shownUnit].orientation, true, true, 6, 13);
    }
    else if(visualizeUnit && unit[shownUnit].orientation == LEFT)
    {
      drawUnit(unit[shownUnit].type, unit[shownUnit].actor, unit[shownUnit].orientation, true, true, 21, 13);
    }    
  }
}

void showResults(boolean redHQDestroyed, boolean redUnitsDestroyed, boolean blueHQDestroyed, boolean blueUnitsDestroyed)
{
  boolean redLoses = false;
  boolean blueLoses = false;
  int counter = 0;
  boolean musicPlayed = false;
  
  if(redHQDestroyed || redUnitsDestroyed){ redLoses = true; }
  if(blueHQDestroyed || blueUnitsDestroyed){ blueLoses = true; }

  do
  {

  if(counter == 0)
  {
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
    matrix.drawLine(0, 6, 32, 6, matrix.Color333(2, 2, 0));
    matrix.drawLine(0, 24, 32, 24, matrix.Color333(2, 2, 0));
   
    if(!redLoses && blueLoses)
    {
      matrix.setTextColor(matrix.Color333(3, 0, 0));
      matrix.setCursor(7, 8);
      matrix.println("Red");
      matrix.setCursor(2, 16);
      matrix.println("wins!");    
    }
    else if(redLoses && !blueLoses)
    {
      matrix.setTextColor(matrix.Color333(0, 0, 3));
      matrix.setCursor(4, 8);
      matrix.println("Blue");
      matrix.setCursor(2, 16);
      matrix.println("wins!");
    }
    else
    {
      matrix.setTextColor(matrix.Color333(2, 2, 0));
      matrix.setCursor(4, 12);
      matrix.println("Draw");
    }
  }

  if(!musicPlayed)
  { 
    for(byte i = 0; i < 3; i++)
    {
      tone(audio, NOTE_C4, 200);
      delay(250);
      tone(audio, NOTE_C4, 200);
      delay(250);
      tone(audio, NOTE_C4, 200);
      delay(250);
      tone(audio, NOTE_G4, 700);        
      delay(750);
    }
    musicPlayed = true;
  }

  if(counter == 2000)
  {
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
    matrix.setTextColor(matrix.Color333(2, 2, 0));
    matrix.setCursor(7, 0);
    matrix.println("HQs");
    matrix.setCursor(1, 16);
    matrix.println("Units");

    if(redHQDestroyed)
    {
      matrix.fillCircle(9, 11, 3, matrix.Color333(4, 1, 0));
      matrix.fillCircle(9, 11, 2, matrix.Color333(4, 4, 0));
      matrix.fillCircle(9, 11, 1, matrix.Color333(5, 5, 1));
      matrix.fillCircle(9, 11, 0, matrix.Color333(5, 5, 5));      
    }
    else
    {
      drawUnit(HQ, RED, RIGHT, true, false, 7, 10);      
    }
    if(blueHQDestroyed)
    {
      matrix.fillCircle(22, 11, 3, matrix.Color333(4, 1, 0));
      matrix.fillCircle(22, 11, 2, matrix.Color333(4, 4, 0));
      matrix.fillCircle(22, 11, 1, matrix.Color333(5, 5, 1));
      matrix.fillCircle(22, 11, 0, matrix.Color333(5, 5, 5));            
    }
    else
    {
      drawUnit(HQ, BLUE, LEFT, true, false, 20, 10);            
    }
    
    if(redUnitsDestroyed)
    {
      matrix.fillCircle(9, 27, 3, matrix.Color333(4, 1, 0));
      matrix.fillCircle(9, 27, 2, matrix.Color333(4, 4, 0));
      matrix.fillCircle(9, 27, 1, matrix.Color333(5, 5, 1));
      matrix.fillCircle(9, 27, 0, matrix.Color333(5, 5, 5));            
    }
    else
    {
      drawUnit(TANK, RED, RIGHT, true, false, 7, 26);            
    }
    if(blueUnitsDestroyed)
    {
      matrix.fillCircle(22, 27, 3, matrix.Color333(4, 1, 0));
      matrix.fillCircle(22, 27, 2, matrix.Color333(4, 4, 0));
      matrix.fillCircle(22, 27, 1, matrix.Color333(5, 5, 1));
      matrix.fillCircle(22, 27, 0, matrix.Color333(5, 5, 5));                  
    }
    else
    {
      drawUnit(TANK, BLUE, LEFT, true, false, 20, 26);                  
    }    
  }

  counter++;
  if(counter == 4000){ counter = 0; }
  
  delay(1);    
  }
  while(!buttonFire1Clicked());
}


void kindOfDijkstra(byte currentUnit, byte x, byte y)
{
  byte newDistance;
  setOut(x, y, true);

  if(x > 0)
  {
    if(unit[currentUnit].groundUnit)
    {
      if(getMapSurface(x - 1, y) == WOOD){ newDistance = getRequiredMovementPoints(x, y) + 2; }
      else if(getMapSurface(x - 1, y) == RUMBLE){ newDistance = 127; }
      else{ newDistance = getRequiredMovementPoints(x, y) + 1; }
    }
    else
    {
      newDistance = getRequiredMovementPoints(x, y) + 1;
    }

    if(newDistance > unit[currentUnit].movement){ newDistance = 127; }
        
    if(!isOut(x - 1, y))
    { 
      setRequiredMovementPoints(x - 1, y, newDistance);
      if(newDistance < 127){ kindOfDijkstra(currentUnit, x - 1, y); }  
    }
    else if(isOut(x - 1, y) && newDistance < getRequiredMovementPoints(x - 1, y))
    {
      setOut(x - 1, y, false);
      setRequiredMovementPoints(x - 1, y, newDistance);
      kindOfDijkstra(currentUnit, x - 1, y);      
    }    
  }
  
  if(x < (mapWidth - 1))
  {
    if(unit[currentUnit].groundUnit)
    {
      if(getMapSurface(x + 1, y) == WOOD){ newDistance = getRequiredMovementPoints(x, y) + 2; }
      else if(getMapSurface(x + 1, y) == RUMBLE){ newDistance = 127; }
      else{ newDistance = getRequiredMovementPoints(x, y) + 1; }
    }
    else
    {
      newDistance = getRequiredMovementPoints(x, y) + 1;
    }

    if(newDistance > unit[currentUnit].movement){ newDistance = 127; }
        
    if(!isOut(x + 1, y))
    { 
      setRequiredMovementPoints(x + 1, y, newDistance);
      if(newDistance < 127){ kindOfDijkstra(currentUnit, x + 1, y); }  
    }
    else if(isOut(x + 1, y) && newDistance < getRequiredMovementPoints(x + 1, y))
    {
      setOut(x + 1, y, false);
      setRequiredMovementPoints(x + 1, y, newDistance);
      kindOfDijkstra(currentUnit, x + 1, y);      
    }    
  }
  
  if(y > 0)
  {
    if(unit[currentUnit].groundUnit)
    {
      if(getMapSurface(x, y - 1) == WOOD){ newDistance = getRequiredMovementPoints(x, y) + 2; }
      else if(getMapSurface(x, y - 1) == RUMBLE){ newDistance = 127; }
      else{ newDistance = getRequiredMovementPoints(x, y) + 1; }
    }
    else
    {
      newDistance = getRequiredMovementPoints(x, y) + 1;
    }

    if(newDistance > unit[currentUnit].movement){ newDistance = 127; }
        
    if(!isOut(x, y - 1))
    { 
      setRequiredMovementPoints(x, y - 1, newDistance);
      if(newDistance < 127){ kindOfDijkstra(currentUnit, x, y - 1); }  
    }
    else if(isOut(x, y - 1) && newDistance < getRequiredMovementPoints(x, y - 1))
    {
      setOut(x, y - 1, false);
      setRequiredMovementPoints(x, y - 1, newDistance);
      kindOfDijkstra(currentUnit, x, y - 1);      
    }    
  }
  
  if(y < (mapHeight - 1))
  {
    if(unit[currentUnit].groundUnit)
    {
      if(getMapSurface(x, y + 1) == WOOD){ newDistance = getRequiredMovementPoints(x, y) + 2; }
      else if(getMapSurface(x, y + 1) == RUMBLE){ newDistance = 127; }
      else{ newDistance = getRequiredMovementPoints(x, y) + 1; }
    }
    else
    {
      newDistance = getRequiredMovementPoints(x, y) + 1;
    }

    if(newDistance > unit[currentUnit].movement){ newDistance = 127; }
        
    if(!isOut(x, y + 1))
    { 
      setRequiredMovementPoints(x, y + 1, newDistance);
      if(newDistance < 127){ kindOfDijkstra(currentUnit, x, y + 1); }  
    }
    else if(isOut(x, y + 1) && newDistance < getRequiredMovementPoints(x, y + 1))
    {
      setOut(x, y + 1, false);
      setRequiredMovementPoints(x, y + 1, newDistance);
      kindOfDijkstra(currentUnit, x, y + 1);      
    }    
  }
}


void loop()
{
  title();
  selectPlayers();
  selectMap();
  
  initializeMap(mapNumber);

  byte gameRound = 0;
  boolean roundShown = false;

  cursorX = 0;
  cursorY = 0;
  upperLeftMapCornerX = 0;
  upperLeftMapCornerY = 0;
  cursorXNew = 0;
  cursorYNew = 0;
  upperLeftMapCornerXNew = 0;
  upperLeftMapCornerYNew = 0;
  overlayedUnit = 0;
  unitOverlayed = false;
  selectedUnit = 0;
  unitSelected = false;

  boolean redHQDestroyed = false;
  boolean redUnitsDestroyed = false;
  boolean blueHQDestroyed = false;
  boolean blueUnitsDestroyed = false;

  do
  {
    gameRound++; // First round is round 1

    // Move units  
    showRound(gameRound, MOVE);
    initiativeDiceRoll();
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

    // Set fields moved to 0 (Status settled is set to false in initiativeDiceRoll())
    for(byte i = 0; i < numberOfUnits; i++)
    {
      unit[i].fieldsMoved = 0;
    }

    for(byte i = 0; i < numberOfUnits; i++)
    {
      if(reset){ break; }
      byte currentUnit = getNextUnit();      
      if(currentUnit != 255 && unit[currentUnit].active)
      { 
        overlayMove = false;
        overlayTarget = false;
        overlaySettled = true;
        
        moveMapToUnit(currentUnit, 4);

        // Calculate distance matrix for new unit
        for(byte j = 0; j < (mapWidth * mapHeight); j++)
        {
          distanceMap[j] = 127; // Initialize with "infinite" (yes, it is just 127 but with the same effect <- not reachable in one turn)
          outMap[j] = false;
        }
        setRequiredMovementPoints(unit[currentUnit].xPosition, unit[currentUnit].yPosition, 0);
        kindOfDijkstra(currentUnit, unit[currentUnit].xPosition, unit[currentUnit].yPosition);
        
        makeMove(currentUnit, MOVE);
      }
    }

    // Aim or select units for fixing
    roundShown = false;
    initiativeDiceRoll();

    for(byte i = 0; i < numberOfUnits; i++)
    {
      shooter[i] = 255;
      target[i] = 255;
    }
    nextShot = 0;
    
    for(byte i = 0; i < numberOfUnits; i++)
    {
      if(reset){ break; }
      byte currentUnit = getNextUnit();
      if(currentUnit != 255 && unit[currentUnit].active)
      {
        if(anyTargetInRange(currentUnit) || (unit[currentUnit].hitPoints < unit[currentUnit].armor && unit[currentUnit].fieldsMoved == 0))
        {
          if(!roundShown)
          {
            showRound(gameRound, AIM);
            matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
            roundShown = true;
          }
          overlayMove = false;
          overlayTarget = false;
          overlaySettled = true;

          moveMapToUnit(currentUnit, 4);
          makeMove(currentUnit, AIM);
        }
        else
        {
          unit[currentUnit].settled = true;
        }
      }
    }

    // Fix (a unit will be fixed if it has "aimed to itself")
    nextShot = 0;
    roundShown = false;

    // Check if there is a unit selected for fixing
    while(shooter[nextShot] != 255 && target[nextShot] != 255)
    {
      if(reset){ break; }
      if(!roundShown && unit[shooter[nextShot]].active && shooter[nextShot] == target[nextShot])
      {
        showRound(gameRound, FIX);
        matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
        roundShown = true;
      }
      
      if(unit[shooter[nextShot]].active && shooter[nextShot] == target[nextShot])
      {
        overlayMove = false;
        overlayTarget = false;
        overlaySettled = false;    

        // Highlight unit selected for fixing
        moveMapToUnit(shooter[nextShot], 2);
        matrix.drawRect((cursorX - upperLeftMapCornerX) * 8, (cursorY - upperLeftMapCornerY) * 8, 8, 8, matrix.Color333(0, 0, 2));
        delay(500);       
        matrix.drawRect((cursorX - upperLeftMapCornerX) * 8, (cursorY - upperLeftMapCornerY) * 8, 8, 8, matrix.Color333(0, 0, 0));

        // Fix
        fix(shooter[nextShot]);        
        matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
        delay(500);
      }
      nextShot++;
    };
    
    // Fire
    nextShot = 0;
    roundShown = false;
    while(shooter[nextShot] != 255 && target[nextShot] != 255)
    {
      if(reset){ break; }
      if(!roundShown && unit[shooter[nextShot]].active && unit[target[nextShot]].active && shooter[nextShot] != target[nextShot])
      {
        showRound(gameRound, FIRE);
        matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
        roundShown = true;
      }
          
      if(unit[shooter[nextShot]].active && unit[target[nextShot]].active && shooter[nextShot] != target[nextShot])
      {
        overlayMove = false;
        overlayTarget = false;
        overlaySettled = false;    

        // Highlight shooter
        moveMapToUnit(shooter[nextShot], 2);
        matrix.drawRect((cursorX - upperLeftMapCornerX) * 8, (cursorY - upperLeftMapCornerY) * 8, 8, 8, matrix.Color333(2, 2, 0));
        delay(500);       
        matrix.drawRect((cursorX - upperLeftMapCornerX) * 8, (cursorY - upperLeftMapCornerY) * 8, 8, 8, matrix.Color333(0, 0, 0));

        // Highlight target
        moveMapToUnit(target[nextShot], 2);
        matrix.drawRect((cursorX - upperLeftMapCornerX) * 8, (cursorY - upperLeftMapCornerY) * 8, 8, 8, matrix.Color333(3, 0, 0));
        delay(500);       
        matrix.drawRect((cursorX - upperLeftMapCornerX) * 8, (cursorY - upperLeftMapCornerY) * 8, 8, 8, matrix.Color333(0, 0, 0));

        // Battle
        byte hitValue = calculateHitValue(shooter[nextShot], target[nextShot]);
        boolean targetDestroyed = battle(shooter[nextShot], target[nextShot], hitValue);

        matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
        delay(500);
        
        if(targetDestroyed)
        {
          drawMapComplete(upperLeftMapCornerX, upperLeftMapCornerY, 0, 0, true);
          for(byte i = 0; i < 25; i++)
          {
            drawMapTile(upperLeftMapCornerX, upperLeftMapCornerY, cursorX, cursorY, 0, 0, true, true);
            delay(25 - i);
            drawMapTile(upperLeftMapCornerX, upperLeftMapCornerY, cursorX, cursorY, 0, 0, false, true);
            drawMapTile(upperLeftMapCornerX, upperLeftMapCornerY, cursorX, cursorY, 0, 0, true, false);
            delay(i);            
          }
          unit[target[nextShot]].active = false; // Deactivate unit
        }
      }
      nextShot++;
    };

    // Check whether a player has won (if other HQ or all other units are destroyed)
    byte numberOfActiveRedUnits = 0;  // HQ is not considered
    byte numberOfActiveBlueUnits = 0; // HQ is not considered
    for(byte i = 0; i < numberOfUnits; i++)
    {
      if(unit[i].actor == RED && unit[i].type == HQ && !unit[i].active){ redHQDestroyed = true; }
      if(unit[i].actor == BLUE && unit[i].type == HQ && !unit[i].active){ blueHQDestroyed = true; }
      if(unit[i].actor == RED && unit[i].type != HQ && unit[i].active){ numberOfActiveRedUnits++; }
      if(unit[i].actor == BLUE && unit[i].type != HQ && unit[i].active){ numberOfActiveBlueUnits++; }
    }
    if(numberOfActiveRedUnits == 0){ redUnitsDestroyed = true; }
    if(numberOfActiveBlueUnits == 0){ blueUnitsDestroyed = true; }    
  }
  while(!redHQDestroyed && !redUnitsDestroyed && !blueHQDestroyed && !blueUnitsDestroyed && !reset);

  if(!reset)
  {
    showResults(redHQDestroyed, redUnitsDestroyed, blueHQDestroyed, blueUnitsDestroyed);
  }
}


