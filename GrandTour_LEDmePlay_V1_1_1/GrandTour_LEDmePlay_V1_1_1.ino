// Grand Tour
// for the LEDmePlay
//
// www.mithotronic.de
//
// Version: 1.1.1
// Author: Michael Rosskopf (2017)
// 
// Thanks to Thomas Laubach! 
// 
// Release Notes:
// V1.1.1: Issue to enter 2-player mode fixed (2018)
// V1.1.0: Support for LEDmePlay Joypad and LEDmePlayBoy (2018)
// V1.0.0: First release

// Include libraries for adafruit matrix
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <Timer.h> // Enables timed events
#include <avr/pgmspace.h> // Necessary to maintain data in program memory
#include <math.h>

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

// Theme
const int theme[] = {NOTE_D5, NOTE_F5, NOTE_A5, NOTE_E5,
                    NOTE_C5, NOTE_E5, NOTE_A5, NOTE_E5,
                    NOTE_G5, NOTE_E5, NOTE_C5, NOTE_A5,
                    NOTE_G5, NOTE_F5, NOTE_D5, NOTE_C5,
                    NOTE_D5};

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

// Defining other buttons
int buttonReset = 42;
int buttonPause = 43;

// Game synchronization variable
unsigned long engineLoopStartPoint; // Contains millisconds since start of LEDmePlay at begin of the main loop (used for synchronization). Note: The framerate differs according to speed of the car.

// Initialize Timer object
Timer carsTimer;          // Used to control the other cars cyclically independent of the speed of the player´s car.
Timer remainingTimeTimer; // Used to count the remaining time. If time has expired, the race ends.

// Define middle line of the road (the left and right margin are calculated based on the middleline)
double centerline[32];    // Middle line
double centerlineOld[32]; // Old middle line (used for removal of pixels)

// Variables for the finish line
byte finishLineY;     // Y-position of finish line (if visible)
byte finishLineX1;    // Left X-position of finish line (if visible)
byte finishLineX2;    // Right X-position of finish line (if visible)
byte oldFinishLineY;  // Old Y-position of finish line (if visible)
byte oldFinishLineX1; // Old Left X-position of finish line (if visible)
byte oldFinishLineX2; // Old Right X-position of finish line (if visible)

// Variables for horizon and landscape
byte landscape[128];        // Contains the height of the background mountains which are randomly created
double orientation;         // Start index of the 32 pixel region from the landscape which is visible on screen
byte landscapeOnScreen[32]; // Contains the height of the background mountains which currently on screen
double xHorizon;            // x-Position of the horizon line on the screen (x-position where the road ends. It is 15.5 on a straight segement.)
int yHorizon;               // y-Position of the horizon line on the screen
int yHorizonOld;            // Buffer
int yLandscape;             // y-Position of the background on the screen 

// Define track element constants
#define STRAIGHT 20
#define CURVE_L1 18
#define CURVE_L2 16
#define CURVE_L3 14
#define CURVE_L4 12
#define CURVE_L5 10
#define CURVE_L6 8
#define CURVE_L7 6
#define CURVE_L8 4
#define CURVE_L9 2
#define CURVE_R1 22
#define CURVE_R2 24
#define CURVE_R3 26
#define CURVE_R4 28
#define CURVE_R5 30
#define CURVE_R6 32
#define CURVE_R7 34
#define CURVE_R8 36
#define CURVE_R9 38

// Define race modes
#define RACE 0
#define TIME 1
#define LINK 2

// define difficulty (only relevant for RACE mode)
#define EASY 0
#define AVERAGE 1
#define HARD 2

// Define auto/manual gear shift
#define AUTO 0
#define MANUAL 1

// Define number of players
#define PLAYER_1 0
#define PLAYER_2 1

// Define shift by collision
#define NOSHIFT 0
#define LEFTSHIFT 1
#define RIGHTSHIFT 2

// Define communication symbols for X-link
#define IDLESTATE    255
#define SETPLAYER2   254
#define PLAYER2READY 253
#define EXITLINKMODE 252
#define TERMINATION  251 // Used for data packages send during race

// Define race tracks (in flash memory)
// 1st number: Number of track segments
// 2nd number: Landscape mode (0 = random walk, 1 - 10 = long plateaus, 11- = longer increases/decreases)
// 3rd number: Horizon color
// 4th number: Landscape color
// Colors: 1 == Blue, 2 == Green, 3 == Turquoise, 4 == Red, 5 == Violett, 6 == Yellow, 7 == White
const byte numberOfRaceTracks = 6;
char* raceTrackNames[]={"GRMNY", "FRANC", "SPAIN", "ITALY", "SWITZ", "SWDN"};
const uint8_t raceTracks[] PROGMEM = {
                                             25,  0,  2,  7,
                                       STRAIGHT, 10, 10, 150,
                                       CURVE_L4, 10, 10,  50,
                                       STRAIGHT, 10, 10,  80,
                                       CURVE_R4, 10, 10,  50,
                                       STRAIGHT, 14, 10,  80,
                                       STRAIGHT,  6,  6,  50,
                                       STRAIGHT, 10, 10,  80,
                                       CURVE_L5, 10, 10,  50,
                                       CURVE_R5, 10, 10,  50,
                                       STRAIGHT, 10, 10,  80,
                                       CURVE_R4, 14, 10,  50,
                                       STRAIGHT, 10, 10,  50,
                                       CURVE_L5, 14, 10,  30,
                                       CURVE_R5,  6,  6,  30,
                                       STRAIGHT,  6,  6,  80,
                                       STRAIGHT, 10, 10,  80,
                                       CURVE_L6, 10, 10,  30,
                                       STRAIGHT, 10, 10,  80,
                                       CURVE_R3, 10, 10,  80,
                                       CURVE_L3, 10, 10,  80,
                                       STRAIGHT, 10, 10,  80,
                                       CURVE_L6, 10, 10,  40,
                                       CURVE_R6, 14, 10,  40,
                                       STRAIGHT, 10, 10, 100,
                                       STRAIGHT, 10, 10, 100,

                                             25, 12,  1,  2,
                                       STRAIGHT, 10, 10, 100,
                                       CURVE_L5, 10, 10, 100,
                                       CURVE_R5, 10, 10,  50,
                                       STRAIGHT, 14, 10,  40,
                                       STRAIGHT,  6,  6,  40,
                                       STRAIGHT, 16, 10,  40,
                                       STRAIGHT,  4,  4,  40,
                                       CURVE_L6, 10, 10,  80,
                                       CURVE_R7, 10, 10,  60,
                                       STRAIGHT, 12, 10, 150,
                                       CURVE_R8,  8,  8,  40,
                                       STRAIGHT, 10, 10, 100,
                                       STRAIGHT, 14, 10,  80,
                                       CURVE_L4,  6,  6,  50,
                                       CURVE_R3, 10, 10,  50,
                                       STRAIGHT, 12, 10, 150,
                                       CURVE_L6,  8,  8,  50,
                                       CURVE_R7, 12, 10,  40,
                                       CURVE_L7,  8,  8,  40,
                                       STRAIGHT, 10, 10,  50,
                                       STRAIGHT, 14, 10, 100,
                                       CURVE_L5,  6,  6,  80,
                                       STRAIGHT,  6,  6,  50,
                                       STRAIGHT, 10, 10, 100,
                                       STRAIGHT, 10, 10, 100,

                                             25,  5,  6,  3,
                                       STRAIGHT, 10, 10, 100,
                                       STRAIGHT, 10, 10,  50,
                                       STRAIGHT, 10, 10,  50,
                                       STRAIGHT, 10, 10,  50,
                                       STRAIGHT, 10, 10,  50,
                                       STRAIGHT, 10, 10,  50,
                                       STRAIGHT, 10, 10,  50,
                                       CURVE_L3,  6,  6,  40,
                                       STRAIGHT, 10, 10,  80,
                                       CURVE_L5, 10, 10,  40,
                                       STRAIGHT, 14, 10,  80,
                                       CURVE_R5,  6,  6,  40,
                                       STRAIGHT, 10, 10,  80,
                                       CURVE_L6, 14, 10,  40,
                                       CURVE_R6, 6,   6,  40,
                                       CURVE_R5, 10, 10,  60,
                                       STRAIGHT, 14, 10,  80,
                                       CURVE_R4,  6,  6,  30,
                                       CURVE_R7, 10, 10,  30,
                                       STRAIGHT, 16, 10,  50,
                                       CURVE_L4,  4,  4, 100,
                                       CURVE_R7, 14, 10,  50,
                                       CURVE_L7,  6,  6,  50,
                                       STRAIGHT, 10, 10, 100,
                                       STRAIGHT, 10, 10, 100,

                                             25,  5,  2,  2,
                                       STRAIGHT, 10, 10, 100,
                                       CURVE_L2, 10, 10,  50,
                                       CURVE_R6, 12, 10,  80,
                                       CURVE_R3,  6,  6,  50,
                                       CURVE_L5, 10, 10,  60,
                                       CURVE_L8, 10, 10,  50,
                                       CURVE_R3,  6, 10,  60,
                                       CURVE_L5, 12, 10,  50,
                                       CURVE_L3, 10, 10,  80,
                                       CURVE_R7, 14, 10,  80,
                                       CURVE_L5,  8,  8,  60,
                                       CURVE_R6, 10, 10,  50,
                                       CURVE_R3, 14, 10,  60,
                                       CURVE_L5,  6,  6,  50,
                                       CURVE_R7, 10, 10,  80,
                                       CURVE_L8, 10, 10,  50,
                                       CURVE_L5,  6, 10,  80,
                                       CURVE_R6, 14, 10,  60,
                                       CURVE_R2, 10, 10,  60,
                                       CURVE_R5, 12, 10,  50,
                                       CURVE_L3,  8,  8,  80,
                                       CURVE_R6, 10, 10,  50,
                                       CURVE_L6, 14, 10,  60,
                                       STRAIGHT, 10, 10,  100,
                                       STRAIGHT, 10, 10,  100,

                                             25, 14,  7,  1,
                                       STRAIGHT, 10, 10, 100,
                                       STRAIGHT, 18, 10, 100,
                                       STRAIGHT,  2, 10, 100,
                                       CURVE_R6, 16, 10,  50,
                                       CURVE_L5,  4, 4,   50,
                                       CURVE_R6, 12, 10,  50,
                                       CURVE_L7,  6,  6,  50,
                                       STRAIGHT, 16, 10,  80,
                                       CURVE_R5,  4,  4,  60,
                                       CURVE_R3, 10, 10,  80,
                                       CURVE_L6, 16, 10,  80,
                                       CURVE_R5,  4,  4, 100,
                                       CURVE_L8, 10, 10,  30,
                                       CURVE_L6, 18, 10,  60,
                                       STRAIGHT,  2, 10, 100,
                                       STRAIGHT, 10, 10, 100,
                                       CURVE_L4, 16, 10,  80,
                                       CURVE_R6,  4,  4,  60,
                                       CURVE_L8, 10, 10,  40,
                                       CURVE_R6, 10, 10,  60,
                                       STRAIGHT, 18, 10,  50,
                                       CURVE_R7,  2,  2,  50,
                                       CURVE_L8, 18, 10,  50,
                                       STRAIGHT, 10, 10, 100,
                                       STRAIGHT, 10, 10, 100,

                                             25, 14,  7,  7,
                                       STRAIGHT, 10, 10, 100,
                                       STRAIGHT, 10, 10,  50,
                                       STRAIGHT, 10, 10,  50,
                                       STRAIGHT, 10, 10,  50,
                                       STRAIGHT, 10, 10,  50,
                                       STRAIGHT, 10, 10,  50,
                                       STRAIGHT, 10, 10,  50,
                                       CURVE_L5,  6, 10,  50,
                                       CURVE_R3, 10, 10,  80,
                                       CURVE_L6,  8, 10,  60,
                                       CURVE_R4, 10, 10,  80,
                                       CURVE_L4, 12, 10,  50,
                                       CURVE_R3,  6,  6,  80,
                                       CURVE_R8, 14, 10,  60,
                                       CURVE_L6,  6,  6,  80,
                                       CURVE_L3,  6,  6, 100,
                                       CURVE_L5,  4,  4,  50,
                                       CURVE_R6,  2,  2,  50,
                                       CURVE_L4, 10, 10,  80,
                                       CURVE_L7, 16, 10,  80,
                                       CURVE_R6,  4,  4,  80,
                                       CURVE_R9, 16, 10,  30,
                                       CURVE_L9,  4,  4,  30,
                                       STRAIGHT, 10, 10, 100,
                                       STRAIGHT, 10, 10, 100
};

// Game
byte gameMode;    // RACE, TIME, LINK
byte difficulty;  // EASY, AVERAGE, HARD

// Race track variables (they are updated with the current track loaded from flash memory)
byte raceTrack;            // 1-6
int numberOfTrackSegments; // Number of segements the current race track has
int trackSegmentCurve[256];          // Stores the curve level of each track segment
int trackSegmentHorizonHeight[256];  // Stores the horizon height of each track segment
int trackSegmentHeightLevel[256];    // Stores the background landscape height level of each track segment
int trackSegmentLength[256];         // Stores the segemnt length of each track segment
byte horizonColorR;                 // Helper to store color information
byte horizonColorG;                 // Helper to store color information
byte horizonColorB;                 // Helper to store color information
byte landscapeColorR;               // Helper to store color information
byte landscapeColorG;               // Helper to store color information
byte landscapeColorB;               // Helper to store color information
int landscapeMode; // Landscape mode (0 = random walk, 1 - 10 = long plateaus, 11- = longer increases/decreases)

// Player car
byte player;                                  // Relevant for multiplayer game
double velocity;                              // Speed of the car
const double maximalVelocity = 160;           // Maximal speed
const double maximalVelocityLowGear = 120;    // Maximal speed in low gear
double boost;                                 // Extra speed in LINK mode if player is behind opponent (to challenge heading player)
const double maximalBoost = 8;                // Fixed extra speed in LINK mode
boolean shiftMode;                            // AUTO or MANUAL
boolean gear;                                 // false == Low, true == High
byte gearShiftUsed;                           // Helper which is used to have a delay between using the gear shift two times
double playerCenterlineDistance;              // Distance of player´s car to the centerline of the road
int remainingTime;                            // Remaining time before the race is stopped in time mode
boolean remainingTimeBlinking;                // Helper to enable blinking of the time bar
byte racePosition;                            // Position in the field. To reach the next level, position 1-3 at the end of the race is required.
long startTime;                               // Time in milli seconds at race start
long endTime;                                 // Time in milli seconds at race end
long lapTime;                                 // Lap time (time difference)
int currentTrackSegment;                      // Current segement of race track
int trackSegmentLengthCounter;                // Position within the current segement
int totalTrackLength;                         // Overall length of the complete race track
int trackPositionCounter;                     // Position within the complete race track
boolean finishReached;                        // true if finish line has been reached
boolean winner;                               // true if race has been won (in race mode position 1-3)
byte collisionSoundOn;                        // If > 0, the collision sound is played and all otgher sounds are muted
byte carShiftByCollision;                     // NOSHIFT, LEFTSHIFT, RIGHTSHIFT

// Other cars
byte numberOfOtherCars;             // Number of other cars
boolean otherCarFinishReached[15];  // true if car has reached finish line (used e.g. to check if player has lost after 3 cars are done with the race)
double otherCarPositionOnTrack[15]; // Position within the complete race track 
double otherCarXPosition[15];       // Centerline distance
double otherCarVelocity[15];        // Randomly selected velocity after start of the race (no longer used after car has reached top speed)
double otherCarMaxVelocity[15];     // Maximal possible velocity
byte otherCarType[15];              // (0 - 11) Defines color and shape of the car
byte otherCarStrategy[15];          // 0 = Straight, 1 = Random, 2 = Ideal line, 3/4 = Drunken driver
boolean otherCarVisible[15];        // true if cra visible on screen
int otherCarXPositionOnScreen[15];  // Screen position X-axis
int otherCarYPositionOnScreen[15];  // Screen position Y-axis
byte otherCarSizeOnScreen[15];      // (1-4) Size of the car
byte numberOfCarsInFinish;          // Counts how many cars are in finish
byte carColorR;                     // Helper to store main color R-value
byte carColorG;                     // Helper to store main color G-value
byte carColorB;                     // Helper to store main color B-value

// Link variables
byte readBuffer[8];

// setup: Initialize game
void setup() 
{
  // Initialize serial connections
  Serial.begin(9600);
  Serial1.begin(9600); // used for communications between two LEDmePlay

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

  // Logos
  mithotronic();
  ledMePlay();

  // Start the timer objects
  carsTimer.every(50, controlOtherCarsAndFinishLine);
  remainingTimeTimer.every(1000, decreaseRemainingTime);
  
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

// drawM: Draw the M of the Mithotronic logo
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

// drawT: Draw the T of the Mithotronic logo
void drawT(int x, int y)
{
  matrix.fillRect(x, y + 5, 6, 1, matrix.Color333(0, 0, 0));
  matrix.fillRect(x + 2, y + 10, 3, 1, matrix.Color333(0, 0, 0));

  matrix.fillRect(x, y + 3, 6, 2, matrix.Color333(0, 0, 7));
  matrix.fillRect(x + 2, y, 2, 10, matrix.Color333(0, 0, 7));
  matrix.fillRect(x + 4, y + 8, 1, 2, matrix.Color333(0, 0, 7));
}

// mithotronic: Draw the animated Mithotronic logo and play jingle
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

// setLEDmePlayColor: Sets color for the next character to show the LEDmePLay logo
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

// ledMePlay: Draw the LEDmePlay logo
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
  tone(audio,1024,20);
  delay(200);
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
}

// setColor: Method to set referenced RGB values depending on selected color
void setColor(byte color, byte& R, byte& G, byte& B)
{
  switch(color)
  {
    // Black
    case 0:
    R = 0;
    G = 0;
    B = 0;
    break;

    // Blue
    case 1:
    R = 0;
    G = 0;
    B = 1;
    break;

    // Green
    case 2:
    R = 0;
    G = 1;
    B = 0;
    break;

    // Turquoise
    case 3:
    R = 0;
    G = 1;
    B = 1;
    break;

    // Red
    case 4:
    R = 1;
    G = 0;
    B = 0;
    break;

    // Violett
    case 5:
    R = 1;
    G = 0;
    B = 1;
    break;

    // Yellow
    case 6:
    R = 1;
    G = 1;
    B = 0;
    break;

    // White
    case 7:
    R = 1;
    G = 1;
    B = 1;
    break;
  }
}

// readLastByte: Reads and returns last byte in the read buffer
byte readLastByteSerial1()
{
  byte value;
  while(Serial1.available() > 0)
  {
     value = Serial1.read();
  }
  return value;  
}

// readTillByte: Reads and returns that byte so that i-1 bytes remain in read buffer
byte readTillByteSerial1(byte i)
{
  byte value;
  while(Serial1.available() > i)
  {
     value = Serial1.read();
  }
  return value;  
}

// title: Show title screen
void title()
{
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Send IDLESTATE into the buffer of a potentially connected LEDmePlay
  Serial1.write(IDLESTATE);
  
  // Text
  matrix.setCursor(2, 0);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.println("Grand");
  matrix.setCursor(1, 1);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.println("Grand");
  matrix.setCursor(5, 8);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.println("Tour");
  matrix.setCursor(4, 9);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.println("Tour");

  // Draw horizon
  matrix.drawLine(0, 16, 14, 16, matrix.Color333(0, 1, 0));
  matrix.drawLine(17, 16, 31, 16, matrix.Color333(0, 1, 0));

  // Draw car
  matrix.drawPixel(14, 29, matrix.Color333(0, 0, 1));
  matrix.drawPixel(15, 29, matrix.Color333(0, 0, 3));
  matrix.drawPixel(16, 29, matrix.Color333(0, 0, 3));
  matrix.drawPixel(17, 29, matrix.Color333(0, 0, 1));
  matrix.drawPixel(15, 30, matrix.Color333(3, 3, 0));
  matrix.drawPixel(16, 30, matrix.Color333(3, 3, 0));
  matrix.drawPixel(13, 31, matrix.Color333(3, 3, 0));
  matrix.drawPixel(15, 31, matrix.Color333(2, 2, 0));
  matrix.drawPixel(16, 31, matrix.Color333(2, 2, 0));
  matrix.drawPixel(18, 31, matrix.Color333(3, 3, 0));
  matrix.drawPixel(14, 31, matrix.Color333(1, 1, 0));
  matrix.drawPixel(17, 31, matrix.Color333(1, 1, 0));
  matrix.drawPixel(13, 30, matrix.Color333(1, 0, 0));
  matrix.drawPixel(14, 30, matrix.Color333(1, 0, 0));
  matrix.drawPixel(17, 30, matrix.Color333(1, 0, 0));
  matrix.drawPixel(18, 30, matrix.Color333(1, 0, 0));    
    
  // Wait for fire button to be pressed
  byte counter = 255;
  byte musicCounter = 0;
  do
  {
    // Draw animated road
    for (byte i = 0; i < 16; i++)
    {
      if((i + counter) % 4 > 1)
      {
        matrix.drawPixel(14 - i, 17 + i, matrix.Color333(1, 0, 0));
        matrix.drawPixel(17 + i, 17 + i, matrix.Color333(1, 0, 0));
      }
      else
      {
        matrix.drawPixel(14 - i, 17 + i, matrix.Color333(1, 1, 1));
        matrix.drawPixel(17 + i, 17 + i, matrix.Color333(1, 1, 1));      
      }
    }   
    counter--;

    // Play theme music
    if(musicCounter < 17)
    {
      // SOUND: Play theme
      if(counter % 4 == 2){ tone(audio, theme[musicCounter], 400); }
      if(counter % 4 == 1){ tone(audio, theme[musicCounter] / 4, 400); }
      if(counter % 4 == 0){ tone(audio, theme[musicCounter] / 2, 400); }
      if(counter % 4 == 3)
      { 
        tone(audio, theme[musicCounter] / 4, 200);
        musicCounter++;
      }
    }
    delay(100);
  }
  while(!joy1Fire());
  do
  {
    delay(1);
  }
  while(joy1Fire());
  // SOUND: Start game
  tone(audio, 1024, 20);
  delay(200);
}

// selectMode: Race, Time, Link (2 players with 2 connected LEDmePlays)
boolean selectMode()
{
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Write text
  matrix.setCursor(8, 3);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.println("Race");
  matrix.setCursor(7, 4);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.println("Race");
  matrix.setCursor(8, 12);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.print("Time");    
  matrix.setCursor(7, 13);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.print("Time");    
  matrix.setCursor(8, 21);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.print("Link");    
  matrix.setCursor(7, 22);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.print("Link");    

  matrix.setCursor(1, 4);
  matrix.setTextColor(matrix.Color333(3, 3, 3));
  matrix.print(">");    

  gameMode = RACE;


  // Wait for fire button to be pressed
  int waitingTime = 30000; // Return to title screen if nothing is selected
  do
  {
    delay(1);
    if(joy1Up() && gameMode != RACE)
    {
      gameMode--;
      matrix.setCursor(1, 4 + (gameMode * 9));
      matrix.fillRect(2, 4, 4, 25, matrix.Color333(0, 0, 0));
      matrix.print(">");    
      delay(200);
    }
    if(joy1Down() && gameMode != LINK)
    {
      gameMode++;
      matrix.setCursor(1, 4 + (gameMode * 9));
      matrix.fillRect(2, 4, 4, 25, matrix.Color333(0, 0, 0));
      matrix.print(">");    
      delay(200);
    }
    
    delay(1);
    waitingTime--;
    if(waitingTime == 0){ return false; }
  }
  while(!joy1Fire());
  
  do
  {
    delay(1);
  }
  while(joy1Fire());
  
  // SOUND: Start game
  tone(audio, 1024, 20);
  delay(200);

  return true;
}

// selectDifficulty: Select the difficulty in race mode
boolean selectDifficulty()
{
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Write text
  matrix.setCursor(8, 3);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.println("Easy");
  matrix.setCursor(7, 4);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.println("Easy");
  matrix.setCursor(8, 12);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.print("Avrg");    
  matrix.setCursor(7, 13);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.print("Avrg");    
  matrix.setCursor(8, 21);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.print("Hard");    
  matrix.setCursor(7, 22);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.print("Hard");    

  matrix.setCursor(1, 4);
  matrix.setTextColor(matrix.Color333(3, 3, 3));
  matrix.print(">");    

  difficulty = EASY;

  // Wait for fire button to be pressed
  int waitingTime = 30000; // Return to title screen if nothing is selected
  do
  {
    if(joy1Up() && difficulty != EASY)
    {
      difficulty--;
      matrix.setCursor(1, 4 + (difficulty * 9));
      matrix.fillRect(2, 4, 4, 25, matrix.Color333(0, 0, 0));
      matrix.print(">");    
      delay(200);
    }
    if(joy1Down() && difficulty != HARD)
    {
      difficulty++;
      matrix.setCursor(1, 4 + (difficulty * 9));
      matrix.fillRect(2, 4, 4, 25, matrix.Color333(0, 0, 0));
      matrix.print(">");    
      delay(200);
    }

    delay(1);
    waitingTime--;
    if(waitingTime == 0){ return false; }
  }
  while(!joy1Fire());
  
  do
  {
    delay(1);
  }
  while(joy1Fire());

  // SOUND: Start game
  tone(audio, 1024, 20);
  delay(200);
  
  return true;   
}

// selectShiftMode: Select automatic or manual shifting
boolean selectShiftMode()
{
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Write text
  matrix.setCursor(2, 3);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.println("Shift");
  matrix.setCursor(1, 4);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.println("Shift");
  matrix.setCursor(8, 12);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.print("Auto");    
  matrix.setCursor(7, 13);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.print("Auto");    
  matrix.setCursor(8, 21);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.print("Manu");    
  matrix.setCursor(7, 22);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.print("Manu");    

  matrix.setCursor(1, 13);
  matrix.setTextColor(matrix.Color333(3, 3, 3));
  matrix.print(">");    

  shiftMode = AUTO;

  // Wait for fire button to be pressed
  int waitingTime = 30000; // Return to title screen if nothing is selected
  do
  {
    delay(1);
    if(joy1Up() && shiftMode != AUTO)
    {
      shiftMode = AUTO;
      matrix.setCursor(1, 13);
      matrix.fillRect(2, 13, 4, 16, matrix.Color333(0, 0, 0));
      matrix.print(">");    
      delay(200);
    }
    if(joy1Down() && shiftMode != MANUAL)
    {
      shiftMode = MANUAL;
      matrix.setCursor(1, 22);
      matrix.fillRect(2, 13, 4, 16, matrix.Color333(0, 0, 0));
      matrix.print(">");    
      delay(200);
    }

    delay(1);
    waitingTime--;
    if(waitingTime == 0){ return false; }  
  }
  while(!joy1Fire());
  
  do
  {
    delay(1);
  }
  while(joy1Fire());

  // SOUND: Start game
  tone(audio, 1024, 20);
  delay(200);

  return true;
}

// selectRaceTrackInLinkMode: Select a race track for 2 player link game
boolean selectRaceTrackInLinkMode()
{
  // Check for link mode if player is player 1 (who can select the track)
  byte inputValue;
  inputValue = readLastByteSerial1();

  // Player is player 2
  if(inputValue == SETPLAYER2)
  { 
    player = PLAYER_2;
    for(byte i = 0; i < 64; i++)
    {
      Serial1.write(PLAYER2READY); // Send: Player 2 is ready
    }
  }
  // Player is player 1
  else
  { 
    player = PLAYER_1;
  }
 
  if(player == PLAYER_1)
  {
    // Waiting for player 2
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
  
    // Write text
    matrix.setCursor(2, 3);
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.println("Plyr1");
    matrix.setCursor(1, 4);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.println("Plyr1");
    matrix.setCursor(2, 12);
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.print("Wait!");    
    matrix.setCursor(1, 13);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.print("Wait!");    
    matrix.setCursor(8, 21);
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.print("Exit");    
    matrix.setCursor(7, 22);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.print("Exit");    
    matrix.setCursor(1, 22);
    matrix.setTextColor(matrix.Color333(3, 3, 3));
    matrix.print(">");    

    byte player2Ready = false;
    byte counter = 0;
    do
    {
      matrix.setCursor(2, 12);
      matrix.setTextColor(matrix.Color888(counter / 8, counter / 8, counter / 8));
      matrix.print("Wait!");    
      matrix.setCursor(1, 13);
      matrix.setTextColor(matrix.Color888(counter, 0, 0));
      matrix.print("Wait!");
      counter++;
      Serial1.write(SETPLAYER2); // Send: Other player is player 2 and has to wait
      delay(1);
      byte inputValue = readLastByteSerial1();
      if(inputValue == EXITLINKMODE){ return false; } // Other player has exit link mode
      if(inputValue == PLAYER2READY){ player2Ready = true; }
    }
    while(!joy1Fire() && !player2Ready);
    do
    {
      delay(1);
    }
    while(joy1Fire() && !player2Ready);

    if(!player2Ready)
    {
      Serial1.write(EXITLINKMODE);
      tone(audio, 1024, 20);
      delay(200);
      return false;
    }
    else
    {
      if(selectRaceTrack())
      {
        Serial1.write(251-raceTrack); // Send selected race track to player 2 (the track number is encoded in a way to avoid a mix up with other sent information)
      }
      else
      {
        Serial1.write(EXITLINKMODE);
        return false;
      }
    }
  }
  else if(player == PLAYER_2)
  {
    // Waiting that player 1 selects a track
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
  
    // Write text
    matrix.setCursor(2, 3);
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.println("Plyr2");
    matrix.setCursor(1, 4);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.println("Plyr2");
    matrix.setCursor(2, 12);
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.print("Wait!");    
    matrix.setCursor(1, 13);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.print("Wait!");    
    matrix.setCursor(8, 21);
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.print("Exit");    
    matrix.setCursor(7, 22);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.print("Exit");    
    matrix.setCursor(1, 22);
    matrix.setTextColor(matrix.Color333(3, 3, 3));
    matrix.print(">"); 
       
    byte player1Ready = false;
    byte counter = 0;
    do
    {
      matrix.setCursor(2, 12);
      matrix.setTextColor(matrix.Color888(counter / 8, counter / 8, counter / 8));
      matrix.print("Wait!");    
      matrix.setCursor(1, 13);
      matrix.setTextColor(matrix.Color888(counter, 0, 0));
      matrix.print("Wait!");
      counter++;
      delay(1);
      byte inputValue = readLastByteSerial1();
      if(inputValue == EXITLINKMODE){ return false; } // Other player has exit link mode
      if(inputValue <= 250 && inputValue > 250 - numberOfRaceTracks)
      { 
        player1Ready = true;
        raceTrack = 251 - inputValue; // Encodes race track
      }
    }
    while(!joy1Fire() && !player1Ready);
    do
    {
      delay(1);
    }
    while(joy1Fire() && !player1Ready);

    if(!player1Ready)
    {
      Serial1.write(EXITLINKMODE);
      tone(audio, 1024, 20);
      delay(200);
      return false;
    }
  }
  return true;
}
  
// selectRaceTrack: Select a race track (used in mode TIME)
boolean selectRaceTrack()
{  
  // Clear screen
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // Write text
  matrix.setCursor(2, 3);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.println("Track");
  matrix.setCursor(1, 4);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.println("Track");
  
  matrix.setCursor(7, 12);
  matrix.setTextColor(matrix.Color333(3, 3, 3));
  matrix.println("< >");
  delay(200);

  // Select level by moving joystick left and right
  raceTrack = 1;
  boolean updateRaceTrackName = true;
  int waitingTime = waitingTime; // Return to title screen if nothing is selected
  do
  {
    if(updateRaceTrackName)
    {
      matrix.setCursor(2, 21);
      matrix.setTextColor(matrix.Color333(1, 1, 1));
      matrix.println(raceTrackNames[raceTrack - 1]);
      matrix.setCursor(1, 22);
      matrix.setTextColor(matrix.Color333(3, 0, 0));
      matrix.println(raceTrackNames[raceTrack - 1]);
      updateRaceTrackName = false;
    }
    
    if(joy1Left())
    {
      raceTrack--;
      if(raceTrack < 1)
      {
        raceTrack = 1;
      }
      else
      {
        tone(audio,1024,20);
        matrix.fillRect(0, 21, 32, 8, matrix.Color333(0, 0, 0));
        updateRaceTrackName = true;
      }
      delay(200);
    }
    
    if(joy1Right())
    {
      raceTrack++;
      if(raceTrack > numberOfRaceTracks)
      {
        raceTrack = numberOfRaceTracks;
      }
      else
      {
        tone(audio,1024,20);
        matrix.fillRect(0, 21, 32, 8, matrix.Color333(0, 0, 0));
        updateRaceTrackName = true;
      }
      delay(200);
    }

    delay(1);
    waitingTime--;
    if(waitingTime == 0){ return false; }  
  }
  while(!joy1Fire());     

  do
  {
    delay(1);
  }
  while(joy1Fire());

  // SOUND: Start game
  tone(audio, 1024, 20);
  delay(200);

  return true;
}

// showNextTrack: Shows next track in race mode
void showNextTrack()
{
  // Show next track
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setCursor(5, 0);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.println("Next");
  matrix.setCursor(4, 1);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.println("Next");
  matrix.setCursor(5, 8);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.println("Race");
  matrix.setCursor(4, 9);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.println("Race");
  matrix.setCursor(2, 20);
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.println(raceTrackNames[raceTrack - 1]);
  matrix.setCursor(1, 21);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.println(raceTrackNames[raceTrack - 1]);

  delay(200);

  int waitingTime = 30000; // Games is proceeded after
  do
  {
    delay(1);
    waitingTime--;
  }
  while(!joy1Fire() && waitingTime > 0);
  do
  {
    delay(1);
  }
  while(joy1Fire() && waitingTime > 0);
  // SOUND: Select
  tone(audio, 1024, 20);
  delay(200);
}

// calculateCenterline: xHorizon is the screen position where the street ends, yHorizon is the height of the horizon. Based on this, the centerline of the visible part of the street is calculated via a curve function.
void calculateCenterline(int xHorizon, int yHorizon)
{
  int16_t r;
  double x0 = xHorizon;
  int16_t y0 = 31;
  double rHor = abs(15.5 - xHorizon);
  double rVer = 31 - yHorizon;

  for(byte i = 0; i < 31; i++)
  {
    centerline[i] = 1000.0;
  }
  centerline[31] = 15.5;
  
  if(rHor > rVer){
    r = rHor;
  }
  else
  {
    r = rVer;
  }
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while(x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    int test;

    if(rHor > rVer)
    {
      if(xHorizon < 15.5)
      {
        centerline[int(y0 - (double(y) * rVer/rHor))] = x0 + x;
        centerline[int(y0 - (double(x) * rVer/rHor))] = x0 + y;

        test = int(y0 - (double(y) * rVer/rHor));
        test = int(y0 - (double(x) * rVer/rHor));
      }
      else
      {
        centerline[int(y0 - (double(y) * rVer/rHor))] = x0 - x;
        centerline[int(y0 - (double(x) * rVer/rHor))] = x0 - y;

        test = int(y0 - (double(y) * rVer/rHor));
        test = int(y0 - (double(x) * rVer/rHor));
      }
    }
    else
    {
      if(xHorizon < 15.5)
      {
        centerline[int(y0 - y)] = x0 + (double(x) * rHor/rVer);
        centerline[int(y0 - x)] = x0 + (double(y) * rHor/rVer);

        test = int(y0 - y);
        test = int(y0 - x);
     }
      else
      {
        centerline[int(y0 - y)] = x0 - (double(x) * rHor/rVer);
        centerline[int(y0 - x)] = x0 - (double(y) * rHor/rVer);

        test = int(y0 - y);
        test = int(y0 - x);
      }
    }
  }
}

// modifyCenterline: Modifies the centerline with respect to the distance of player´s car.
void modifyCenterline(double _yHorizon, double _playerCenterlineDistance)
{
  for(byte i = 0; i < 32; i++)
  {
    if(i > _yHorizon)
    {
      centerline[i] = centerline[i] + (_playerCenterlineDistance * ((double(i) - _yHorizon) / (31.0 - _yHorizon)));
    }
  }  
}

// copyCenterline: Helper method to store current screen position of the centerline
void copyCenterline()
{
  for(byte i = 0; i < 32; i++)
  {
    centerlineOld[i] = centerline[i];
  }
}

// drawRoad: Based on the previously calculated centerline, the road is drawn on the screen.
void drawRoad(int yHorizon, byte counter)
{
  for(byte i = 0; i < 32; i++)
  {
    if(i == yHorizon)
    {
      // Draw horizon
      matrix.drawLine(0, yHorizon, centerline[i] - (double(i - yHorizon)*(double(16.0/(32.0 - double(yHorizon))))) - 1, yHorizon, matrix.Color333(horizonColorR, horizonColorG, horizonColorB));
      matrix.drawLine(centerline[i] + 1.0 + (double(i - yHorizon)*(double(16.0/(32.0 - double(yHorizon))))) + 1, yHorizon, 31, yHorizon, matrix.Color333(horizonColorR, horizonColorG, horizonColorB));
    }
    else if(i > yHorizon)
    {
      // Draw road
      if((i + counter) % 4 < 2)
      {
        matrix.drawPixel(centerline[i] - (double(i - yHorizon)*(double(16.0/(32.0 - double(yHorizon))))), i, matrix.Color333(1, 0, 0));
        matrix.drawPixel(centerline[i] + 1.0 + (double(i - yHorizon)*(double(16.0/(32.0 - double(yHorizon))))), i, matrix.Color333(1, 0, 0));        
      }
      else
      {
        matrix.drawPixel(centerline[i] - (double(i - yHorizon)*(double(16.0/(32.0 - double(yHorizon))))), i, matrix.Color333(1, 1, 1));
        matrix.drawPixel(centerline[i] + 1.0 + (double(i - yHorizon)*(double(16.0/(32.0 - double(yHorizon))))), i, matrix.Color333(1, 1, 1));
      }
    }
  }
}

// removesRoad: Removes current visualization of the road (using the previously copied centerline).
void removeRoad(int yHorizonOld, boolean removeHorizonLine)
{
  if(removeHorizonLine)
  {
    matrix.drawLine(0, yHorizonOld, 31, yHorizonOld, matrix.Color333(0, 0, 0));  
  }
  for(byte i = 0; i < 32; i++)
  {
    if(centerlineOld[i] < 1000.0)
    {
      // Draw
      matrix.drawPixel(centerlineOld[i] - (double(i - yHorizonOld)*(double(16.0/(32.0 - double(yHorizonOld))))), i, matrix.Color333(0, 0, 0));
      matrix.drawPixel(centerlineOld[i] + 1.0 + (double(i - yHorizonOld)*(double(16.0/(32.0 - double(yHorizonOld))))), i, matrix.Color333(0, 0, 0));
    }
  }
}

// initLandscape: Generates a random mountain background (depending on landscape mode)
void initLandscape()
{
  int height;
  landscape[0] = 15;
  int j = landscapeMode; // Helper
  boolean mountainUp;    // Helper

  if(gameMode != LINK || (gameMode == LINK && player == PLAYER_1))
  {
    for(byte i = 1; i < 128; i++)
    {
    // Mode: Random walk
    if(landscapeMode == 0)
      {
        height = landscape[i - 1] + random(3) - 1;
        if(height < 0){ landscape[i] = 0; }
        else if(height > 15){ landscape[i] = 15; }
        else { landscape[i] = height; }
      }
      // Mode: Long plateaus
      else if(landscapeMode > 0 && landscapeMode < 10)
      {
        if(j == landscapeMode)
        {
          height = landscape[i - 1] + random(3) - 1;
          if(height < 0){ landscape[i] = 0; }
          else if(height > 15){ landscape[i] = 15; }
          else { landscape[i] = height; }
          j--;
        }
        else
        {
          height = landscape[i - 1];
          if(height < 0){ landscape[i] = 0; }
          else if(height > 15){ landscape[i] = 15; }
          else { landscape[i] = height; }
          j--;
          if(j == 0){ j = landscapeMode; }           
        }
      }
      // Mode: Longer increases/decreases
      else if(landscapeMode > 10)
      {
        if(j == landscapeMode)
        {
          height = landscape[i - 1] + random(3) - 1;
          if(height < 0){ landscape[i] = 0; }
          else if(height > 15){ landscape[i] = 15; }
          else { landscape[i] = height; }
          if(random(2) == 0){ mountainUp = true; }
          else{ mountainUp = false; }
          j = j - 10;
        }
        else
        {
          if(mountainUp){
            height = landscape[i - 1] + 1;
          }
          else
          {
            height = landscape[i - 1] - 1;
          }         
          if(height < 0){ landscape[i] = 0; }
          else if(height > 15){ landscape[i] = 15; }
          else { landscape[i] = height; }
          j--;
          if(j == 0){ j = landscapeMode; }
        }
      }
    }

    // Make sure that the landscape is seamless if scrolling across the boarder from one end to the other
    if(landscape[127] > landscape[0])
    {
      byte i = 127;
      do
      {
        landscape[i] = landscape[0] + (128 - i);
        i--;
      }
      while(landscape[i] > landscape[i + 1]);
    }
    else if(landscape[127] < landscape[0])
    {
      byte i = 127;
      do
      {
        landscape[i] = landscape[0] - (128 - i);
        i--;
      }
      while(landscape[i] < landscape[i + 1]);
    }

    // LINK mode: The background is copied from player 1 to player 2
    // Send landscape in link mode to other player
    if(gameMode == LINK)
    {
      for(byte i = 0; i < 128; i++)
      {
        Serial1.write(landscape[i]);
        Serial1.write('.');
        delay(5);
      }
    }
  }
  else if(gameMode == LINK && player == PLAYER_2)
  {
    // Receive landscape
    byte i = 0;
    do
    {
      if(Serial1.available() > 1)
      {
        Serial1.readBytesUntil('.', readBuffer, 2);
        landscape[i] = readBuffer[0];
        i++;
      }
    }
    while(i < 128);    
  }
}

// drawLandscape: Draws landscape depending on orientation and height
void drawLandscape(double yHorizon, double yLandscape)
{
  byte index;
  byte height;

  // Remove old horizon
  for(byte i = 0; i < 32; i++)
  {
    matrix.drawPixel(i, landscapeOnScreen[i], matrix.Color333(0, 0, 0));
    // Remove ocean if visible
    if((23 - landscapeOnScreen[i]) > 16 && yHorizon > 16)
    {
      matrix.drawPixel(i, 16, matrix.Color333(0, 0, 0));         
    }
  }
    
  // Draw new horizon
  for(byte i = 0; i < 32; i++)
  {
    index = (int(orientation + 0.5) + i) % 128;
    // Draw ocean if visible
    if((23 - landscape[index]) > yLandscape && yHorizon > yLandscape)
    {
      matrix.drawPixel(i, yLandscape, matrix.Color333(0, 0, 1));         
    }
    // Draw mountains
    if((23 - landscape[index] - (16 - yLandscape)) < yHorizon)
    {
      height = 23 - landscape[index] - (16 - yLandscape);
      matrix.drawPixel(i, height, matrix.Color333(landscapeColorR, landscapeColorG, landscapeColorB)); 
      landscapeOnScreen[i] = height;       
    }
  }
}

// drawPlayerCar: Draws player car
void drawPlayerCar()
{  
  if(gameMode == LINK && player == PLAYER_2)
  {
    carColorR = 0;
    carColorG = 1;
    carColorB = 0;
  }
  else
  {
    carColorR = 1;
    carColorG = 1;
    carColorB = 0;
  }
  matrix.drawPixel(14, 29, matrix.Color333(0, 0, 1));
  matrix.drawPixel(15, 29, matrix.Color333(0, 0, 3));
  matrix.drawPixel(16, 29, matrix.Color333(0, 0, 3));
  matrix.drawPixel(17, 29, matrix.Color333(0, 0, 1));
  matrix.drawPixel(15, 30, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));
  matrix.drawPixel(16, 30, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));
  matrix.drawPixel(13, 31, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));
  matrix.drawPixel(15, 31, matrix.Color333(carColorR * 2, carColorG * 2, carColorB * 2));
  matrix.drawPixel(16, 31, matrix.Color333(carColorR * 2, carColorG * 2, carColorB * 2));
  matrix.drawPixel(18, 31, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));
  matrix.drawPixel(14, 31, matrix.Color333(carColorR, carColorG, carColorB));
  matrix.drawPixel(17, 31, matrix.Color333(carColorR, carColorG, carColorB));
  if((!joy1IsLEDmePlayJoypad() && joy1Down()) || (joy1IsLEDmePlayJoypad() && (joy1YValue() < 0.0)))
  {
    matrix.drawPixel(13, 30, matrix.Color333(7, 0, 0));
    matrix.drawPixel(14, 30, matrix.Color333(7, 0, 0));
    matrix.drawPixel(17, 30, matrix.Color333(7, 0, 0));
    matrix.drawPixel(18, 30, matrix.Color333(7, 0, 0));        
  }
  else
  {
    matrix.drawPixel(13, 30, matrix.Color333(1, 0, 0));
    matrix.drawPixel(14, 30, matrix.Color333(1, 0, 0));
    matrix.drawPixel(17, 30, matrix.Color333(1, 0, 0));
    matrix.drawPixel(18, 30, matrix.Color333(1, 0, 0));    
  }
  // Show a bit the side of the car if player stears
  if(joy1Left() && velocity > 0)
  {
    matrix.drawPixel(13, 29, matrix.Color333(0, 0, 3));  
    matrix.drawPixel(12, 30, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));  
    matrix.drawPixel(12, 31, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));  
  }
  else
  {
    matrix.drawPixel(13, 29, matrix.Color333(0, 0, 0));  
    matrix.drawPixel(12, 30, matrix.Color333(0, 0, 0));  
    matrix.drawPixel(12, 31, matrix.Color333(0, 0, 0));      
  }
  if(joy1Right() && velocity > 0)
  {
    matrix.drawPixel(18, 29, matrix.Color333(0, 0, 3));  
    matrix.drawPixel(19, 30, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));  
    matrix.drawPixel(19, 31, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));  
  }
  else
  {
    matrix.drawPixel(18, 29, matrix.Color333(0, 0, 0));  
    matrix.drawPixel(19, 30, matrix.Color333(0, 0, 0));  
    matrix.drawPixel(19, 31, matrix.Color333(0, 0, 0));          
  }
}

// updateInstruments: Redraws cockpit instruments
void updateInstruments()
{
  // Show gear (Low or High)
  if(gearShiftUsed == 8)
  {
    if(shiftMode == AUTO)
    {
      // Automatic
      matrix.drawLine(2, 2, 2, 5, matrix.Color333(1, 1, 1));
      matrix.drawLine(5, 2, 5, 5, matrix.Color333(1, 1, 1));    
      matrix.drawLine(3, 1, 4, 1, matrix.Color333(1, 1, 1));
      matrix.drawLine(3, 3, 4, 3, matrix.Color333(1, 1, 1));     
    }
    else
    {
      // Manual shift
      matrix.drawLine(2, 1, 2, 5, matrix.Color333(0, 0, 0));
      matrix.drawLine(5, 1, 5, 5, matrix.Color333(0, 0, 0));
      matrix.drawLine(3, 3, 4, 3, matrix.Color333(0, 0, 0));
      matrix.drawLine(3, 5, 4, 5, matrix.Color333(0, 0, 0));
      if(!gear)
      {
        matrix.drawLine(2, 1, 2, 5, matrix.Color333(1, 1, 1));
        matrix.drawLine(3, 5, 5, 5, matrix.Color333(1, 1, 1));
      }
      else
      {
        matrix.drawLine(2, 1, 2, 5, matrix.Color333(1, 1, 1));
        matrix.drawLine(5, 1, 5, 5, matrix.Color333(1, 1, 1));    
        matrix.drawLine(3, 3, 4, 3, matrix.Color333(1, 1, 1));
      }
    }
  }

  
  // Show velocity
  if(velocity == 0)
  { 
    matrix.drawLine(8, 1, 23, 1, matrix.Color333(1, 1, 1));
  }
  else
  {
    byte velocityRounded = velocity;
    if(velocityRounded > 159){ velocityRounded = 159; }
    matrix.drawLine(8 + (velocityRounded / 10), 1, 23, 1, matrix.Color333(1, 1, 1));    
    matrix.drawLine(8, 1, 8 + (velocityRounded / 10), 1, matrix.Color333(1, 0, 0));
  }

  // Show position on race track
  matrix.drawLine(8, 3, 23, 3, matrix.Color333(0, 0, 1));
  byte value;
  if(gameMode == RACE)
  {
    value = (double(otherCarPositionOnTrack[14]) / double(totalTrackLength - trackSegmentLength[numberOfTrackSegments - 1])) * 16;
    if(value < 16){ matrix.drawPixel(8 + value, 3, matrix.Color333(1, 0, 0)); }
  }
  value = (double(trackPositionCounter) / double(totalTrackLength - trackSegmentLength[numberOfTrackSegments - 1])) * 16;
  if(value < 16){ matrix.drawPixel(8 + value, 3, matrix.Color333(1, 1, 0)); }

  if(gameMode == RACE)
  {
    // Show position in the field
    matrix.drawLine(8, 5, 23, 5, matrix.Color333(0, 0, 1));
    matrix.drawPixel(24 - racePosition, 5, matrix.Color333(3, 3, 0));
  }
  
  // gameMode == TIME: 
  // Show remaining time: This is done in the method decreaseRemainingTime() (called by a timer)

  else if(gameMode == LINK)
  {
    // Show position of other player
    matrix.drawLine(8, 5, 23, 5, matrix.Color333(0, 0, 1));
    if((8 + (double(otherCarPositionOnTrack[0]) / double(totalTrackLength)) * 16) <= 23)
    {
      matrix.drawPixel(8 + (double(otherCarPositionOnTrack[0]) / double(totalTrackLength - trackSegmentLength[numberOfTrackSegments - 1])) * 16, 5, matrix.Color333(1, 0, 0));
    }
  }

  // Show next curve
  if(trackSegmentLengthCounter < 30)
  {
    byte trackSegment = currentTrackSegment + 1;
    boolean sharpCurve = false; // Highlight arrow in red if curve is sharp
    if(trackSegment == numberOfTrackSegments){ trackSegment = 0; }
    if(trackSegmentCurve[trackSegment] > 0)
    {
      // Show arrow to left hand side
      if(trackSegmentCurve[trackSegment] >= 14){ sharpCurve = true; }
      matrix.drawLine(26, 3, 29, 3, matrix.Color333(1, 1 - sharpCurve, 1 - sharpCurve));
      matrix.drawPixel(28, 1, matrix.Color333(1, 1 - sharpCurve, 1 - sharpCurve));
      matrix.drawPixel(27, 2, matrix.Color333(1, 1 - sharpCurve, 1 - sharpCurve));
      matrix.drawPixel(27, 4, matrix.Color333(1, 1 - sharpCurve, 1 - sharpCurve));
      matrix.drawPixel(28, 5, matrix.Color333(1, 1 - sharpCurve, 1 - sharpCurve)); 
    }
    else if(trackSegmentCurve[trackSegment] < 0)
    {
      // Show arrow to right hand side
      if(trackSegmentCurve[trackSegment] <= -14){ sharpCurve = true; }
      matrix.drawLine(26, 3, 29, 3, matrix.Color333(1, 1 - sharpCurve, 1 - sharpCurve));
      matrix.drawPixel(27, 1, matrix.Color333(1, 1 - sharpCurve, 1 - sharpCurve));
      matrix.drawPixel(28, 2, matrix.Color333(1, 1 - sharpCurve, 1 - sharpCurve));
      matrix.drawPixel(28, 4, matrix.Color333(1, 1 - sharpCurve, 1 - sharpCurve));
      matrix.drawPixel(27, 5, matrix.Color333(1, 1 - sharpCurve, 1 - sharpCurve));      
    }
    else
    {
      matrix.fillRect(26, 1, 4, 5, matrix.Color333(0, 0, 0));        
    }
  }
  else
  {
    matrix.fillRect(26, 1, 4, 5, matrix.Color333(0, 0, 0));  
  }
}

// drawFinishLine: Draws the finish line (if visible)
void drawFinishLine()
{
    // Finish line (must be drawn first to avoid that cars are overlaid)
  if(oldFinishLineY != 255)
  {
    // Remove finish line at old position
    matrix.drawLine(oldFinishLineX1, oldFinishLineY, oldFinishLineX2, oldFinishLineY, matrix.Color333(0, 0, 0));      
  }
  int finishDistance = (totalTrackLength - trackSegmentLength[numberOfTrackSegments - 1]) - trackPositionCounter;
  if(finishDistance >= 0 && finishDistance < 32)
  {
    finishDistance = finishDistance / (2.0 * ((finishDistance / 64.0) + 0.5));
  }
  if(finishDistance >= 0 && finishDistance < 16)
  {      
    // Calculate new position of finish line
    finishLineY = 31 - ((double(finishDistance) / 16.0) * double(31 - yHorizon));
    if(finishLineY >= 0 && finishLineY <= 31)
    {
      finishLineX1 = centerline[finishLineY] - (double(finishLineY - yHorizon)*(double(16.0/(32.0 - double(yHorizon))))) + 1;
      finishLineX2 = centerline[finishLineY] + 1.0 + (double(finishLineY - yHorizon)*(double(16.0/(32.0 - double(yHorizon))))) - 1;        
      matrix.drawLine(finishLineX1, finishLineY, finishLineX2, finishLineY, matrix.Color333(1, 1, 1));
    }
    oldFinishLineY = finishLineY;
    oldFinishLineX1 = finishLineX1;
    oldFinishLineX2 = finishLineX2;
  }
  else
  {
    oldFinishLineY = 255;
  }
}


// controlOtherCarsAndFinishLine: 
// Called by carsTimer to control the position of other cars and draw them on-screen.
// Additionally, the method triggers the visualization of the finish line and play collision sounds
void controlOtherCarsAndFinishLine()
{
  // Play collision sound (in case there was a collision)
  if(collisionSoundOn > 0){
    // SOUND: Collision
    tone(audio, 50 + random(350), 20);     
  }

  // Draw finish line (if visible)
  drawFinishLine();
  
  // Race position set to zero for recalculation (as long as finish has not been reached)
  if(!finishReached)
  {
    if(gameMode == RACE || gameMode == TIME){ racePosition = 16; }
    else if(gameMode == LINK){ racePosition = 2; }
  }
  
  // Iterate over all other cars
  for(byte i = 0; i < numberOfOtherCars; i++)
  {
    // Remove other car from old screen positions
    if(otherCarVisible[i])
    {
      removeOtherCar(i);
    }

    // Calculate other car movement
    if(gameMode == RACE)
    {
      // Update position
      otherCarPositionOnTrack[i] = otherCarPositionOnTrack[i] + otherCarVelocity[i];

      // Car reaches finish
      if(otherCarPositionOnTrack[i] > totalTrackLength - trackSegmentLength[numberOfTrackSegments - 1] && !otherCarFinishReached[i])
      { 
        otherCarFinishReached[i] = true;
        numberOfCarsInFinish++; // If player has no chance to finish on position 1-3, the race ends (checked by numberOfCarsInFinish variable)
      }
      // After other car has reached end of track it is moved to the start of the track and its speed is set to zero (avoids that finished cars block the road).
      if(otherCarPositionOnTrack[i] > totalTrackLength && otherCarFinishReached[i])
      {
        otherCarPositionOnTrack[i] = 0;
        otherCarVelocity[i] = 0.0;
      }
      
      // Steering
      // Car makes random direction changes
      if(otherCarStrategy[i] == 1)
      {
        if(difficulty == EASY)
        {
          if(random(8) == 0 && otherCarXPosition[i] > -13){ otherCarXPosition[i]--; }
          if(random(8) == 0 && otherCarXPosition[i] < 13){ otherCarXPosition[i]++; }
        }
        else if(difficulty == AVERAGE)
        {
          if(random(5) == 0 && otherCarXPosition[i] > -13){ otherCarXPosition[i]--; }
          if(random(5) == 0 && otherCarXPosition[i] < 13){ otherCarXPosition[i]++; }
        }
        else if(difficulty == HARD)
        {
          if(random(3) == 0 && otherCarXPosition[i] > -13){ otherCarXPosition[i]--; }
          if(random(3) == 0 && otherCarXPosition[i] < 13){ otherCarXPosition[i]++; }
        }
      }
      // Car tends to ideal line
      if(otherCarStrategy[i] == 2)
      {
        if(xHorizon < 15.5 && otherCarXPosition[i] > -13 && trackPositionCounter < (otherCarPositionOnTrack[i] - 3)){ otherCarXPosition[i] = otherCarXPosition[i] - 0.2; }
        if(xHorizon > 15.5 && otherCarXPosition[i] < 13 && trackPositionCounter < (otherCarPositionOnTrack[i] - 3)){ otherCarXPosition[i] = otherCarXPosition[i] + 0.2; }
      }
      // Car driven by drunken driver
      if(otherCarStrategy[i] == 3 && otherCarXPosition[i] > -13 && trackPositionCounter < (otherCarPositionOnTrack[i] - 3))
      { 
        if(difficulty == EASY){ otherCarXPosition[i] = otherCarXPosition[i] - 0.2; }
        else if(difficulty == AVERAGE){ otherCarXPosition[i] = otherCarXPosition[i] - 0.4; }
        else if(difficulty == HARD){ otherCarXPosition[i] = otherCarXPosition[i] - 0.6; }
      }
      if(otherCarStrategy[i] == 3 && otherCarXPosition[i] <= -13 && trackPositionCounter < (otherCarPositionOnTrack[i] - 3))
      { 
        otherCarStrategy[i] = 4; 
      }
      if(otherCarStrategy[i] == 4 && otherCarXPosition[i] < 13 && trackPositionCounter < (otherCarPositionOnTrack[i] - 3))
      { 
        if(difficulty == EASY){ otherCarXPosition[i] = otherCarXPosition[i] + 0.2; }
        if(difficulty == AVERAGE){ otherCarXPosition[i] = otherCarXPosition[i] + 0.4; }
        if(difficulty == HARD){ otherCarXPosition[i] = otherCarXPosition[i] + 0.6; }
      }
      if(otherCarStrategy[i] == 4 && otherCarXPosition[i] >= 13 && trackPositionCounter < (otherCarPositionOnTrack[i] - 3))
      { 
        otherCarStrategy[i] = 3; 
      }
      
      // Accelerate other cars after start of the race
      if((otherCarVelocity[i] < otherCarMaxVelocity[i]) && !otherCarFinishReached[i])
      {
        otherCarVelocity[i] = otherCarVelocity[i] + (5.0/1000.0);
      }
    }
    else if(gameMode == TIME)
    {
      // Update position
      otherCarPositionOnTrack[i] = otherCarPositionOnTrack[i] + otherCarVelocity[i];
      if(otherCarPositionOnTrack[i] > totalTrackLength){ otherCarPositionOnTrack[i] = otherCarPositionOnTrack[i] - totalTrackLength; }
    }
    else if(gameMode == LINK)
    {
      readTillByteSerial1(7); // Remove outdated packages from hardware buffer   
      byte value = Serial1.available(); // Check whether at least one package with other player position is available (4 bytes)
      if(value > 3)
      {
        if(value > 8){ value = 8; } // If additional bytes reached hardware buffer, restrict length to 8
        for(byte k = 0; k < 8; k++)
        {
          readBuffer[k] = 255; // Fill all software buffer bytes with 1
        }
        Serial1.readBytes(readBuffer, value); 
        // Read the last received package in reverse manner (ends with termination symbol)
        for(byte k = 7; k > 2; k--)
        {
          if(readBuffer[k] == TERMINATION)
          {
            // After package has been found, encode track position and distance from centerline
            otherCarPositionOnTrack[i] = readBuffer[k - 3] * 250; 
            otherCarPositionOnTrack[i] = otherCarPositionOnTrack[i] + readBuffer[k - 2]; 
            otherCarXPosition[i] = -(readBuffer[k - 1] - 14); // subtract -14 since transmission is unsigned
            break; 
          }
        }
        if(otherCarPositionOnTrack[i] > totalTrackLength - trackSegmentLength[numberOfTrackSegments - 1])
        { 
          otherCarFinishReached[i] = true;
          numberOfCarsInFinish++;
        }
      }
    }

    // Avoid collisions if overtaking (for computer controlled cars)
    if(gameMode != LINK)
    {
      // Check if a fast car would hit you from behind and avoid collision;
      if((velocity == 0 || ((20.0 * otherCarVelocity[i]) > (1000.0 / (200 - int(velocity))))) 
      && (otherCarPositionOnTrack[i] + 3 < trackPositionCounter && otherCarPositionOnTrack[i] + 5 > trackPositionCounter)
      || (otherCarPositionOnTrack[i] + 3 - totalTrackLength < trackPositionCounter && otherCarPositionOnTrack[i] + 5 - totalTrackLength > trackPositionCounter))
      {
        if(playerCenterlineDistance < 0)
        {
          otherCarXPosition[i] = random(11 - playerCenterlineDistance - 6) - 13;
        }
        else if(playerCenterlineDistance > 0)
        {
          otherCarXPosition[i] = random(12 + playerCenterlineDistance - 6) - playerCenterlineDistance + 7;        
        }
        else
        {
          if(random(2) == 0)
          {
            otherCarXPosition[i] = random(5) - 13;                  
          }
          else
          {
            otherCarXPosition[i] = random(6) + 7;                            
          }
        }
      }
    }

    // Re-calculate race position (as long as finish has not been reached)
    if(!finishReached && (otherCarPositionOnTrack[i] < trackPositionCounter)){ racePosition--; }

    // Check if other car is visible on screen
    int distance = otherCarPositionOnTrack[i] - trackPositionCounter;
    // Modulate distance for visualization to have a more realistic effect
    if(distance >= -2 && distance < 32)
    {
      distance = distance / (2.0 * ((distance / 64.0) + 0.5));
    }
    // Draw other cars if they are in visible distance
    if(distance >= -2 && distance < 16)
    {
      otherCarVisible[i] = true;
      otherCarYPositionOnScreen[i] = 29 - ((double(distance) / 16.0) * double(29 - yHorizon));
      if(distance >= 15 && distance < 16)
      {
        otherCarSizeOnScreen[i] = 1;                
        otherCarXPositionOnScreen[i] = centerline[otherCarYPositionOnScreen[i]] + 0.5 + (double(otherCarYPositionOnScreen[i] - yHorizon)*(double(otherCarXPosition[i] /(32.0 - double(yHorizon)))));
      }
      else if(distance >= 11 && distance < 15)
      {
        otherCarSizeOnScreen[i] = 2;                
        otherCarXPositionOnScreen[i] = centerline[otherCarYPositionOnScreen[i]] + (double(otherCarYPositionOnScreen[i] - yHorizon)*(double(otherCarXPosition[i] /(32.0 - double(yHorizon)))));
      }
      else if(distance >= 7 && distance < 11)
      {
        otherCarSizeOnScreen[i] = 3;                
        otherCarXPositionOnScreen[i] = centerline[otherCarYPositionOnScreen[i]] - 1 + (double(otherCarYPositionOnScreen[i] - yHorizon)*(double(otherCarXPosition[i] /(32.0 - double(yHorizon)))));
      }
      else if(distance < 7)
      {
        otherCarSizeOnScreen[i] = 4;        
        otherCarXPositionOnScreen[i] = centerline[otherCarYPositionOnScreen[i]] - 2 + (double(otherCarYPositionOnScreen[i] - yHorizon)*(double(otherCarXPosition[i] /(32.0 - double(yHorizon)))));
      }

      // SOUND: Other car (not in LINK mode)
      if(distance < 5 && gameMode != LINK)
      {
        if(collisionSoundOn == 0){ tone(audio, 50 + (otherCarVelocity[i] * 200.0) + (distance * 10), 100); }
      }      
    }
    else
    {
      otherCarVisible[i] = false;
    }

    // Draw other car on new screen position if visible
    if(otherCarVisible[i])
    {
      drawOtherCar(i);
    }    
  }

  // Draw player car to avoid overlaps
  drawPlayerCar();
  
  // Play collision sound (in case there was a collision)
  if(collisionSoundOn > 0){
    // SOUND: Collision
    tone(audio, 50 + random(350), 20);     
    collisionSoundOn--;
  }
}

// removeOtherCar: Removes car from the screen
void removeOtherCar(byte i)
{
  switch(otherCarSizeOnScreen[i])
  {
    case 1:
      matrix.drawPixel(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i], matrix.Color333(0, 0, 0));
      break;
    case 2:
      matrix.drawPixel(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i], matrix.Color333(0, 0, 0));
      matrix.drawPixel(otherCarXPositionOnScreen[i] + 1, otherCarYPositionOnScreen[i], matrix.Color333(0, 0, 0));
      break;
    case 3:
      matrix.drawLine(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i], otherCarXPositionOnScreen[i] + 3, otherCarYPositionOnScreen[i], matrix.Color333(0, 0, 0));
      matrix.drawLine(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i] + 1, otherCarXPositionOnScreen[i] + 3, otherCarYPositionOnScreen[i] + 1, matrix.Color333(0, 0, 0));
      break;
    case 4:
      if(otherCarType[i] < 8)
      { 
        matrix.drawLine(otherCarXPositionOnScreen[i] + 1, otherCarYPositionOnScreen[i], otherCarXPositionOnScreen[i] + 4, otherCarYPositionOnScreen[i], matrix.Color333(0, 0, 0));
      }
      else
      {
        matrix.drawPixel(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(otherCarXPositionOnScreen[i] + 2, otherCarYPositionOnScreen[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(otherCarXPositionOnScreen[i] + 3, otherCarYPositionOnScreen[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(otherCarXPositionOnScreen[i] + 5, otherCarYPositionOnScreen[i], matrix.Color333(0, 0, 0));
      }
      matrix.drawLine(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i] + 1, otherCarXPositionOnScreen[i] + 5, otherCarYPositionOnScreen[i] + 1, matrix.Color333(0, 0, 0));
      matrix.drawLine(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i] + 2, otherCarXPositionOnScreen[i] + 5, otherCarYPositionOnScreen[i] + 2, matrix.Color333(0, 0, 0));
      break;
    default:
      break;
  }  
}

// drawOtherCar: Draws car on the screen
void drawOtherCar(byte i)
{
  if(gameMode != LINK)
  {
    switch(otherCarType[i] % 4)
    {
      case 0:
        carColorR = 0;
        carColorG = 1;
        carColorB = 0;
        break;
      case 1:
        carColorR = 1;
        carColorG = 0;
        carColorB = 1;
        break;
      case 2:
        carColorR = 0;
        carColorG = 1;
        carColorB = 1;
        break;
      case 3:
        carColorR = 1;
        carColorG = 1;
        carColorB = 1;
        break;
      default:
        break;
    }
  }
  else
  {
    if(player == PLAYER_1)
    {
      carColorR = 0;
      carColorG = 1;
      carColorB = 0;      
    }
    else
    {
      carColorR = 1;
      carColorG = 1;
      carColorB = 0;            
    }
  }
  switch(otherCarSizeOnScreen[i])
  {
    case 1:
      matrix.drawPixel(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i], matrix.Color333(carColorR, carColorG, carColorB));
      break;
    case 2:
      matrix.drawPixel(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i], matrix.Color333(carColorR, carColorG, carColorB));
      matrix.drawPixel(otherCarXPositionOnScreen[i] + 1, otherCarYPositionOnScreen[i], matrix.Color333(carColorR, carColorG, carColorB));
      break;
    case 3:
      matrix.drawLine(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i], otherCarXPositionOnScreen[i] + 3, otherCarYPositionOnScreen[i], matrix.Color333(0, 0, 1));
      matrix.drawPixel(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i] + 1, matrix.Color333(carColorR, carColorG, carColorB));
      matrix.drawPixel(otherCarXPositionOnScreen[i] + 1, otherCarYPositionOnScreen[i] + 1, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));
      matrix.drawPixel(otherCarXPositionOnScreen[i] + 2, otherCarYPositionOnScreen[i] + 1, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));
      matrix.drawPixel(otherCarXPositionOnScreen[i] + 3, otherCarYPositionOnScreen[i] + 1, matrix.Color333(carColorR, carColorG, carColorB));
      break;
    case 4:
      if(otherCarType[i] < 8)
      { 
        matrix.drawPixel(otherCarXPositionOnScreen[i] + 1, otherCarYPositionOnScreen[i], matrix.Color333(0, 0, 1));   
        matrix.drawPixel(otherCarXPositionOnScreen[i] + 4, otherCarYPositionOnScreen[i], matrix.Color333(0, 0, 1));     
        matrix.drawPixel(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i] + 1, matrix.Color333(1, 0, 0));
        matrix.drawPixel(otherCarXPositionOnScreen[i] + 5, otherCarYPositionOnScreen[i] + 1, matrix.Color333(1, 0, 0));
      }
      else
      {
        matrix.drawPixel(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i], matrix.Color333(1, 0, 0));   
        matrix.drawPixel(otherCarXPositionOnScreen[i] + 5, otherCarYPositionOnScreen[i], matrix.Color333(1, 0, 0));             
        matrix.drawPixel(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i] + 1, matrix.Color333(carColorR, carColorG, carColorB));
        matrix.drawPixel(otherCarXPositionOnScreen[i] + 5, otherCarYPositionOnScreen[i] + 1, matrix.Color333(carColorR, carColorG, carColorB));
      }
      matrix.drawPixel(otherCarXPositionOnScreen[i] + 2, otherCarYPositionOnScreen[i], matrix.Color333(0, 0, 3));
      matrix.drawPixel(otherCarXPositionOnScreen[i] + 3, otherCarYPositionOnScreen[i], matrix.Color333(0, 0, 3));
      if(otherCarType[i] < 4)
      {
        matrix.drawPixel(otherCarXPositionOnScreen[i] + 1, otherCarYPositionOnScreen[i] + 1, matrix.Color333(1, 0, 0));
        matrix.drawPixel(otherCarXPositionOnScreen[i] + 4, otherCarYPositionOnScreen[i] + 1, matrix.Color333(1, 0, 0));
      }
      else
      {
        matrix.drawPixel(otherCarXPositionOnScreen[i] + 1, otherCarYPositionOnScreen[i] + 1, matrix.Color333(carColorR, carColorG, carColorB));
        matrix.drawPixel(otherCarXPositionOnScreen[i] + 4, otherCarYPositionOnScreen[i] + 1, matrix.Color333(carColorR, carColorG, carColorB));        
      }
      matrix.drawPixel(otherCarXPositionOnScreen[i] + 2, otherCarYPositionOnScreen[i] + 1, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));
      matrix.drawPixel(otherCarXPositionOnScreen[i] + 3, otherCarYPositionOnScreen[i] + 1, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));
      matrix.drawPixel(otherCarXPositionOnScreen[i], otherCarYPositionOnScreen[i] + 2, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));
      matrix.drawPixel(otherCarXPositionOnScreen[i] + 1, otherCarYPositionOnScreen[i] + 2, matrix.Color333(carColorR, carColorG, carColorB));
      matrix.drawPixel(otherCarXPositionOnScreen[i] + 2, otherCarYPositionOnScreen[i] + 2, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));
      matrix.drawPixel(otherCarXPositionOnScreen[i] + 3, otherCarYPositionOnScreen[i] + 2, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));
      matrix.drawPixel(otherCarXPositionOnScreen[i] + 4, otherCarYPositionOnScreen[i] + 2, matrix.Color333(carColorR, carColorG, carColorB));
      matrix.drawPixel(otherCarXPositionOnScreen[i] + 5, otherCarYPositionOnScreen[i] + 2, matrix.Color333(carColorR * 3, carColorG * 3, carColorB * 3));
      break;
    default:
      break;
  }  
}

// decreaseRemainingTime: Called by remainingTimeTimer every second. If time has expired, the game ends (only used in TIME mode).
void decreaseRemainingTime()
{        
  if(gameMode == TIME)
  {
    // Enable blinking of remaining time just before it expires
    if(remainingTimeBlinking){ remainingTimeBlinking = false; }
    else{ remainingTimeBlinking = true; }
    
    if(remainingTime > 63)
    {
      matrix.drawLine(8, 5, 23, 5, matrix.Color333(1, 1, 0));
    }
    else
    {
      if(remainingTime < 17 & remainingTimeBlinking)
      {
        matrix.drawLine(23 - ((double(remainingTime) / 64.0) * 15.0), 5, 23, 5, matrix.Color333(7, 7, 0));
        tone(audio, 400, 100); // Warning beep
      }
      else
      {
        matrix.drawLine(23 - ((double(remainingTime) / 64.0) * 15.0), 5, 23, 5, matrix.Color333(1, 1, 0));
      }
      matrix.drawLine(8, 5, 23 - ((double(remainingTime) / 64.0) * 15.0), 5, matrix.Color333(0, 0, 1));
    }
    if(remainingTime > 0){ remainingTime--; }
  }
}

// checkForCollisionWithOtherCars: Reduces the velocity by factor 2
void checkForCollisionsWithOtherCars()
{
  boolean collisionDetected = false;
  carShiftByCollision = NOSHIFT;
  
  for(byte i = 0; i < numberOfOtherCars; i++)
  {
    if(otherCarVisible[i] && otherCarSizeOnScreen[i] == 4)
    {
      // Collision
      if(joy1Left())
      {
        if(otherCarXPositionOnScreen[i] >= 7 && otherCarXPositionOnScreen[i] <= 18 && otherCarYPositionOnScreen[i] >= 28 && otherCarYPositionOnScreen[i] <= 30)
        {
          collisionDetected = true;
          velocity = velocity / 2;
          collisionSoundOn = 10;
        }                
      }
      else if(joy1Right())
      {
        if(otherCarXPositionOnScreen[i] >= 8 && otherCarXPositionOnScreen[i] <= 19 && otherCarYPositionOnScreen[i] >= 28 && otherCarYPositionOnScreen[i] <= 30)
        {
          collisionDetected = true;
          velocity = velocity / 2;
          collisionSoundOn = 10;
        }                
      }
      else
      {
        if(otherCarXPositionOnScreen[i] >= 8 && otherCarXPositionOnScreen[i] <= 18 && otherCarYPositionOnScreen[i] >= 28 && otherCarYPositionOnScreen[i] <= 30)
        {
          collisionDetected = true;
          velocity = velocity / 2;
          collisionSoundOn = 10;
        }        
      }      
    }
  }

  // Special behavior in case of LINK mode to avoid that two cars become wedged together
  if(gameMode == LINK && collisionDetected)
  {
    // Player´s car is pushed to the right side
    if(otherCarXPositionOnScreen[0] <= 12 && playerCenterlineDistance > -14.0)
    {
      carShiftByCollision = RIGHTSHIFT; // Car is automatically moved to the right hand side
    }
    // Player´s car is pushed to the left side
    else if(otherCarXPositionOnScreen[0] >= 14 && playerCenterlineDistance < 14.0)
    {
      carShiftByCollision = LEFTSHIFT; // Car is automatically moved to the left hand side
    }
  }
}

// setupRaceTrack: Loads race track from progmem
void setupRaceTrack(byte trackNumber)
{
  for(int i = 0; i < 256; i++)
  {
    trackSegmentCurve[i] = 0;
    trackSegmentHorizonHeight[i] = 0;
    trackSegmentHeightLevel[i] = 0;
    trackSegmentLength[i] = 0;  
  }

  // Copy race track form progmem
  int j = 0;
  for(byte i = 0; i < trackNumber; i++)
  {
    numberOfTrackSegments = byte(pgm_read_byte_near(raceTracks + j));
    landscapeMode = byte(pgm_read_byte_near(raceTracks + j + 1));
    byte horizonColor = byte(pgm_read_byte_near(raceTracks + j + 2));
    setColor(horizonColor, horizonColorR, horizonColorG, horizonColorB);
    byte landscapeColor = byte(pgm_read_byte_near(raceTracks + j + 3));
    setColor(landscapeColor, landscapeColorR, landscapeColorG, landscapeColorB);

    if(i < trackNumber - 1)
    {
      j = j + (numberOfTrackSegments * 4) + 4;
    }
  }

  for(byte i = 0; i < numberOfTrackSegments; i++)
  {
    trackSegmentCurve[i] = byte(pgm_read_byte_near(raceTracks + j + (i * 4) + 4)) - 20;
    trackSegmentHorizonHeight[i] = byte(pgm_read_byte_near(raceTracks + j + (i * 4) + 5)) - 10;
    trackSegmentHeightLevel[i] = byte(pgm_read_byte_near(raceTracks + j + (i * 4) + 6)) - 10;
    trackSegmentLength[i] = byte(pgm_read_byte_near(raceTracks + j + (i * 4) + 7));
  }
}

// startSignal: Shows the starting light
void startSignal(){
  matrix.fillRect(8, 10, 16, 4, matrix.Color333(0, 0, 0));
  matrix.drawRect(8, 10, 16, 4, matrix.Color333(2, 2, 3));
  delay(1000);
  matrix.fillRect(9, 11, 2, 2, matrix.Color333(7, 0, 0));
  tone(audio, 400, 100);
  delay(1000);
  matrix.fillRect(12, 11, 2, 2, matrix.Color333(7, 0, 0));
  tone(audio, 400, 100);
  delay(1000);
  matrix.fillRect(15, 11, 2, 2, matrix.Color333(7, 0, 0));
  tone(audio, 400, 100);
  delay(1000);
  matrix.fillRect(18, 11, 2, 2, matrix.Color333(7, 0, 0));
  tone(audio, 400, 100);
  delay(1000);
  matrix.fillRect(21, 11, 2, 2, matrix.Color333(7, 0, 0));
  tone(audio, 400, 100);
  delay(1000);
  matrix.fillRect(9, 11, 2, 2, matrix.Color333(0, 7, 0));
  matrix.fillRect(12, 11, 2, 2, matrix.Color333(0, 7, 0));
  matrix.fillRect(15, 11, 2, 2, matrix.Color333(0, 7, 0));
  matrix.fillRect(18, 11, 2, 2, matrix.Color333(0, 7, 0));
  matrix.fillRect(21, 11, 2, 2, matrix.Color333(0, 7, 0));
  tone(audio, 800, 200);
  delay(1000);
  matrix.fillRect(8, 10, 16, 4, matrix.Color333(0, 0, 0));
}

// showResults: Shows position, time etc. (depends on mode)
void showResults()
{
  // Show whether player is winner or loser in upper part of the screen
  if(!winner)
  { 
    matrix.fillRect(0, 0, 32, 16, matrix.Color333(0, 0, 0));
    matrix.setCursor(8, 0);
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.println("You");
    matrix.setCursor(7, 1);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.println("You");
    matrix.setCursor(5, 8);
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.println("lose");
    matrix.setCursor(4, 9);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.println("lose");

    // SOUND: Play reverse theme if loser (not in LINK mode to avoid a sound chaos)
    if(gameMode != LINK)
    {
      for(byte i = 16; i >= 1; i--)
      {
        tone(audio, theme[i] / 2, 300);
        delay(400);
        tone(audio, theme[i] / 4, 300);
        delay(400);
      }
    }  
  }
  else
  {
    matrix.fillRect(0, 0, 32, 16, matrix.Color333(0, 0, 0));
    matrix.setCursor(8, 0);
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.println("You");
    matrix.setCursor(7, 1);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.println("You");
    matrix.setCursor(6, 8);
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.println("win!");
    matrix.setCursor(5, 9);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.println("win!");    

    // SOUND: Play theme if winner
    for(byte i = 0; i < 17; i++)
    {
      tone(audio, theme[i], 200);
      delay(200);
      tone(audio, theme[i] / 2, 200);
      delay(200);
      tone(audio, theme[i] / 4, 200);
      delay(200);
      tone(audio, theme[i] / 2, 200);
      delay(200);
    }  
  } 

  int counter = 0;
  byte musicCounter = 0;
  byte infoPage = 0;
  int waitingTime = 30000; // Games is proceeded on selection screens after waitingTime in milli seconds
  do
  {
    delay(1);
    waitingTime--;
    if(counter == 0)
    {
      if(infoPage == 0)
      {
        // Show track name
        matrix.fillRect(0, 16, 32, 16, matrix.Color333(0, 0, 0));
        matrix.setCursor(2, 20);
        matrix.setTextColor(matrix.Color333(1, 1, 1));
        matrix.println(raceTrackNames[raceTrack - 1]);
        matrix.setCursor(1, 21);
        matrix.setTextColor(matrix.Color333(3, 0, 0));
        matrix.println(raceTrackNames[raceTrack - 1]);
        infoPage++;        
      }
      else if(infoPage == 1)
      {
        if(gameMode != TIME)
        {
          // Show position
          matrix.fillRect(0, 16, 32, 16, matrix.Color333(0, 0, 0));
          if(racePosition < 10){ matrix.setCursor(8, 20); }
          else{ matrix.setCursor(5, 20); }
          matrix.setTextColor(matrix.Color333(1, 1, 1));
          matrix.print(racePosition);
          if(racePosition == 1){ matrix.print("st"); }
          else if(racePosition == 2){ matrix.print("nd"); }
          else if(racePosition == 3){ matrix.print("rd"); }
          else{ matrix.print("th"); }
          if(racePosition < 10){ matrix.setCursor(7, 21); }
          else{matrix.setCursor(4, 21); }
          matrix.setTextColor(matrix.Color333(3, 0, 0));
          matrix.print(racePosition);    
          if(racePosition == 1){ matrix.print("st"); }
          else if(racePosition == 2){ matrix.print("nd"); }
          else if(racePosition == 3){ matrix.print("rd"); }
          else{ matrix.print("th"); }
          infoPage++;
        }
        else
        {
          counter = 2500;
          infoPage++;
        }
      }
      else if(infoPage == 2)
      {
        if(winner)
        {
          // Show lap time
          matrix.fillRect(0, 16, 32, 16, matrix.Color333(0, 0, 0));        
          int minutes = (lapTime / 1000) / 60;
          int seconds = (lapTime / 1000) % 60;
          int tenthSeconds = (lapTime / 100) % 10;
          matrix.setTextColor(matrix.Color333(1, 1, 1));
          matrix.setCursor(2, 20);
          matrix.print(minutes);
          matrix.drawPixel(8, 22, matrix.Color333(1, 1, 1));
          matrix.drawPixel(8, 24, matrix.Color333(1, 1, 1));
          matrix.setCursor(11, 20);
          if(seconds < 10){ matrix.print("0"); }
          matrix.print(seconds);
          matrix.drawLine(23, 26, 23, 27, matrix.Color333(1, 1, 1));
          matrix.setCursor(26, 20);
          matrix.print(tenthSeconds);  
          matrix.setTextColor(matrix.Color333(3, 0, 0));
          matrix.setCursor(1, 21);
          matrix.print(minutes);
          matrix.drawPixel(7, 23, matrix.Color333(3, 0, 0));
          matrix.drawPixel(7, 25, matrix.Color333(3, 0, 0));
          matrix.setCursor(10, 21);
          if(seconds < 10){ matrix.print("0"); }
          matrix.print(seconds);
          matrix.drawLine(22, 27, 22, 28, matrix.Color333(3, 0, 0));
          matrix.setCursor(25, 21);
          matrix.print(tenthSeconds);
          infoPage++;
        }
        else
        {
          counter = 2500;          
          infoPage++;
        }
      }
    }

    counter++;
    if(counter > 2500){ counter = 0; }
    if(infoPage == 3){ infoPage = 0; }
  }
  while(!joy1Fire() && waitingTime > 0);
  do
  {
    delay(1);
  }
  while(joy1Fire());
  
  // SOUND: Start game
  tone(audio, 1024, 20);
  delay(200);
}

// race: Contains the main loop for the race
void race()
{
  // Initialize variables for the visualization of the race track and the landscape
  xHorizon = 15.5;
  yHorizon = 16;
  yHorizonOld;
  yLandscape = 16; // Position of the background landscape (Note: Old positions are stored in a specific array)
  boolean updateCenterline = true;  // Helper to save CPU cycles and to avoid flickering
  boolean updateLandscape = true;   // Helper to save CPU cycles and to avoid flickering
  byte counter = 0;                 // Helper used for synchronization
  playerCenterlineDistance = 0.0; 
  boolean showStartSignal = true;   // If true, race has not yet started.
  boolean blockGasPedal = false;    // If player has no more chance to win or if player reaches the finish, block gas pedal.
  finishReached = false;
  carShiftByCollision = NOSHIFT;
  collisionSoundOn = 0;
  winner = false;
  boolean reset = false;            // If true, go back to title.

  // Setup race track
  setupRaceTrack(raceTrack);

  // Calculate total track length from all track segments
  totalTrackLength = 0;
  for(byte i = 0; i < numberOfTrackSegments; i++)
  {
    totalTrackLength = totalTrackLength + trackSegmentLength[i];
  }
  if(gameMode == RACE)
  {
    trackPositionCounter = 5; // Finished cars are moved to position 0 with velocity 0. Starting at 5 avoids that they appear suddenly if player not starts the race.
  }
  else
  {
    trackPositionCounter = 0;
  }

  // Define player variables
  velocity = 0;
  gear = false;
  gearShiftUsed = 8;
  racePosition = 16;

  // Initialize other cars
  if(gameMode != LINK)
  {
    numberOfOtherCars = 15;
    for(byte i = 0; i < numberOfOtherCars; i++)
    {
      otherCarFinishReached[i] = false;
      
      // Depending on the difficulty, the max velocity of other cars differ
      if(difficulty == EASY){ otherCarMaxVelocity[i] = 0.5 + (double(i) / 100.0); }
      else if(difficulty == AVERAGE){ otherCarMaxVelocity[i] = 0.575 + (double(i) / 100.0); }
      else if(difficulty == HARD){ otherCarMaxVelocity[i] = 0.65 + (double(i) / 100.0); }
      
      if(gameMode == RACE)
      {
        otherCarPositionOnTrack[i] = ((i * 12) + 12) + 5;
        otherCarXPosition[i] = -6 + ((i % 2) * 12);
        otherCarVelocity[i] = 0.0;
      }
      else if(gameMode == TIME)
      {
        otherCarPositionOnTrack[i] = random(((totalTrackLength - 32) / 4) / 15) + ((((totalTrackLength - 32) / 4) / 15) * i) + 32; // random((totalTrackLength - 32) / 4) + 32; // Make sure that cars are out of sight
        otherCarXPosition[i] = random(24) - 12;
        otherCarVelocity[i] = otherCarMaxVelocity[i];      
      }    
      otherCarType[i] = random(12);    
      otherCarStrategy[i] = random(5);
      otherCarVisible[i] = false;
      otherCarXPositionOnScreen[i] = 0;
      otherCarYPositionOnScreen[i] = 0;
      otherCarSizeOnScreen[i] = 0;
      
      // Define minimal time
      if(raceTrack == 1){ remainingTime = 110; }      // Germany
      else if(raceTrack == 2){ remainingTime = 130; } // France
      else if(raceTrack == 3){ remainingTime = 120; } // Spain
      else if(raceTrack == 4){ remainingTime = 140; } // Italy
      else if(raceTrack == 5){ remainingTime = 140; } // Switzerland
      else if(raceTrack == 6){ remainingTime = 130; } // Sweden
    }
  }
  else if(gameMode == LINK)
  {
    numberOfOtherCars = 1;
    otherCarFinishReached[0] = false;   
    otherCarPositionOnTrack[0] = 0;
    if(player == PLAYER_1)
    { 
      otherCarXPosition[0] = 8; 
      playerCenterlineDistance = 6;
    }
    else
    { 
      otherCarXPosition[0] = -8; 
      playerCenterlineDistance = -6;
    }
    otherCarType[0] = 0;    
    otherCarVisible[0] = false;
    otherCarXPositionOnScreen[0] = 0;
    otherCarYPositionOnScreen[0] = 0;
    otherCarSizeOnScreen[0] = 0;
  }
  numberOfCarsInFinish = 0;

  // Initial calculation of centerline
  calculateCenterline(xHorizon, yHorizon);
  modifyCenterline(yHorizon, playerCenterlineDistance);

  // Initialize race track
  currentTrackSegment = 0;
  trackSegmentLengthCounter = trackSegmentLength[currentTrackSegment];

  // Flush screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Define a random landscape
  initLandscape();
  orientation = 48;

  // Enable initial visualization of screen
  updateCenterline = true;
  gearShiftUsed = 9; // Important to show gear initially (even to show "A" in automatic mode)

  // Take time before race
  startTime = millis();
  
  do
  {    
    // Get time of engine loop start point (important for a constant game speed)
    engineLoopStartPoint = millis();

    // Check if boost is active (only in LINK mode if player is behind opponent)
    if(trackPositionCounter < otherCarPositionOnTrack[0] - 5)
    {
      boost = maximalBoost;
    }
    else
    {
      boost = 0.0;
      if(velocity > maximalVelocity){ velocity = velocity - 1.0; }      
    }

    // Check joystick

    if(joy1IsLEDmePlayJoypad()) // LEDmePlay Joypad
    {
      // Accelerate

      // Automatic shift
      if(shiftMode == AUTO && (joy1YValue() > 0.0) && velocity < (maximalVelocity + boost) && !blockGasPedal)
      {
        velocity = velocity + ((joy1YValue() * 10.0) / (1.0 + (velocity / 50.0)));
      }
      // Manual shift
      // Low gear
      if(shiftMode == MANUAL && (joy1YValue() > 0.0) && !gear && velocity < maximalVelocityLowGear && !blockGasPedal)
      {
        velocity = velocity + ((joy1YValue() * 12.0) / (1.0 + (velocity / 40.0)));
      }
      // High gear
      if(shiftMode == MANUAL && (joy1YValue() > 0.0) && gear && velocity < (maximalVelocity + boost) && !blockGasPedal)
      {
        if(velocity < 80)
        {
          velocity = velocity + ((joy1YValue() * 5.0) / (1.0 + ((80 - velocity) / 25.0)));        
        }
        else
        {
          velocity = velocity + ((joy1YValue() * 5.0) / (1.0 + ((velocity - 80) / 25.0)));
        }
      }

      // Break
    
      else if((joy1YValue() < 0.0) && velocity > 0)
      {
        velocity = velocity + (joy1YValue() * 8.0);
        if(velocity < 0){ velocity = 0; }
      }
      else if(!blockGasPedal)
      {
        // Motor break
        if(!gear && velocity > 120 && shiftMode == MANUAL)
        {
          velocity = velocity - 4.0;       
        }
        else
        {
          velocity = velocity - 2.0;
        }
        if(velocity < 0){ velocity = 0; }      
      }
      else
      {
        velocity = velocity - 6.0;
        if(velocity < 0){ velocity = 0; }            
      }
    }
    else // Classic Joystick
    {
      // Accelerate

      // Automatic shift
      if(shiftMode == AUTO && joy1Up() && velocity < (maximalVelocity + boost) && !blockGasPedal)
      {
        velocity = velocity + (10.0 / (1.0 + (velocity / 50.0)));
      }
      // Manual shift
      // Low gear
      if(shiftMode == MANUAL && joy1Up() && !gear && velocity < maximalVelocityLowGear && !blockGasPedal)
      {
        velocity = velocity + (12.0 / (1.0 + (velocity / 40.0)));
      }
      // High gear
      if(shiftMode == MANUAL && joy1Up() && gear && velocity < (maximalVelocity + boost) && !blockGasPedal)
      {
        if(velocity < 80)
        {
          velocity = velocity + (5.0 / (1.0 + ((80 - velocity) / 25.0)));        
        }
        else
        {
          velocity = velocity + (5.0 / (1.0 + ((velocity - 80) / 25.0)));
        }
      }

      // Break
    
      else if(joy1Down() && velocity > 0)
      {
        velocity = velocity - 8.0;
        if(velocity < 0){ velocity = 0; }
      }
      else if(!blockGasPedal)
      {
        // Motor break
        if(!gear && velocity > 120 && shiftMode == MANUAL)
        {
          velocity = velocity - 4.0;       
        }
        else
        {
          velocity = velocity - 2.0;
        }
        if(velocity < 0){ velocity = 0; }      
      }
      else
      {
        velocity = velocity - 6.0;
        if(velocity < 0){ velocity = 0; }            
      }      
    }

    // Modify speed according to track slope
    if(yHorizon < 16 && velocity > 0)
    {
      velocity = velocity - ((16 - yHorizon) / 16.0);
    }
    else if(yHorizon > 16 && velocity < (maximalVelocity + boost))
    {
      velocity = velocity + ((yHorizon - 16) / 32.0);
    }

    // Use gear shift (if available)

    if(gearShiftUsed > 0)
    {
      gearShiftUsed--;      
    }
    if(joy1Fire() && gearShiftUsed == 0)
    {
      if(!gear){ gear = true; }
      else if(gear){ gear = false; }
      gearShiftUsed = 8;
    }

    if(velocity == 0)
    {
      // Car moved by crashes (only used in LINK mode)
      if(carShiftByCollision == LEFTSHIFT && playerCenterlineDistance < 14.0)
      {
        playerCenterlineDistance = playerCenterlineDistance + 0.5;
        yHorizonOld = yHorizon;
        updateCenterline = true;        
      }
      if(carShiftByCollision == RIGHTSHIFT && playerCenterlineDistance > -14.0)
      {
        playerCenterlineDistance = playerCenterlineDistance - 0.5;
        yHorizonOld = yHorizon;
        updateCenterline = true;
      }      
    }
    else if(velocity > 0)
    {
      
      // Steering

      if(joy1IsLEDmePlayJoypad()) // LEDmePlay Joypad
      {
        if(((joy1XValue() > 0.0) || carShiftByCollision == LEFTSHIFT) && playerCenterlineDistance < 14.0)
        {
          playerCenterlineDistance = playerCenterlineDistance + (joy1XValue() * 0.5);
          yHorizonOld = yHorizon;
          updateCenterline = true;
        }
        if(((joy1XValue() < 0.0) || carShiftByCollision == RIGHTSHIFT) && playerCenterlineDistance > -14.0)
        {
          playerCenterlineDistance = playerCenterlineDistance + (joy1XValue() * 0.5);
          yHorizonOld = yHorizon;
          updateCenterline = true;
        }        
      }
      else // Classic Joystick
      {
        if((joy1Left() || carShiftByCollision == LEFTSHIFT) && playerCenterlineDistance < 14.0)
        {
          playerCenterlineDistance = playerCenterlineDistance + 0.5;
          yHorizonOld = yHorizon;
          updateCenterline = true;
        }
        if((joy1Right() || carShiftByCollision == RIGHTSHIFT) && playerCenterlineDistance > -14.0)
        {
          playerCenterlineDistance = playerCenterlineDistance - 0.5;
          yHorizonOld = yHorizon;
          updateCenterline = true;
        }
      }
           
      // Check for curves
      if((15.5 - double(trackSegmentCurve[currentTrackSegment])) < xHorizon)
      {
        yHorizonOld = yHorizon;
        xHorizon = xHorizon - 1.0;
        updateCenterline = true;      
      }
      else if((15.5 - double(trackSegmentCurve[currentTrackSegment])) > xHorizon)
      {
        yHorizonOld = yHorizon;
        xHorizon = xHorizon + 1.0;
        updateCenterline = true;      
      }
  
      // Calculate effects of velocity and curve radius on player distance to the centerline
      double playerCenterlineDistanceOld = playerCenterlineDistance;
      playerCenterlineDistance = playerCenterlineDistance - (((15.5 - xHorizon) / 18.5) * (velocity / 120));
      if(playerCenterlineDistanceOld != playerCenterlineDistance)
      {
        yHorizonOld = yHorizon;
        updateCenterline = true;
      }
      
      if(playerCenterlineDistance > 12.0)
      {
        velocity = velocity - 10.0;
        collisionSoundOn = 10;
      }
      else if(playerCenterlineDistance < -12.0)
      {
        velocity = velocity - 10.0;        
        collisionSoundOn = 10;
      }
      if(velocity < 1){ velocity = 1; }
  
      // Check for height changes of the horizon
      if((16 - trackSegmentHorizonHeight[currentTrackSegment]) < yHorizon)
      {
        yHorizonOld = yHorizon;
        yHorizon--;  
        updateCenterline = true;      
      }
      else if((16 - trackSegmentHorizonHeight[currentTrackSegment]) > yHorizon)
      {
        yHorizonOld = yHorizon;
        yHorizon++;
        updateCenterline = true;      
      }
  
      // Check for height changes of the landscape
      if((16 - trackSegmentHeightLevel[currentTrackSegment]) < yLandscape)
      {
        yLandscape--;
        updateLandscape = true;
      }
      else if((16 - trackSegmentHeightLevel[currentTrackSegment]) > yLandscape)
      {
        yLandscape++;
        updateLandscape = true;      
      }
      
      // Update orientation
      double orientationOld = orientation;
      orientation = orientation - ((15.5 - xHorizon) / 18.5);
      
      if(orientation < 0){ orientation = (128 + orientation); }
      if(orientation > 128){ orientation = orientation - 128; }
      
      if(abs(int(orientationOld + 0.5) - int(orientation + 0.5)) != 0){ updateLandscape = true; }
    }

    if(updateCenterline)
    {
      copyCenterline();
      calculateCenterline(xHorizon, yHorizon);
      modifyCenterline(yHorizon, playerCenterlineDistance);
      if(yHorizon == yHorizonOld)
      {
        removeRoad(yHorizonOld, false);
      }
      else
      {
        removeRoad(yHorizonOld, true);        
        updateLandscape = true;
      }
      updateCenterline = false;
    }
    if(updateLandscape)
    {
      drawLandscape(yHorizon, yLandscape);
      updateLandscape = false;
    }
    drawRoad(yHorizon, counter);

    if(velocity > 0)
    {
      counter--;
      
      // SOUND: Car sound
      if(collisionSoundOn == 0 && shiftMode == AUTO){ tone(audio, 50 + (velocity * 1.2) + random(10), 200); }
      else if(collisionSoundOn == 0 && shiftMode == MANUAL && !gear){ tone(audio, 50 + (velocity * 1.4) + random(10), 200); }
      else if(collisionSoundOn == 0 && shiftMode == MANUAL && gear){ tone(audio, 50 + (velocity * 1.0) + random(10), 200); }
    
      // Update track segment length counter and start new track segment if reached
      trackSegmentLengthCounter--;
      trackPositionCounter++;
      if(trackSegmentLengthCounter == 0)
      {
        currentTrackSegment++;
        // Finish reached
        // Note: Last segement is the finish
        if(currentTrackSegment == numberOfTrackSegments - 1)
        {
          finishReached = true;
        }
        if(currentTrackSegment == numberOfTrackSegments)
        {
          currentTrackSegment = 0;
          trackPositionCounter = 0;
        }
        trackSegmentLengthCounter = trackSegmentLength[currentTrackSegment];
      }

      // Send position on track in link mode
      // Note: Positions are sent in packages consisting of 4 bytes
      // byte 1: First part of the position on track
      // byte 2: Second part of the position on track
      // byte 3: Distance from centerline
      // byte 4: Value 250 as termination symbol
      if(gameMode == LINK)
      {
        Serial1.write(byte(trackPositionCounter / 250));
        Serial1.write(byte(trackPositionCounter % 250));
        Serial1.write(byte(playerCenterlineDistance + 14)); // +14 is added to remove negative values (is decoded on other side)
        Serial1.write(TERMINATION); // Write termination symbol
      }
    }

    checkForCollisionsWithOtherCars();
    drawPlayerCar();
    updateInstruments();

    // Wait loop to synchronize the game
    // Note: The game has no fixed frame rate. It depends on the velocity of the player car.
    if(velocity > 0)
    {
      do
      {
        // Timers are used to control those things which needs to be independent of the car velocity.
        
        // Decrease remaining time
        remainingTimeTimer.update();

        // Control and draw other cars
        carsTimer.update();        
      }
      while(abs(millis() - engineLoopStartPoint) < (200 - int(velocity)));
    }
    else if(velocity == 0)
    {
        // Update timers in case the car is not moving

        // Decrease remaining time
        remainingTimeTimer.update();

        // Control and draw other cars
        carsTimer.update();        
    }

    // Show start signal
    if(showStartSignal)
    { 
      startSignal();
      showStartSignal = false;
      updateLandscape = true; // Redraw after removal of start signal
    }

    // If player has no more chance to win or if player reaches the finish, block gas pedal.
    if(gameMode == RACE && numberOfCarsInFinish > 2){ blockGasPedal = true; }
    else if(gameMode == TIME && remainingTime == 0){ blockGasPedal = true; }
    else if(gameMode == LINK && numberOfCarsInFinish > 1){ blockGasPedal = true; }
    else if(finishReached && !blockGasPedal)
    { 
      blockGasPedal = true;
      winner = true; 
    }

    // Reset
    if(digitalRead(buttonReset) == LOW)
    {
      reset = true;
      break;
    }
    
    // Pause
    if(digitalRead(buttonPause) == LOW)
    {          
      tone(audio,1024,20);
      delay(500);
      do
      {
      }
      while((!digitalRead(buttonPause) == LOW) && !joy1Fire());
      tone(audio,1024,20);
      delay(500);
    }    
  }
  while(!(velocity == 0 && blockGasPedal));

  // Take time after race
  endTime = millis();

  // Calculate lap time
  lapTime = endTime - startTime;

  if(!reset){ showResults(); }
}

// endSequence: Shows cup for winners
void endSequence()
{
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  if(difficulty == EASY)
  {
    matrix.setCursor(5, 0);
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.println("Easy");
    matrix.setCursor(4, 1);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.println("Easy");
  }
  else if(difficulty = AVERAGE)
  {
    matrix.setCursor(5, 0);
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.print("Avrg");    
    matrix.setCursor(4, 1);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.print("Avrg");
  }
  else if(difficulty == HARD)
  {
    matrix.setCursor(5, 0);
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.print("Hard");    
    matrix.setCursor(4, 1);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.print("Hard");
  }

  // Draw cup
  matrix.drawLine(11, 10, 20, 10, matrix.Color333(5, 3, 0));
  matrix.drawLine(11, 11, 11, 13, matrix.Color333(5, 3, 0));
  matrix.drawLine(20, 11, 20, 13, matrix.Color333(5, 3, 0));
  matrix.drawLine(12, 14, 12, 15, matrix.Color333(5, 3, 0));
  matrix.drawLine(19, 14, 19, 15, matrix.Color333(5, 3, 0));
  matrix.drawLine(13, 16, 13, 17, matrix.Color333(5, 3, 0));
  matrix.drawLine(18, 16, 18, 17, matrix.Color333(5, 3, 0));
  matrix.drawLine(14, 18, 14, 20, matrix.Color333(5, 3, 0));
  matrix.drawLine(17, 18, 17, 20, matrix.Color333(5, 3, 0));
  matrix.drawLine(14, 21, 12, 23, matrix.Color333(5, 3, 0));
  matrix.drawLine(17, 21, 19, 23, matrix.Color333(5, 3, 0));
  matrix.drawLine(13, 23, 18, 23, matrix.Color333(5, 3, 0));
 
  matrix.drawLine(9, 11, 10, 11, matrix.Color333(4, 2, 0));
  matrix.drawLine(21, 11, 22, 11, matrix.Color333(4, 2, 0));
  matrix.drawLine(9, 12, 9, 13, matrix.Color333(4, 2, 0));
  matrix.drawLine(22, 12, 22, 13, matrix.Color333(4, 2, 0));
  matrix.drawLine(10, 14, 10, 15, matrix.Color333(4, 2, 0));
  matrix.drawLine(21, 14, 21, 15, matrix.Color333(4, 2, 0));
  matrix.drawLine(11, 16, 12, 17, matrix.Color333(4, 2, 0));
  matrix.drawLine(20, 16, 19, 17, matrix.Color333(4, 2, 0));

    matrix.setCursor(5, 24);
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.println("Won!");
    matrix.setCursor(4, 25);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.println("Won!");

  // SOUND: Play theme if winner
  for(int i = 0; i < 17; i++)
  {
    tone(audio, theme[i] * 2, 200);
    delay(200);
    tone(audio, theme[i] / 2, 200);
    delay(200);
    tone(audio, theme[i], 200);
    delay(200);
    tone(audio, theme[i] / 2, 200);
    delay(200);
  }  

  int waitingTime = 30000; // Games is proceeded after
  do
  {
    delay(1);
    waitingTime--;
  }
  while(!joy1Fire() && waitingTime > 0);
  do
  {
    delay(1);
  }
  while(joy1Fire() && waitingTime > 0);
  // SOUND: Select
  tone(audio, 1024, 20);
  delay(200);
}

// loop: Main loop of the game
void loop() 
{ 
  title();
  if(selectMode())
  {
    if(gameMode == RACE)
    {
      if(selectDifficulty())
      {
        if(selectShiftMode())
        {
          raceTrack = 1;
          do
          {
            showNextTrack();
            race();
            if(winner)
            {
              raceTrack++;  
            }
            if(raceTrack > numberOfRaceTracks)
            {
              endSequence();
              raceTrack = 1;
              if(difficulty < HARD){ difficulty++; }
            }
          }
          while(winner);        
        }
      }
    }
    else if(gameMode == TIME)
    {
      if(selectShiftMode())
      {
        if(selectRaceTrack())
        {
          race();        
        }
      }
    }
    else if(gameMode == LINK)
    {
      if(selectShiftMode())
      {
        if(selectRaceTrackInLinkMode())
        {
          race();
        }
      }
    }
  }
}

