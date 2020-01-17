// Sneak
// for the LEDmePlay
//
// www.mithotronic.de
//
// Version: 1.0.1
// Author: Michael Rosskopf (2017)
// 
// Thanks to Thomas Laubach! 
// 
// Release Notes:
// V1.0.1: Bug in directional control fixed (Death on diagonal direction) (18.03.2018)
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

// Sensitivity of analog joysticks in case of digital use (detects movement if deviates from center position value of 512 by sensitivity value)
const int sensitivity = 128;

// Other buttons
int buttonReset = 42;
int buttonPause = 43;

byte numberOfPlayers; // 1 or 2

// Difficulty levels differ in speed and required length to reach next arena (only 1-player mode)
byte difficulty;
#define EASY 1
#define NORMAL 2
#define HARD 3

const byte numberOfArenas = 16;
byte arena;
byte arenaSelection;
#define RANDOM 1
#define MANUAL 2

boolean villiansIn2PlayerMode; // if false, game is without villains (only in 2-player mode)

byte playfield[32][32]; // Array to store screen content (e.g. used for collision detection)

// Colors of player´s snakes
#define GREEN 1     // Player 1
#define TURQUOISE 2 // Player 2

// Movement directions
#define NORTH 1
#define NORTHEAST 5
#define EAST 2
#define SOUTHEAST 6
#define SOUTH 3
#define SOUTHWEST 7
#define WEST 4
#define NORTHWEST 8

// "Items" on the screen
#define EMPTY 0
#define WALL 1
#define SNAKE1 2
#define SNAKE2 3
#define VILLAIN 4
#define FOOD 5
#define SPEED 6
#define SHIELD 7
#define BARRIER 8
#define EXTRALIFE 9
#define PORTAL 10

// Villain types
#define PARADROID 1
#define COPTER 2
#define ENERGYFIELD 3
#define SNAKE 4
#define SENTINEL 5
#define GUMBALL 6

// Structure to store a color
struct RGBColor
{
  byte r;
  byte g;
  byte b;
};
RGBColor arenaColor;

// Structure to store particles (used for snake explosions)
struct Particle
{
  boolean active;
  double x;
  double y;
  double xDirection;
  double yDirection;
  byte age;
};
const byte numberOfParticles = 128;
Particle particle[numberOfParticles];

// Structure to store barriers (extra item)
struct Barrier
{
  boolean active;
  byte x;
  byte y;
  byte movingDirection;
  byte counter;
};

const byte maxSnakeLength = 64; // Defines the maximal length of a snake

// Structure for a player´s snake
struct Snake
{
  byte id;                 // 1 or 2 (for player 1 and player 2)
  byte numberOfSegments;
  byte x[maxSnakeLength];
  byte y[maxSnakeLength];
  byte frontSegment;       // Index of the fron segment
  byte movingDirection;
  byte collectedExtra;
  byte extraInUse;         // true, if extra has been activated
  byte extraActiveCounter; // after activation, extra is used till counter reaches 0
  Barrier barrier;
};

Snake snake1;
Snake snake2;

const byte maxNumberOfVillains = 8;

// Structure for villain
struct Villain
{
  boolean active;
  byte type;
  byte x;
  byte y;
  byte xDimension; // Size in x-dimension
  byte yDimension; // Size in y-diemnsion
  byte movingDirection;
};
Villain villain[maxNumberOfVillains];

const int probabilityThatFoodAppears = 40;   // Probability is 1:(value of variable)
const int probabilityThatExtraAppears = 160; // Probability is 1:(value of variable)

byte millisecondsPerFrame;          // Depends on the difficulty
byte numberOfSegmentsToOpenPortal;  // Depends on the difficulty (only relevant in 1-player game)
boolean reset;                      // If true, go back to title.

boolean onePlayerGamePortalOpen;    // true, after player has eaten enough food chunks (depending on difficulty)
byte portalX;                       // x-position of portal
byte portalY;                       // y-position of portal
boolean onePlayerGamePortalReached; // true, after player has reached portal
byte counter;
boolean extraLifeAppeared;

byte lives;
int overallEatenFood;
byte snake1Score;
byte snake2Score;

// Theme
const int theme[] = {NOTE_F4, NOTE_G4, NOTE_A4, NOTE_A4,                     
                     NOTE_F4, NOTE_G4, NOTE_A4, NOTE_A4,
                     NOTE_F4, NOTE_G4, NOTE_A4, NOTE_A4,
                     NOTE_F4, NOTE_G4, NOTE_E4, NOTE_E4
                     };

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

  // Default settings
  numberOfPlayers = 1;
  difficulty = EASY;
  arena = 1;
  arenaSelection = RANDOM;
  villiansIn2PlayerMode = true;
  
  // Enter the game loop
  loop();
}

// Joystick monitoring (allows to use a classic joystick or a LEDmePlay Joypad without any configuration)
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
    while(j < 250 && !joy1Fire() && !joy2Fire());
  }
  while(!joy1Fire() && !joy2Fire());
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

// clickSound: Sound effect
void clickSound()
{
  // SOUND: Menu button clicked
  for(byte i = 0; i < 40; i++)
  {
    tone(audio, 40 + 20 * i, 2);    
  }
}

// title: Shows title screen and plays melody
void title()
{
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Text
  matrix.setCursor(1, 3);
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  matrix.println("Sneak");
  matrix.drawPixel(29, 4, matrix.Color333(3, 2, 0));
  matrix.drawLine(25, 10, 25, 11, matrix.Color333(3, 2, 0));
  matrix.drawLine(29, 10, 30, 11, matrix.Color333(3, 2, 0));
  matrix.drawLine(1, 11, 24, 11, matrix.Color333(3, 2, 0));

  // Worm
  matrix.fillCircle(25, 24, 2, matrix.Color333(0, 2, 1));
  matrix.fillCircle(25, 24, 1, matrix.Color333(0, 3, 1));
  matrix.fillCircle(21, 24, 3, matrix.Color333(0, 0, 2));
  matrix.fillCircle(21, 24, 2, matrix.Color333(0, 0, 3));
  matrix.fillCircle(21, 24, 1, matrix.Color333(0, 0, 4));
  matrix.fillCircle(15, 23, 4, matrix.Color333(0, 2, 1));
  matrix.fillCircle(15, 23, 3, matrix.Color333(0, 3, 1));
  matrix.fillCircle(15, 23, 2, matrix.Color333(0, 4, 1));

  int counter = 0;
  boolean melody = true;

  do
  {
    if(counter == 0)
    {
      // Front segment
      matrix.fillCircle(9, 23, 5, matrix.Color333(0, 0, 3));
      matrix.fillCircle(9, 23, 4, matrix.Color333(0, 0, 4));
      matrix.fillCircle(9, 23, 3, matrix.Color333(0, 0, 5));

      // Eyes
      matrix.fillRect(6, 21, 2, 2, matrix.Color333(1, 1, 1));
      matrix.drawPixel(7, 22, matrix.Color333(0, 0, 0));
      matrix.fillRect(11, 21, 2, 2, matrix.Color333(1, 1, 1));
      matrix.drawPixel(12, 22, matrix.Color333(0, 0, 0));

      // Mouth
      matrix.drawLine(7, 26, 11, 26, matrix.Color333(0, 0, 0));

      // Antenna
      matrix.drawLine(7, 17, 7, 16, matrix.Color333(1, 1, 1));
      matrix.drawPixel(6, 15, matrix.Color333(2, 2, 2));
      matrix.drawPixel(6, 16, matrix.Color333(0, 0, 0));
      matrix.drawLine(11, 17, 11, 16, matrix.Color333(1, 1, 1));
      matrix.drawPixel(12, 15, matrix.Color333(2, 2, 2));
      matrix.drawPixel(12, 16, matrix.Color333(0, 0, 0));

      if(melody)
      {
        for(byte i = 0; i < 16; i++)
        {
          tone(audio, theme[i], 75);
          delay(75);
          tone(audio, theme[i] / 2, 75);
          delay(125);
        }
        melody = false;
      }
    }
    
    if(counter == 1000)
    {
      // Front segment
      matrix.fillCircle(9, 23, 5, matrix.Color333(0, 0, 3));
      matrix.fillCircle(9, 23, 4, matrix.Color333(0, 0, 4));
      matrix.fillCircle(9, 23, 3, matrix.Color333(0, 0, 5));

      // Eyes
      matrix.fillRect(6, 21, 2, 2, matrix.Color333(1, 1, 1));
      matrix.drawPixel(6, 22, matrix.Color333(0, 0, 0));
      matrix.fillRect(11, 21, 2, 2, matrix.Color333(1, 1, 1));
      matrix.drawPixel(11, 22, matrix.Color333(0, 0, 0));

      // Mouth
      matrix.drawLine(7, 26, 11, 26, matrix.Color333(0, 0, 0));
      matrix.drawPixel(6, 25, matrix.Color333(0, 0, 0));
      matrix.drawPixel(12, 25, matrix.Color333(0, 0, 0));
      matrix.drawPixel(8, 26, matrix.Color333(1, 1, 1));
      matrix.drawPixel(10, 26, matrix.Color333(1, 1, 1));

      // Antenna
      matrix.drawLine(7, 17, 7, 16, matrix.Color333(1, 1, 1));
      matrix.drawPixel(6, 15, matrix.Color333(0, 0, 0));
      matrix.drawPixel(6, 16, matrix.Color333(2, 2, 2));
      matrix.drawLine(11, 17, 11, 16, matrix.Color333(1, 1, 1));
      matrix.drawPixel(12, 15, matrix.Color333(0, 0, 0));
      matrix.drawPixel(12, 16, matrix.Color333(2, 2, 2));      
    }
    counter++;
    if(counter == 2000){ counter = 0; }
    delay(1);
  }
  while(!buttonFire1Clicked());
}

// selectNumberOfPlayers: Menu to select the number of players
void selectNumberOfPlayers()
{    
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  matrix.drawLine(0, 1, 32, 1, matrix.Color333(0, 0, 5));
  matrix.drawLine(0, 30, 32, 30, matrix.Color333(0, 0, 5));

  // Text
  matrix.setCursor(1, 3);
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  matrix.println("Plyrs");
  matrix.setCursor(13, 12);
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  matrix.println("1");
  matrix.setCursor(13, 21);
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  matrix.println("2");

  boolean updateMenu = true;
  do
  {
    if(updateMenu)
    {
      if(numberOfPlayers == 1)
      {
        matrix.setCursor(7, 12);
        matrix.setTextColor(matrix.Color333(3, 2, 0));
        matrix.println(">");
        matrix.fillRect(7, 21, 5, 7, matrix.Color333(0, 0, 0));
      }
      else if(numberOfPlayers == 2)
      {
        matrix.fillRect(7, 12, 5, 7, matrix.Color333(0, 0, 0));
        matrix.setCursor(7, 21);
        matrix.setTextColor(matrix.Color333(3, 2, 0));
        matrix.println(">");                  
      }
      delay(200);  
      updateMenu = false;
    }
    if(joy1Up() && numberOfPlayers == 2)
    {
      numberOfPlayers = 1;
      updateMenu = true;
      clickSound();
    }
    if(joy1Down() && numberOfPlayers == 1)
    {
      numberOfPlayers = 2;
      updateMenu = true;        
      clickSound();
    }    
  }
  while(!buttonFire1Clicked());  
}

// selectDifficulty: Menu to select difficulty
void selectDifficulty()
{  
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  matrix.drawLine(0, 1, 32, 1, matrix.Color333(0, 0, 5));
  matrix.drawLine(0, 30, 32, 30, matrix.Color333(0, 0, 5));

  // Text
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  matrix.setCursor(7, 3);
  matrix.println("Easy");
  matrix.setCursor(7, 12);
  matrix.println("Norm");
  matrix.setCursor(7, 21);
  matrix.println("Hard");

  boolean updateMenu = true;  
  do
  {
    if(updateMenu)
    {
      matrix.fillRect(1, 3, 5, 26, matrix.Color333(0, 0, 0));
      matrix.setCursor(1, 3 + (difficulty - 1) * 9);
      matrix.setTextColor(matrix.Color333(3, 2, 0));
      matrix.println(">");
      delay(200);  
      updateMenu = false;
    }
    if(joy1Up() && difficulty > EASY)
    {
      difficulty--;
      updateMenu = true;
      clickSound();
    }
    if(joy1Down() && difficulty < HARD)
    {
      difficulty++;
      updateMenu = true;        
      clickSound();
    }        
  }
  while(!buttonFire1Clicked());  
}

// selectArena: Menu to select the arena
void selectArena()
{ 
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  matrix.drawLine(0, 1, 32, 1, matrix.Color333(0, 0, 5));
  matrix.drawLine(0, 30, 32, 30, matrix.Color333(0, 0, 5));

  // Text
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  matrix.setCursor(1, 3);
  matrix.println("Arena");
  matrix.setCursor(7, 12);
  matrix.println("Rndm");

  boolean updateMenu = true;
  do
  {
    if(updateMenu)
    {
      matrix.fillRect(13, 21, 12, 7, matrix.Color333(0, 0, 0));
      matrix.setCursor(13, 21);
      matrix.println(arena);

      if(arenaSelection == RANDOM)
      {
        matrix.setCursor(1, 12);
        matrix.setTextColor(matrix.Color333(3, 2, 0));
        matrix.println(">");
        matrix.fillRect(1, 21, 5, 7, matrix.Color333(0, 0, 0));
      }
      else if(arenaSelection == MANUAL)
      {
        matrix.fillRect(1, 12, 5, 7, matrix.Color333(0, 0, 0));
        matrix.setCursor(1, 21);
        matrix.setTextColor(matrix.Color333(3, 2, 0));
        matrix.println(">");                  
      }
      delay(200);  
      updateMenu = false;
    }
    if(joy1Up() && arenaSelection == MANUAL)
    {
      arenaSelection = RANDOM;
      updateMenu = true;
      clickSound();
    }
    if(joy1Down() && arenaSelection == RANDOM)
    {
      arenaSelection = MANUAL;
      updateMenu = true;        
      clickSound();
    }    
    if(joy1Left() && arena > 1)
    {
      arena--;
      updateMenu = true;
      clickSound();
    }
    if(joy1Right() && arena < numberOfArenas)
    {
      arena++;
      updateMenu = true;        
      clickSound();
    }    
  }
  while(!buttonFire1Clicked());  
}

// selectVillains: Menu to set villains on/off
void selectVillains()
{    
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  matrix.drawLine(0, 1, 32, 1, matrix.Color333(0, 0, 5));
  matrix.drawLine(0, 30, 32, 30, matrix.Color333(0, 0, 5));

  // Text
  matrix.setCursor(1, 3);
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  matrix.println("Enemy");
  matrix.setCursor(10, 12);
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  matrix.println("On");
  matrix.setCursor(10, 21);
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  matrix.println("Off");

  boolean updateMenu = true;
  do
  {
    if(updateMenu)
    {
      if(villiansIn2PlayerMode)
      {
        matrix.setCursor(4, 12);
        matrix.setTextColor(matrix.Color333(3, 2, 0));
        matrix.println(">");
        matrix.fillRect(4, 21, 5, 7, matrix.Color333(0, 0, 0));
      }
      else if(!villiansIn2PlayerMode)
      {
        matrix.fillRect(4, 12, 5, 7, matrix.Color333(0, 0, 0));
        matrix.setCursor(4, 21);
        matrix.setTextColor(matrix.Color333(3, 2, 0));
        matrix.println(">");                  
      }
      delay(200);  
      updateMenu = false;
    }
    if(joy1Up() && !villiansIn2PlayerMode)
    {
      villiansIn2PlayerMode = true;
      updateMenu = true;
      clickSound();
    }
    if(joy1Down() && villiansIn2PlayerMode)
    {
      villiansIn2PlayerMode = false;
      updateMenu = true;        
      clickSound();
    }    
  }
  while(!buttonFire1Clicked());  
}

// showArena: Shows the next/selected arena
void showArena()
{ 
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Randomize the color of the walls in the arena
  do
  {
    arenaColor.r =random(4);
    arenaColor.g =random(4);
    arenaColor.b =random(4);
  }
  while((arenaColor.r + arenaColor.g + arenaColor.b) < 2 && (arenaColor.r + arenaColor.g + arenaColor.b) > 6);

  matrix.drawLine(0, 1, 32, 1, matrix.Color333(arenaColor.r, arenaColor.g, arenaColor.b));
  matrix.drawLine(0, 30, 32, 30, matrix.Color333(arenaColor.r, arenaColor.g, arenaColor.b));

  // Text
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  matrix.setCursor(1, 3);
  matrix.println("Arena");
  tone(audio, NOTE_F3, 100);
  delay(250);

  matrix.setTextColor(matrix.Color333(0, 0, 0));
  matrix.setCursor(1, 3);
  matrix.println("Arena");
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  matrix.setCursor(1, 12);
  matrix.println("Arena");
  tone(audio, NOTE_G3, 100);
  delay(250);

  matrix.setTextColor(matrix.Color333(0, 0, 0));
  matrix.setCursor(1, 12);
  matrix.println("Arena");
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  matrix.setCursor(1, 21);
  matrix.println("Arena");
  tone(audio, NOTE_A3, 100);
  delay(250);

  matrix.setTextColor(matrix.Color333(0, 0, 0));
  matrix.setCursor(1, 21);
  matrix.println("Arena");
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  matrix.setCursor(1, 3);
  matrix.println("Arena");
  tone(audio, NOTE_F3, 100);
  delay(250);
  
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  if(arena < 10){ matrix.setCursor(13, 16); }
  else{ matrix.setCursor(10, 16); }
  matrix.println(arena);
  tone(audio, NOTE_G3, 100);
  delay(250);

  matrix.setTextColor(matrix.Color333(0, 0, 0));
  if(arena < 10){ matrix.setCursor(13, 16); }
  else{ matrix.setCursor(10, 16); }
  matrix.println(arena);

  if(numberOfPlayers ==2 && arenaSelection == RANDOM)
  {
    matrix.setTextColor(matrix.Color333(3, 2, 0));
    for(byte i = 0; i < 25; i++)
    {
      matrix.fillRect(10, 16, 11, 7, matrix.Color333(0, 0, 0));
      arena = random(numberOfArenas) + 1;
      if(arena < 10){ matrix.setCursor(13, 16); }
      else{ matrix.setCursor(10, 16); }
      matrix.println(arena);
      clickSound();
      delay(10 + i * 10);
    }
    delay(250);
  }
  
  matrix.setTextSize(2);
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  if(arena < 10){ matrix.setCursor(11, 12); }
  else{ matrix.setCursor(5, 12); }
  matrix.println(arena);
  tone(audio, NOTE_A3, 100);
  delay(250);

  matrix.setTextColor(matrix.Color333(0, 0, 0));
  if(arena < 10){ matrix.setCursor(11, 12); }
  else{ matrix.setCursor(5, 12); }
  matrix.println(arena);
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  if(arena < 10){ matrix.setCursor(13, 16); }
  else{ matrix.setCursor(10, 16); }
  matrix.println(arena);
  tone(audio, NOTE_F3, 100);
  delay(250);
  
  matrix.setTextColor(matrix.Color333(0, 0, 0));
  if(arena < 10){ matrix.setCursor(13, 16); }
  else{ matrix.setCursor(10, 16); }
  matrix.println(arena);
  matrix.setTextSize(2);
  matrix.setTextColor(matrix.Color333(3, 2, 0));
  if(arena < 10){ matrix.setCursor(11, 12); }
  else{ matrix.setCursor(5, 12); }
  matrix.println(arena);
  tone(audio, NOTE_E3, 100);
  delay(1250);

  matrix.setTextSize(1);

  for(byte i = 0; i < 32; i++)
  {
    matrix.drawPixel(i, 1, matrix.Color333(0, 0, 0));
    delay(10);
  }
  for(byte i = 0; i < 32; i++)
  {
    matrix.drawPixel(i, 30, matrix.Color333(0, 0, 0));
    delay(10);
  }
}

// initializeGame: Set all variables to their initial values (depending on difficulty etc.)
void initializeGame()
{
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Initialize playfield
  for(byte i = 0; i < 32; i++)
  {
    for(byte j = 0; j < 32; j++)
    {
      playfield[i][j] = EMPTY;
    }
  }

  // Initialize arena
  initializeArena();

  // Initialize snake 1
  snake1.id = SNAKE1;
  snake1.frontSegment = 0;
  snake1.numberOfSegments = 4;
  snake1.movingDirection = EAST;
  snake1.collectedExtra = 0;
  snake1.extraInUse = 0;
  snake1.extraActiveCounter = 0;
  snake1.barrier.active = false;
  snake1.barrier.x = 0;
  snake1.barrier.y = 0;
  snake1.barrier.movingDirection = 0;
  snake1.barrier.counter = 0;
  snake1.x[0] = 5;
  snake1.y[0] = 15;
  snake1.x[1] = 4;
  snake1.y[1] = 15;
  snake1.x[2] = 3;
  snake1.y[2] = 15;
  snake1.x[3] = 2;
  snake1.y[3] = 15;
  playfield[snake1.x[0]][snake1.y[0]] = snake1.id;
  playfield[snake1.x[1]][snake1.y[1]] = snake1.id;
  playfield[snake1.x[2]][snake1.y[2]] = snake1.id;
  playfield[snake1.x[3]][snake1.y[3]] = snake1.id;
  for(byte i = 4; i < maxSnakeLength; i++)
  {
    snake1.x[i] = 0;
    snake1.y[i] = 0;
  }
  matrix.drawPixel(snake1.x[0], snake1.y[0], matrix.Color333(0, 7, 0));
  for(byte i = 1; i < 4; i++)
  {
    matrix.drawPixel(snake1.x[i], snake1.y[i], matrix.Color333(0, 1, 0));
  }

  // Initialize snake 2
  if(numberOfPlayers == 2)
  {
    snake2.id = SNAKE2;
    snake2.frontSegment = 0;
    snake2.numberOfSegments = 4;
    snake2.movingDirection = WEST;
    snake2.collectedExtra = 0;
    snake2.extraInUse = 0;
    snake2.extraActiveCounter = 0;
    snake2.barrier.active = false;
    snake2.barrier.x = 0;
    snake2.barrier.y = 0;
    snake2.barrier.movingDirection = 0;
    snake2.barrier.counter = 0;
    snake2.x[0] = 26;
    snake2.y[0] = 15;
    snake2.x[1] = 27;
    snake2.y[1] = 15;
    snake2.x[2] = 28;
    snake2.y[2] = 15;
    snake2.x[3] = 29;
    snake2.y[3] = 15;
    playfield[snake2.x[0]][snake2.y[0]] = snake2.id;
    playfield[snake2.x[1]][snake2.y[1]] = snake2.id;
    playfield[snake2.x[2]][snake2.y[2]] = snake2.id;
    playfield[snake2.x[3]][snake2.y[3]] = snake2.id;
    for(byte i = 4; i < maxSnakeLength; i++)
    {
      snake2.x[i] = 0;
      snake2.y[i] = 0;
    }
    matrix.drawPixel(snake2.x[0], snake2.y[0], matrix.Color333(0, 7, 7));
    for(byte i = 1; i < 4; i++)
    {
      matrix.drawPixel(snake2.x[i], snake2.y[i], matrix.Color333(0, 1, 1));
    }
  }

  extraLifeAppeared = false; // Appears only once per arena

  // Define number of segements top open portal in one player mode
  if(difficulty == EASY)
  { 
    numberOfSegmentsToOpenPortal = 12; 
    millisecondsPerFrame = 80;
  }
  else if(difficulty == NORMAL)
  { 
    numberOfSegmentsToOpenPortal = 14;
    millisecondsPerFrame = 65;
  }
  else if(difficulty == HARD)
  { 
    numberOfSegmentsToOpenPortal = 16;
    millisecondsPerFrame = 50;
  }

  // Start game loop
  gameLoop();
}

// setLine: Draws a line in a defined color
void setLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, byte r, byte g, byte b)
{
  int16_t swapBuffer;
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if(steep)
  {
    swapBuffer = x0; x0 = y0; y0 = swapBuffer;
    swapBuffer = x1; x1 = y1; y1 = swapBuffer;
  }

  if(x0 > x1)
  {
    swapBuffer = x0; x0 = x1; x1 = swapBuffer;
    swapBuffer = y0; y0 = y1; y1 = swapBuffer;
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for(; x0<=x1; x0++)
  {
    if(steep)
    {
      matrix.drawPixel(y0, x0, matrix.Color333(r, g, b));
      playfield[y0][x0] = WALL;
    } 
    else
    {
      matrix.drawPixel(x0, y0, matrix.Color333(r, g, b));
      playfield[x0][y0] = WALL;
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
    delay(5);
  }
}

// initializeArena: Draws the next/selected arena
void initializeArena()
{
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  for(byte i = 0; i < maxNumberOfVillains; i++)
  {
    villain[i].active = false;
  }
  
  if(arena == 1) // The square
  {
    setLine(0, 0, 31, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 0, 31, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 31, 0, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(0, 31, 0, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], PARADROID, 15, 2, WEST);
      initializeVillain(villain[1], PARADROID, 14, 27, EAST);
    }
  }
  else if(arena == 2) // Barrier in a Box
  {
    setLine(0, 0, 31, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 0, 31, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 31, 0, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(0, 31, 0, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(15, 9, 15, 22, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(16, 9, 16, 22, arenaColor.r, arenaColor.g, arenaColor.b);    
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], SNAKE, 8, 26, NORTH);
      initializeVillain(villain[1], SNAKE, 23, 26, NORTH);
      initializeVillain(villain[2], PARADROID, 14, 2, SOUTH);
    }
  }
  else if(arena == 3) // 2 Openings
  {
    setLine(0, 0, 31, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 0, 31, 7, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 24, 31, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 31, 0, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(0, 31, 0, 24, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(0, 7, 0, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], ENERGYFIELD, 13, 13, SOUTH);
      initializeVillain(villain[1], PARADROID, 15, 2, WEST);
      initializeVillain(villain[2], PARADROID, 14, 27, EAST);
    }
  }
  else if(arena == 4) // 2 Walls
  {
    setLine(0, 0, 31, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 0, 31, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 31, 0, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(0, 31, 0, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(8, 8, 23, 8, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(8, 23, 23, 23, arenaColor.r, arenaColor.g, arenaColor.b);
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], PARADROID, 2, 2, SOUTH);
      initializeVillain(villain[1], SNAKE, 14, 4, WEST);
      initializeVillain(villain[2], SNAKE, 14, 27, EAST);
      initializeVillain(villain[3], PARADROID, 27, 27, NORTH);
    }
  }
  else if(arena == 5) // Many Barriers
  {
    setLine(24, 0, 31, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(8, 8, 15, 8, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(16, 23, 23, 23, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(0, 31, 7, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], COPTER, 14, 14, SOUTH);
      initializeVillain(villain[1], PARADROID, 15, 2, WEST);
      initializeVillain(villain[2], PARADROID, 14, 27, EAST);
    }
  }
  else if(arena == 6) // The Cover
  {
    setLine(0, 0, 31, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 0, 31, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 31, 0, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(0, 31, 0, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(16, 8, 23, 8, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(8, 23, 15, 23, arenaColor.r, arenaColor.g, arenaColor.b);    
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], ENERGYFIELD, 13, 2, SOUTH);
      initializeVillain(villain[1], ENERGYFIELD, 14, 25, NORTH);
    }
  }
  else if(arena == 7) // 4 Directions
  {
    setLine(0, 0, 7, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(24, 0, 31, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 0, 31, 7, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 24, 31, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 31, 24, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(7, 31, 0, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(0, 31, 0, 24, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(0, 7, 0, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], GUMBALL, 16, 14, NORTHEAST);
      initializeVillain(villain[1], GUMBALL, 16, 16, SOUTHEAST);
      initializeVillain(villain[2], GUMBALL, 13, 16, SOUTHWEST);
      initializeVillain(villain[3], GUMBALL, 13, 14, NORTHWEST);
    }
  }
  else if(arena == 8) // The Crossbar
  {
    setLine(0, 0, 31, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 0, 31, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 31, 0, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(0, 31, 0, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(8, 8, 23, 8, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(8, 23, 23, 23, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(15, 9, 15, 22, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(16, 9, 16, 22, arenaColor.r, arenaColor.g, arenaColor.b);
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], COPTER, 14, 2, EAST);
      initializeVillain(villain[1], COPTER, 15, 27, WEST);
    }
  }
  else if(arena == 9) // The Zoom
  {
    setLine(0, 0, 7, 7, arenaColor.r, arenaColor.g, arenaColor.b);    
    setLine(31, 0, 24, 7, arenaColor.r, arenaColor.g, arenaColor.b);    
    setLine(31, 31, 24, 24, arenaColor.r, arenaColor.g, arenaColor.b);    
    setLine(0, 31, 7, 24, arenaColor.r, arenaColor.g, arenaColor.b);    
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], ENERGYFIELD, 13, 2, SOUTH);
      initializeVillain(villain[1], ENERGYFIELD, 14, 25, NORTH);
      initializeVillain(villain[2], COPTER, 14, 14, NORTH);
    }
  }
  else if(arena == 10) // The Barrier
  {
    setLine(0, 0, 31, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 31, 0, 31, arenaColor.r, arenaColor.g, arenaColor.b);    
    setLine(15, 9, 15, 22, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(16, 9, 16, 22, arenaColor.r, arenaColor.g, arenaColor.b);
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], SENTINEL, 14, 2, SOUTH);
      initializeVillain(villain[1], SENTINEL, 15, 27, NORTH);
    }
  }
  else if(arena == 11) // The Tank-Arena
  {
    setLine(0, 0, 31, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 0, 31, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 31, 0, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(0, 31, 0, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(6, 12, 6, 6, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(6, 6, 12, 6, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(19, 6, 25, 6, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(25, 6, 25, 12, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(25, 19, 25, 25, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(25, 25, 19, 25, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(12, 25, 6, 25, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(6, 25, 6, 19, arenaColor.r, arenaColor.g, arenaColor.b);    
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], COPTER, 14, 2, EAST);
      initializeVillain(villain[1], COPTER, 15, 27, WEST);
      initializeVillain(villain[2], ENERGYFIELD, 13, 11, NORTH);
      initializeVillain(villain[3], ENERGYFIELD, 14, 16, SOUTH);
    }
  }
  else if(arena == 12) // The Chevron
  {
    setLine(0, 0, 31, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 0, 31, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 31, 0, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(0, 31, 0, 0, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(8, 12, 15, 8, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(16, 8, 23, 12, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(8, 24, 15, 20, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(16, 20, 23, 24, arenaColor.r, arenaColor.g, arenaColor.b);    
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], SNAKE, 2, 4, EAST);
      initializeVillain(villain[1], SNAKE, 27, 2, SOUTH);
      initializeVillain(villain[2], SNAKE, 26, 27, WEST);
      initializeVillain(villain[3], SNAKE, 4, 26, NORTH);
      initializeVillain(villain[4], SENTINEL, 14, 13, NORTH);
      initializeVillain(villain[5], SENTINEL, 15, 16, SOUTH);
    }
  }
  else if(arena == 13) // The Spikes
  {
    setLine(0, 16, 7, 23, arenaColor.r, arenaColor.g, arenaColor.b);    
    setLine(16, 0, 9, 7, arenaColor.r, arenaColor.g, arenaColor.b);    
    setLine(31, 15, 24, 8, arenaColor.r, arenaColor.g, arenaColor.b);    
    setLine(16, 31, 23, 24, arenaColor.r, arenaColor.g, arenaColor.b);        
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], COPTER, 2, 2, SOUTH);
      initializeVillain(villain[1], COPTER, 27, 2, WEST);
      initializeVillain(villain[2], COPTER, 27, 27, NORTH);
      initializeVillain(villain[3], COPTER, 2, 27, EAST);
      initializeVillain(villain[4], SENTINEL, 14, 13, NORTH);
      initializeVillain(villain[5], SENTINEL, 15, 16, SOUTH);
    }
  }
  else if(arena == 14) // The Crazies
  {
    setLine(0, 10, 7, 10, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(0, 21, 7, 21, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(0, 10, 0, 21, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(24, 10, 31, 10, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(24, 21, 31, 21, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(31, 10, 31, 21, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(15, 0, 15, 10, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(16, 0, 16, 10, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(15, 21, 15, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(16, 21, 16, 31, arenaColor.r, arenaColor.g, arenaColor.b);
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], GUMBALL, 16, 14, NORTHEAST);
      initializeVillain(villain[1], GUMBALL, 16, 16, SOUTHEAST);
      initializeVillain(villain[2], GUMBALL, 13, 16, SOUTHWEST);
      initializeVillain(villain[3], GUMBALL, 13, 14, NORTHWEST);
      initializeVillain(villain[4], SENTINEL, 14, 13, NORTH);
      initializeVillain(villain[5], SENTINEL, 15, 16, SOUTH);
    }
  }
  else if(arena == 15) // The X-Files
  {
    setLine(8, 8, 23, 23, arenaColor.r, arenaColor.g, arenaColor.b);
    setLine(23, 8, 8, 23, arenaColor.r, arenaColor.g, arenaColor.b);    
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], ENERGYFIELD, 2, 2, NORTH);
      initializeVillain(villain[1], ENERGYFIELD, 25, 2, EAST);
      initializeVillain(villain[2], ENERGYFIELD, 25, 25, SOUTH);
      initializeVillain(villain[3], ENERGYFIELD, 2, 25, WEST);
    }
  }
  else // Fully Open
  {
    if(numberOfPlayers == 1 || (numberOfPlayers == 2 && villiansIn2PlayerMode))
    {
      initializeVillain(villain[0], PARADROID, 2, 2, NORTHEAST);
      initializeVillain(villain[1], COPTER, 27, 27, SOUTHEAST);
      initializeVillain(villain[2], ENERGYFIELD, 13, 2, SOUTHWEST);
      initializeVillain(villain[3], SNAKE, 12, 16, EAST);
      initializeVillain(villain[4], SENTINEL, 15, 27, NORTHWEST);
      initializeVillain(villain[5], GUMBALL, 14, 14, NORTHWEST);
    }
  }
}

// initializeVillain: Initializes a villain
void initializeVillain(Villain &villain, byte type, byte x, byte y, byte movingDirection)
{
  villain.active = true;
  villain.type = type;
  villain.x = x;
  villain.y = y;
  villain.movingDirection = movingDirection;
}

// drawSnake: Draws a complete snake
void drawSnake(Snake &snake, byte i, byte color)
{
  while(i < snake.numberOfSegments)
  {
    if(color == GREEN)
    { 
      if(i == 0){ matrix.drawPixel(snake.x[(snake.frontSegment + i) % maxSnakeLength], snake.y[(snake.frontSegment + i) % maxSnakeLength], matrix.Color333(0, 7, 0)); }
      else{ matrix.drawPixel(snake.x[(snake.frontSegment + i + 1) % maxSnakeLength], snake.y[(snake.frontSegment + i + 1) % maxSnakeLength], matrix.Color333(0, 1, 0)); }
    }
    if(color == TURQUOISE)
    { 
      if(i == 0){ matrix.drawPixel(snake.x[(snake.frontSegment + i) % maxSnakeLength], snake.y[(snake.frontSegment + i) % maxSnakeLength], matrix.Color333(0, 7, 7)); }
      else{ matrix.drawPixel(snake.x[(snake.frontSegment + i + 1) % maxSnakeLength], snake.y[(snake.frontSegment + i + 1) % maxSnakeLength], matrix.Color333(0, 1, 1)); }
    }
    playfield[snake.x[(snake.frontSegment + i) % maxSnakeLength]][snake.y[(snake.frontSegment + i) % maxSnakeLength]] = snake.id;
    i++;
  }
  
  // Remove last segment
  matrix.drawPixel(snake.x[(snake.frontSegment + snake.numberOfSegments) % maxSnakeLength], snake.y[(snake.frontSegment + snake.numberOfSegments) % maxSnakeLength], matrix.Color333(0, 0, 0));
  playfield[snake.x[(snake.frontSegment + snake.numberOfSegments) % maxSnakeLength]][snake.y[(snake.frontSegment + snake.numberOfSegments) % maxSnakeLength]] = EMPTY;
}

// redrawSnake: Draws the new front segment and removes the old last segment
void redrawSnake(Snake &snake, byte color)
{
  if(snake.collectedExtra == SPEED){ matrix.drawPixel(snake.x[(snake.frontSegment) % maxSnakeLength], snake.y[(snake.frontSegment) % maxSnakeLength], matrix.Color333(3, 3, 0)); }
  else if(snake.collectedExtra == SHIELD){ matrix.drawPixel(snake.x[(snake.frontSegment) % maxSnakeLength], snake.y[(snake.frontSegment) % maxSnakeLength], matrix.Color333(0, 0, 3)); }
  else if(snake.collectedExtra == BARRIER){ matrix.drawPixel(snake.x[(snake.frontSegment) % maxSnakeLength], snake.y[(snake.frontSegment) % maxSnakeLength], matrix.Color333(3, 0, 3)); }

  if(snake.extraInUse == SPEED && snake.extraActiveCounter > 0 && (counter % 4 < 2)){ matrix.drawPixel(snake.x[(snake.frontSegment) % maxSnakeLength], snake.y[(snake.frontSegment) % maxSnakeLength], matrix.Color333(7, 7, 0)); }
  else if(snake.extraInUse == SHIELD && snake.extraActiveCounter > 0 && (counter % 4 < 2)){ matrix.drawPixel(snake.x[(snake.frontSegment) % maxSnakeLength], snake.y[(snake.frontSegment) % maxSnakeLength], matrix.Color333(0, 0, 7)); }
  else if(snake.extraInUse == BARRIER && snake.extraActiveCounter > 0 && (counter % 4 < 2)){ matrix.drawPixel(snake.x[(snake.frontSegment) % maxSnakeLength], snake.y[(snake.frontSegment) % maxSnakeLength], matrix.Color333(7, 0, 7)); }
  
  if(color == GREEN)
  { 
    if((snake.collectedExtra == 0 && snake.extraActiveCounter == 0) || (snake.extraActiveCounter > 0 && counter % 4 > 1)){ matrix.drawPixel(snake.x[(snake.frontSegment) % maxSnakeLength], snake.y[(snake.frontSegment) % maxSnakeLength], matrix.Color333(0, 7, 0)); }
    matrix.drawPixel(snake.x[(snake.frontSegment + 1) % maxSnakeLength], snake.y[(snake.frontSegment + 1) % maxSnakeLength], matrix.Color333(0, 1, 0)); 
  }
  if(color == TURQUOISE)
  { 
    if((snake.collectedExtra == 0 && snake.extraActiveCounter == 0) || (snake.extraActiveCounter > 0 && counter % 4 > 1)){ matrix.drawPixel(snake.x[(snake.frontSegment) % maxSnakeLength], snake.y[(snake.frontSegment) % maxSnakeLength], matrix.Color333(0, 7, 7)); }
    matrix.drawPixel(snake.x[(snake.frontSegment + 1) % maxSnakeLength], snake.y[(snake.frontSegment + 1) % maxSnakeLength], matrix.Color333(0, 1, 1)); 
  }
  playfield[snake.x[(snake.frontSegment) % maxSnakeLength]][snake.y[(snake.frontSegment) % maxSnakeLength]] = snake.id;
  
  // Remove last segment
  matrix.drawPixel(snake.x[(snake.frontSegment + snake.numberOfSegments) % maxSnakeLength], snake.y[(snake.frontSegment + snake.numberOfSegments) % maxSnakeLength], matrix.Color333(0, 0, 0));
  playfield[snake.x[(snake.frontSegment + snake.numberOfSegments) % maxSnakeLength]][snake.y[(snake.frontSegment + snake.numberOfSegments) % maxSnakeLength]] = EMPTY;
}

// moveSnake: Updates snake variables for 1-pixel movement (depending on direction)
// Algorithm:
// Two arrays are used for the x- and y-positions of all snake segments.
// The arrays have the maximal length of a snake.
// In every step, a new front segement position is calculated and an index pointer is set to it.
// The following segements have the following indices (modulo).
// The last segment from the previous step is then no longer considered (defined by snake length).
byte moveSnake(Snake &snake)
{
  byte bufferX = snake.x[snake.frontSegment];
  byte bufferY = snake.y[snake.frontSegment];
  byte elementOnNewFrontSegment;
  
  if(snake.frontSegment == 0)
  { 
    snake.frontSegment = maxSnakeLength - 1; 
  }
  else
  {
    snake.frontSegment--;
  }
  
  if(snake.movingDirection == NORTH)
  {
    elementOnNewFrontSegment = playfield[bufferX][byte(bufferY - 1) % 32];
    snake.x[snake.frontSegment] = bufferX;
    snake.y[snake.frontSegment] = byte(bufferY - 1) % 32;
    playfield[bufferX][byte(bufferY - 1) % 32] = snake.id;
  }
  else if(snake.movingDirection == EAST)
  {
    elementOnNewFrontSegment = playfield[(bufferX + 1) % 32][bufferY];
    snake.x[snake.frontSegment] = (bufferX + 1) % 32;
    snake.y[snake.frontSegment] = bufferY;
    playfield[(bufferX + 1) % 32][bufferY] = snake.id;
  }
  else if(snake.movingDirection == SOUTH)
  {
    elementOnNewFrontSegment = playfield[bufferX][(bufferY + 1) % 32];
    snake.x[snake.frontSegment] = bufferX;
    snake.y[snake.frontSegment] = (bufferY + 1) % 32;
    playfield[bufferX][(bufferY + 1) % 32] = snake.id; 
  }
  else if(snake.movingDirection == WEST)
  {
    elementOnNewFrontSegment = playfield[byte(bufferX - 1) % 32][bufferY];
    snake.x[snake.frontSegment] = byte(bufferX - 1) % 32;
    snake.y[snake.frontSegment] = bufferY;
    playfield[byte(bufferX - 1) % 32][bufferY] = snake.id;
  }

  return elementOnNewFrontSegment;
}

// initializeBarrier: Initializes a new barrier (extra used by player)
void initializeBarrier(Barrier &barrier, byte x, byte y, byte movingDirection)
{
  barrier.active = true;
  barrier.x = x;
  barrier.y = y;
  barrier.movingDirection = movingDirection;
  barrier.counter = 0;
  if(movingDirection == NORTH)
  {
    barrier.y = byte(barrier.y - 2) % 32;
  }
  else if(movingDirection == EAST)
  {
    barrier.x = byte(barrier.x + 2) % 32;
  }
  else if(movingDirection == SOUTH)
  {
    barrier.y = byte(barrier.y + 2) % 32;
  }
  else if(movingDirection == WEST)
  {
    barrier.x = byte(barrier.x - 2) % 32;
  }
  if(playfield[barrier.x][barrier.y] == EMPTY)
  {
    matrix.drawPixel(barrier.x, barrier.y, matrix.Color333(3, 0, 3));
    playfield[barrier.x][barrier.y] = WALL;
  }
  else
  {
    barrier.active = false;
  }
}

// checkPixelForVillainMovement: Returns false if villain cannot move to a position covering position (x,y)
boolean checkPixelForVillainMovement(byte x, byte y)
{
  byte p = playfield[x][y];
  if(p == WALL || p == SNAKE1 || p == SNAKE2 || p == VILLAIN || p == PORTAL){ return false; }
  return true;
}

// checkNewVillainPosition: Checks potential new position of a villain (to check whether movement to this position is possible)
boolean checkNewVillainPosition(Villain &villain, byte newX, byte newY)
{
  byte p;
  if(villain.type == PARADROID)
  {
    if(!checkPixelForVillainMovement((newX + 1) % 32, (newY) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX) % 32, (newY + 1) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 1) % 32, (newY + 1) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 2) % 32, (newY + 1) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 1) % 32, (newY + 2) % 32)){ return false; }
  }
  else if(villain.type == COPTER)
  {
    if(!checkPixelForVillainMovement((newX) % 32, (newY) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 1) % 32, (newY) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 2) % 32, (newY) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX) % 32, (newY + 1) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 1) % 32, (newY + 1) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 2) % 32, (newY + 1) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX) % 32, (newY + 2) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 1) % 32, (newY + 2) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 2) % 32, (newY + 2) % 32)){ return false; }
  }
  else if(villain.type == ENERGYFIELD)
  {
    if(!checkPixelForVillainMovement((newX + 1) % 32, (newY) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 2) % 32, (newY) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 3) % 32, (newY) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX) % 32, (newY + 1) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 1) % 32, (newY + 1) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 2) % 32, (newY + 1) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 3) % 32, (newY + 1) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 4) % 32, (newY + 1) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX) % 32, (newY + 2) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 1) % 32, (newY + 2) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 2) % 32, (newY + 2) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 3) % 32, (newY + 2) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 4) % 32, (newY + 2) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX) % 32, (newY + 3) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 1) % 32, (newY + 3) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 2) % 32, (newY + 3) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 3) % 32, (newY + 3) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 4) % 32, (newY + 3) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 1) % 32, (newY + 4) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 2) % 32, (newY + 4) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 3) % 32, (newY + 4) % 32)){ return false; }
  }
  else if(villain.type == SNAKE)
  {
    if(villain.movingDirection == NORTH)
    { 
      if(!checkPixelForVillainMovement((newX) % 32, (newY) % 32)){ return false; }
    }
    if(villain.movingDirection == EAST)
    { 
      if(!checkPixelForVillainMovement((newX + 3) % 32, (newY) % 32)){ return false; }
    }
    if(villain.movingDirection == SOUTH)
    { 
      if(!checkPixelForVillainMovement((newX) % 32, (newY + 3) % 32)){ return false; }
    }
    if(villain.movingDirection == WEST)
    { 
      if(!checkPixelForVillainMovement((newX) % 32, (newY) % 32)){ return false; }
    }
  }
  if(villain.type == SENTINEL)
  {
    if(!checkPixelForVillainMovement((newX) % 32, (newY) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 2) % 32, (newY) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 1) % 32, (newY + 1) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX) % 32, (newY + 2) % 32)){ return false; }
    if(!checkPixelForVillainMovement((newX + 2) % 32, (newY + 2) % 32)){ return false; }
  }
  else if(villain.type == GUMBALL)
  {
    if(villain.movingDirection == NORTHEAST)
    { 
      if(!checkPixelForVillainMovement((newX) % 32, (newY) % 32)){ return false; }
      if(!checkPixelForVillainMovement((newX + 1) % 32, (newY) % 32)){ return false; }
      if(!checkPixelForVillainMovement((newX + 1) % 32, (newY + 1) % 32)){ return false; }
    }
    if(villain.movingDirection == SOUTHEAST)
    { 
      if(!checkPixelForVillainMovement((newX + 1) % 32, (newY) % 32)){ return false; }
      if(!checkPixelForVillainMovement((newX) % 32, (newY + 1) % 32)){ return false; }
      if(!checkPixelForVillainMovement((newX + 1) % 32, (newY + 1) % 32)){ return false; }
    }
    if(villain.movingDirection == SOUTHWEST)
    { 
      if(!checkPixelForVillainMovement((newX) % 32, (newY) % 32)){ return false; }
      if(!checkPixelForVillainMovement((newX) % 32, (newY + 1) % 32)){ return false; }
      if(!checkPixelForVillainMovement((newX + 1) % 32, (newY + 1) % 32)){ return false; }
    }
    if(villain.movingDirection == NORTHWEST)
    { 
      if(!checkPixelForVillainMovement((newX) % 32, (newY) % 32)){ return false; }
      if(!checkPixelForVillainMovement((newX + 1) % 32, (newY) % 32)){ return false; }
      if(!checkPixelForVillainMovement((newX) % 32, (newY + 1) % 32)){ return false; }
    }   
  }
  return true;
}

// setPixel: Set a pixel on screen and on playfield (storing extra information e.g. used for collision detection)
void setPixel(byte x, byte y, byte r, byte g, byte b, byte value)
{
  matrix.drawPixel(x, y, matrix.Color333(r, g, b));
  playfield[x][y] = value;
}

// moveAndDrawVillain: Removes villain at old position, calculates movement, and draws villain at new position
void moveAndDrawVillain(Villain &villain)
{
  // Remove villain at old position
  if(villain.type == PARADROID)
  {
    setPixel((villain.x + 1) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 1) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 2) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 1) % 32, (villain.y + 2) % 32, 0, 0, 0, EMPTY);    
  }
  else if(villain.type == COPTER)
  {
    setPixel((villain.x) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 1) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 2) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 1) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 2) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x) % 32, (villain.y + 2) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 1) % 32, (villain.y + 2) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 2) % 32, (villain.y + 2) % 32, 0, 0, 0, EMPTY);
  }
  else if(villain.type == ENERGYFIELD)
  {
    setPixel((villain.x + 1) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 2) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 3) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 1) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 2) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 3) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 4) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x) % 32, (villain.y + 2) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 1) % 32, (villain.y + 2) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 2) % 32, (villain.y + 2) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 3) % 32, (villain.y + 2) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 4) % 32, (villain.y + 2) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x) % 32, (villain.y + 3) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 1) % 32, (villain.y + 3) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 2) % 32, (villain.y + 3) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 3) % 32, (villain.y + 3) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 4) % 32, (villain.y + 3) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 1) % 32, (villain.y + 4) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 2) % 32, (villain.y + 4) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 3) % 32, (villain.y + 4) % 32, 0, 0, 0, EMPTY);
  }
  else if(villain.type == SNAKE)
  {
    if(villain.movingDirection == NORTH || villain.movingDirection == SOUTH)
    { 
      setPixel((villain.x) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
      setPixel((villain.x) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
      setPixel((villain.x) % 32, (villain.y + 2) % 32, 0, 0, 0, EMPTY);
      setPixel((villain.x) % 32, (villain.y + 3) % 32, 0, 0, 0, EMPTY);
    }
    if(villain.movingDirection == EAST || villain.movingDirection == WEST)
    { 
      setPixel((villain.x) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
      setPixel((villain.x + 1) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
      setPixel((villain.x + 2) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
      setPixel((villain.x + 3) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
    }
  }    
  else if(villain.type == SENTINEL)
  {
    setPixel((villain.x) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 2) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 1) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x) % 32, (villain.y + 2) % 32, 0, 0, 0, EMPTY);
    setPixel((villain.x + 2) % 32, (villain.y + 2) % 32, 0, 0, 0, EMPTY);    
  }
  else if(villain.type == GUMBALL)
  {
    if(villain.movingDirection == NORTHEAST || villain.movingDirection == SOUTHWEST)
    { 
      setPixel((villain.x + 1) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
      setPixel((villain.x) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
    }
    if(villain.movingDirection == SOUTHEAST || villain.movingDirection == NORTHWEST)
    { 
      setPixel((villain.x) % 32, (villain.y) % 32, 0, 0, 0, EMPTY);
      setPixel((villain.x + 1) % 32, (villain.y + 1) % 32, 0, 0, 0, EMPTY);
    }    
  }

  // Move villain
  if(villain.type == PARADROID && counter % 4 == 0 || villain.type == COPTER && counter % 2 == 0)
  {
    if(random(32) == 0)
    {
      villain.movingDirection = random(4) + 1;    
    }
    if(villain.movingDirection == NORTH)
    {
      if(checkNewVillainPosition(villain, villain.x, (villain.y - 1) % 32))
      {
        villain.y = (villain.y - 1) % 32;
      }
      else
      {
        villain.movingDirection = random(4) + 1;
      }
    }
    if(villain.movingDirection == EAST)
    {
      if(checkNewVillainPosition(villain, (villain.x + 1) % 32, villain.y))
      {
        villain.x = (villain.x + 1) % 32;
      }
      else
      {
        villain.movingDirection = random(4) + 1;
      }
    }
    if(villain.movingDirection == SOUTH)
    {
      if(checkNewVillainPosition(villain, villain.x, (villain.y + 1) % 32))
      {
        villain.y = (villain.y + 1) % 32;
      }
      else
      {
        villain.movingDirection = random(4) + 1;
      }
    }
    if(villain.movingDirection == WEST)
    {
      if(checkNewVillainPosition(villain, (villain.x - 1) % 32, villain.y))
      {
        villain.x = (villain.x - 1) % 32;
      }
      else
      {
        villain.movingDirection = random(4) + 1;
      }
    }
  }
  else if(villain.type == SNAKE && counter % 4 == 0)
  {
    if(villain.movingDirection == NORTH)
    { 
      if(checkNewVillainPosition(villain, (villain.x) % 32, (villain.y - 1) % 32))
      {
        villain.y = (villain.y - 1) % 32;
      }
      else
      {
        villain.movingDirection = SOUTH;        
      }
    }
    if(villain.movingDirection == EAST)
    { 
      if(checkNewVillainPosition(villain, (villain.x + 1) % 32, (villain.y) % 32))
      {
        villain.x = (villain.x + 1) % 32;
      }
      else
      {
        villain.movingDirection = WEST;        
      }
    }
    if(villain.movingDirection == SOUTH)
    { 
      if(checkNewVillainPosition(villain, (villain.x) % 32, (villain.y + 1) % 32))
      {
        villain.y = (villain.y + 1) % 32;
      }
      else
      {
        villain.movingDirection = NORTH;        
      }
    }
    if(villain.movingDirection == WEST)
    { 
      if(checkNewVillainPosition(villain, (villain.x - 1) % 32, (villain.y) % 32))
      {
        villain.x = (villain.x - 1) % 32;
      }
      else
      {
        villain.movingDirection = EAST;        
      }
    }
  }
  else if(villain.type == GUMBALL && counter % 2 == 0)
  {
    if(villain.movingDirection == NORTHEAST)
    { 
      if(checkNewVillainPosition(villain, (villain.x + 1) % 32, (villain.y - 1) % 32))
      {
        villain.x = (villain.x + 1) % 32;
        villain.y = (villain.y - 1) % 32;
      }
      else
      {
        villain.movingDirection = random(4) + 5;        
      }
    }
    if(villain.movingDirection == SOUTHEAST)
    { 
      if(checkNewVillainPosition(villain, (villain.x + 1) % 32, (villain.y + 1) % 32))
      {
        villain.x = (villain.x + 1) % 32;
        villain.y = (villain.y + 1) % 32;
      }
      else
      {
        villain.movingDirection = random(4) + 5;        
      }
    }
    if(villain.movingDirection == SOUTHWEST)
    { 
      if(checkNewVillainPosition(villain, (villain.x - 1) % 32, (villain.y + 1) % 32))
      {
        villain.x = (villain.x - 1) % 32;
        villain.y = (villain.y + 1) % 32;
      }
      else
      {
        villain.movingDirection = random(4) + 5;        
      }
    }
    if(villain.movingDirection == NORTHWEST)
    { 
      if(checkNewVillainPosition(villain, (villain.x - 1) % 32, (villain.y - 1) % 32))
      {
        villain.x = (villain.x - 1) % 32;
        villain.y = (villain.y - 1) % 32;
      }
      else
      {
        villain.movingDirection = random(4) + 5;        
      }
    }
  }
  else if(villain.type == SENTINEL && counter % 4 == 0 || villain.type == ENERGYFIELD && counter % 8 == 0)
  {
    int snake1XDistance;
    int snake1YDistance;
    int snake2XDistance;
    int snake2YDistance;
    int xModifier = 0;
    int yModifier = 0;
    byte closestSnake = SNAKE1;
    if(villain.type == SENTINEL)
    {
      snake1XDistance = int(snake1.x[snake1.frontSegment]) - int(villain.x) - 1;
      snake1YDistance = int(snake1.y[snake1.frontSegment]) - int(villain.y) - 1;
      if(numberOfPlayers == 2)
      {
        snake2XDistance = int(snake2.x[snake2.frontSegment]) - int(villain.x) - 1;
        snake2YDistance = int(snake2.y[snake2.frontSegment]) - int(villain.y) - 1;
        if(sqrt(sq(snake2XDistance) + sq(snake2YDistance)) < sqrt(sq(snake1XDistance) + sq(snake1YDistance))){ closestSnake = SNAKE2; }      
      }
    }
    else if(villain.type == ENERGYFIELD)
    {
      snake1XDistance = int(snake1.x[snake1.frontSegment]) - int(villain.x) - 2;
      snake1YDistance = int(snake1.y[snake1.frontSegment]) - int(villain.y) - 2;        
      if(numberOfPlayers == 2)
      {
        snake2XDistance = int(snake2.x[snake2.frontSegment]) - int(villain.x) - 2;
        snake2YDistance = int(snake2.y[snake2.frontSegment]) - int(villain.y) - 2;        
        if(sqrt(sq(snake2XDistance) + sq(snake2YDistance)) < sqrt(sq(snake1XDistance) + sq(snake1YDistance))){ closestSnake = SNAKE2; }      
      }
    }
    if(closestSnake == SNAKE1)
    {
      if(snake1XDistance < 0){ xModifier = -1; }
      if(snake1XDistance > 0){ xModifier = 1; }
      if(snake1YDistance < 0){ yModifier = -1; }
      if(snake1YDistance > 0){ yModifier = 1; }
      if(checkNewVillainPosition(villain, (villain.x + xModifier) % 32, (villain.y + yModifier) % 32))
      {
        villain.x = (villain.x + xModifier) % 32;
        villain.y = (villain.y + yModifier) % 32;
      }       
    }
    else if(closestSnake == SNAKE2 && numberOfPlayers == 2)
    {
      if(snake2XDistance < 0){ xModifier = -1; }
      if(snake2XDistance > 0){ xModifier = 1; }
      if(snake2YDistance < 0){ yModifier = -1; }
      if(snake2YDistance > 0){ yModifier = 1; }
      if(checkNewVillainPosition(villain, (villain.x + xModifier) % 32, (villain.y + yModifier) % 32))
      {
        villain.x = (villain.x + xModifier) % 32;
        villain.y = (villain.y + yModifier) % 32;
      }             
    }
  }

  // Draw villain at new position
  if(villain.type == PARADROID)
  {
    setPixel((villain.x + 1) % 32, (villain.y) % 32, 0, 0, 2, VILLAIN);
    setPixel((villain.x) % 32, (villain.y + 1) % 32, 0, 0, 2, VILLAIN);
    if(counter % 8 < 4){ setPixel((villain.x + 1) % 32, (villain.y + 1) % 32, 4, 0, 0, VILLAIN); }
    else { setPixel((villain.x + 1) % 32, (villain.y + 1) % 32, 1, 0, 0, VILLAIN); }
    setPixel((villain.x + 2) % 32, (villain.y + 1) % 32, 0, 0, 2, VILLAIN);
    setPixel((villain.x + 1) % 32, (villain.y + 2) % 32, 0, 0, 2, VILLAIN);    
  }
  else if(villain.type == COPTER)
  {
    if(counter % 4 == 0){ setPixel((villain.x) % 32, (villain.y) % 32, 2, 2, 0, VILLAIN); }
    if(counter % 4 == 1){ setPixel((villain.x + 1) % 32, (villain.y) % 32, 2, 2, 0, VILLAIN); }
    if(counter % 4 == 2){ setPixel((villain.x + 2) % 32, (villain.y) % 32, 2, 2, 0, VILLAIN); }
    if(counter % 4 == 3){ setPixel((villain.x) % 32, (villain.y + 1) % 32, 2, 2, 0, VILLAIN); }
    setPixel((villain.x + 1) % 32, (villain.y + 1) % 32, 0, 0, 3, VILLAIN);
    if(counter % 4 == 3){ setPixel((villain.x + 2) % 32, (villain.y + 1) % 32, 2, 2, 0, VILLAIN); }
    if(counter % 4 == 2){ setPixel((villain.x) % 32, (villain.y + 2) % 32, 2, 2, 0, VILLAIN); }
    if(counter % 4 == 1){ setPixel((villain.x + 1) % 32, (villain.y + 2) % 32, 2, 2, 0, VILLAIN); }
    if(counter % 4 == 0){ setPixel((villain.x + 2) % 32, (villain.y + 2) % 32, 2, 2, 0, VILLAIN); }
  }
  else if(villain.type == ENERGYFIELD)
  {
    byte v = random(6);
    if(random(10) == 0){ setPixel((villain.x + 1) % 32, (villain.y) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 2) % 32, (villain.y) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 3) % 32, (villain.y) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x) % 32, (villain.y + 1) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 1) % 32, (villain.y + 1) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 2) % 32, (villain.y + 1) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 3) % 32, (villain.y + 1) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 4) % 32, (villain.y + 1) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x) % 32, (villain.y + 2) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 1) % 32, (villain.y + 2) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 2) % 32, (villain.y + 2) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 3) % 32, (villain.y + 2) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 4) % 32, (villain.y + 2) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x) % 32, (villain.y + 3) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 1) % 32, (villain.y + 3) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 2) % 32, (villain.y + 3) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 3) % 32, (villain.y + 3) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 4) % 32, (villain.y + 3) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 1) % 32, (villain.y + 4) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 2) % 32, (villain.y + 4) % 32, v, v, 5, VILLAIN); }
    if(random(10) == 0){ setPixel((villain.x + 3) % 32, (villain.y + 4) % 32, v, v, 5, VILLAIN); }
  }
  else if(villain.type == SNAKE)
  {
    if(villain.movingDirection == NORTH)
    { 
      setPixel((villain.x) % 32, (villain.y) % 32, 3, 3, 0, VILLAIN);
      setPixel((villain.x) % 32, (villain.y + 1) % 32, 1, 1, 0, VILLAIN);
      setPixel((villain.x) % 32, (villain.y + 2) % 32, 1, 1, 0, VILLAIN);
      setPixel((villain.x) % 32, (villain.y + 3) % 32, 1, 1, 0, VILLAIN);
    }
    if(villain.movingDirection == EAST)
    { 
      setPixel((villain.x + 3) % 32, (villain.y) % 32, 3, 3, 0, VILLAIN);
      setPixel((villain.x + 2) % 32, (villain.y) % 32, 1, 1, 0, VILLAIN);
      setPixel((villain.x + 1) % 32, (villain.y) % 32, 1, 1, 0, VILLAIN);
      setPixel((villain.x) % 32, (villain.y) % 32, 1, 1, 0, VILLAIN);
    }
    if(villain.movingDirection == SOUTH)
    { 
      setPixel((villain.x) % 32, (villain.y + 3) % 32, 3, 3, 0, VILLAIN);
      setPixel((villain.x) % 32, (villain.y + 2) % 32, 1, 1, 0, VILLAIN);
      setPixel((villain.x) % 32, (villain.y + 1) % 32, 1, 1, 0, VILLAIN);
      setPixel((villain.x) % 32, (villain.y) % 32, 1, 1, 0, VILLAIN);
    }
    if(villain.movingDirection == WEST)
    { 
      setPixel((villain.x) % 32, (villain.y) % 32, 3, 3, 0, VILLAIN);
      setPixel((villain.x + 1) % 32, (villain.y) % 32, 1, 1, 0, VILLAIN);
      setPixel((villain.x + 2) % 32, (villain.y) % 32, 1, 1, 0, VILLAIN);
      setPixel((villain.x + 3) % 32, (villain.y) % 32, 1, 1, 0, VILLAIN);
    }    
  }
  else if(villain.type == SENTINEL)
  {
    setPixel((villain.x + 1) % 32, (villain.y + 1) % 32, 0, 3, 0, VILLAIN);
    if(counter % 8 < 4)
    {
      setPixel((villain.x) % 32, (villain.y) % 32, 3, 0, 0, VILLAIN);
      setPixel((villain.x + 2) % 32, (villain.y) % 32, 0, 2, 0, VILLAIN);
      setPixel((villain.x) % 32, (villain.y + 2) % 32, 0, 2, 0, VILLAIN);
      setPixel((villain.x + 2) % 32, (villain.y + 2) % 32, 3, 0, 0, VILLAIN);
    }
    else
    {
      setPixel((villain.x) % 32, (villain.y) % 32, 0, 2, 0, VILLAIN);
      setPixel((villain.x + 2) % 32, (villain.y) % 32, 3, 0, 0, VILLAIN);
      setPixel((villain.x) % 32, (villain.y + 2) % 32, 3, 0, 0, VILLAIN);
      setPixel((villain.x + 2) % 32, (villain.y + 2) % 32, 0, 2, 0, VILLAIN);      
    }
  }
  else if(villain.type == GUMBALL)
  {
    if(villain.movingDirection == NORTHEAST)
    { 
      setPixel((villain.x + 1) % 32, (villain.y) % 32, 3, 0, 0, VILLAIN);
      setPixel((villain.x) % 32, (villain.y + 1) % 32, 1, 0, 0, VILLAIN);
    }
    if(villain.movingDirection == SOUTHEAST)
    { 
      setPixel((villain.x) % 32, (villain.y) % 32, 1, 0, 0, VILLAIN);
      setPixel((villain.x + 1) % 32, (villain.y + 1) % 32, 3, 0, 0, VILLAIN);
    }
    if(villain.movingDirection == SOUTHWEST)
    { 
      setPixel((villain.x + 1) % 32, (villain.y) % 32, 1, 0, 0, VILLAIN);
      setPixel((villain.x) % 32, (villain.y + 1) % 32, 3, 0, 0, VILLAIN);
    }
    if(villain.movingDirection == NORTHWEST)
    { 
      setPixel((villain.x) % 32, (villain.y) % 32, 3, 0, 0, VILLAIN);
      setPixel((villain.x + 1) % 32, (villain.y + 1) % 32, 1, 0, 0, VILLAIN);
    }    
  }
}

// moveAndDrawBarrier: Moves barrier (extra used by players)
void moveAndDrawBarrier(Barrier &barrier)
{
  if(barrier.active)
  { 
    if(barrier.counter == 0 || barrier.counter == 8)
    {   
      // Remove barrier at old position
      matrix.drawPixel(barrier.x, barrier.y, matrix.Color333(0, 0, 0));
      playfield[barrier.x][barrier.y] = EMPTY;
    }
    
    // Move barrier
    if(barrier.movingDirection == NORTH)
    {
      barrier.y = byte(barrier.y - 1) % 32;
    }
    else if(barrier.movingDirection == EAST)
    {
      barrier.x = byte(barrier.x + 1) % 32;
    }
    else if(barrier.movingDirection == SOUTH)
    {
      barrier.y = byte(barrier.y + 1) % 32;
    }
    else if(barrier.movingDirection == WEST)
    {
      barrier.x = byte(barrier.x - 1) % 32;
    }
    
    if(playfield[barrier.x][barrier.y] == EMPTY || playfield[barrier.x][barrier.y] == FOOD || playfield[barrier.x][barrier.y] == SPEED || playfield[barrier.x][barrier.y] == SHIELD || playfield[barrier.x][barrier.y] == BARRIER || playfield[barrier.x][barrier.y] == EXTRALIFE)
    {
      if(barrier.counter == 0)
      {
        matrix.drawPixel(barrier.x, barrier.y, matrix.Color333(3, 0, 3));
        // SOUND: Rolling barrier
        tone(audio, 100, 50);
        playfield[barrier.x][barrier.y] = WALL;
      }
      else
      {
        matrix.drawPixel(barrier.x, barrier.y, matrix.Color333(arenaColor.r, arenaColor.g, arenaColor.b));
        // SOUND: Barrier unrolls
        tone(audio, 200, 50);
        playfield[barrier.x][barrier.y] = WALL;
        barrier.counter--;
        if(barrier.counter == 0){ barrier.active = false; }        
      }      
    }
    else
    {
      barrier.counter = 0;
      barrier.active = false;    
    }
  }
}

// rollTheDiceForNewFood: Checks whether new food appears
boolean rollTheDiceForNewFood()
{
  if(random(probabilityThatFoodAppears) == 0)
  {
    byte foodX = random(32);
    byte foodY = random(32);
    if(playfield[foodX][foodY] == EMPTY)
    {
      playfield[foodX][foodY] = FOOD;
      matrix.drawPixel(foodX, foodY, matrix.Color333(1, 1, 1));
    }
  }
}

// rollTheDiceForNewExtra: Checks whether new extra appears
boolean rollTheDiceForNewExtra()
{
  if(random(probabilityThatExtraAppears) == 0)
  {
    byte extraX = random(32);
    byte extraY = random(32);
    if(playfield[extraX][extraY] == EMPTY)
    {
      byte extraType = random(3);
      if(extraType == (SPEED - 6))
      {
        playfield[extraX][extraY] = SPEED;
        matrix.drawPixel(extraX, extraY, matrix.Color333(3, 3, 0));        
      }
      if(extraType == (SHIELD - 6))
      {
        playfield[extraX][extraY] = SHIELD;
        matrix.drawPixel(extraX, extraY, matrix.Color333(0, 0, 3));                
      }
      if(extraType == (BARRIER - 6))
      {
        playfield[extraX][extraY] = BARRIER;
        matrix.drawPixel(extraX, extraY, matrix.Color333(3, 0, 3));                
      }
      if(numberOfPlayers == 1 && random(20) == 0 && !extraLifeAppeared)
      {
        playfield[extraX][extraY] = EXTRALIFE;
        extraLifeAppeared = true;
        matrix.drawPixel(extraX, extraY, matrix.Color333(3, 0, 0));                        
      }
    }
  }
}

// openPortal: Opens portal after enough food chunks have been eaten (only in 1-player mode)
void openPortal()
{
  do
  {
    portalX = random(32);
    portalY = random(32);
  }
  while(playfield[portalX][portalY] != EMPTY);
  playfield[portalX][portalY] = PORTAL;
  onePlayerGamePortalOpen = true;
}

// cutSnake: Cut a snake (happens if player has an active shield and collides with a segement which is not the head)
void cutSnake(byte x, byte y, Snake &snake)
{
  byte i = 1;
  do
  {
    if(x == snake.x[(snake.frontSegment + i) % maxSnakeLength] && y == snake.y[(snake.frontSegment + i) % maxSnakeLength])
    { 
      snake.numberOfSegments = i;
      break; 
    }
    i++;
  }
  while(i < snake.numberOfSegments);
}

// gameLoop: Main game loop (during gameplay)
void gameLoop()
{
  onePlayerGamePortalOpen = false;
  onePlayerGamePortalReached = false;
  boolean snake1Collision = false;
  boolean snake2Collision = false;
  unsigned long engineLoopStartPoint;
  byte elementOnNewFrontSegment = EMPTY;
  counter = 0;
  boolean firstCycle = true; // Required to have a short delay before starting the game (after that first cycle is set to false)
  reset = false;             // If true, go back to title.

  do
  { 
    // Get time of engine loop start point (important for a constant game speed)
    engineLoopStartPoint = millis();

    // Check whether new food or extras appear
    rollTheDiceForNewFood();
    rollTheDiceForNewExtra();

    // SOUND: Background sound
    if(!firstCycle)
    {
      byte multiplier = 1;
      if(snake1.extraInUse == SPEED || (numberOfPlayers == 2 && snake2.extraInUse == SPEED)){ multiplier = 2; }
      if(snake1.extraInUse == SHIELD || (numberOfPlayers == 2 && snake2.extraInUse == SHIELD))
      {
        if(counter % 4 == 0){ tone(audio, 400 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 4 == 2){ tone(audio, 200 * multiplier, (millisecondsPerFrame * 2) - 10); }      
      }
      else
      {
        if(counter % 32 == 0){ tone(audio, 60 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 2){ tone(audio, 70 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 4){ tone(audio, 80 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 6){ tone(audio, 90 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 8){ tone(audio, 100 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 10){ tone(audio, 110 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 12){ tone(audio, 120 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 14){ tone(audio, 130 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 16){ tone(audio, 140 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 18){ tone(audio, 130 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 20){ tone(audio, 120 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 22){ tone(audio, 110 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 24){ tone(audio, 100 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 26){ tone(audio, 90 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 28){ tone(audio, 80 * multiplier, (millisecondsPerFrame * 2) - 10); }
        if(counter % 32 == 30){ tone(audio, 70 * multiplier, (millisecondsPerFrame * 2) - 10); }
      }
    }

    // Open and show portal to leave arena (one player game)
    if(numberOfPlayers == 1 && snake1.numberOfSegments == 16 && !onePlayerGamePortalOpen){ openPortal(); }
    if(onePlayerGamePortalOpen)
    { 
      matrix.drawPixel(portalX, portalY, matrix.Color333(random(8), random(8), random(8)));
      playfield[portalX][portalY] = PORTAL;
    }

    // Move active villains
    for(byte i = 0; i < maxNumberOfVillains; i++)
    {
      if(villain[i].active){ moveAndDrawVillain(villain[i]); }
    }

    // First cycle delay (automatic delay so that players can orientate themselves)
    if(firstCycle)
    {
      delay(1800);
      firstCycle = false;
      tone(audio, 400, 200);
      delay(200);
    }

    // Move active barriers
    moveAndDrawBarrier(snake1.barrier);
    if(numberOfPlayers == 2){ moveAndDrawBarrier(snake2.barrier); }

    // Player 1
    if(counter % 2 == 0 || (snake1.extraInUse == SPEED && snake1.extraActiveCounter > 0))
    {
      // Check joystick
      if(joy1Up() && !joy1Left() && !joy1Right() && snake1.movingDirection != SOUTH)
      {
        snake1.movingDirection = NORTH;
      }
      else if(joy1Down() && !joy1Left() && !joy1Right() && snake1.movingDirection != NORTH)
      {
        snake1.movingDirection = SOUTH;
      }    
      else if(joy1Left() && !joy1Up() && !joy1Down() && snake1.movingDirection != EAST)
      {
        snake1.movingDirection = WEST;
      }
      else if(joy1Right() && !joy1Up() && !joy1Down() && snake1.movingDirection != WEST)
      {
        snake1.movingDirection = EAST;
      }
      if(joy1Fire())
      {        
        if(snake1.extraActiveCounter > 0)
        {
          snake1.extraActiveCounter--;
          if(snake1.extraActiveCounter == 0){ snake1.extraInUse = EMPTY; }
          if(snake1.extraActiveCounter == 0 && snake1.barrier.active)
          {
            snake1.barrier.active = false;
            matrix.drawPixel(snake1.barrier.x, snake1.barrier.y, matrix.Color333(0, 0, 0));
            playfield[snake1.barrier.x][snake1.barrier.y] = EMPTY;
          }
        }
        if(snake1.extraInUse == 0 && snake1.collectedExtra != 0)
        {
          snake1.extraInUse = snake1.collectedExtra;
          snake1.collectedExtra = EMPTY;
          snake1.extraActiveCounter = 32;
          if(snake1.extraInUse == BARRIER && snake1.barrier.counter == 0){ initializeBarrier(snake1.barrier, snake1.x[snake1.frontSegment], snake1.y[snake1.frontSegment], snake1.movingDirection); }
        }
      }
      else
      {
        snake1.extraActiveCounter = 0;
        snake1.extraInUse = EMPTY;
        if(snake1.barrier.active && snake1.barrier.counter == 0)
        {
          snake1.barrier.counter = 8;
        }
      }

      // Move player 1 snake
      elementOnNewFrontSegment = moveSnake(snake1);

      // Check collsion with walls, villains, food, extras, etc.
      if(elementOnNewFrontSegment == EMPTY){ redrawSnake(snake1, GREEN); }
      else if(elementOnNewFrontSegment == FOOD)
      { 
        if(snake1.numberOfSegments < maxSnakeLength)
        { 
          snake1.numberOfSegments++;
          overallEatenFood++;
        }
        tone(audio, 400, 50);
        redrawSnake(snake1, GREEN); 
      }
      else if(elementOnNewFrontSegment == SPEED)
      { 
        snake1.collectedExtra = SPEED;
        tone(audio, 800, 50);
        redrawSnake(snake1, GREEN); 
      }
      else if(elementOnNewFrontSegment == SHIELD)
      { 
        snake1.collectedExtra = SHIELD;
        tone(audio, 800, 50);
        redrawSnake(snake1, GREEN); 
      }
      else if(elementOnNewFrontSegment == BARRIER)
      { 
        snake1.collectedExtra = BARRIER;
        tone(audio, 800, 50);
        redrawSnake(snake1, GREEN); 
      }
      else if(elementOnNewFrontSegment == EXTRALIFE) // Appear only in 1-player game
      { 
        lives++;;
        tone(audio, NOTE_C4, 75);
        delay(75);
        tone(audio, NOTE_C3, 75);
        delay(125);
        tone(audio, NOTE_E4, 75);
        delay(75);
        tone(audio, NOTE_E3, 75);
        delay(125);
        tone(audio, NOTE_G4, 75);
        delay(75);
        tone(audio, NOTE_G3, 75);
        delay(125);
        tone(audio, NOTE_C5, 150);        
        delay(150);
        tone(audio, NOTE_C4, 150);        
        delay(250);
        redrawSnake(snake1, GREEN); 
      }
      else if(elementOnNewFrontSegment == PORTAL)
      { 
        onePlayerGamePortalReached = true;
        redrawSnake(snake1, GREEN); 
      }
      else if(elementOnNewFrontSegment == snake1.id || elementOnNewFrontSegment == snake2.id || elementOnNewFrontSegment == WALL || elementOnNewFrontSegment == VILLAIN)
      { 
        if(snake1.extraInUse == SHIELD && snake1.extraActiveCounter > 0)
        {
          snake1.extraActiveCounter = 0;
          snake1.extraInUse = 0;
          // Cut snake
          if(elementOnNewFrontSegment == snake1.id){ cutSnake(snake1.x[snake1.frontSegment], snake1.y[snake1.frontSegment], snake1); }         
          if(elementOnNewFrontSegment == snake2.id){ cutSnake(snake1.x[snake1.frontSegment], snake1.y[snake1.frontSegment], snake2); }         
          redrawSnake(snake1, GREEN); 
        }
        else
        {
          snake1Collision = true;
          redrawSnake(snake1, GREEN); 
        }
      }
    }

    // Player 2
    if(numberOfPlayers == 2)
    {
      if(counter % 2 == 0 || (snake2.extraInUse == SPEED && snake2.extraActiveCounter > 0))
      {
        // Check joystick
        if(joy2Up() && !joy2Left() && !joy2Right() && snake2.movingDirection != SOUTH)
        {
          snake2.movingDirection = NORTH;
        }
        else if(joy2Down() && !joy2Left() && !joy2Right() && snake2.movingDirection != NORTH)
        {
          snake2.movingDirection = SOUTH;
        }    
        else if(joy2Left() && !joy2Up() && !joy2Down() && snake2.movingDirection != EAST)
        {
          snake2.movingDirection = WEST;
        }
        else if(joy2Right() && !joy2Up() && !joy2Down() && snake2.movingDirection != WEST)
        {
          snake2.movingDirection = EAST;
        }      
        if(joy2Fire())
        {        
          if(snake2.extraActiveCounter > 0)
          {
            snake2.extraActiveCounter--;
            if(snake2.extraActiveCounter == 0){ snake2.extraInUse = EMPTY; }
            if(snake2.extraActiveCounter == 0 && snake2.barrier.active)
            {
              snake2.barrier.active = false;
              matrix.drawPixel(snake2.barrier.x, snake2.barrier.y, matrix.Color333(0, 0, 0));
              playfield[snake2.barrier.x][snake2.barrier.y] = EMPTY;
            }
          }
          if(snake2.extraInUse == 0 && snake2.collectedExtra != 0)
          {
            snake2.extraInUse = snake2.collectedExtra;
            snake2.collectedExtra = EMPTY;
            snake2.extraActiveCounter = 32;
            if(snake2.extraInUse == BARRIER && snake2.barrier.counter == 0){ initializeBarrier(snake2.barrier, snake2.x[snake2.frontSegment], snake2.y[snake2.frontSegment], snake2.movingDirection); }
          }
        }
        else
        {
          snake2.extraActiveCounter = 0;
          snake2.extraInUse = EMPTY;
          if(snake2.barrier.active && snake2.barrier.counter == 0)
          {
            snake2.barrier.counter = 8;
          }
        }

        // Move player 2 snake
        elementOnNewFrontSegment = moveSnake(snake2);

        // Check collsion with walls, villains, food, extras, etc.
        if(elementOnNewFrontSegment == EMPTY){ redrawSnake(snake2, TURQUOISE); }
        else if(elementOnNewFrontSegment == FOOD)
        { 
          if(snake2.numberOfSegments < maxSnakeLength){ snake2.numberOfSegments++; }
          tone(audio, 400, 50);
          redrawSnake(snake2, TURQUOISE); 
        }
        else if(elementOnNewFrontSegment == SPEED)
        { 
          snake2.collectedExtra = SPEED;
          tone(audio, 800, 50);
          redrawSnake(snake2, TURQUOISE); 
        }
        else if(elementOnNewFrontSegment == SHIELD)
        { 
          snake2.collectedExtra = SHIELD;
          tone(audio, 800, 50);
          redrawSnake(snake2, TURQUOISE); 
        }
        else if(elementOnNewFrontSegment == BARRIER)
        { 
          snake2.collectedExtra = BARRIER;
          tone(audio, 800, 50);
          redrawSnake(snake2, TURQUOISE); 
        }
        else if(elementOnNewFrontSegment == snake1.id || elementOnNewFrontSegment == snake2.id || elementOnNewFrontSegment == WALL || elementOnNewFrontSegment == VILLAIN)
        {
          if(snake2.extraInUse == SHIELD && snake2.extraActiveCounter > 0)
          {
            snake2.extraActiveCounter = 0;
            snake2.extraInUse = 0;          
            // Cut snake
            if(elementOnNewFrontSegment == snake1.id){ cutSnake(snake2.x[snake2.frontSegment], snake2.y[snake2.frontSegment], snake1); }         
            if(elementOnNewFrontSegment == snake2.id){ cutSnake(snake2.x[snake2.frontSegment], snake2.y[snake2.frontSegment], snake2); }         
            redrawSnake(snake2, TURQUOISE); 
          }
          else
          {
            snake2Collision = true;          
            redrawSnake(snake2, TURQUOISE); 
          }
        }
      }
    }

    // Draw in case that both collide with their heads (even if one has a shield)
    if(numberOfPlayers == 2)
    {
      if(snake1.x[snake1.frontSegment] == snake2.x[snake2.frontSegment] && snake1.y[snake1.frontSegment] == snake2.y[snake2.frontSegment])
      {
         snake1Collision = true;
         snake2Collision = true;
      }
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
      while((!digitalRead(buttonPause) == LOW) && (!joy1Fire()) && (!joy2Fire()));
      tone(audio,1024,20);
      delay(500);
    }
    
    delay(15);
    do
    {
    }
    while(abs(millis() - engineLoopStartPoint) < millisecondsPerFrame);

    counter++;
  }
  while(!snake1Collision && !snake2Collision && !onePlayerGamePortalReached);

  if(!reset)
  {
    // Explosion or next arena (1-player mode)
    if(numberOfPlayers == 1)
    {
      if(snake1Collision)
      { 
        snakeExplosion(true, false);
        lives--;
        showLives();
      }
      if(onePlayerGamePortalReached)
      {
        snakeGoesThroughPortal(snake1);
        arena++;
        if(arena > numberOfArenas)
        { 
          arena = 1;
          difficulty++;
          if(difficulty == 4){ difficulty = HARD; } 
        }  
      }
    }
    // Update of score (2-player mode)
    else if(numberOfPlayers == 2)
    { 
      if(snake1Collision)
      { 
        snake2Score++;
      }
      if(snake2Collision)
      { 
        snake1Score++;
      }
      snakeExplosion(snake1Collision, snake2Collision);
      showScore();
    }
  } 
}

// snakeGoesThroughPortal: Animation if snake moves to portal (only 1-player mode)
void snakeGoesThroughPortal(Snake &snake)
{
  for(byte i = 0; i < snake.numberOfSegments; i++)
  {
    matrix.drawPixel(snake.x[(snake.frontSegment + snake.numberOfSegments - i - 1) % maxSnakeLength], snake.y[(snake.frontSegment + snake.numberOfSegments - i - 1) % maxSnakeLength], matrix.Color333(0, 0, 0));
    matrix.drawPixel(snake.x[snake.frontSegment], snake.y[snake.frontSegment], matrix.Color333(random(8), random(8), random(8)));
    // SOUND: Snake goes through portal
    tone(audio, 400 + i * 50, 40);
    delay(millisecondsPerFrame);
    matrix.drawPixel(snake.x[snake.frontSegment], snake.y[snake.frontSegment], matrix.Color333(random(8), random(8), random(8)));
    delay(millisecondsPerFrame);
  }
  for(int i = 7; i >= 0; i--)
  {
    matrix.drawPixel(snake.x[snake.frontSegment], snake.y[snake.frontSegment], matrix.Color333(i, i, i));
    delay(millisecondsPerFrame);
  }
  delay(1000);
}

// showLives: Shows remaining lives
void showLives()
{
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.setCursor(13, 14);
  matrix.println(lives);
  delay(500);
  
  matrix.setCursor(13, 14);
  matrix.setTextColor(matrix.Color333(0, 0, 0));
  matrix.println(lives);
  matrix.setTextSize(2);
  matrix.setCursor(11, 10);
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.println(lives);
  delay(2500);

  matrix.setTextSize(1);

  if(lives == 0)
  {
    matrix.setTextSize(2);
    matrix.setCursor(11, 10);
    matrix.setTextColor(matrix.Color333(0, 0, 0));
    matrix.println(lives);
    matrix.setTextSize(1);    
    matrix.setCursor(4, 8);
    matrix.setTextColor(matrix.Color333(3, 0, 0));
    matrix.println("Game");
    matrix.setCursor(4, 16);
    matrix.println("Over");
    delay(3000);

    matrix.setCursor(4, 8);
    matrix.setTextColor(matrix.Color333(0, 0, 0));
    matrix.println("Game");
    matrix.setCursor(4, 16);
    matrix.println("Over");

    matrix.setCursor(1, 8);
    matrix.setTextColor(matrix.Color333(3, 2, 0));
    matrix.println("Score");
    matrix.setCursor(7, 16);
    matrix.println(overallEatenFood);

    delay(3000);  
  }
}

// showScore: Shows score
void showScore()
{

  matrix.setCursor(7, 12);
  matrix.setTextColor(matrix.Color333(0, 3, 0));
  matrix.print(snake1Score);
  matrix.setTextColor(matrix.Color333(2, 2, 0));
  matrix.print(":");
  matrix.setTextColor(matrix.Color333(0, 3, 3));
  matrix.print(snake2Score);
  delay(3000);

  if(snake1Score == 3)
  {
    matrix.setCursor(7, 12);
    matrix.setTextColor(matrix.Color333(0, 0, 0));
    matrix.print(snake1Score);    
    matrix.setTextSize(2);
    matrix.setCursor(2, 8);
    matrix.setTextColor(matrix.Color333(0, 3, 0));
    matrix.print(snake1Score);    
    matrix.setTextSize(1);    
    delay(3000);
  }
  if(snake2Score == 3)
  {
    matrix.setCursor(19, 12);
    matrix.setTextColor(matrix.Color333(0, 0, 0));
    matrix.print(snake2Score);    
    matrix.setTextSize(2);
    matrix.setCursor(20, 8);
    matrix.setTextColor(matrix.Color333(0, 3, 3));
    matrix.print(snake2Score);    
    matrix.setTextSize(1);    
    delay(3000);
  }
}

// initParticle: Initializes particle
boolean initParticle(byte nextParticle, byte x, byte y)
{
  if(!particle[nextParticle].active)
  {
    particle[nextParticle].active = true;
    particle[nextParticle].x = x;
    particle[nextParticle].y = y;
    int angle = random(360);
    particle[nextParticle].xDirection = sin(angle);
    particle[nextParticle].yDirection = cos(angle);
    particle[nextParticle].age = 0;
    return true;
  }
  return false;
}

// snakeExplosion: Shows explosion animation
void snakeExplosion(boolean snake1Collision, boolean snake2Collision)
{
  for(byte i = 0; i < numberOfParticles; i++)
  {
    particle[i].active = false;
    particle[i].x = 0.0;
    particle[i].y = 0.0;
    particle[i].xDirection = 0.0;
    particle[i].yDirection = 0.0;
    particle[i].age = 0;
  }

  byte snake1SegmentCounter = 0;
  byte snake2SegmentCounter = 0;
  byte snake1TimeCounter = 8;
  byte snake2TimeCounter = 8;
  byte nextParticle = 0;
  do
  {
    // Remove explosion flash
    if(snake1Collision && snake1SegmentCounter > 0) // No explosion flash available for segmentCounter == 0
    {
      matrix.fillCircle(snake1.x[(snake1.frontSegment + snake1SegmentCounter - 1) % maxSnakeLength], snake1.y[(snake1.frontSegment + snake1SegmentCounter - 1) % maxSnakeLength], 2, matrix.Color333(0, 0, 0));
    }
    if(snake2Collision && snake2SegmentCounter > 0) // No explosion flash available for segmentCounter == 0
    {
      matrix.fillCircle(snake2.x[(snake2.frontSegment + snake2SegmentCounter - 1) % maxSnakeLength], snake2.y[(snake2.frontSegment + snake2SegmentCounter - 1) % maxSnakeLength], 2, matrix.Color333(0, 0, 0));
    }

    drawSnake(snake1, snake1SegmentCounter, GREEN);
    if(numberOfPlayers == 2){ drawSnake(snake2, snake2SegmentCounter, TURQUOISE); }

    for(byte i = 0; i < numberOfParticles; i++)
    {
      if(particle[i].active)
      {
        // Remove old position;
        matrix.drawPixel(particle[i].x, particle[i].y, matrix.Color333(0, 0, 0));

        // Calculate new position
        particle[i].x = particle[i].x + particle[i].xDirection;
        particle[i].y = particle[i].y + particle[i].yDirection;

        // Decrease particle speed
        particle[i].xDirection = particle[i].xDirection * 0.975;
        particle[i].yDirection = particle[i].yDirection * 0.975;
        
        // SOUND: Explosion (every 4th particle)
        if(i % 4 == 0 && particle[i].age < 6)
        {
          tone(audio, (350 + random(200)) / pow(2, particle[i].age), 40);
        }

        // Update age
        particle[i].age++;

        // Deactivate particle if out of sight
        if(int(particle[i].x) < 0 || int(particle[i].x) > 31 || int(particle[i].y) < 0 || int(particle[i].y) > 31){ particle[i].active = false; }
        
        // Draw new position
        byte modifier = particle[i].age;
        if(modifier > 12){ modifier = 12; }
        if(particle[i].active && random(particle[i].age) < 6 ){ matrix.drawPixel(particle[i].x, particle[i].y, matrix.Color333(7 - (modifier / 2), 7 - (modifier / 2), max(0, 7 - modifier))); }
      }
    }

    // Next segement explodes
    if(snake1Collision && snake1TimeCounter == 8 && snake1SegmentCounter < snake1.numberOfSegments)
    {
      matrix.fillCircle(snake1.x[(snake1.frontSegment + snake1SegmentCounter) % maxSnakeLength], snake1.y[(snake1.frontSegment + snake1SegmentCounter) % maxSnakeLength], 2, matrix.Color333(7, 7, 7));
      if(initParticle(nextParticle, snake1.x[(snake1.frontSegment + snake1SegmentCounter) % maxSnakeLength], snake1.y[(snake1.frontSegment + snake1SegmentCounter) % maxSnakeLength])){ nextParticle++; }
      if(nextParticle == numberOfParticles){ nextParticle = 0; }            
      if(initParticle(nextParticle, snake1.x[(snake1.frontSegment + snake1SegmentCounter) % maxSnakeLength], snake1.y[(snake1.frontSegment + snake1SegmentCounter) % maxSnakeLength])){ nextParticle++; }
      if(nextParticle == numberOfParticles){ nextParticle = 0; }            
      if(initParticle(nextParticle, snake1.x[(snake1.frontSegment + snake1SegmentCounter) % maxSnakeLength], snake1.y[(snake1.frontSegment + snake1SegmentCounter) % maxSnakeLength])){ nextParticle++; }
      if(nextParticle == numberOfParticles){ nextParticle = 0; }            
      if(initParticle(nextParticle, snake1.x[(snake1.frontSegment + snake1SegmentCounter) % maxSnakeLength], snake1.y[(snake1.frontSegment + snake1SegmentCounter) % maxSnakeLength])){ nextParticle++; }
      if(nextParticle == numberOfParticles){ nextParticle = 0; }            
      snake1TimeCounter = random(5);
      snake1SegmentCounter++;
    }
    if(snake2Collision && snake2TimeCounter == 8 && snake2SegmentCounter < snake2.numberOfSegments)
    {
      matrix.fillCircle(snake2.x[(snake2.frontSegment + snake2SegmentCounter) % maxSnakeLength], snake2.y[(snake2.frontSegment + snake2SegmentCounter) % maxSnakeLength], 2, matrix.Color333(7, 7, 7));
      if(initParticle(nextParticle, snake2.x[(snake2.frontSegment + snake2SegmentCounter) % maxSnakeLength], snake2.y[(snake2.frontSegment + snake2SegmentCounter) % maxSnakeLength])){ nextParticle++; }
      if(nextParticle == numberOfParticles){ nextParticle = 0; }            
      if(initParticle(nextParticle, snake2.x[(snake2.frontSegment + snake2SegmentCounter) % maxSnakeLength], snake2.y[(snake2.frontSegment + snake2SegmentCounter) % maxSnakeLength])){ nextParticle++; }
      if(nextParticle == numberOfParticles){ nextParticle = 0; }            
      if(initParticle(nextParticle, snake2.x[(snake2.frontSegment + snake2SegmentCounter) % maxSnakeLength], snake2.y[(snake2.frontSegment + snake2SegmentCounter) % maxSnakeLength])){ nextParticle++; }
      if(nextParticle == numberOfParticles){ nextParticle = 0; }            
      if(initParticle(nextParticle, snake2.x[(snake2.frontSegment + snake2SegmentCounter) % maxSnakeLength], snake2.y[(snake2.frontSegment + snake2SegmentCounter) % maxSnakeLength])){ nextParticle++; }
      if(nextParticle == numberOfParticles){ nextParticle = 0; }            
      snake2TimeCounter = random(5);
      snake2SegmentCounter++;
    }
    
    delay(20);
    snake1TimeCounter++;
    snake2TimeCounter++;
  }
  while(snake1TimeCounter < 64 || snake2TimeCounter < 64);
}

// loop: Overall loop
void loop()
{
  title();
  selectNumberOfPlayers();
  if(numberOfPlayers == 1)
  {
    selectDifficulty();
    if(difficulty == EASY){ arena = 1; }
    else if(difficulty == NORMAL){ arena = 5; }
    else if(difficulty == HARD){ arena = 9; }
    if(difficulty != EASY)
    {
      matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
      matrix.setTextColor(matrix.Color333(3, 2, 0));
      matrix.setCursor(4, 8);
      matrix.println("Warp");
      matrix.setCursor(1, 16);
      matrix.println("to...");
      delay(2000);
    }
    lives = 3;
    overallEatenFood = 0;
    do
    {
      showArena();
      initializeGame();
    }
    while(lives != 0 && !reset);
  }
  else if(numberOfPlayers == 2)
  {
    selectDifficulty();
    selectArena();
    selectVillains();
    snake1Score = 0;
    snake2Score = 0;
    do
    {
      if(arenaSelection == RANDOM){ arena = random(numberOfArenas) + 1; }
      showArena();
      initializeGame();
    }
    while(snake1Score < 3 && snake2Score < 3 && !reset);
  }
}

