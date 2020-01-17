// Warehouse
// for the LEDmePlay
//
// www.mithotronic.de
//
// Version: 1.0.1
// Author: Thomas Laubach and Michael Rosskopf (2019)
// 
// Release Notes:
// V1.0.1: Small bug in undo function and graphical glitch fixed (24.08.2019)
// V1.0.0: First release

// Include libraries for adafruit matrix
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <avr/pgmspace.h> // Necessary to maintain data in program memory

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

// Other buttons
int buttonReset = 42;
int buttonPause = 43;

// Map encoding
#define W  0 // Wall
#define E  1 // Empty field
#define S  2 // Start position of player
#define X  3 // Storage location
#define B  4 // Box
#define XS 5 // Start position on a storage location
#define XB 6 // Box on a storage location

// Maps
// The maps are modified variants from maps presented on the following website: http://larc.unt.edu/ian/research/sokoban/
// The original maps have been generated automatically by an algorithm which is described on the website and in the following paper:
// Taylor, Joshua and Parberry, Ian (2011), Procedural Generation of Sokoban Levels, 
// Proceedings of the 6th International North American Conference on Intelligent Games and Simulation (GAMEON-NA), EUROSIS, Troy NY
const int numberOfMaps = 50;
const uint8_t maps[] PROGMEM = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, 0, 0, E, X, 0, 0,
  0, 0, E, E, B, S, B, E, 0, 0,
  0, 0, X, E, 0, 0, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, E, E, E, E, 0, 0,
  0, 0, E, 0, E, B, B, E, 0, 0,
  0, 0,XS, X, E, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, B,XB, 0, 0, 0,
  0, 0, S, E, E, E, X, 0, 0, 0,
  0, 0, E, E, E, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, E, S, 0, 0, 0, 0, 0,
  0, 0, E, B, B, E, E, 0, 0, 0,
  0, 0, X, E, E, X, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, E, 0, X, E, 0, 0,
  0, 0, E, E, B, E, E, E, 0, 0,
  0, 0, S, E, E, B, X, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, X, E, E, E, E, S, 0, 0,
  0, 0, E, E, B, B, E, E, 0, 0,
  0, 0, E, E, E, 0, E, E, 0, 0,
  0, 0, 0, E, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, E, 0, 0, 0, 0, 0,
  0, 0, E, E, X, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, E, E, X, E, 0, 0,
  0, 0, E, E, B, E, E, E, 0, 0,
  0, 0, E, E, E, 0, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, E, 0, 0, 0,
  0, 0, 0, 0, 0, E, B, S, 0, 0,
  0, 0, 0, 0, 0, E, E, X, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, E, E, E, 0, 0, 0,
  0, 0, 0, 0, E, E, E, 0, 0, 0,
  0, 0, 0, 0, E, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, B, E, 0, 0, 0,
  0, 0, E, E, E, B, E, 0, 0, 0,
  0, 0, S, X, X, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, X, E, E, 0, 0, 0, 0, 0,
  0, 0, E, B, S, 0, 0, 0, 0, 0,
  0, 0, 0, E, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, X, 0, E, E, 0, 0,
  0, 0, E, E, B, E, E, E, 0, 0,
  0, 0, E, E, 0, E, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, E, E, 0, 0, 0,
  0, 0, X, E, X, 0, B, E, 0, 0,
  0, 0, E, E, B, E, E, E, 0, 0,
  0, 0, S, E, 0, E, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, S, 0, E, E, 0, 0,
  0, 0, E, E, E, 0, E, E, 0, 0,
  0, 0, 0, E, 0, 0, E, E, 0, 0,
  0, 0, E, E, E, 0, B, 0, 0, 0,
  0, 0, E, E, B, E, E, E, 0, 0,
  0, 0, 0, 0, 0, E, X, X, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, X, X, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, B, E, B, S, 0, 0,
  0, 0, E, E, E, E, B, X, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, E, E, E, 0, 0, 0, 0,
  0, 0, 0, E, E, E, 0, 0, 0, 0,
  0, 0, 0, 0, B, E, 0, 0, 0, 0,
  0, 0, 0, E, B, E, 0, 0, 0, 0,
  0, 0, 0, E, E, X, 0, 0, 0, 0,
  0, 0, 0, X, E, 0, 0, 0, 0, 0,
  0, 0, 0, 0, E, 0, 0, 0, 0, 0,
  0, 0, 0, E, E, E, 0, 0, 0, 0,
  0, 0, 0, S, E, E, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, E, E, 0, E, E, 0, 0, 0, 0,
  0, E, E, E, E, E, 0, 0, 0, 0,
  0, E, E, 0, X, E, 0, 0, 0, 0,
  0, E, E, E, 0, X, 0, E, E, 0,
  0, E, B, E, E, E, B, S, E, 0,
  0, E, E, E, 0, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, E, E, E, X, E, 0, E, X, 0,
  0, S, B, E, 0, E, B, B, E, 0,
  0, 0, 0, E, E, X, E, E, E, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  X, E, 0, E, E, E, 0, 0, 0, 0,
  E, E, E, E, E, E, 0, 0, 0, 0,
  X, B, 0, E, E, E, 0, 0, 0, 0,
  E, E, 0, 0, E, 0, E, E, E, 0,
  E, E, 0, E, B, E, E, E, E, 0,
  0, 0, 0, S, E, E, E, E, E, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, X, E, E, 0, 0, 0, 0,
  0, 0, E, 0, E, E, 0, 0, 0, 0,
  0, 0,XB, B, B, E, 0, 0, 0, 0,
  0, 0, E, X, E, E, E, E, 0, 0,
  0, 0, 0, 0, 0, E, E, E, 0, 0,
  0, 0, 0, 0, 0, E, E, S, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, 0, 0, E, S, 0, 0,
  0, 0, E, E, 0, 0, B, E, 0, 0,
  0, 0, X, E, 0, E, X, 0, 0, 0,
  0, 0, E, B, B, E, E, 0, 0, 0,
  0, 0, X, E, E, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, X, X, E, E, E, S, 0, 0,
  0, 0, E, 0, B, B, B, E, 0, 0,
  0, 0, E, E, E, 0, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, X, 0, 0, 0, 0, 0,
  0, 0, E, B, E, 0, 0, 0, 0, 0,
  0, 0, X, E, 0, E, E, 0, 0, 0,
  0, 0, E, B, E, B, E, E, 0, 0,
  0, 0,XS, E, 0, E, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, X, E, X, E, E, E, 0, 0,
  0, 0, E, E, E, E, E, E, 0, 0,
  0, 0, 0, E, 0, E, E, E, 0, 0,
  0, 0, E, E, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, E, E, E, 0, 0, 0,
  0, 0, E, B, E, B, E, 0, 0, 0,
  0, 0, 0, 0, E, E, 0, 0, 0, 0,
  0, 0, E, E, E, E, E, E, 0, 0,
  0, 0, S, E, E, E, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, E, E, 0, 0, 0, 0, 0,
  0, 0, 0, E, E, 0, 0, 0, 0, 0,
  0, 0, 0, B, E, 0, 0, 0, 0, 0,
  0, 0, E, B, E, 0, E, S, 0, 0,
  0, 0, E, 0, E, B, E, E, 0, 0,
  0, 0, X, X, E, E, E, X, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, E, E, E, E, 0, 0,
  0, 0, X, E, E, B, B, E, 0, 0,
  0, 0, X, X, 0, E, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, E, E, 0, 0,
  0, 0, 0, E, E, S, B, E, 0, 0,
  0, 0, 0, E, E, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, E, E, 0, 0, 0, 0, 0,
  0, 0, 0, E, E, 0, 0, 0, 0, 0,
  0, 0, 0, B, E, 0, 0, 0, 0, 0,
  0, 0, E, B, E, B, E, E, 0, 0,
  0, 0, E, E, E, E, E, E, 0, 0,
  0, 0, E, E, E, 0, X, 0, 0, 0,
  0, 0, X, E, 0, 0, X, E, 0, 0,
  0, 0, E, E, E, E, E, E, 0, 0,
  0, 0, S, E, E, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, X, E, E, 0, S, E, 0, 0, 0,
  0, E, E, B, E, B, B, E, 0, 0,
  0, X, X, E, B, E, E, X, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  X, X, X, E, E, 0, E, E, 0, 0,
  S, E, E, E, E, E, E, E, E, 0,
  0, 0, 0, E, E, 0, B, B, E, 0,
  0, 0, E, E, 0, E, B, E, 0, 0,
  0, 0, E, E, E, E, E, E, E, 0,
  0, 0, E, E, 0, E, E, E, E, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, X, X, 0, 0,
  0, 0, 0, 0, 0, 0, E, E, 0, 0,
  0, 0, E, E, X, 0, B, B, 0, 0,
  0, 0, E, B, E, E, B, S, 0, 0,
  0, 0, 0, E, X, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, E, E, E, 0, 0, 0,
  0, 0, X, E, X, 0, B, 0, 0, 0,
  0, 0, E, B, 0, 0, E, X, 0, 0,
  0, 0, S, B, E, E, E, E, 0, 0,
  0, 0, 0, E, E, 0, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, 0, E, E, E, 0, 0,
  0, 0, E, E, E, B, B, E, 0, 0,
  0, 0, E, E, 0, E, E, E, 0, 0,
  0, 0, 0, E, 0, 0, E, E, 0, 0,
  0, 0, E, E, E, E, B, E, 0, 0,
  0, 0, X, X, X, E, S, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, E, E, E, 0, 0,
  0, 0, 0, E, B, E, E, E, 0, 0,
  0, 0, 0, E, E, 0, E, 0, 0, 0,
  0, 0, E, X, E, 0, E, E, 0, 0,
  0, 0, E, E, E, B, B, E, 0, 0,
  0, 0, S, X, E, X, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, E, E, 0, 0, 0, 0, 0,
  0, 0, 0, E, E, 0, 0, 0, 0, 0,
  0, 0, 0, E, B, 0, 0, 0, 0, 0,
  0, 0, E, E, B, E, S, 0, 0, 0,
  0, 0, E, X, B, X, E, 0, 0, 0,
  0, 0, 0, 0, E, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, E,XB, E, E, E, 0, 0,
  0, 0, E, B, E, E, E, E, 0, 0,
  0, 0, E, E, 0, E, E, 0, 0, 0,
  0, 0, X, E, X, 0, B, 0, 0, 0,
  0, 0, S, E, E, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, E, E, E, 0, 0, 0, 0,
  0, E, E, E, B, E, 0, 0, 0, 0,
  0, B, E, B, S, 0, 0, 0, 0, 0,
  E, E, 0, 0, 0, 0, X, X, 0, 0,
  X, E, E, E, E, E, E, E, E, 0,
  0, E, E, 0, 0, 0, E, E, E, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, 0, E, S, 0, 0, 0,
  0, 0, E, E, E,XB, E, 0, 0, 0,
  0, 0, X, B, E, 0, 0, 0, 0, 0,
  0, 0, X, E, 0, 0, E, E, 0, 0,
  0, 0, E, B, E, E, E, E, 0, 0,
  0, 0, X, E, E, B, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, E, X,XB, S, 0, 0,
  0, 0, E, E, B, E, B, E, 0, 0,
  0, 0, 0, E, X, 0, 0, 0, 0, 0,
  0, 0, E, B, 0, E, E, E, 0, 0,
  0, 0, E, E, E, E, E, E, 0, 0,
  0, 0, 0, E, E, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, X, E, 0, E, E, E, 0, 0,
  0, 0, X, E, 0, S, B, E, 0, 0,
  0, 0, X, B, 0, 0, E, 0, 0, 0,
  0, 0, E, B, E, 0, E, E, 0, 0,
  0, 0, E, E, E, E, E, E, 0, 0,
  0, 0, X, B, 0, 0, E, E, 0, 0,
  0, 0, E, E, E, E, 0, 0, 0, 0,
  0, 0, E, 0, E, E, 0, 0, 0, 0,
  0, 0, E, E, E, E, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, X, E, E, X, E, X, 0, 0,
  0, 0, E, E, B, E, B, E, 0, 0,
  0, 0, S,XB, E, 0, E, 0, 0, 0,
  0, 0, 0, 0, 0, E, B, E, 0, 0,
  0, 0, E, E, E, E, E, E, 0, 0,
  0, 0, E, E, E, 0, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  X, E, E, E, 0, E, E, E, E, 0,
  E, B, E, B, S, B, E, 0, E, 0,
  0, B, E, E, 0, E, E, E, E, 0,
  E, X, 0, 0, 0, 0, E, E, X, 0,
  E, E, E, E, E, 0, E, E, X, 0,
  0, E, E, E, E, 0, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, E, E, X, 0, 0, 0, 0, 0,
  0, 0, E, 0, X, 0, 0, 0, 0, 0,
  0, 0, E, E, E, 0, 0, 0, 0, 0,
  0, 0, S, B, E, 0, 0, 0, 0, 0,
  0, 0, 0, E, 0, 0, E, E, 0, 0,
  0, 0, X, E, E, 0, E, E, 0, 0,
  0, 0, E, E, E, 0, E, E, 0, 0,
  0, 0, E, B, B, E, E, E, 0, 0,
  0, 0, E, E, 0, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, E, X, E, X, 0, 0,
  0, 0, S, E, B, E, B, B, 0, 0,
  0, 0, 0, 0, 0, 0, B, X, 0, 0,
  0, 0, E, E, E, E, E, E, 0, 0,
  0, 0, E, 0, E, E, 0, E, 0, 0,
  0, 0, E, E, E, E, E, X, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  E, E, 0, E, E, 0, E, X, E, 0,
  E, E, 0, E, E, E, E, E, E, 0,
  E, E, 0, E, E, 0, E, X, X, 0,
  0, B, E, 0, E, E, 0, E, 0, 0,
  E, E, B, S, B, E, E, E, 0, 0,
  E, E, E, 0, E, E, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, E, 0, 0, 0, 0, 0,
  0, 0, E, 0, E, B, S, 0, 0, 0,
  0, 0, E, E, E, B, B, 0, 0, 0,
  0, 0, 0, E, 0, 0, E, E, 0, 0,
  0, 0, E, E, E, X, E, X, 0, 0,
  0, 0, E, X, E, E, E, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, E, E, E, 0, 0, 0, 0, 0,
  0, 0, E, 0, E, B, E, 0, 0, 0,
  0, 0, E, E, E, E, E, 0, 0, 0,
  0, 0, E, E, B, 0, X, X, 0, 0,
  0, 0, E, B, S, 0, E, X, 0, 0,
  0, 0, E, E, 0, 0, X, E, 0, 0,
  0, 0, E, E, 0, E, E, E, 0, 0,
  0, 0, E, B, 0, E, E, E, 0, 0,
  0, 0, E, E, 0, E, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, E, S, 0, 0, 0, 0, 0,
  0, 0, 0, B, B, X, E, 0, 0, 0,
  0, 0, 0, E, B, E, E, 0, 0, 0,
  0, 0, E, B, E, 0, E, E, 0, 0,
  0, 0, X, E, E, 0, E, E, 0, 0,
  0, 0, X, E, X, 0, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  E, E, X, X, E, 0, E, E, 0, 0,
  E, E, E, E, E, E, E, E, 0, 0,
  0, E, 0, X, E, 0, E, B, 0, 0,
  E, E, 0, 0, E, E, B, B, E, 0,
  E, E, 0, 0, X, E, B, E, E, 0,
  S, E, 0, 0, 0, 0, E, E, E, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, E, X, X, 0, 0, 0,
  0, 0, 0, 0, E, E, X, 0, 0, 0,
  0, 0, 0, 0, E, E, 0, 0, 0, 0,
  0, 0, E, E, E, E, E, E, 0, 0,
  0, 0, E, B, B, E, B, E, 0, 0,
  0, 0, S, X, 0, 0, B, 0, 0, 0,
  0, 0, 0, 0, 0, E, E, 0, 0, 0,
  0, 0, 0, E, E, E, E, E, 0, 0,
  0, 0, 0, E, E, E, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, E, E, E, E,XB, E, 0, 0,
  0, 0, E, E, B, E, B, S, 0, 0,
  0, 0, E, B, 0, 0, E, 0, 0, 0,
  0, 0, E, E, 0, X, E, X, 0, 0,
  0, 0, E, E, 0, E, E, E, 0, 0,
  0, 0, E, E, 0, X, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, E, E, E, 0,
  X, X, E,XS, 0, E, B, E, E, 0,
  E, E, E, B, B, E, E, E, E, 0,
  0, 0, E,XB, E, E, 0, 0, 0, 0,
  E, E, E, E, 0, E, E, E, 0, 0,
  E, E, E, 0, 0, 0, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, E, E, 0, 0, E, E, 0, 0,
  0, 0, E, E, 0, E, E, E, 0, 0,
  0, 0, E, E, 0, E, B, E, 0, 0,
  0, 0, 0, X, 0, 0, E,XB, 0, 0,
  0, 0, E, E, E, B, E, E, 0, 0,
  0, 0, X, X, E, 0, E, 0, 0, 0,
  0, 0, 0, E, 0, 0, E, 0, 0, 0,
  0, 0, 0, S, B, E, E, E, 0, 0,
  0, 0, 0, 0, 0, E, E, E, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, E, E, E, E, 0, 0,
  0, E, B, E, B, E, E, B, S, 0,
  0, E, E, E, E, 0, B, E, X, 0,
  E, E, 0, E, E, E, E, E, E, 0,
  X, E, E, E, 0, E, E, 0, E, 0,
  X, E, X, 0, 0, 0, E, E, E, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Map codes (a code is presented for every map which can be entered by joystick movement after restart to directly reach the corresponding map)
const int codes[] PROGMEM = {
  1377,
  2459,
  2879,
  204,
  121,
  1125,
  378,
  873,
  1987,
  1324,
  1050,
  2317,
  719,
  2655,
  3059,
  2278,
  2184,
  1880,
  478,
  2722,
  3118,
  938,
  820,
  2653,
  1280,
  2856,
  728,
  1624,
  1066,
  39,
  306,
  2155,
  483,
  2743,
  1655,
  2759,
  796,
  3093,
  2491,
  1574,
  1052,
  839,
  2461,
  2121,
  1762,
  2376,
  1995,
  60,
  2532,
  1964
};

byte currentMap[100];   // Array to store the current map
byte lastStepMap[100];  // Array to store the current map before the last movement of a box
byte mapNumber;         // Number of current map
byte lastMapNumber;     // Number of last played map

byte numberOfBoxes;                           // Number of boxes in current map
byte numberOfStorageLocations;                // Number of storage locations in current map
byte numberOfBoxesOnStorageLocations;         // Number of boxes on storage locations in current map
byte numberOfBoxesOnStorageLocationsLastStep; // Number of boxes on storage locations in current map before last movement of a box

int steps;        // Counter for steps
long startTime;   // Start time at map start                               
long endTime;     // End time after map has been completed successfully                                 
long overallTime; // Calculated time after start and end which is presented to the player
                          
// Background effect
// A single dot is moved within a tile of 3 * 3 pixels with a specific movement vector. This tile is replicated to fill the whole screen.
float dotX, dotY, dotXNew, dotYNew, dotXVector, dotYVector = 0.0;
byte dotXScreen, dotYScreen, dotXScreenNew, dotYScreenNew;
int angle = 0;  // Movement angle
byte r = 2;     // Red color
byte g = 2;     // Green color
byte b = 2;     // Blue color
byte rNew, gNew, bNew; // Buffer for new color values (colors chnage continuously)
boolean fx = true; // If false, no effect is shown. The player can select this in the start menu.

// Orientation
#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3

// Player
byte playerX;                   // Current X position in the current map (10 * 10)
byte playerY;                   // Current Y position in the current map (10 * 10)
byte playerXNew;                // Proposed new X position (if movement is possible)
byte playerYNew;                // Proposed new Y position (if movement is possible)
byte playerOrientation;         // Current orientation (NORTH, EAST, SOUTH, WEST)
byte playerXLastStep;           // X position before last movement of a box (required for undo function)
byte playerYLastStep;           // Y position before last movement of a box (required for undo function)
byte playerOrientationLastStep; // Orientation before last movement of a box (required for undo function)

// Game synchronization
unsigned long engineLoopStartPoint; // Contains millisconds since start of LEDmePlay at begin of each engine loop

boolean undoPossible; // True if undo is possible
boolean rerun;        // True if rerun has been selected
boolean reset;        // True if reset has been selected
boolean mapCompleted; // True if map has been completed successfully

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

  // Logos
  mithotronic();
  ledMePlay();
  
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



// Helper procedures

// buttonFire1Clicked: Used for menu selection (triggers a sound effect)
boolean buttonFire1Clicked(int delayValue)
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
    delay(delayValue);  
    return true;
  }
}

// clickSound: Sound effect
void clickSound()
{
  // SOUND: Menu button clicked
  for(byte i = 0; i < 25; i++)
  {
    tone(audio, 100 + 50 * i, 2);
    delay(1);   
  }
}

// powInt: Calculated the power function since the inbuild method makes some strange rounding
int powInt(int base, int exponent)
{
  int exponentiation = 1;  
  for(int i = 0; i < exponent; i++)
  {
    exponentiation = exponentiation * base;
  }
  return exponentiation;
}



// Background effect

// updateBackgroundTile: Updates background tile (x, y) with the current dot position (Note: A tile has the size of 3 * 3 pixels and contains exactly one dot).
// A tile can have the positions (0 ... 11, 0 ... 11) while (1 ... 10, 1 ... 10) correspond to the position which can also contain map elements (10 * 10).
void updateBackgroundTile(byte x, byte y)
{
  boolean draw;

  // Remove old dot and draw new one for tiles at the border of the screen (only small parts are visible). Dots are only removed/drawn if they are not covered by the map.
  if(x == 0 || x == 11 || y == 0 || y == 11)
  {
    draw = true;
    if(x == 0 && y > 0 && getMapPosition(0, y - 1) != W){ draw = false; }
    if(x == 11 && y > 0 && getMapPosition(9, y - 1) != W){ draw = false; }
    if(y == 0 && x > 0 && getMapPosition(x - 1, 0) != W){ draw = false; }
    if(y == 11 && x > 0 && getMapPosition(x - 1, 9) != W){ draw = false; }
    if(x == 0 && y == 0 && getMapPosition(0, 0) != W){ draw = false; }
    if(x == 11 && y == 0 && getMapPosition(9, 0) != W){ draw = false; }
    if(x == 0 && y == 11 && getMapPosition(0, 9) != W){ draw = false; }
    if(x == 11 && y == 11 && getMapPosition(9, 9) != W){ draw = false; }

    if(x == 0 && y > 1 && getMapPosition(0, y - 2) != W && dotXScreen == 2 && dotYScreen == 0){ draw = false; }
    if(x == 0 && y > 0 && y < 10 && getMapPosition(0, y) != W && dotXScreen == 2 && dotYScreen == 2){ draw = false; }
    if(x == 11 && y > 1 && getMapPosition(9, y - 2) != W && dotXScreen == 0 && dotYScreen == 0){ draw = false; }
    if(x == 11 && y > 0 && y < 10 && getMapPosition(9, y) != W && dotXScreen == 0 && dotYScreen == 2){ draw = false; }
    if(y == 0 && x > 1 && getMapPosition(x - 2, 0) != W && dotXScreen == 0 && dotYScreen == 2){ draw = false; }
    if(y == 0 && x > 0 && x < 10 && getMapPosition(x, 0) != W && dotXScreen == 2 && dotYScreen == 2){ draw = false; }
    if(y == 11 && x > 1 && getMapPosition(x - 2, 9) != W && dotXScreen == 0 && dotYScreen == 0){ draw = false; }
    if(y == 11 && x > 0 && x < 10 && getMapPosition(x, 9) != W && dotXScreen == 2 && dotYScreen == 0){ draw = false; }
    if(draw)
    {
      // Remove old dot
      matrix.drawPixel((x * 3) - 2 + dotXScreen, (y * 3) - 2 + dotYScreen, matrix.Color333(0, 0, 0));
    }
    draw = true;
    if(x == 0 && y > 0 && getMapPosition(0, y - 1) != W){ draw = false; }
    if(x == 11 && y > 0 && getMapPosition(9, y - 1) != W){ draw = false; }
    if(y == 0 && x > 0 && getMapPosition(x - 1, 0) != W){ draw = false; }
    if(y == 11 && x > 0 && getMapPosition(x - 1, 9) != W){ draw = false; }
    if(x == 0 && y == 0 && getMapPosition(0, 0) != W){ draw = false; }
    if(x == 11 && y == 0 && getMapPosition(9, 0) != W){ draw = false; }
    if(x == 0 && y == 11 && getMapPosition(0, 9) != W){ draw = false; }
    if(x == 11 && y == 11 && getMapPosition(9, 9) != W){ draw = false; }

    if(x == 0 && y > 1 && getMapPosition(0, y - 2) != W && dotXScreenNew == 2 && dotYScreenNew == 0){ draw = false; }
    if(x == 0 && y > 0 && y < 10 && getMapPosition(0, y) != W && dotXScreenNew == 2 && dotYScreenNew == 2){ draw = false; }
    if(x == 11 && y > 1 && getMapPosition(9, y - 2) != W && dotXScreenNew == 0 && dotYScreenNew == 0){ draw = false; }
    if(x == 11 && y > 0 && y < 10 && getMapPosition(9, y) != W && dotXScreenNew == 0 && dotYScreenNew == 2){ draw = false; }
    if(y == 0 && x > 1 && getMapPosition(x - 2, 0) != W && dotXScreenNew == 0 && dotYScreenNew == 2){ draw = false; }
    if(y == 0 && x > 0 && x < 10 && getMapPosition(x, 0) != W && dotXScreenNew == 2 && dotYScreenNew == 2){ draw = false; }
    if(y == 11 && x > 1 && getMapPosition(x - 2, 9) != W && dotXScreenNew == 0 && dotYScreenNew == 0){ draw = false; }
    if(y == 11 && x > 0 && x < 10 && getMapPosition(x, 9) != W && dotXScreenNew == 2 && dotYScreenNew == 0){ draw = false; }
    if(draw)
    {
      // Draw new dot
      matrix.drawPixel((x * 3) - 2 + dotXScreenNew, (y * 3) - 2 + dotYScreenNew, matrix.Color444(r, g, b));
    }
  }
    
  // Remove old dot and draw new one for tiles which might be covered by the map. If dots are not covered, they are removed/drawn.
  if(x > 0 && x < 11 && y > 0 && y < 11 && getMapPosition(x - 1, y - 1) == W)
  { 
    draw = true;
    if(x > 1 && getMapPosition(x - 2, y - 1) != W && dotXScreen == 0){ draw = false; }
    if(x < 10 && getMapPosition(x, y - 1) != W && dotXScreen == 2){ draw = false; }
    if(y > 1 && getMapPosition(x - 1, y - 2) != W && dotYScreen == 0){ draw = false; }
    if(y < 10 && getMapPosition(x - 1, y) != W && dotYScreen == 2){ draw = false; }
    if(x > 1 && y > 1 && getMapPosition(x - 2, y - 2) != W && dotXScreen == 0 && dotYScreen == 0){ draw = false; }
    if(x < 10 && y > 1 && getMapPosition(x, y - 2) != W && dotXScreen == 2 && dotYScreen == 0){ draw = false; }
    if(x > 1 && y < 10 && getMapPosition(x - 2, y) != W && dotXScreen == 0 && dotYScreen == 2){ draw = false; }
    if(x < 10 && y < 10 && getMapPosition(x, y) != W && dotXScreen == 2 && dotYScreen == 2){ draw = false; }        
    if(draw)
    {
      matrix.drawPixel((x * 3) - 2 + dotXScreen, (y * 3) - 2 + dotYScreen, matrix.Color333(0, 0, 0));      
    }
    draw = true;
    if(x > 1 && getMapPosition(x - 2, y - 1) != W && dotXScreenNew == 0){ draw = false; }
    if(x < 10 && getMapPosition(x, y - 1) != W && dotXScreenNew == 2){ draw = false; }
    if(y > 1 && getMapPosition(x - 1, y - 2) != W && dotYScreenNew == 0){ draw = false; }
    if(y < 10 && getMapPosition(x - 1, y) != W && dotYScreenNew == 2){ draw = false; }
    if(x > 1 && y > 1 && getMapPosition(x - 2, y - 2) != W && dotXScreenNew == 0 && dotYScreenNew == 0){ draw = false; }
    if(x < 10 && y > 1 && getMapPosition(x, y - 2) != W && dotXScreenNew == 2 && dotYScreenNew == 0){ draw = false; }
    if(x > 1 && y < 10 && getMapPosition(x - 2, y) != W && dotXScreenNew == 0 && dotYScreenNew == 2){ draw = false; }
    if(x < 10 && y < 10 && getMapPosition(x, y) != W && dotXScreenNew == 2 && dotYScreenNew == 2){ draw = false; }        
    if(draw)
    {
      matrix.drawPixel((x * 3) - 2 + dotXScreenNew, (y * 3) - 2 + dotYScreenNew, matrix.Color444(r, g, b));
    }
  }
}

// moveBackground: Update position and color of dot in background tiles
void moveBackground()
{
  // Change dot color randomly
  if(random(100) < 10)
  {
    switch(random(3))
    {
      case 0:
        rNew = r + random(3) - 1;        
        if(rNew > 15){ rNew = 0; }
        break;
      case 1:
        gNew = g + random(3) - 1;
        if(gNew > 15){ gNew = 0; }
        break;
      case 2:
        bNew = b + random(3) - 1;
        if(bNew > 15){ bNew = 0; }
        break;
    }
    if((rNew + gNew + bNew) > 4 && (rNew + gNew + bNew) < 10)
    {
      r = rNew;
      g = gNew;
      b = bNew;
    }
  }
  
  // Change movement angle randomly
  if(random(1000) < 2)
  {
    angle = angle + random(91) - 45;
    if(angle < 0){ angle = angle + 360; }
    else if(angle > 360){ angle = angle - 360; }
    dotXVector = sin(angle * 3.14159 / 180.0) / 50.0;
    dotYVector = cos(angle * 3.14159 / 180.0) / 50.0;
  }

  // Calculate new dot position
  dotXNew = dotX + dotXVector;
  dotYNew = dotY + dotYVector;
  if(dotXNew < 0.0){ dotXNew = dotXNew + 3.0; }
  else if (dotXNew > 3.0){ dotXNew = dotXNew - 3.0; }
  if(dotYNew < 0.0){ dotYNew = dotYNew + 3.0; }
  else if (dotYNew > 3.0){ dotYNew = dotYNew - 3.0; }

  dotXScreen = byte(dotX);
  dotYScreen = byte(dotY);
  dotXScreenNew = byte(dotXNew);
  dotYScreenNew = byte(dotYNew);

  // Map dot position to every tile
  if(dotXScreen != dotXScreenNew || dotYScreen != dotYScreenNew)
  {
    for(byte y = 0; y < 12; y++)
    {
      for(byte x = 0; x < 12; x++)
      {
        updateBackgroundTile(x, y);
      }      
    }
  }

  // Update dot position for next iteration
  dotX = dotXNew;
  dotY = dotYNew;
}



// Procedures to initialize and draw the map and to draw the player

// initializeMap: Initialize map with mapNumber (0 ... 49) as the current map
void initializeMap(byte mapNumber)
{
  numberOfBoxes = 0;
  numberOfStorageLocations = 0;
  numberOfBoxesOnStorageLocations = 0;
  
  // Reset map
  for(byte i = 0; i < 100; i++)
  {
    currentMap[i] = 0;  
  }

  // Copy map from progmem
  byte j;
  for(byte i = 0; i < 100; i++)
  {
    j = byte(pgm_read_byte_near(maps + i + (mapNumber * 100)));
    if(j == B){ numberOfBoxes++; }
    if(j == X){ numberOfStorageLocations++; }
    if(j == XB){ numberOfStorageLocations++; numberOfBoxes++; numberOfBoxesOnStorageLocations++; }
    if(j == XS){ numberOfStorageLocations++; }
    if(j != S && j != XS)
    {
      currentMap[i] = j;
    }
    else if(j == S)
    {
      playerX = i % 10;
      playerY = i / 10;
      playerXNew = playerX;
      playerYNew = playerY;
      playerOrientation = NORTH;
      currentMap[i] = E;      
    }
    else if(j == XS)
    {
      playerX = i % 10;
      playerY = i / 10;
      playerXNew = playerX;
      playerYNew = playerY;
      playerOrientation = NORTH;
      currentMap[i] = X;      
    } 
  }
}

// getMapPosition: Returns map position (x, y)
byte getMapPosition(byte x, byte y)
{
  if(x > 9 || y > 9){ return 0; }
  return currentMap[(y * 10) + x];
}

// setMapPosition: Sets map position (x, y) with value
boolean setMapPosition(byte x, byte y, byte value)
{
  if(x > 9 || y > 9){ return false; }
  currentMap[(y * 10) + x] = value;
  return true;
}

// drawBox: Draw a box. (x, y) is an absoluet pixel position. If draw == false, the box is removed.
void drawBox(byte x, byte y, boolean draw)
{
  if(draw)
  {
    matrix.drawPixel(x, y, matrix.Color333(4, 3, 1)); 
    matrix.drawPixel(x + 1, y, matrix.Color333(4, 2, 1)); 
    matrix.drawPixel(x + 2, y, matrix.Color333(4, 3, 1)); 
    matrix.drawPixel(x, y + 1, matrix.Color333(4, 2, 1)); 
    matrix.drawPixel(x + 2, y + 1, matrix.Color333(4, 3, 1)); 
    matrix.drawPixel(x, y + 2, matrix.Color333(4, 2, 1)); 
    matrix.drawPixel(x + 1, y + 2, matrix.Color333(4, 3, 1)); 
    matrix.drawPixel(x + 2, y + 2, matrix.Color333(4, 2, 1));
  }
  else
  {
    matrix.drawRect(x, y, 3, 3, matrix.Color333(0, 0, 0));     
  }
}

// drawStorageLocation: Draw an 'X'. (x, y) is an absolute pixel position
void drawStorageLocation(byte x, byte y)
{
  matrix.drawPixel(x, y, matrix.Color333(1, 1, 5));
  matrix.drawPixel(x + 2, y, matrix.Color333(1, 1, 5));
  matrix.drawPixel(x + 1, y + 1, matrix.Color333(0, 1, 5));
  matrix.drawPixel(x, y + 2, matrix.Color333(1, 1, 5));
  matrix.drawPixel(x + 2, y + 2, matrix.Color333(1, 1, 5));
}

// drawMapPosition: Draws map position (x, y) on screen
void drawMapPosition(byte x, byte y)
{
  if(getMapPosition(x, y) != W)
  {
    if(x == 0){ matrix.drawLine(0, (y * 3), 0, (y * 3) + 4, matrix.Color333(2, 2, 2)); }
    if(x == 9){ matrix.drawLine(31, (y * 3), 31, (y * 3) + 4, matrix.Color333(2, 2, 2)); }
    if(y == 0){ matrix.drawLine((x * 3), 0, (x * 3) + 4, 0, matrix.Color333(2, 2, 2)); }
    if(y == 9){ matrix.drawLine((x * 3), 31, (x * 3) + 4, 31, matrix.Color333(2, 2, 2)); }
  }
  else if(getMapPosition(x, y) == W)
  {
    if(x > 0 && getMapPosition(x - 1, y) != W){ matrix.drawLine((x * 3) + 1, (y * 3) + 1, (x * 3) + 1, (y * 3) + 3, matrix.Color333(2, 2, 2)); }
    if(x < 9 && getMapPosition(x + 1, y) != W){ matrix.drawLine((x * 3) + 3, (y * 3) + 1, (x * 3) + 3, (y * 3) + 3, matrix.Color333(2, 2, 2)); }
    if(y > 0 && getMapPosition(x, y - 1) != W){ matrix.drawLine((x * 3) + 1, (y * 3) + 1, (x * 3) + 3, (y * 3) + 1, matrix.Color333(2, 2, 2)); }
    if(y < 9 && getMapPosition(x, y + 1) != W){ matrix.drawLine((x * 3) + 1, (y * 3) + 3, (x * 3) + 3, (y * 3) + 3, matrix.Color333(2, 2, 2)); }
    if(x > 0 && y > 0 && getMapPosition(x - 1, y - 1) != W){ matrix.drawPixel((x * 3) + 1, (y * 3) + 1, matrix.Color333(2, 2, 2)); }
    if(x < 9 && y > 0 && getMapPosition(x + 1, y - 1) != W){ matrix.drawPixel((x * 3) + 3, (y * 3) + 1, matrix.Color333(2, 2, 2)); }
    if(x > 0 && y < 9 && getMapPosition(x - 1, y + 1) != W){ matrix.drawPixel((x * 3) + 1, (y * 3) + 3, matrix.Color333(2, 2, 2)); }
    if(x < 9 && y < 9 && getMapPosition(x + 1, y + 1) != W){ matrix.drawPixel((x * 3) + 3, (y * 3) + 3, matrix.Color333(2, 2, 2)); }
  }
  if(getMapPosition(x, y) == X)
  {
    drawStorageLocation((x * 3) + 1, (y * 3) + 1);
  }
  else if(getMapPosition(x, y) == B)
  {
    drawBox((x * 3) + 1, (y * 3) + 1, true);
  }
  else if(getMapPosition(x, y) == XB)
  {
    drawStorageLocation((x * 3) + 1, (y * 3) + 1);
    drawBox((x * 3) + 1, (y * 3) + 1, true);
  }
}

// drawCurrentMap: Draws the complete current map
void drawCurrentMap()
{
  for(byte y = 0; y < 10; y++)
  {
    for(byte x = 0; x < 10; x++)
    {
      drawMapPosition(x, y);
    }      
  }
}

// drawPlayer: Draw player at the current orientation and position with offset (0 ... 2) to realize movement animations. If draw == false, the player is removed.
void drawPlayer(byte offset, boolean draw)
{
  if(draw)
  {
    if(playerOrientation == NORTH)
    {
      matrix.drawPixel((playerX * 3) + 1, (playerY * 3) + 1 - offset, matrix.Color333(3, 0, 0));
      matrix.drawPixel((playerX * 3) + 3, (playerY * 3) + 1 - offset, matrix.Color333(3, 0, 0));    
      matrix.drawPixel((playerX * 3) + 2, (playerY * 3) + 2 - offset, matrix.Color333(0, 4, 0));
      matrix.drawPixel((playerX * 3) + 1, (playerY * 3) + 2 - offset, matrix.Color333(0, 2, 0));
      matrix.drawPixel((playerX * 3) + 3, (playerY * 3) + 2 - offset, matrix.Color333(0, 2, 0));
      matrix.drawPixel((playerX * 3) + 2, (playerY * 3) + 3 - offset, matrix.Color333(0, 3, 0));
    }
    else if(playerOrientation == EAST)
    {
      matrix.drawPixel((playerX * 3) + 3 + offset, (playerY * 3) + 1, matrix.Color333(3, 0, 0));
      matrix.drawPixel((playerX * 3) + 3 + offset, (playerY * 3) + 3, matrix.Color333(3, 0, 0));    
      matrix.drawPixel((playerX * 3) + 2 + offset, (playerY * 3) + 2, matrix.Color333(0, 4, 0));
      matrix.drawPixel((playerX * 3) + 2 + offset, (playerY * 3) + 1, matrix.Color333(0, 2, 0));
      matrix.drawPixel((playerX * 3) + 2 + offset, (playerY * 3) + 3, matrix.Color333(0, 2, 0));
      matrix.drawPixel((playerX * 3) + 1 + offset, (playerY * 3) + 2, matrix.Color333(0, 3, 0));    
    }
    else if(playerOrientation == SOUTH)
    {
      matrix.drawPixel((playerX * 3) + 3, (playerY * 3) + 3 + offset, matrix.Color333(3, 0, 0));
      matrix.drawPixel((playerX * 3) + 1, (playerY * 3) + 3 + offset, matrix.Color333(3, 0, 0));    
      matrix.drawPixel((playerX * 3) + 2, (playerY * 3) + 2 + offset, matrix.Color333(0, 4, 0));
      matrix.drawPixel((playerX * 3) + 1, (playerY * 3) + 2 + offset, matrix.Color333(0, 2, 0));
      matrix.drawPixel((playerX * 3) + 3, (playerY * 3) + 2 + offset, matrix.Color333(0, 2, 0));
      matrix.drawPixel((playerX * 3) + 2, (playerY * 3) + 1 + offset, matrix.Color333(0, 3, 0));    
    }
    else if(playerOrientation == WEST)
    {
      matrix.drawPixel((playerX * 3) + 1 - offset, (playerY * 3) + 1, matrix.Color333(3, 0, 0));
      matrix.drawPixel((playerX * 3) + 1 - offset, (playerY * 3) + 3, matrix.Color333(3, 0, 0));    
      matrix.drawPixel((playerX * 3) + 2 - offset, (playerY * 3) + 2, matrix.Color333(0, 4, 0));
      matrix.drawPixel((playerX * 3) + 2 - offset, (playerY * 3) + 1, matrix.Color333(0, 2, 0));
      matrix.drawPixel((playerX * 3) + 2 - offset, (playerY * 3) + 3, matrix.Color333(0, 2, 0));
      matrix.drawPixel((playerX * 3) + 3 - offset, (playerY * 3) + 2, matrix.Color333(0, 3, 0));    
    }
  }
  else
  {
    if(playerOrientation == NORTH)
    {
      matrix.fillRect((playerX * 3) + 1, (playerY * 3) + 1 - offset, 3, 3, matrix.Color333(0, 0, 0));
    }
    else if(playerOrientation == EAST)
    {    
      matrix.fillRect((playerX * 3) + 1 + offset, (playerY * 3) + 1, 3, 3, matrix.Color333(0, 0, 0));
    }
    else if(playerOrientation == SOUTH)
    {    
      matrix.fillRect((playerX * 3) + 1, (playerY * 3) + 1 + offset, 3, 3, matrix.Color333(0, 0, 0));
    }
    else if(playerOrientation == WEST)
    {    
      matrix.fillRect((playerX * 3) + 1 - offset, (playerY * 3) + 1, 3, 3, matrix.Color333(0, 0, 0));
    }
  }
}

// drawBoxMovedByPlayer: Draws box in front of the current player position with offset (0 ... 2) to realize movement animations.
void drawBoxMovedByPlayer(byte offset, boolean draw)
{
  if(draw)
  {
    if(playerOrientation == NORTH)
    {
      drawBox((playerX * 3) + 1, (playerY * 3) + 1 - 3 - offset, true);
    }
    else if(playerOrientation == EAST)
    {        
      drawBox((playerX * 3) + 1 + 3 + offset, (playerY * 3) + 1, true);
    }
    else if(playerOrientation == SOUTH)
    {        
      drawBox((playerX * 3) + 1, (playerY * 3) + 1 + 3 + offset, true);
    }
    else if(playerOrientation == WEST)
    {        
      drawBox((playerX * 3) + 1 - 3 - offset, (playerY * 3) + 1, true);
    }
  }
  else
  {
    if(playerOrientation == NORTH)
    {
      drawBox((playerX * 3) + 1, (playerY * 3) + 1 - 3 - offset, false);
    }
    else if(playerOrientation == EAST)
    {        
      drawBox((playerX * 3) + 1 + 3 + offset, (playerY * 3) + 1, false);
    }
    else if(playerOrientation == SOUTH)
    {        
      drawBox((playerX * 3) + 1, (playerY * 3) + 1 + 3 + offset, false);
    }
    else if(playerOrientation == WEST)
    {        
      drawBox((playerX * 3) + 1 - 3 - offset, (playerY * 3) + 1, false);
    }
  }
}



// Title screen, menus, and other screens

// title: Title screen of the game
void title()
{
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Text
  matrix.setTextColor(matrix.Color333(1, 1, 3));
  matrix.setCursor(4, 1);
  matrix.println("WARE");
  matrix.setCursor(1, 9);
  matrix.println("HOUSE");

  // Box
  matrix.drawLine(7, 22, 15, 22, matrix.Color333(4, 2, 1));
  matrix.drawLine(7, 30, 15, 30, matrix.Color333(4, 2, 1));
  matrix.drawLine(7, 23, 7, 29, matrix.Color333(4, 3, 1));
  matrix.drawLine(9, 23, 9, 29, matrix.Color333(4, 2, 1));
  matrix.drawLine(11, 23, 11, 29, matrix.Color333(4, 3, 1));
  matrix.drawLine(13, 23, 13, 29, matrix.Color333(4, 2, 1));
  matrix.drawLine(15, 23, 15, 29, matrix.Color333(4, 3, 1));

  // Robot
  // Body
  matrix.fillRect(22, 22, 2, 4, matrix.Color333(0, 1, 0));
  // Hand
  matrix.drawLine(16, 22, 16, 25, matrix.Color333(3, 0, 0));
  matrix.drawLine(17, 23, 17, 24, matrix.Color333(1, 0, 0));
  // Leg
  matrix.drawLine(18, 30, 21, 30, matrix.Color333(0, 5, 0));
  matrix.drawLine(19, 29, 24, 24, matrix.Color333(0, 5, 0));
  matrix.drawLine(20, 29, 24, 25, matrix.Color333(0, 1, 0));
  matrix.drawLine(21, 29, 22, 28, matrix.Color333(0, 3, 0));
  // Arms
  matrix.drawLine(18, 23, 20, 23, matrix.Color333(0, 5, 0));
  matrix.drawLine(18, 24, 20, 24, matrix.Color333(0, 3, 0));
  matrix.drawLine(21, 22, 23, 20, matrix.Color333(0, 5, 0));
  matrix.drawLine(21, 23, 23, 21, matrix.Color333(0, 3, 0));
  // Head
  matrix.fillRect(20, 18, 5, 2, matrix.Color333(0, 1, 0));
  matrix.drawLine(18, 19, 19, 18, matrix.Color333(0, 0, 3));
  matrix.drawLine(19, 19, 20, 18, matrix.Color333(0, 0, 1));
  matrix.drawLine(20, 17, 23, 17, matrix.Color333(0, 3, 0));
  matrix.drawLine(23, 20, 18, 20, matrix.Color333(0, 1, 0));
  matrix.drawLine(19, 21, 21, 21, matrix.Color333(0, 3, 0));
  matrix.drawPixel(18, 20, matrix.Color333(0, 3, 0));

  // Ground
  matrix.drawLine(0, 31, 31, 31, matrix.Color333(1, 1, 1));
  
  do
  {    
  }
  while(!buttonFire1Clicked(200));
}

// showMapNumber: Shows the number of the next map and the corresponding map code (not for the first map)
void showMapNumber()
{
  int code;
  int divider;
  
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Text
  matrix.setTextColor(matrix.Color333(1, 1, 3));
  matrix.setCursor(7, 0);
  matrix.println("Map");
  if(mapNumber < 9)
  {
    matrix.setCursor(13, 8);
  }
  else if(mapNumber < 99)
  {
    matrix.setCursor(10, 8);    
  }
  else
  {
    matrix.setCursor(7, 8);        
  }
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.println(mapNumber + 1);    

  if(mapNumber != 0)
  {
    matrix.setTextColor(matrix.Color333(1, 1, 3));
    matrix.setCursor(4, 16);
    matrix.println("Code");

    code = pgm_read_word(codes + mapNumber);
    
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.setCursor(1, 24);
    for(int i = 0; i < 5; i++)
    {
      divider = code / pow(5, i);
      divider = divider % 5;
      if(divider == 0)     { matrix.print("U"); }
      else if(divider == 1){ matrix.print("D"); }
      else if(divider == 2){ matrix.print("L"); }
      else if(divider == 3){ matrix.print("R"); }
      else if(divider == 4){ matrix.print("F"); }
    }
  }

  do
  {    
  }
  while(!buttonFire1Clicked(200));  
}

// startMenu: Start menu to select a new game, proceed in the last used map, enter a code for another map or toggle background effect (on/off)
void startMenu()
{
  byte selector = 0;
  byte updateMenu = true;
  boolean next = false;
  boolean joystickReleased = true;

  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  do
  {
    if(updateMenu)
    {
      // Update menu if the selection has changed
      if(selector == 0){ matrix.setTextColor(matrix.Color333(1, 1, 4)); }
      else{ matrix.setTextColor(matrix.Color333(1, 1, 1)); }
      matrix.setCursor(7, 0);
      matrix.println("New");
      if(selector == 1){ matrix.setTextColor(matrix.Color333(1, 1, 4)); }
      else{ matrix.setTextColor(matrix.Color333(1, 1, 1)); }
      matrix.setCursor(4, 8);
      matrix.println("Last");
      if(selector == 2){ matrix.setTextColor(matrix.Color333(1, 1, 4)); }
      else{ matrix.setTextColor(matrix.Color333(1, 1, 1)); }
      matrix.setCursor(4, 16);
      matrix.println("Code");
      if(selector == 3){ matrix.setTextColor(matrix.Color333(1, 1, 4)); }
      else{ matrix.setTextColor(matrix.Color333(1, 1, 1)); }
      matrix.setCursor(1, 24);
      matrix.println("FX");
      if(fx)
      {
        matrix.setTextColor(matrix.Color333(1, 1, 1));
        matrix.setCursor(19, 24);
        matrix.println("on");
      }
      else
      {
        matrix.setTextColor(matrix.Color333(1, 1, 1));
        matrix.setCursor(13, 24);
        matrix.println("off");        
      }
      updateMenu = false;  
      delay(100);    
    }

    // Make sure that the joystick has been released between movement events
    if(!joy1Up() && !joy1Down() && !joy1Fire()){ joystickReleased = true; delay(100); }
 
    if(joy1Up() && selector > 0 && joystickReleased)
    {
      selector--;
      updateMenu = true;
      joystickReleased = false;
    }    
    else if(joy1Down() && selector < 3 && joystickReleased)
    {
      selector++;
      updateMenu = true;
      joystickReleased = false;
    }
    else if(buttonFire1Clicked(200))
    {
      if(selector == 0)
      { 
        // Start a new game with the first map
        mapNumber = 0;
        next = true;
      }
      else if(selector == 1)
      { 
        // Jump to the last map which has been played
        mapNumber = lastMapNumber;
        next = true;
      }
      else if(selector == 2)
      { 
        // Selection to enter a map code
        mapNumber = enterCode();
        if(mapNumber != 0){ next = true; } // If the code not exists, a value of 0 is returned. In this case, the game is not started.
        else{ updateMenu = true; }
        matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
      }               
      else if(selector == 3)
      { 
        // Toggle between background effects (on/off)
        if(fx){ fx = false; updateMenu = true; }
        else{ fx = true; updateMenu = true; }
        matrix.fillRect(13, 24, 18, 8, matrix.Color333(0, 0, 0));
      }               
    }
  }
  while(!next);
}

// enterCode: The player enters a map code and can, therefore, directly jump to the corresponding map.
// The map code is a 5-letter code of length 5 for the different joystick actions: "U" == Up, "D" == Down, "L" == Left, "R" == Right, "F" == Fire.
// The entered code is translated into an int number (0 ... 3124).
int enterCode()
{
  boolean joystickReleased = true;
  int code = 0;
  byte currentCodeLength = 0;
  mapNumber = 0;

  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  matrix.setTextColor(matrix.Color333(1, 1, 4));
  matrix.setCursor(1, 0);
  matrix.println("Enter");
  matrix.setCursor(4, 8);
  matrix.println("Code");
  
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.setCursor(1, 16); // Jump to position to enter code
      
  do
  {
    // Make sure that the joystick has been released between movement events
    if(!joy1Up() && !joy1Down() && !joy1Left() && !joy1Right()&& !joy1Fire()){ joystickReleased = true; delay(100); }
    
    if(joy1Up() && joystickReleased)
    {
      matrix.print("U");
      joystickReleased = false;
      code = code + 0;
      currentCodeLength++;
      clickSound();
      delay(100);
    }
    if(joy1Down() && joystickReleased)
    {
      matrix.print("D");
      joystickReleased = false;
      code = code + powInt(5, currentCodeLength) * 1;
      currentCodeLength++;
      clickSound();
      delay(100);
    }
    if(joy1Left() && joystickReleased)
    {
      matrix.print("L");
      joystickReleased = false;
      code = code + powInt(5, currentCodeLength) * 2;
      currentCodeLength++;
      clickSound();
      delay(100);
    }
    if(joy1Right() && joystickReleased)
    {
      matrix.print("R");
      joystickReleased = false;
      code = code + powInt(5, currentCodeLength) * 3;
      currentCodeLength++;
      clickSound();
      delay(100);
    }
    if(joy1Fire() && joystickReleased)
    {
      matrix.print("F");
      joystickReleased = false;
      code = code + powInt(5, currentCodeLength) * 4;
      currentCodeLength++;
      clickSound();
      delay(100);
    }
  }
  while(currentCodeLength < 5);
  delay(1000);

  // Check if the entered code corresponds to a map
  for(int i = 0; i < numberOfMaps; i++)
  {
    if(code == pgm_read_word(codes + i)){ mapNumber = i; }
  }

  matrix.setTextColor(matrix.Color333(1, 1, 4));
  matrix.setCursor(1, 24);  
  if(mapNumber != 0){ matrix.println("Right"); }
  else{               matrix.println("Wrong");}
  
  delay(1000);
  
  return mapNumber;
}

// gameMenu: Menu within game to perform an undo, restart the current map or return to title screen
void gameMenu()
{
  byte selector = 0;
  byte updateMenu = true;
  boolean next = false;
  boolean joystickReleased = true;

  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  do
  {
    if(updateMenu)
    {
      // Update menu if the selection has changed
      if(selector == 0){ matrix.setTextColor(matrix.Color333(1, 1, 4)); }
      else{ matrix.setTextColor(matrix.Color333(1, 1, 1)); }
      matrix.setCursor(4, 0);
      matrix.println("Back");
      if(selector == 1){ matrix.setTextColor(matrix.Color333(1, 1, 4)); }
      else{ matrix.setTextColor(matrix.Color333(1, 1, 1)); }
      matrix.setCursor(4, 8);
      matrix.println("Undo");
      if(selector == 2){ matrix.setTextColor(matrix.Color333(1, 1, 4)); }
      else{ matrix.setTextColor(matrix.Color333(1, 1, 1)); }
      matrix.setCursor(1, 16);
      matrix.println("Rerun");
      if(selector == 3){ matrix.setTextColor(matrix.Color333(1, 1, 4)); }
      else{ matrix.setTextColor(matrix.Color333(1, 1, 1)); }
      matrix.setCursor(4, 24);
      matrix.println("Exit");
      updateMenu = false;  
      delay(100);    
    }
    
    // Make sure that the joystick has been released between movement events
    if(!joy1Up() && !joy1Down() && !joy1Fire()){ joystickReleased = true; delay(100); }
 
    if(joy1Up() && selector > 0 && joystickReleased)
    {
      selector--;
      updateMenu = true;
      joystickReleased = false;
    }    
    else if(joy1Down() && selector < 3 && joystickReleased)
    {
      selector++;
      updateMenu = true;
      joystickReleased = false;
    }
    else if(buttonFire1Clicked(200))
    {
      if(selector == 0)
      { 
        // Go back to game
        next = true;
      }
      else if(selector == 1)
      { 
        // Undo of the last step in which a box has been moved
        if(!undoPossible)
        {
          // SOUND: Undo not possible (already done or start position)
          tone(audio, 100, 50);
          delay(50);
          joystickReleased = false;
        }
        else
        {
          // Undo functionality;
               
          // Copy old values back
          playerX = playerXLastStep;
          playerY = playerYLastStep;
          playerXNew = playerXLastStep;
          playerYNew = playerYLastStep;
          playerOrientation = playerOrientationLastStep;
          numberOfBoxesOnStorageLocations = numberOfBoxesOnStorageLocationsLastStep;
          for(byte i = 0; i < 100; i++)
          {
            currentMap[i] = lastStepMap[i];
          }
          next = true;
          undoPossible = false;
         }
      }
      else if(selector == 2)
      { 
        // Restart the current map
        rerun = true;
        next = true;
      }
      else if(selector == 3)
      { 
        // Go back to title (reset)
        reset = true;
        next = true;
      }      
    }
  }
  while(!next);

  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
}

// showResults: Shows the number of steps required for the finished map and the corresponding time
void showResults()
{
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Text
  matrix.setTextColor(matrix.Color333(1, 1, 3));
  matrix.setCursor(1, 0);
  matrix.println("Steps");
  if(steps < 10)
  {
    matrix.setCursor(13, 8);
  }
  else if(steps < 100)
  {
    matrix.setCursor(10, 8);    
  }
  else if(steps < 1000)
  {
    matrix.setCursor(7, 8);    
  }
  else
  {
    matrix.setCursor(3, 8);        
  }
  matrix.setTextColor(matrix.Color333(1, 1, 1));
  matrix.println(steps);    

  matrix.setTextColor(matrix.Color333(1, 1, 3));
  matrix.setCursor(4, 16);
  matrix.println("Time");
  int minutes = (overallTime / 1000) / 60;
  int seconds = (overallTime / 1000) % 60;
  if(minutes < 60)
  {
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.setCursor(3, 24);
    if(minutes < 10){ matrix.print("0"); }
    matrix.print(minutes);
    matrix.print(":");
    matrix.drawPixel(15, 26, matrix.Color333(1, 1, 1));
    matrix.drawPixel(15, 28, matrix.Color333(1, 1, 1));
    matrix.setCursor(17, 24);
    if(seconds < 10){ matrix.print("0"); }
    matrix.print(seconds);
  }
  else
  {
    matrix.setTextColor(matrix.Color333(1, 1, 1));
    matrix.setCursor(3, 24);
    matrix.print("XX");    
    matrix.drawPixel(15, 26, matrix.Color333(1, 1, 1));
    matrix.drawPixel(15, 28, matrix.Color333(1, 1, 1));
    matrix.setCursor(17, 24);
    matrix.print("XX");    
  }
   
  do
  {    
  }
  while(!buttonFire1Clicked(200));  
}



// Game loop

// loop: Main loop of the game
void loop()
{
  // Show title screen
  title();

  // Show start menu
  startMenu();

  do
  {
    // Show map number
    showMapNumber();

    // Clear screen
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

    // Initialize map and draw it
    initializeMap(mapNumber);
    drawCurrentMap();
    drawPlayer(0, true);

    // Initialize required variables
    lastMapNumber = mapNumber;
    steps = 0;    
    playerXNew = playerX;
    playerYNew = playerY;

    // Reset undo function
    playerXLastStep = playerX;
    playerYLastStep = playerY;
    playerOrientationLastStep = playerOrientation;
    numberOfBoxesOnStorageLocationsLastStep = numberOfBoxesOnStorageLocations;
    for(byte i = 0; i < 100; i++)
    {
      lastStepMap[i] = currentMap[i];
    }
     
    undoPossible = false;
    rerun = false;
    reset = false;
    mapCompleted = false;
  
    int movementCounter = 0;
    byte stepCounter = 0;
    byte updateMap = false;
    boolean moveBox = false;
    boolean moveBoxOnStorageLocation = false;
    boolean footsteps = 0;

    // Start to measure time
    startTime = millis();

    // Draw background initially
    if(fx){ moveBackground(); }

    do
    {
      // Get time of engine loop start point (important for a constant game speed)
      engineLoopStartPoint = millis();

      // Player movement if joystick is moved (movement counter == 0 to sync with the background, stepCounter == 0 to make sure that the last movement animation has been completed)
      if(movementCounter == 0 && stepCounter == 0)
      {
        // Move up
        if(joy1Up() && playerOrientation != NORTH)
        {
          playerOrientation = NORTH;
          stepCounter = 1;
        }
        else if(joy1Up() && playerOrientation == NORTH)
        {
          if(playerY > 0 && (getMapPosition(playerX, playerY - 1) == E || getMapPosition(playerX, playerY - 1) == X))
          {
            playerYNew = playerY - 1;
            steps++;
            stepCounter = 3;
          }
          else if(playerY > 1 && (getMapPosition(playerX, playerY - 1) == B || getMapPosition(playerX, playerY - 1) == XB) && (getMapPosition(playerX, playerY - 2) == E || getMapPosition(playerX, playerY - 2) == X))
          {
            playerYNew = playerY - 1;
            steps++;
            stepCounter = 3;
            moveBox = true;
            if(getMapPosition(playerX, playerY - 2) == X){ moveBoxOnStorageLocation = true; }
          }
          else
          {
            // SOUND: Cannot move her
            tone(audio, 100, 50);
          }
        }
        // Move right
        else if(joy1Right() && playerOrientation != EAST)
        {
          playerOrientation = EAST;
          stepCounter = 1;
        }
        else if(joy1Right() && playerOrientation == EAST)
        {
          if(playerX < 9 && (getMapPosition(playerX + 1, playerY) == E || getMapPosition(playerX + 1, playerY) == X))
          {
            playerXNew = playerX + 1;
            steps++;
            stepCounter = 3;
          }
          else if(playerX < 8 && (getMapPosition(playerX + 1, playerY) == B || getMapPosition(playerX + 1, playerY) == XB) && (getMapPosition(playerX + 2, playerY) == E || getMapPosition(playerX + 2, playerY) == X))
          {
            playerXNew = playerX + 1;
            steps++;
            stepCounter = 3;
            moveBox = true;
            if(getMapPosition(playerX + 2, playerY) == X){ moveBoxOnStorageLocation = true; }
          }
          else
          {
            // SOUND: Cannot move her
            tone(audio, 100, 50);            
          }
        }
        // Move down
        else if(joy1Down() && playerOrientation != SOUTH)
        {
          playerOrientation = SOUTH;
          stepCounter = 1;
        }
        else if(joy1Down() && playerOrientation == SOUTH)
        {
          if(playerY < 9 && (getMapPosition(playerX, playerY + 1) == E || getMapPosition(playerX, playerY + 1) == X))
          {
            playerYNew = playerY + 1;
            steps++;
            stepCounter = 3;
          }
          else if(playerY < 8 && (getMapPosition(playerX, playerY + 1) == B || getMapPosition(playerX, playerY + 1) == XB) && (getMapPosition(playerX, playerY + 2) == E || getMapPosition(playerX, playerY + 2) == X))
          {
            playerYNew = playerY + 1;
            steps++;
            stepCounter = 3;
            moveBox = true;
            if(getMapPosition(playerX, playerY + 2) == X){ moveBoxOnStorageLocation = true; }
          }
          else
          {
            // SOUND: Cannot move her
            tone(audio, 100, 50);
          }
        }
        // Move left
        else if(joy1Left() && playerOrientation != WEST)
        {
          playerOrientation = WEST;
          stepCounter = 1;
        }
        else if(joy1Left() && playerOrientation == WEST)
        {
          if(playerX > 0 && (getMapPosition(playerX - 1, playerY) == E || getMapPosition(playerX - 1, playerY) == X))
          {
            playerXNew = playerX - 1;
            steps++;
            stepCounter = 3;
          }
          else if(playerX > 1 && (getMapPosition(playerX - 1, playerY) == B || getMapPosition(playerX - 1, playerY) == XB) && (getMapPosition(playerX - 2, playerY) == E || getMapPosition(playerX - 2, playerY) == X))
          {
            playerXNew = playerX - 1;
            steps++;
            stepCounter = 3;
            moveBox = true;
            if(getMapPosition(playerX - 2, playerY) == X){ moveBoxOnStorageLocation = true; }
          }
          else
          {
            // SOUND: Cannot move her
            tone(audio, 100, 50);
          }
        }    
      }
  
      if(movementCounter > 0)
      { 
        movementCounter--;

        // SOUND: Move box
        if(moveBox)
        {
          if(moveBoxOnStorageLocation && movementCounter == 0)
          {
            if(stepCounter == 3){ tone(audio, NOTE_C4, 20); }
            else if(stepCounter == 2){ tone(audio, NOTE_E4, 20); }
            else if(stepCounter == 1){ tone(audio, NOTE_C5, 100); }
          }
          else
          {
            tone(audio, 200 + random(100),10);
          }
        }
      }
      else if(movementCounter == 0 && stepCounter > 0)
      {
        stepCounter--;
        movementCounter = 3;
        undoPossible = true;

        // SOUND: Footsteps or move box
        if(!moveBox && footsteps == 0){ tone(audio, 200, 40); footsteps = 1; }
        else if(!moveBox && footsteps == 1){ tone(audio, 100, 40); footsteps = 0; }
        else if(moveBox)
        {
          if(moveBoxOnStorageLocation && movementCounter == 0)
          {
            if(stepCounter == 3){ tone(audio, NOTE_C4, 20); }
            else if(stepCounter == 2){ tone(audio, NOTE_E4, 20); }
            else if(stepCounter == 1){ tone(audio, NOTE_C5, 100); }             
          }
          else
          {
            tone(audio, 200 + random(100),10);
          }
        }
        
        // Remove player from old position
        if(stepCounter == 2){ drawPlayer(0, false); }      
        else if(stepCounter == 1){ drawPlayer(1, false); }      
        else if(stepCounter == 0 && playerX == playerXNew && playerY == playerYNew){ drawPlayer(0, false); }   
        else if(stepCounter == 0 && !(playerX == playerXNew && playerY == playerYNew)){ drawPlayer(2, false); }
  
        // Remove box if box is moved
        if(moveBox)
        {
          if(stepCounter == 2){ drawBoxMovedByPlayer(0, false); }      
          else if(stepCounter == 1){ drawBoxMovedByPlayer(1, false); }      
          else if(stepCounter == 0){ drawBoxMovedByPlayer(2, false); }
        }
  
        // Update fields the player steps on
        drawMapPosition(playerX, playerY);
        if(getMapPosition(playerXNew, playerYNew) == X){ drawMapPosition(playerXNew, playerYNew); }
        else if(getMapPosition(playerXNew, playerYNew) == XB)
        {
          drawStorageLocation((playerXNew * 3) + 1, (playerYNew * 3) + 1);
        }
        // Animate box movement
        if(moveBox)
        {
          if(playerOrientation == NORTH)
          {
            drawMapPosition(playerXNew, playerYNew - 1);
          }
          else if(playerOrientation == EAST)
          {        
            drawMapPosition(playerXNew + 1, playerYNew);
          }
          else if(playerOrientation == SOUTH)
          {        
            drawMapPosition(playerXNew, playerYNew + 1);
          }
          else if(playerOrientation == WEST)
          {        
            drawMapPosition(playerXNew - 1, playerYNew);
          }        
        }
        
        // Update player position if new position has been reached
        if(stepCounter == 0)
        {
          // Copy player position to playerXLastStep/playerYLastStep to enable the undo function
          if(moveBox)
          {
            playerXLastStep = playerX;
            playerYLastStep = playerY;
            playerOrientationLastStep = playerOrientation;
          }
          
          playerX = playerXNew;
          playerY = playerYNew;
        }
  
        // Draw player at new position
        if(stepCounter == 2){ drawPlayer(1, true); }      
        else if(stepCounter == 1){ drawPlayer(2, true); }      
        else if(stepCounter == 0){ drawPlayer(0, true); } // No offset since player position has already been updated      
  
        // Draw box at new position if box has been moved
        if(moveBox)
        {
          if(stepCounter == 2){ drawBoxMovedByPlayer(1, true); }      
          else if(stepCounter == 1){ drawBoxMovedByPlayer(2, true); }      
          else if(stepCounter == 0)
          { 
            drawBoxMovedByPlayer(0, true); // No offset since player position has already been updated
            
            moveBox = false;
            moveBoxOnStorageLocation = false;
  
            // Copy map into lastStepMap to enable the undo function
            numberOfBoxesOnStorageLocationsLastStep = numberOfBoxesOnStorageLocations;
            for(byte i = 0; i < 100; i++)
            {
              lastStepMap[i] = currentMap[i];
            }
  
            // Update map after a box has been moved
            if(playerOrientation == NORTH)
            {
              if(getMapPosition(playerX, playerY) == B){ setMapPosition(playerX, playerY, E); }
              else if(getMapPosition(playerX, playerY) == XB){ setMapPosition(playerX, playerY, X); numberOfBoxesOnStorageLocations--; }
              if(getMapPosition(playerX, playerY - 1) == E){ setMapPosition(playerX, playerY - 1, B); }
              else if(getMapPosition(playerX, playerY - 1) == X){ setMapPosition(playerX, playerY - 1, XB); numberOfBoxesOnStorageLocations++; }
            }
            else if(playerOrientation == EAST)
            {        
              if(getMapPosition(playerX, playerY) == B){ setMapPosition(playerX, playerY, E); }
              else if(getMapPosition(playerX, playerY) == XB){ setMapPosition(playerX, playerY, X); numberOfBoxesOnStorageLocations--; }
              if(getMapPosition(playerX + 1, playerY) == E){ setMapPosition(playerX + 1, playerY, B); }
              else if(getMapPosition(playerX + 1, playerY) == X){ setMapPosition(playerX + 1, playerY, XB); numberOfBoxesOnStorageLocations++; }
            }
            else if(playerOrientation == SOUTH)
            {        
              if(getMapPosition(playerX, playerY) == B){ setMapPosition(playerX, playerY, E); }
              else if(getMapPosition(playerX, playerY) == XB){ setMapPosition(playerX, playerY, X); numberOfBoxesOnStorageLocations--; }
              if(getMapPosition(playerX, playerY + 1) == E){ setMapPosition(playerX, playerY + 1, B); }
              else if(getMapPosition(playerX, playerY + 1) == X){ setMapPosition(playerX, playerY + 1, XB); numberOfBoxesOnStorageLocations++; }
            }
            else if(playerOrientation == WEST)
            {        
              if(getMapPosition(playerX, playerY) == B){ setMapPosition(playerX, playerY, E); }
              else if(getMapPosition(playerX, playerY) == XB){ setMapPosition(playerX, playerY, X); numberOfBoxesOnStorageLocations--; }
              if(getMapPosition(playerX - 1, playerY) == E){ setMapPosition(playerX - 1, playerY, B); }
              else if(getMapPosition(playerX - 1, playerY) == X){ setMapPosition(playerX - 1, playerY, XB); numberOfBoxesOnStorageLocations++; }
            }
          }
        }
      }

      // Open game menu
      if(joy1Fire() && movementCounter == 0 && stepCounter == 0)
      {
        gameMenu();
        if(!rerun && !reset)
        { 
          drawCurrentMap();
          drawPlayer(0, true);
        }
      }
  
      // Check whether map has been completed
      if(numberOfBoxesOnStorageLocations == numberOfStorageLocations || numberOfBoxesOnStorageLocations == numberOfBoxes)
      {
        mapCompleted = true;

        // Stop time
        endTime = millis();
  
        // Animation (player spins)
        byte i = 40;
        do
        {
          drawPlayer(0, false);
          playerOrientation++;
          if(playerOrientation == 4){ playerOrientation = 0; }
          drawPlayer(0, true);
          delay(i * 5);
          // SOUND: Map finished
          tone(audio, 100 + 25 * (40 - i), i * 4);
          i--;
        }
        while(i > 0);
  
        matrix.drawPixel((playerX * 3) + 2, (playerY * 3) + 2, matrix.Color333(7, 7, 7));
        delay(100);
        matrix.drawPixel((playerX * 3) + 2, (playerY * 3) + 1, matrix.Color333(7, 7, 3));
        matrix.drawPixel((playerX * 3) + 1, (playerY * 3) + 2, matrix.Color333(7, 7, 3));
        matrix.drawPixel((playerX * 3) + 3, (playerY * 3) + 2, matrix.Color333(7, 7, 3));
        matrix.drawPixel((playerX * 3) + 2, (playerY * 3) + 3, matrix.Color333(7, 7, 3));
        delay(100);
        matrix.drawPixel((playerX * 3) + 1, (playerY * 3) + 1, matrix.Color333(7, 7, 0));
        matrix.drawPixel((playerX * 3) + 3, (playerY * 3) + 1, matrix.Color333(7, 7, 0));
        matrix.drawPixel((playerX * 3) + 1, (playerY * 3) + 3, matrix.Color333(7, 7, 0));
        matrix.drawPixel((playerX * 3) + 3, (playerY * 3) + 3, matrix.Color333(7, 7, 0));
        matrix.drawPixel((playerX * 3) + 2, (playerY * 3) + 2, matrix.Color333(0, 0, 0));
        delay(100);
        matrix.drawPixel((playerX * 3) + 2, (playerY * 3) + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel((playerX * 3) + 1, (playerY * 3) + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel((playerX * 3) + 3, (playerY * 3) + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel((playerX * 3) + 2, (playerY * 3) + 3, matrix.Color333(0, 0, 0));
        delay(100);
        matrix.fillRect((playerX * 3) + 1, (playerY * 3) + 1, 3, 3, matrix.Color333(0, 0, 0));
        delay(1000);
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
        clickSound();
        delay(100);
        do
        {
        }
        while((!digitalRead(buttonPause) == LOW) && (!joy1Fire()) && (!joy2Fire()));
        clickSound();
        delay(100);
      }

      // Loop until 20 milliseconds has passed since begin of engine loop (to sync the game with the background animation)
      do
      {
        if(fx){ moveBackground(); }
        else{ delay(1); }
      }
      while(abs(millis() - engineLoopStartPoint) < 40);
    }
    while(!(rerun || reset || mapCompleted));
    if(mapCompleted)
    {
      overallTime = endTime - startTime;
      showResults();
      mapNumber++;
      if(mapNumber == numberOfMaps)
      { 
        // Clear screen
        matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

        // Text if all maps have been completed successfully
        matrix.setTextColor(matrix.Color333(1, 1, 3));
        matrix.setCursor(7, 0);
        matrix.println("You");
        matrix.setCursor(4, 8);
        matrix.println("made");
        matrix.setCursor(7, 16);
        matrix.println("all");
        matrix.setCursor(1, 24);
        matrix.println("maps!");

        delay(1000);

        do
        {    
        }
        while(!buttonFire1Clicked(200));  

        mapNumber = 0;
      }
    }
  }
  while(rerun || mapCompleted);
}

