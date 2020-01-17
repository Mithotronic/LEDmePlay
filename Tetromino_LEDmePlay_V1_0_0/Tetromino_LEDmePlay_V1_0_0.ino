// Tetromino
// for the LEDmePlay
//
// www.mithotronic.de
//
// Version: 1.0.0
// Author: Thomas Laubach and Michael Rosskopf (2019)
// 
// Release Notes:
// V1.0.0: First release

// Include libraries for adafruit matrix
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <avr/pgmspace.h>   // Necessary to maintain data in program memory
#include <Timer.h>          // Enables timed events

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

byte playfield[10][20]; // Playfield to store information where tetrominos are present

// Variables for game music
int korobeinikiLength = 234;
int korobeiniki[] ={
      NOTE_E4, 4,
      NOTE_B3, 2,
      NOTE_C4, 2,
      NOTE_D4, 2,
      NOTE_E4, 1,
      NOTE_D4, 1,
      NOTE_C4, 2,
      NOTE_B3, 2,
      NOTE_A3, 4,
      NOTE_A3, 2,
      
      NOTE_C4, 2,
      NOTE_E4, 4,
      NOTE_D4, 2,
      NOTE_C4, 2,
      NOTE_B3, 4,
      NOTE_B3, 1,
      NOTE_B3, 1,
      NOTE_C4, 2,
      NOTE_D4, 4,
      NOTE_E4, 4,
      
      NOTE_C4, 4,
      NOTE_A3, 4,
      NOTE_A3, 8,
      NOTE_D4, 4,
      NOTE_D4, 2,
      NOTE_F4, 2, 
      NOTE_A4, 4,
      NOTE_G4, 2,
      NOTE_F4, 2,
      NOTE_E4, 4,
      
      NOTE_E4, 2,
      NOTE_C4, 2,
      NOTE_E4, 4,
      NOTE_D4, 2,
      NOTE_C4, 2,
      NOTE_B3, 4,
      NOTE_B3, 1,
      NOTE_B3, 1,
      NOTE_C4, 2,      
      NOTE_D4, 4,
      
      NOTE_E4, 4,
      NOTE_C4, 4,
      NOTE_A3, 4,
      NOTE_A3, 8,
      NOTE_E4, 4,
      NOTE_B3, 2,
      NOTE_C4, 2,
      NOTE_D4, 2,
      NOTE_E4, 1,
      NOTE_D4, 1,
      
      NOTE_C4, 2,
      NOTE_B3, 2,
      NOTE_A3, 4,
      NOTE_A3, 2,
      NOTE_C4, 2,
      NOTE_E4, 4,
      NOTE_D4, 2,
      NOTE_C4, 2,
      NOTE_B3, 4,
      NOTE_B3, 2,
      
      NOTE_C4, 2,
      NOTE_D4, 4,
      NOTE_E4, 4,
      NOTE_C4, 4,
      NOTE_A3, 4,
      NOTE_A3, 8,
      NOTE_D4, 4,
      NOTE_D4, 2,
      NOTE_F4, 2,
      NOTE_A4, 4,
      
      NOTE_G4, 2,
      NOTE_F4, 2,
      NOTE_E4, 4,
      NOTE_E4, 2,
      NOTE_C4, 2,
      NOTE_E4, 4,
      NOTE_D4, 2,
      NOTE_C4, 2,
      NOTE_B3, 4,
      NOTE_B3, 2,
      
      NOTE_C4, 2,
      NOTE_D4, 4,
      NOTE_E4, 4,
      NOTE_C4, 4,
      NOTE_A3, 4,
      NOTE_A3, 8,
      NOTE_C4, 8,
      NOTE_A3, 8,
      NOTE_B3, 8,
      NOTE_GS3, 8,
      
      NOTE_A3, 8,
      NOTE_E3, 8,
      NOTE_E3, 8,
      NOTE_GS3, 8,
      NOTE_C4, 8,
      NOTE_A3, 8,
      NOTE_B3, 8,
      NOTE_GS3, 8,
      NOTE_A3, 4,
      NOTE_C4, 4,
      
      NOTE_E4, 4,
      NOTE_E4, 4,
      NOTE_E4, 16,
      NOTE_B3, 4,
      NOTE_GS3, 2,
      NOTE_A3, 2,
      NOTE_B3, 2,
      NOTE_E4, 1,
      NOTE_D4, 1,
      NOTE_A3, 2,
      
      NOTE_GS3, 2,
      NOTE_E3, 4,
      NOTE_E3, 2,
      NOTE_A3, 2,
      NOTE_C4, 4,
      NOTE_B3, 2,
      NOTE_A3, 2      
};

// Variables for the music player
Timer melodyPlayer;
boolean musicOn;
int stepsPerEightNote;
byte noteLength;
byte halfLength;
byte notePointer;

// Game type (the mode depends also on the number of players)
#define GameA 0
#define GameB 1
byte gameType;

byte numberOfPlayers; // 1 or 2 (2 only if 2 LEDmePlays are connected via X-Link)

// Definition of tetrominos (the letters correspond rougly to the shape of the tetrominos)
#define I 1
#define O 2
#define T 3
#define L 4
#define J 5
#define S 6
#define Z 7

// Definition of rotation positions
#define North 1
#define East  2
#define South 3
#define West  4

// Structure to describe a tetromino
struct Tetromino
{
  byte type; 
  int xPosition[4];
  int yPosition[4];
  byte rotation;         // This is North, East, South or West
  byte colorR;
  byte colorG;
  byte colorB;
  boolean active;
};
Tetromino tetromino;
Tetromino nextTetromino; // Used for preview

boolean reset;           // Back to title if set to true somewhere in the game
boolean gameOver;        // Game ends if set to true somewhere in the game

byte level;              // Current level
byte preselectedLevel;   // Level set in the game menu (you can start in level 1 to 9)
byte rows;               // Cleared rows in current level
int rowsOverall;         // Cleared rows in running game

int singles;             // Number of singles in current game
int doubles;             // Number of doubles in current game (2 rows cleared at once)
int triples;             // Number of triples in current game (3 rows cleared at once)
int quadruples;          // Number of quadruples in current game (4 rows cleared at once)

int score;               // Current score
int hiScoreA = 123;      // Highscore in game type A
int hiScoreB = 123;      // Highscore in game type B

int fallingByOneRowInterval; // Number of cycles in the main loop before the current tetromino falls by one row
int controlInterval;         // Number of cycles in the main loop before the next user action (movement or rotation) canbe performed

boolean fastFallingEnabled;  // Status to avoid that player drops all tetrominos by accident

// Game synchronization
unsigned long engineLoopStartPoint; // Contains millisconds since start of LEDmePlay at begin of each engine loop

// X-Link variables

// Define communication symbols for X-link
#define IDLESTATE     255
#define SETPLAYER2   254 // Used for setup a 2-player game
#define PLAYER2READY 253 // Used for setup a 2-player game
#define EXITLINKMODE 252 // Used for setup a 2-player game
#define TERMINATION  251 // Used at the end of data packages sent during game
#define WIN          250 // Payload of data packages sent during game
#define LOSE         249 // Payload of data packages sent during game
#define SEND1ROW     248 // Payload of data packages sent during game
#define SEND2ROWS    247 // Payload of data packages sent during game
#define SEND4ROWS    246 // Payload of data packages sent during game

byte readBuffer[6]; // Buffer to read bytes from connected LEDmePlay
byte player;        // Player 1 or 2 (1 is the master synchronizing the game music)
byte payload = 0;   // Symbol from the list above
byte addRows = 0;   // Number of rows to be added to own playfield (sent from other player)
byte gapColumn;     // Position of the gap within the sent rows (randomly defined at the beginning of each game)

#define WINNER 1
#define LOSER  2
byte twoPlayerResult = 0;

void setup() 
{
  // Initialize serial connection
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
  melodyPlayer.every(50, playMelody);

  gameType = GameA;
  preselectedLevel = 1;
  numberOfPlayers = 1;
  
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



// Helping procedures

// playMelody: Timer controlled music player for the Korobeiniki melody
void playMelody()
{
  if(musicOn)
  {
    if(noteLength == 0)
    {
      tone(audio, korobeiniki[notePointer], korobeiniki[notePointer + 1] * 25);
      noteLength = (korobeiniki[notePointer + 1]) * 2;
      halfLength = noteLength / 2;
    }
    if(noteLength == halfLength)
    {
      tone(audio, korobeiniki[notePointer] / 2, korobeiniki[notePointer + 1] * 25);      
      if(player == 1)
      { 
        notePointer = notePointer + 2;
        
        // Send sync signal in x-link mode
        if(numberOfPlayers == 2){ xLinkWrite(0); }
      }
      if(notePointer >= korobeinikiLength){ notePointer = 0; }
    }
    noteLength--;
  }
}

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
  for(byte i = 0; i < 20; i++)
  {
    tone(audio, 80 + 80 * i, 2);
    delay(1);   
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



// Title screen, menu, and results

// title: Title screen
void title()
{
  int counter = 0;

  // Send IDLESTATE into the buffer of a potentially connected LEDmePlay
  Serial1.write(IDLESTATE);

  do
  {
    if(counter == 0)
    {
      // Clear screen
      matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

      // Text
      matrix.setTextColor(matrix.Color333(2, 2, 2));
      matrix.setCursor(10, 2);
      matrix.println("te");
      matrix.setCursor(7, 10);
      matrix.println("tro");
      matrix.setCursor(4, 18);
      matrix.println("mino");
    
      drawSquare(6, 2, 0, 1, 1, true);
      drawSquare(6, 4, 0, 1, 1, true);
      drawSquare(6, 6, 0, 1, 1, true);
      drawSquare(4, 6, 0, 1, 1, true);
    
      drawSquare(24, 2, 1, 0, 1, true);
      drawSquare(24, 4, 1, 0, 1, true);
      drawSquare(24, 6, 1, 0, 1, true);
      drawSquare(26, 6, 1, 0, 1, true);

      drawSquare(3, 12, 1, 1, 1, true);
      drawSquare(3, 14, 1, 1, 1, true);
      drawSquare(1, 14, 1, 1, 1, true);
      drawSquare(1, 16, 1, 1, 1, true);

      drawSquare(27, 12, 1, 1, 0, true);
      drawSquare(27, 14, 1, 1, 0, true);
      drawSquare(29, 14, 1, 1, 0, true);
      drawSquare(29, 16, 1, 1, 0, true);

      drawSquare(2, 28, 0, 1, 0, true);
      drawSquare(4, 28, 0, 1, 0, true);
      drawSquare(4, 26, 0, 1, 0, true);
      drawSquare(6, 28, 0, 1, 0, true);

      drawSquare(13, 27, 1, 0, 0, true);
      drawSquare(15, 27, 1, 0, 0, true);
      drawSquare(13, 29, 1, 0, 0, true);
      drawSquare(15, 29, 1, 0, 0, true);

      drawSquare(22, 28, 0, 0, 1, true);
      drawSquare(24, 28, 0, 0, 1, true);
      drawSquare(26, 28, 0, 0, 1, true);
      drawSquare(28, 28, 0, 0, 1, true);
    }

    if(counter == 4000)
    {
      // Clear screen
      matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
      
      matrix.setTextColor(matrix.Color333(2, 2, 2));
      matrix.setCursor(4, 0);
      matrix.print("Hi A");
      if(hiScoreA < 10)       { matrix.setCursor(13, 8); }
      else if(hiScoreA < 100) { matrix.setCursor(10, 8); }
      else if(hiScoreA < 1000){ matrix.setCursor(7, 8); }
      else if(hiScoreA < 10000){ matrix.setCursor(4, 8); }
      else                   { matrix.setCursor(1, 8);  }
      matrix.setTextColor(matrix.Color333(0, 0, 3));
      matrix.print(hiScoreA);

      matrix.setTextColor(matrix.Color333(2, 2, 2));
      matrix.setCursor(4, 16);
      matrix.print("Hi B");
      if(hiScoreB < 10)       { matrix.setCursor(13, 24); }
      else if(hiScoreB < 100) { matrix.setCursor(10, 24); }
      else if(hiScoreB < 1000){ matrix.setCursor(7, 24); }
      else if(hiScoreB < 10000){ matrix.setCursor(4, 24); }
      else                   { matrix.setCursor(1, 24);  }
      matrix.setTextColor(matrix.Color333(0, 0, 3));
      matrix.print(hiScoreB);
    }
    
    counter++;
    if(counter == 6000){ counter = 0; }
    random(100);
    delay(1);
  }
  while(!buttonFire1Clicked(200));
}

// menu: Show menu to select the game mode, the level, and the number of players
void menu()
{
  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Send IDLESTATE into the buffer of a potentially connected LEDmePlay
  Serial1.write(IDLESTATE);

  boolean updateMenu = true;
  boolean startGame = false;
  
  byte menuPointer = 3;

  matrix.setTextColor(matrix.Color333(2, 2, 2));

  matrix.setCursor(4, 0);
  matrix.println("Menu");
  matrix.setCursor(7, 8);
  matrix.println("Typ");
  matrix.setCursor(7, 16);
  matrix.println("Lvl");
  matrix.setCursor(7, 24);
  matrix.println("1");
  matrix.setCursor(19, 24);
  matrix.println("2");

  matrix.setTextColor(matrix.Color333(0, 0, 3));
  
  do
  {
    if(updateMenu)
    {
      matrix.fillRect(1, 8, 6, 7, matrix.Color333(0, 0, 0));
      matrix.fillRect(1, 16, 6, 7, matrix.Color333(0, 0, 0));
      matrix.fillRect(1, 24, 6, 7, matrix.Color333(0, 0, 0));
      matrix.fillRect(13, 24, 6, 7, matrix.Color333(0, 0, 0));
      
      if(menuPointer == 1){ matrix.setCursor(1, 8); }
      else if(menuPointer == 2){ matrix.setCursor(1, 16); }
      else if(menuPointer == 3){ matrix.setCursor(1, 24); }
      else if(menuPointer == 4){ matrix.setCursor(13, 24); }
      matrix.println(">");        

      matrix.fillRect(25, 8, 6, 7, matrix.Color333(0, 0, 0));
      if(gameType == GameA)
      {
        matrix.setCursor(25, 8);
        matrix.print("A");
      }
      else if(gameType == GameB)
      {
        matrix.setCursor(25, 8);
        matrix.print("B");        
      }

      matrix.fillRect(25, 16, 6, 7, matrix.Color333(0, 0, 0));
      matrix.setCursor(25, 16);
      matrix.print(preselectedLevel);

      updateMenu = false;
      
      delay(200);
    }

    if(joy1Up())
    {
      if(menuPointer > 1)
      { 
        menuPointer--;
        if(menuPointer == 3){ menuPointer = 2; }
        clickSound();
        updateMenu = true;
      }
    }
    else if(joy1Down())
    {
      if(menuPointer < 3)
      { 
        menuPointer++;
        if(menuPointer == 3 && numberOfPlayers == 2){ menuPointer = 4; }
        clickSound();
        updateMenu = true;
      }      
    }
    else if(joy1Left())
    {
      if(menuPointer == 4)
      {
        menuPointer = 3;
        numberOfPlayers = 1;
        clickSound();
        updateMenu = true;
      }
    }
    else if(joy1Right())
    {
      if(menuPointer == 3)
      {
        menuPointer = 4;
        numberOfPlayers = 2;
        clickSound();
        updateMenu = true;
      }      
    }
    else if(buttonFire1Clicked(0))
    {
      if(menuPointer == 1)
      {
        if(gameType == GameA){ gameType = GameB; }
        else if(gameType == GameB){ gameType = GameA; }
      }
      else if(menuPointer == 2)
      {
        preselectedLevel++;
        if(preselectedLevel > 9){ preselectedLevel = 1; }
      }
      else if(menuPointer == 3)
      {
        numberOfPlayers = 1;
        startGame = true;
      }
      else if(menuPointer == 4)
      {
        numberOfPlayers = 2;
        startGame = true;        
      }
      updateMenu = true;
    }
  }
  while(!startGame);
  delay(100);
  
  player = 1; // Reset to 1 (selection follows)
}

// syncLEDmePlays: Used for 2-player games via X-link cable
boolean syncLEDmePlays()
{
  twoPlayerResult = 0;
  
  // Check for link mode if player is player 1 (who can select the track)
  byte inputValue;
  readTillByteSerial1(4);
  readBuffer[0] = 255;
  readBuffer[1] = 255;
  readBuffer[2] = 255;
  readBuffer[3] = 255;
  Serial1.readBytes(readBuffer, 4); 

  // Player is player 2 (only difference is that LEDmePlay gets music sync signal from player 1)
  if(readBuffer[0] == SETPLAYER2)
  { 
    player = 2;
    gameType = readBuffer[1];
    preselectedLevel = readBuffer[2];
    gapColumn = readBuffer[3];
    for(byte i = 0; i < 64; i++)
    {
      Serial1.write(PLAYER2READY); // Send: Player 2 is ready
    }
    return true;
  }    
  // Player is player 1
  else
  {
    player = 1;
    
    // Clear screen
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

    // Write text
    matrix.setTextColor(matrix.Color333(2, 2, 2));
    matrix.setCursor(4, 0);
    matrix.println("Wait");
    matrix.setCursor(7, 8);
    matrix.print("for");    
    matrix.setCursor(1, 16);
    matrix.print("Plyr");    
    matrix.setCursor(7, 24);
    matrix.print("Exit");    
    matrix.setTextColor(matrix.Color333(0, 0, 3));
    matrix.setCursor(25, 16);
    matrix.print("2");    
    matrix.setCursor(1, 24);
    matrix.print(">");    

    gapColumn = random(10);

    Serial1.write(SETPLAYER2);
    Serial1.write(gameType);
    Serial1.write(preselectedLevel);
    Serial1.write(gapColumn);
    
    do
    {
      if(buttonFire1Clicked(200))
      {
        return false;
      }
    }
    while(readLastByteSerial1() != PLAYER2READY);
    return true;
  }
}

// showResults: Shows the result of the game: In 1-player mode a statistics and high scores are shown, in 2-player mode it is only shown who is winner/loser.
void showResults()
{
  // 1-player game
  if(numberOfPlayers == 1)
  {
  
    int counter = 0;
    
    do
    {
      // Show reached level and number of cleared rows
      if(counter == 0)
      {        
        matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));        
        matrix.setTextColor(matrix.Color333(2, 2, 2));
        matrix.setCursor(1, 0);
        matrix.print("Level");
        if(level < 10)
        {
          matrix.setCursor(13, 8);
        }
        else
        {
          matrix.setCursor(10, 8);    
        }
        matrix.setTextColor(matrix.Color333(0, 0, 3));
        matrix.print(level);
      
        matrix.setTextColor(matrix.Color333(2, 2, 2));
        matrix.setCursor(4, 16);
        matrix.print("Rows");
        if(rowsOverall < 10)       { matrix.setCursor(13, 24); }
        else if(rowsOverall < 100) { matrix.setCursor(10, 24); }
        else if(rowsOverall < 1000){ matrix.setCursor(7, 24);  }
        else                       {matrix.setCursor(4, 24);   }
        matrix.setTextColor(matrix.Color333(0, 0, 3));
        matrix.print(rowsOverall);
      }   

      // Show the number of singles, doubles, triples, and tetrominos (quadruples)
      if(counter == 2000)
      {        
        matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));  
        matrix.setTextColor(matrix.Color333(2, 2, 2));
  
        // Draw squares
        for(byte i = 0; i < 3; i++)
        {
          drawSquare(i * 2, 3, 1, 0, 0, true);
  
          drawSquare(i * 2, 10, 0, 1, 0, true);
          drawSquare(i * 2, 12, 0, 0, 1, true);
  
          drawSquare(i * 2, 17, 1, 1, 0, true);
          drawSquare(i * 2, 19, 1, 0, 1, true);
          drawSquare(i * 2, 21, 0, 1, 1, true);
  
          drawSquare(i * 2, 24, 1, 1, 1, true);
          drawSquare(i * 2, 26, 1, 0, 0, true);
          drawSquare(i * 2, 28, 0, 1, 0, true);
          drawSquare(i * 2, 30, 0, 0, 1, true);
        }
  
        // Show number of singles
        if(singles < 10)       { matrix.setCursor(25, 0); }
        else if(singles < 100) { matrix.setCursor(19, 0); }
        else if(singles < 1000){ matrix.setCursor(13, 0); }
        else                   { matrix.setCursor(7, 0);  }
        matrix.print(singles);
        
        // Show number of doubles
        if(doubles < 10)       { matrix.setCursor(25, 8); }
        else if(doubles < 100) { matrix.setCursor(19, 8); }
        else if(doubles < 1000){ matrix.setCursor(13, 8); }
        else                   { matrix.setCursor(7, 8);  }
        matrix.print(doubles);
  
        // Show number of triples
        if(triples < 10)       { matrix.setCursor(25, 16); }
        else if(triples < 100) { matrix.setCursor(19, 16); }
        else if(triples < 1000){ matrix.setCursor(13, 16); }
        else                   { matrix.setCursor(7, 16);  }
        matrix.print(triples);
  
        // Show number of quadruples
        if(quadruples < 10)       { matrix.setCursor(25, 24); }
        else if(quadruples < 100) { matrix.setCursor(19, 24); }
        else if(quadruples < 1000){ matrix.setCursor(13, 24); }
        else                      { matrix.setCursor(7, 24);  }
        matrix.print(quadruples);
      }

      // Show high scores (Note: The setCursor procedures are required for correct formatting)
      if(counter >= 4000)
      { 
        if(counter == 4000)
        {       
          matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
          matrix.setTextColor(matrix.Color333(2, 2, 2));
          matrix.setCursor(1, 0);
          matrix.print("Score");
          if(score < 10)        { matrix.setCursor(13, 8); }
          else if(score < 100)  { matrix.setCursor(10, 8); }
          else if(score < 1000) { matrix.setCursor(7, 8);  }
          else if(score < 10000){ matrix.setCursor(4, 8);  }
          else                  { matrix.setCursor(1, 8);  }
          matrix.setTextColor(matrix.Color333(0, 0, 3));
          matrix.print(score);
        }
        
        if(gameType == GameA && counter % 200 == 0)
        {
          matrix.setTextColor(matrix.Color333(2, 2, 2));
          if(score == hiScoreA && hiScoreA != 0 && counter % 400 == 0){ matrix.setTextColor(matrix.Color333(0, 0, 0)); } // Flash effect if new High Score              
          matrix.setCursor(4, 16);
          matrix.print("Hi A");
          matrix.setTextColor(matrix.Color333(2, 2, 2));
          if(hiScoreA < 10)        { matrix.setCursor(13, 24); }
          else if(hiScoreA < 100)  { matrix.setCursor(10, 24); }
          else if(hiScoreA < 1000) { matrix.setCursor(7, 24);  }
          else if(hiScoreA < 10000){ matrix.setCursor(4, 24);  }
          else                     { matrix.setCursor(1, 24);  }
          matrix.setTextColor(matrix.Color333(0, 0, 3));
          matrix.print(hiScoreA);
        }
        else if(gameType == GameB && counter % 200 == 0)
        { 
          matrix.setTextColor(matrix.Color333(2, 2, 2));
          if(score == hiScoreB && hiScoreB != 0 && counter % 400 == 0){ matrix.setTextColor(matrix.Color333(0, 0, 0)); } // Flash effect if new High Score               
          matrix.setCursor(4, 16);
          matrix.print("Hi B");
          matrix.setTextColor(matrix.Color333(2, 2, 2));
          if(hiScoreB < 10)        { matrix.setCursor(13, 24); }
          else if(hiScoreB < 100)  { matrix.setCursor(10, 24); }
          else if(hiScoreB < 1000) { matrix.setCursor(7, 24);  }
          else if(hiScoreB < 10000){ matrix.setCursor(4, 24);  }
          else                     { matrix.setCursor(1, 24);  }
          matrix.setTextColor(matrix.Color333(0, 0, 3));
          matrix.print(hiScoreB);
        }
      }
      
      counter++;
      if(counter == 6000){ counter = 0; }
      random(100);
      delay(1);
    }
    while(!buttonFire1Clicked(200));
  }

  // 2-player game
  else if(numberOfPlayers == 2)
  {
    // Clear screen
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

    if(twoPlayerResult == WINNER)
    {
      // Text
      matrix.setTextColor(matrix.Color333(0, 0, 3));
      matrix.setCursor(7, 8);
      matrix.print("You");
      matrix.setCursor(4, 16);
      matrix.print("won!");

      for(byte i = 0; i < 100; i++)
      {
        // SOUND: You won!
        tone(audio, 100 + i * 10 + random(10), 15);
        delay(20);
      }
    }
    else if(twoPlayerResult == LOSER)
    {
      // Text
      matrix.setTextColor(matrix.Color333(3, 0, 0));
      matrix.setCursor(7, 8);
      matrix.print("You");
      matrix.setCursor(1, 16);
      matrix.print("lost!");

      delay(2000);
      
      for(byte i = 0; i < 100; i++)
      {
        // SOUND: You won!
        tone(audio, 1100 - i * 10 + random(10), 15);
        delay(20);
      }
    }
    
    delay(200);
    do
    {
      random(100);
      delay(1);      
    }
    while(!buttonFire1Clicked(200));
  }
}

// printMode: Helper procedure for showMode
void printMode(int scrollOffset, byte r, byte g, byte b, boolean draw)
{
  matrix.setCursor(scrollOffset, 12);

  if(draw)
  {
    matrix.setTextColor(matrix.Color333(r, g, b));
  }
  else
  { 
    matrix.setTextColor(matrix.Color333(0, 0, 0));
  }

  if(gameType == GameA && numberOfPlayers == 1){ matrix.println("1 Player  Type A  Continuous"); }
  else if(gameType == GameB && numberOfPlayers == 1){ matrix.println("1 Player  Type B  Discrete"); }
  else if(gameType == GameA && numberOfPlayers == 2){ matrix.println("2 Players  Type A  Last Man Standing");}
  else if(gameType == GameB && numberOfPlayers == 2){ matrix.println("2 Players  Type B  Race");}
}

// showMode: Ticker of the selected number of players and the mode
void showMode()
{
  byte r, g, b;
  r = 1;
  g = 1;
  b = 1;

  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  int scrollOffset = 32;
  matrix.setTextColor(matrix.Color333(0, 0, 3));

  do
  {
    // Get time of engine loop start point (important for a constant game speed)
    engineLoopStartPoint = millis();

    // Scroll map name
    // Remove old position
    printMode(scrollOffset, r, g, b, false);

    // Update position
    scrollOffset--;
    if(scrollOffset < -240){ scrollOffset = 32; }

    // Draw new position 
    if(random(100) < 10)
    { 
      // Color effect
      do
      {
        r = random(8);
        g = random(8);
        b = random(8);
      }
      while((r + g + b) < 3 && (r + g + b) > 5);
    }
    printMode(scrollOffset, r, g, b, true);

    // SOUND: Show Mode
    tone(audio, (272 - (scrollOffset + 240) + 50) * 2, 10);

    // Loop until 20 milliseconds has passed since begin of engine loop
    do
    {
    }
    while(abs(millis() - engineLoopStartPoint) < 20);
  }
  while(scrollOffset > -240);  
}



// Procedures specific to draw and move tetrominos

// initializeTetromino: Initializes a tetromino of random type
void initializeTetromino(Tetromino &tetromino)
{
  byte type = random(7) + 1;
  initializeTetromino(tetromino, type);
}

// initializeTetromino: Initializes a new tetromino of the defined type above the upper border of the screen
void initializeTetromino(Tetromino &tetromino, byte type)
{
  tetromino.type = type;
  
  switch(tetromino.type)
  {
    case I:
      tetromino.xPosition[0] = 4; tetromino.yPosition[0] = 1; tetromino.xPosition[1] = 3; tetromino.yPosition[1] = 1; tetromino.xPosition[2] = 5; tetromino.yPosition[2] = 1; tetromino.xPosition[3] = 6; tetromino.yPosition[3] = 1;
      tetromino.colorR = 0; tetromino.colorG = 0; tetromino.colorB = 1;
      break;
    case O:
      tetromino.xPosition[0] = 4; tetromino.yPosition[0] = 1; tetromino.xPosition[1] = 5; tetromino.yPosition[1] = 1; tetromino.xPosition[2] = 4; tetromino.yPosition[2] = 2; tetromino.xPosition[3] = 5; tetromino.yPosition[3] = 2;
      tetromino.colorR = 1; tetromino.colorG = 0; tetromino.colorB = 0;
      break;
    case T:
      tetromino.xPosition[0] = 4; tetromino.yPosition[0] = 1; tetromino.xPosition[1] = 3; tetromino.yPosition[1] = 1; tetromino.xPosition[2] = 5; tetromino.yPosition[2] = 1; tetromino.xPosition[3] = 4; tetromino.yPosition[3] = 2;
      tetromino.colorR = 0; tetromino.colorG = 1; tetromino.colorB = 0;
      break;
    case L:
      tetromino.xPosition[0] = 4; tetromino.yPosition[0] = 1; tetromino.xPosition[1] = 3; tetromino.yPosition[1] = 1; tetromino.xPosition[2] = 5; tetromino.yPosition[2] = 1; tetromino.xPosition[3] = 3; tetromino.yPosition[3] = 2;
      tetromino.colorR = 1; tetromino.colorG = 0; tetromino.colorB = 1;
      break;
    case J:
      tetromino.xPosition[0] = 4; tetromino.yPosition[0] = 1; tetromino.xPosition[1] = 3; tetromino.yPosition[1] = 1; tetromino.xPosition[2] = 5; tetromino.yPosition[2] = 1; tetromino.xPosition[3] = 5; tetromino.yPosition[3] = 2;
      tetromino.colorR = 0; tetromino.colorG = 1; tetromino.colorB = 1;
      break;
    case S:
      tetromino.xPosition[0] = 4; tetromino.yPosition[0] = 1; tetromino.xPosition[1] = 5; tetromino.yPosition[1] = 1; tetromino.xPosition[2] = 3; tetromino.yPosition[2] = 2; tetromino.xPosition[3] = 4; tetromino.yPosition[3] = 2;
      tetromino.colorR = 1; tetromino.colorG = 1; tetromino.colorB = 0;
      break;
    case Z:
      tetromino.xPosition[0] = 4; tetromino.yPosition[0] = 1; tetromino.xPosition[1] = 3; tetromino.yPosition[1] = 1; tetromino.xPosition[2] = 4; tetromino.yPosition[2] = 2; tetromino.xPosition[3] = 5; tetromino.yPosition[3] = 2;
      tetromino.colorR = 1; tetromino.colorG = 1; tetromino.colorB = 1;
      break;
    default:
      break;
  }

  tetromino.active = true;
}

// drawSquare: Draws a square at position (x, y) in RGB colors. If draw == false, the square is removed from position (x, y).
void drawSquare(byte x, byte y, byte r, byte g, byte b, boolean draw)
{
  // Draw
  if(draw)
  {
    matrix.drawPixel(x, y, matrix.Color333(r * 5, g * 5, b * 5)); 
    matrix.drawPixel(x + 1, y, matrix.Color333(r * 3, g * 3, b * 3)); 
    matrix.drawPixel(x, y + 1, matrix.Color333(r * 3, g * 3, b * 3)); 
    matrix.drawPixel(x + 1, y + 1, matrix.Color333(r, g, b)); 
  }
  // Erase
  else
  {
    matrix.drawPixel(x, y, matrix.Color333(0, 0, 0)); 
    matrix.drawPixel(x + 1, y, matrix.Color333(0, 0, 0)); 
    matrix.drawPixel(x, y + 1, matrix.Color333(0, 0, 0)); 
    matrix.drawPixel(x + 1, y + 1, matrix.Color333(0, 0, 0));       
  }
}

// drawTetromino: Draws a tetromino on its current position. If draw == false, the tetromino is removed from the current position.
void drawTetromino(Tetromino &tetromino, boolean draw)
{
  for(byte i = 0; i < 4; i++)
  {
    drawSquare((tetromino.xPosition[i] * 2) + 6, (tetromino.yPosition[i] * 2) - 8, tetromino.colorR, tetromino.colorG, tetromino.colorB, draw);
  }
}

// rotateTetromino: Performs a rotation of a tetromino by 90 degree
void rotateTetromino(Tetromino &tetromino)
{
  int relativeX;
  int relativeY;

  // Perform the rotation
  relativeX = tetromino.xPosition[1] - tetromino.xPosition[0];
  relativeY = tetromino.yPosition[1] - tetromino.yPosition[0];
  tetromino.xPosition[1] = tetromino.xPosition[0] + relativeY * (-1);
  tetromino.yPosition[1] = tetromino.yPosition[0] + relativeX;

  relativeX = tetromino.xPosition[2] - tetromino.xPosition[0];
  relativeY = tetromino.yPosition[2] - tetromino.yPosition[0];
  tetromino.xPosition[2] = tetromino.xPosition[0] + relativeY * (-1);
  tetromino.yPosition[2] = tetromino.yPosition[0] + relativeX;

  relativeX = tetromino.xPosition[3] - tetromino.xPosition[0];
  relativeY = tetromino.yPosition[3] - tetromino.yPosition[0];
  tetromino.xPosition[3] = tetromino.xPosition[0] + relativeY * (-1);
  tetromino.yPosition[3] = tetromino.yPosition[0] + relativeX;  
}

// moveTetromino: Performs movement of a tetromino on the xAxis
void moveTetromino(Tetromino &tetromino, int xAxis)
{
  tetromino.xPosition[0] = tetromino.xPosition[0] + xAxis;
  tetromino.xPosition[1] = tetromino.xPosition[1] + xAxis;
  tetromino.xPosition[2] = tetromino.xPosition[2] + xAxis;
  tetromino.xPosition[3] = tetromino.xPosition[3] + xAxis;
}

// checkWhetherPositionIsReachable: Returns false if potential new position of tetromino is not reachable
boolean checkWhetherPositionIsReachable(Tetromino &tetromino)
{
  // Check whether tetromino would collide with another one
  if(    playfield[tetromino.xPosition[0]][tetromino.yPosition[0]] != 0
      || playfield[tetromino.xPosition[1]][tetromino.yPosition[1]] != 0
      || playfield[tetromino.xPosition[2]][tetromino.yPosition[2]] != 0
      || playfield[tetromino.xPosition[3]][tetromino.yPosition[3]] != 0)
  {
    return false;
  }

  // Check whether tetromino would leave the left or right border of the playfield
  if(    tetromino.xPosition[0] < 0 || tetromino.xPosition[1] < 0 || tetromino.xPosition[2] < 0 || tetromino.xPosition[3] < 0
      || tetromino.xPosition[0] > 9 || tetromino.xPosition[1] > 9 || tetromino.xPosition[2] > 9 || tetromino.xPosition[3] > 9)
  {
    return false;
  }

  // If upper checks are passed, true is returned
  return true;
}

// updateTetromino: Main method of tetromino movement: It is first checked whether the new position based on xAxis or yAxis (falling) movement or rotation is possible. If so, the tetromino position is updated.
void updateTetromino(Tetromino &tetromino, int xAxis, boolean yAxis, boolean rotate)
{
  boolean rotationPossible = true;
  boolean movingPossible = true;
  boolean fallingPossible = true;
  
  // Check whether new position is reachable
  
  Tetromino bufferTetromino;
  for(byte i = 0; i < 4; i++)
  {
    bufferTetromino.xPosition[i] = tetromino.xPosition[i];
    bufferTetromino.yPosition[i] = tetromino.yPosition[i];
  }

  // Check whether moving is possible
  if(xAxis != 0)
  {
    moveTetromino(bufferTetromino, xAxis);
    
    // Check whether area is free of other tetrominos
    if(!checkWhetherPositionIsReachable(bufferTetromino))
    {
      movingPossible = false;
    }    
  }
  
  // Check whether rotation is possible
  if(rotate)
  {
    if(tetromino.type !=O) // it makes no sense to rotate type 'O'
    {
      rotateTetromino(bufferTetromino);
      if(!checkWhetherPositionIsReachable(bufferTetromino))
      {        
        rotationPossible = false;
      }

      // Check whether rotation is possible after moving left
      if(!rotationPossible)
      {
        rotationPossible = true;
        movingPossible = true;

        for(byte i = 0; i < 4; i++)
        {
          bufferTetromino.xPosition[i] = tetromino.xPosition[i];
          bufferTetromino.yPosition[i] = tetromino.yPosition[i];
        }
        
        moveTetromino(bufferTetromino, -1);
        rotateTetromino(bufferTetromino);

        // Check whether area is free of other tetrominos
        if(!checkWhetherPositionIsReachable(bufferTetromino))
        {        
          rotationPossible = false;
          movingPossible = false;
        }

        if(movingPossible){ xAxis = -1; }
      }        
      
      // Check whether rotation is possible after moving right
      if(!rotationPossible)
      {
        rotationPossible = true;
        movingPossible = true;

        for(byte i = 0; i < 4; i++)
        {
          bufferTetromino.xPosition[i] = tetromino.xPosition[i];
          bufferTetromino.yPosition[i] = tetromino.yPosition[i];
        }
        
        moveTetromino(bufferTetromino, 1);
        rotateTetromino(bufferTetromino);

        // Check whether area is free of other tetrominos
        if(!checkWhetherPositionIsReachable(bufferTetromino))
        {        
          rotationPossible = false;
          movingPossible = false;
        }

        if(movingPossible){ xAxis = 1; }
      }    
      
      // Check whether rotation is possible after moving two to the left
      if(!rotationPossible)
      {
        rotationPossible = true;
        movingPossible = true;

        for(byte i = 0; i < 4; i++)
        {
          bufferTetromino.xPosition[i] = tetromino.xPosition[i];
          bufferTetromino.yPosition[i] = tetromino.yPosition[i];
        }
        
        moveTetromino(bufferTetromino, -2);
        rotateTetromino(bufferTetromino);

        // Check whether area is free of other tetrominos
        if(!checkWhetherPositionIsReachable(bufferTetromino))
        {        
          rotationPossible = false;
          movingPossible = false;
        }

        if(movingPossible){ xAxis = -2; }
      }        
      
      // Check whether rotation is possible after moving two to the right
      if(!rotationPossible)
      {
        rotationPossible = true;
        movingPossible = true;

        for(byte i = 0; i < 4; i++)
        {
          bufferTetromino.xPosition[i] = tetromino.xPosition[i];
          bufferTetromino.yPosition[i] = tetromino.yPosition[i];
        }
        
        moveTetromino(bufferTetromino, 2);
        rotateTetromino(bufferTetromino);

        // Check whether area is free of other tetrominos
        if(!checkWhetherPositionIsReachable(bufferTetromino))
        {        
          rotationPossible = false;
          movingPossible = false;
        }

        if(movingPossible){ xAxis = 2; }
      }    
      
      if(!rotationPossible)
      {        
        // SOUND: Rotation not possible
        tone(audio, 100, 50);
      }
    }
  }
  
  // Check whether falling by one row is possible
  if(yAxis)
  {
    // Last row reached
    if(tetromino.yPosition[0] == 19 || tetromino.yPosition[1] == 19 || tetromino.yPosition[2] == 19 || tetromino.yPosition[3] == 19)
    { 
      tetromino.active = false;
      fallingPossible = false; 

      // SOUND: Falling ends
      tone(audio, 200, 50);
    }
    // Tetromino on top of other tetrominos
    else
    {
      bufferTetromino.yPosition[0]++;
      bufferTetromino.yPosition[1]++;
      bufferTetromino.yPosition[2]++;
      bufferTetromino.yPosition[3]++;
      if(!checkWhetherPositionIsReachable(bufferTetromino))
      {
        tetromino.active = false;   
        fallingPossible = false;
        
        // SOUND: Falling ends
        tone(audio, 200, 50);
 
        // Upper screen exceeded
        if(bufferTetromino.yPosition[0] < 4 || bufferTetromino.yPosition[1] < 4 || bufferTetromino.yPosition[2] < 3 || bufferTetromino.yPosition[3] < 3)
        {
          gameOver = true;
          payload = LOSE;
          xLinkWrite(payload);
          twoPlayerResult = LOSER;
        }
      }
    }
  }

  // If tetromino is no longer active, its last position is stored in the playfield (for later collision detection)
  if(!tetromino.active)
  {
    playfield[tetromino.xPosition[0]][tetromino.yPosition[0]] = tetromino.type;
    playfield[tetromino.xPosition[1]][tetromino.yPosition[1]] = tetromino.type;
    playfield[tetromino.xPosition[2]][tetromino.yPosition[2]] = tetromino.type;
    playfield[tetromino.xPosition[3]][tetromino.yPosition[3]] = tetromino.type;
  }
  
  // If still active, update position
  if(movingPossible || fallingPossible)
  {  
    // Remove tetromino from old position
    drawTetromino(tetromino, false);

    // Moving left or right
    if(xAxis != 0 && movingPossible)
    {
      tetromino.xPosition[0] = tetromino.xPosition[0] + xAxis;
      tetromino.xPosition[1] = tetromino.xPosition[1] + xAxis;
      tetromino.xPosition[2] = tetromino.xPosition[2] + xAxis;
      tetromino.xPosition[3] = tetromino.xPosition[3] + xAxis;
    }
    
    // Rotate
    if(rotate && rotationPossible && tetromino.type !=O)
    {
      // SOUND: Rotate
      tone(audio, 400, 50);      
      rotateTetromino(tetromino);
      tone(audio, 200, 50);
    }

    // Falling by one line
    if(yAxis && fallingPossible)
    {
      tetromino.yPosition[0]++;
      tetromino.yPosition[1]++;
      tetromino.yPosition[2]++;
      tetromino.yPosition[3]++;
    }
  
    // Draw tetromino at new position
    drawTetromino(tetromino, true);
  }
}



// Specific gameplay procedures

// initializeGame: Sets everything to the correct starting values and draws the basic graphics
void initializeGame()
{
  // Clear playfield
  for(byte i = 0; i < 10; i++)
  {
    for(byte j = 0; j < 20; j++)
    {
      playfield[i][j] = 0;
    }
  }

  // Clear screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Draw lines to boundary lines
  matrix.drawLine(5, 0, 5, 31, matrix.Color333(1, 1, 1));
  matrix.drawLine(26, 0, 26, 31, matrix.Color333(1, 1, 1));

  // Draw 'L' for level and 'N' for next tetromino
  matrix.drawLine(0, 2, 0, 7, matrix.Color333(2, 2, 2));
  matrix.drawLine(1, 7, 3, 7, matrix.Color333(2, 2, 2));
  matrix.drawLine(28, 2, 28, 7, matrix.Color333(2, 2, 2));
  matrix.drawLine(31, 2, 31, 7, matrix.Color333(2, 2, 2));
  matrix.drawLine(28, 2, 31, 7, matrix.Color333(2, 2, 2));

  level = preselectedLevel;
  
  rows = 0;
  rowsOverall = 0;
  singles = 0;
  doubles = 0;
  triples = 0;
  quadruples = 0;

  fallingByOneRowInterval = calcFallingByOneRowInterval(level);
  controlInterval = 5;
  if(controlInterval > fallingByOneRowInterval / 2){ controlInterval = fallingByOneRowInterval / 2; }
  
  payload = 0;
}

// calcFallingInterval: Returns the interval for automatic falling based on the level
int calcFallingByOneRowInterval(byte _level)
{
  if(_level < 10)
  {
    return (40 - (_level * 3));
  }
  else if(_level < 20)
  {
    return (20 - _level);    
  }
  else
  {
    return 1;
  }
}

// playfieldMoveUp: Move down of upper squares if a row has been flushed
void playfieldMoveDown(byte row)
{
  for(byte j = row; j > 3; j--)
  {
    if(numberOfPlayers == 2){ xLinkRead(); }
    melodyPlayer.update();

    for(byte i = 0; i < 10; i++)
    {
      playfield[i][j] = playfield[i][j - 1];
      switch(playfield[i][j])
      {
        case I:
          drawSquare((i * 2) + 6, (j * 2) - 8, 0, 0, 1, true);
          break;
        case O:
          drawSquare((i * 2) + 6, (j * 2) - 8, 1, 0, 0, true);
          break;
        case T:
          drawSquare((i * 2) + 6, (j * 2) - 8, 0, 1, 0, true);
          break;
        case L:
          drawSquare((i * 2) + 6, (j * 2) - 8, 1, 0, 1, true);
          break;
        case J:
          drawSquare((i * 2) + 6, (j * 2) - 8, 0, 1, 1, true);
          break;
        case S:
          drawSquare((i * 2) + 6, (j * 2) - 8, 1, 1, 0, true);
          break;
        case Z:
          drawSquare((i * 2) + 6, (j * 2) - 8, 1, 1, 1, true);
          break;
        default:
          drawSquare((i * 2) + 6, (j * 2) - 8, 0, 0, 0, true);
          break;
      }
    }
  }
}

// playfieldMoveDownCompletely: Move playfield completely down after next level has been reached (Game type B)
void playfieldMoveDownCompletely()
{
  for(byte k = 0; k < 16; k++)
  {
    for(byte j = 19; j > 3; j--)
    {
      for(byte i = 0; i < 10; i++)
      {
        playfield[i][j] = playfield[i][j - 1];
        switch(playfield[i][j])
        {
          case I:
            drawSquare((i * 2) + 6, (j * 2) - 8, 0, 0, 1, true);
            break;
          case O:
            drawSquare((i * 2) + 6, (j * 2) - 8, 1, 0, 0, true);
            break;
          case T:
            drawSquare((i * 2) + 6, (j * 2) - 8, 0, 1, 0, true);
            break;
          case L:
            drawSquare((i * 2) + 6, (j * 2) - 8, 1, 0, 1, true);
            break;
          case J:
            drawSquare((i * 2) + 6, (j * 2) - 8, 0, 1, 1, true);
            break;
          case S:
            drawSquare((i * 2) + 6, (j * 2) - 8, 1, 1, 0, true);
            break;
          case Z:
            drawSquare((i * 2) + 6, (j * 2) - 8, 1, 1, 1, true);
            break;
          default:
            drawSquare((i * 2) + 6, (j * 2) - 8, 0, 0, 0, true);
            break;
        }
      }
    }
    delay(20);
  }
}

// showFlushEffect: Effect if a row is cleared
void showFlushEffect(byte row)
{
  for(byte i = 0; i < 10; i++)
  {
    if(numberOfPlayers == 2){ xLinkRead(); }
    melodyPlayer.update();

    // Draw effect
    matrix.drawPixel(15 - i, (row * 2) - 8, matrix.Color333(7, 7, 1));
    matrix.drawPixel(15 - i, (row * 2) - 8 + 1, matrix.Color333(7, 7, 1));
    matrix.drawPixel(16 + i, (row * 2) - 8, matrix.Color333(7, 7, 1));
    matrix.drawPixel(16 + i, (row * 2) - 8 + 1, matrix.Color333(7, 7, 1));

    // Delay
    // SOUND: Flush row
    tone(audio, (i + 1) * 100, 10);
    delay(20);

    // Remove effect
    matrix.drawPixel(15 - i, (row * 2) - 8, matrix.Color333(0, 0, 0));
    matrix.drawPixel(15 - i, (row * 2) - 8 + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(16 + i, (row * 2) - 8, matrix.Color333(0, 0, 0));
    matrix.drawPixel(16 + i, (row * 2) - 8 + 1, matrix.Color333(0, 0, 0));
  }
}

// addRowsToPlayfield: Move everything up by adding rows in 2-player mode
void addRowsToPlayfield(byte numberOfRows)
{
  for(byte k = 0; k < numberOfRows; k++)
  {
    for(byte j = 0; j < 19; j++)
    {
      if(numberOfPlayers == 2){ xLinkRead(); }
      melodyPlayer.update();

      for(byte i = 0; i < 10; i++)
      {
        playfield[i][j] = playfield[i][j + 1];
        switch(playfield[i][j])
        {
          case I:
            drawSquare((i * 2) + 6, (j * 2) - 8, 0, 0, 1, true);
            break;
          case O:
            drawSquare((i * 2) + 6, (j * 2) - 8, 1, 0, 0, true);
            break;
          case T:
            drawSquare((i * 2) + 6, (j * 2) - 8, 0, 1, 0, true);
            break;
          case L:
            drawSquare((i * 2) + 6, (j * 2) - 8, 1, 0, 1, true);
            break;
          case J:
            drawSquare((i * 2) + 6, (j * 2) - 8, 0, 1, 1, true);
            break;
          case S:
            drawSquare((i * 2) + 6, (j * 2) - 8, 1, 1, 0, true);
            break;
          case Z:
            drawSquare((i * 2) + 6, (j * 2) - 8, 1, 1, 1, true);
            break;
          default:
            drawSquare((i * 2) + 6, (j * 2) - 8, 0, 0, 0, true);
            break;
        }
      }          
    }
    for(byte i = 0; i < 10; i++)
    {
      if(i != gapColumn)
      { 
        playfield[i][19] = random(7) + 1;
      }
      else
      {
        playfield[i][19] = 0;
      }
      switch(playfield[i][19])
      {
        case I:
          drawSquare((i * 2) + 6, (19 * 2) - 8, 0, 0, 1, true);
          break;
        case O:
          drawSquare((i * 2) + 6, (19 * 2) - 8, 1, 0, 0, true);
          break;
        case T:
          drawSquare((i * 2) + 6, (19 * 2) - 8, 0, 1, 0, true);
          break;
        case L:
          drawSquare((i * 2) + 6, (19 * 2) - 8, 1, 0, 1, true);
          break;
        case J:
          drawSquare((i * 2) + 6, (19 * 2) - 8, 0, 1, 1, true);
          break;
        case S:
          drawSquare((i * 2) + 6, (19 * 2) - 8, 1, 1, 0, true);
          break;
        case Z:
          drawSquare((i * 2) + 6, (19 * 2) - 8, 1, 1, 1, true);
          break;
        default:
          drawSquare((i * 2) + 6, (19 * 2) - 8, 0, 0, 0, true);
          break;
      }
    }
    for(byte i = 0; i < 10; i++)
    {
      // SOUND: Add row to playfield
      tone(audio, (i + 1) * 50, 20);
      delay(40);
    }               
  }
}

// showLevelUp: Animation if next level has been reached
void showLevelUp()
{
  for(byte i = 0; i < 3; i++)
  {
    for(int j = 31; j >= 0; j--)
    {
      if(numberOfPlayers == 2){ xLinkRead(); }
      melodyPlayer.update();

      matrix.drawPixel(5, j, matrix.Color333(5, 5, 7));
      matrix.drawPixel(26, j, matrix.Color333(5, 5, 7));

      // SOUND: Flush row
      tone(audio, (j + 1) * 100, 5);
      delay(10);
      
      matrix.drawPixel(5, j, matrix.Color333(1, 1, 1));
      matrix.drawPixel(26, j, matrix.Color333(1, 1, 1));
    }
  }
  if(gameType == GameB)
  {
    if(numberOfPlayers == 1)
    {
      playfieldMoveDownCompletely(); // Remove all squares from screen
      showLevel();
      notePointer = 0; 
      delay(500);
    }
    else if(numberOfPlayers == 2)
    {
      // In 2 player game type B, the game ends after one player has reached 20 rows
      playfieldMoveDownCompletely(); // Remove all squares from screen
      gameOver = true;
      payload = WIN;
      xLinkWrite(payload);
      twoPlayerResult = WINNER;
      notePointer = 0; 
      delay(500);      
    }
  }
}

// showLevel: Displays the level (only in 1-player mode type B)
void showLevel()
{
  for(int i = 0; i < 16; i++)
  {
    matrix.setTextColor(matrix.Color444(i, 0, 0));
    if(level < 10)       { matrix.setCursor(13, 12); }
    else if(level < 100) { matrix.setCursor(10, 12); }
    else if(level < 1000){ matrix.setCursor(7, 12); }
    else                   { matrix.setCursor(4, 12);  }
    matrix.print(level);
    delay(50);    
  }
  for(int i = 15; i > -1; i--)
  {
    matrix.setTextColor(matrix.Color444(i, 0, 0));
    if(level < 10)       { matrix.setCursor(13, 12); }
    else if(level < 100) { matrix.setCursor(10, 12); }
    else if(level < 1000){ matrix.setCursor(7, 12); }
    else                   { matrix.setCursor(4, 12);  }
    matrix.print(level);    
    delay(50);    
  }
}

// updatePlayfield: Checks whether rows are complete. If so, they are removed.
void updatePlayfield()
{
  boolean rowFilled;
  byte filledRows = 0;

  // All possible rows on the playfield are checked whether they are filled completely
  for(byte j = 19; j > 3; j--)
  {
    rowFilled = true;
    for(byte i = 0; i < 10; i++)
    {  
      if(playfield[i][j] == 0)
      {
        rowFilled = false;
        break;
      }
    }

    // If a row is filled ...
    if(rowFilled)
    {
      showFlushEffect(j);   // Show a flush effect
      filledRows++;         // Count (there couild be up to 4 rows cleared at once)
      playfieldMoveDown(j); // After flushing a row everything above slides down
      j++;
      rows++;
      rowsOverall++;

      // Next level
      if(rows == 20)
      {
        level++;
        showLevelUp();
        rows = 0;
        fallingByOneRowInterval = calcFallingByOneRowInterval(level);
        controlInterval = 5;
        if(controlInterval > fallingByOneRowInterval / 2){ controlInterval = fallingByOneRowInterval / 2; }
      }
      updateLevelAndRows();
    }
  }
  
  switch(filledRows)
  {
    case 1:
      singles++;
      score = score + level;
      break;
    case 2:
      doubles++;
      score = score + (level * 4);
      payload = SEND1ROW;
      xLinkWrite(payload);
      break;
    case 3:
      triples++;
      score = score + (level * 9);
      payload = SEND2ROWS;
      xLinkWrite(payload);
      break;
    case 4:
      quadruples++;
      score = score + (level * 16);
      payload = SEND4ROWS;
      xLinkWrite(payload);
      break;    
    default:
      break;
  }
}

// updateLevelAndRows: Displays the current level and the number of rows by bars on the left-hand side of the screen
void updateLevelAndRows()
{
  matrix.drawLine(1, 29, 1, 10, matrix.Color333(0, 0, 0)); 
  matrix.drawLine(3, 29, 3, 10, matrix.Color333(0, 0, 0)); 
  for(byte i = 0; i < min(level, 20); i++)
  {
    matrix.drawPixel(1, 29 - i, matrix.Color333(4, 0, 0));
  }
  for(byte i = 0; i < rows; i++)
  {
    matrix.drawPixel(3, 29 - i, matrix.Color333(0, 0, 4));
  }
}

// updatePreviews: Shows the next tetromino on the right-hand side of the screen
void updatePreview(Tetromino &nextTetromino)
{
  matrix.fillRect(27, 10, 5, 8, matrix.Color333(0, 0, 0));
  if(nextTetromino.type == I)
  {
    for(byte i = 0; i < 4; i++)
    {
      drawSquare((nextTetromino.xPosition[i] * 2) + 6 + 14, (nextTetromino.yPosition[i] * 2) - 8 + 18, nextTetromino.colorR, nextTetromino.colorG, nextTetromino.colorB, true);
    }      
  }
  else
  {
    for(byte i = 0; i < 4; i++)
    {
      drawSquare((nextTetromino.xPosition[i] * 2) + 6 + 16, (nextTetromino.yPosition[i] * 2) - 8 + 18, nextTetromino.colorR, nextTetromino.colorG, nextTetromino.colorB, true);
    }
  }  
}

// gameOverAnimation: Floods the screen with squares
void gameOverAnimation()
{
  for(byte j = 19; j > 3; j--)
  {
    for(byte i = 0; i < 10; i++)
    {
      switch(random(7))
      {
      case 0:
        drawSquare((i * 2) + 6, (j * 2) - 8, 0, 0, 1, true);
        break;
      case 1:
        drawSquare((i * 2) + 6, (j * 2) - 8, 1, 0, 0, true);
        break;
      case 2:
        drawSquare((i * 2) + 6, (j * 2) - 8, 0, 1, 0, true);
        break;
      case 3:
        drawSquare((i * 2) + 6, (j * 2) - 8, 1, 0, 1, true);
        break;
      case 4:
        drawSquare((i * 2) + 6, (j * 2) - 8, 0, 1, 1, true);
        break;
      case 5:
        drawSquare((i * 2) + 6, (j * 2) - 8, 1, 1, 0, true);
        break;
      case 6:
        drawSquare((i * 2) + 6, (j * 2) - 8, 1, 1, 1, true);
        break;
      }
    }
    // SOUND: Game over
    tone(audio, 1000 - (j * 25) + random(50), 10);
    delay(20);
  }
}

// xLinkRead: Read sent data packages from the other connected LEDmePlay in 2-player games
void xLinkRead()
{
  // Encode data package from connected LEDmePlay
  readTillByteSerial1(5); // Remove outdated packages from hardware buffer   
  byte value = Serial1.available(); // Check whether at least one package with other player position is available (3 bytes)
  if(value > 2)
  {
    if(value > 6){ value = 6; } // If additional bytes reached hardware buffer, restrict length to 8
    for(byte k = 0; k < 6; k++)
    {
      readBuffer[k] = 255; // Fill all software buffer bytes with 1
    }
    Serial1.readBytes(readBuffer, value); 
    // Read the last received package in reverse manner (ends with termination symbol) (Note: The reading rate is much higher than the writing rate, thus, no package gets lost.)
    for(byte k = 5; k > 1; k--)    
    {
      // Data package found
      if(readBuffer[k] == TERMINATION)
      {
        // Encode note pointer (if player == 2 and music is syncronized by other LEDmePlay) and other player´s status
        if(player == 2){ notePointer = readBuffer[k - 2]; }

        // Encode payload
        switch(readBuffer[k - 1])
        {
          case WIN:
          gameOver = true;
          twoPlayerResult = LOSER;  // Other player has won
          break;
          case LOSE:
          gameOver = true;
          twoPlayerResult = WINNER; // Other player has lost
          break;
          case SEND1ROW:
          addRows = 1;              // Other player sent 1 row
          break;
          case SEND2ROWS:
          addRows = 2;              // Other player sent 2 rows
          break;
          case SEND4ROWS:
          addRows = 4;              // Other player sent 4 rows
          break;
          default:
          break;
        }
        
        break; 
      }
    }
  }  
}

// xLinkWrite: Sends a data package to the other connected LEDmePlay in 2-player games
void xLinkWrite(byte payload)
{
  // Send current value of note pointer to other LEDmePlay to sync it if player is player 1
  if(player == 1)
  {
    Serial1.write(byte(notePointer));
  }
  else if(player == 2)
  {
    Serial1.write(byte(0));    
  }
  
  // Send payload (e.g. that the player has won etc.)
  Serial1.write(byte(payload));

  // Send termination symbol
  Serial1.write(TERMINATION);
  payload = 0; // Make sure that payload information is only send once
}



// Main loop

// loop: Main program loop
void loop()
{
  do
  {
    title();
    menu();
  }
  while(numberOfPlayers == 2 && !syncLEDmePlays()); // Back to title screen if the two LEDmePlays cannot be synced in 2-player mode
  showMode();

  // Initialize game
  initializeGame();

  // Set some local variables
  int fallingByOneRowCounter = 0;
  int controlCounter = 0;
  boolean buttonPressed = false;

  stepsPerEightNote = 0;
  noteLength = 0;
  notePointer = 0;

  addRows = 0;
  fastFallingEnabled = true;
  gameOver = false;

  initializeTetromino(nextTetromino); // Required for preview of next tetromino
  tetromino.active = false;

  updateLevelAndRows();

  musicOn = true;
  reset = false;

  // Show level in type B (1-player)
  if(gameType == GameB && numberOfPlayers == 1){ showLevel(); }

  do
  {
    if(numberOfPlayers == 2){ xLinkRead(); }
    melodyPlayer.update();
    
    // If no tetromino active, initialize a new one
    if(!tetromino.active)
    { 
      // Routine to add rows to playfield in 2-player game if other player cleard more than one row at once
      if(numberOfPlayers == 2 && addRows > 0)
      { 
        addRowsToPlayfield(addRows);
        addRows = 0;
      }

      // Avoids that a new appeard tetromino directly falls down
      if(joy1Down() && fastFallingEnabled){ fastFallingEnabled = false; }
      
      initializeTetromino(tetromino, nextTetromino.type); // Take type from tetromino from the preview
      initializeTetromino(nextTetromino);                 // Initialize a new tetromino for the preview
      rotateTetromino(nextTetromino);                     // This rotation is required so that every tetromino fits to the preview display
      updatePreview(nextTetromino);
    }

    // Cyclic falling of the tetromino
    fallingByOneRowCounter++;
    if(fallingByOneRowCounter >= fallingByOneRowInterval)
    {
      updateTetromino(tetromino, 0, true, false);
      fallingByOneRowCounter = 0;
    }

    if(!joy1Fire()){ buttonPressed = false; } // The fire button must be released after every rotation

    // Control of tetrominos (after waiting time since last action has passed)
    if(controlCounter < controlInterval)
    {
      controlCounter++;
    }
    else
    {
      if(joy1Fire() && !buttonPressed)
      { 
        updateTetromino(tetromino, 0, false, true);
        controlCounter = 0;
        buttonPressed = true;
      }
      if(joy1Left())
      { 
        updateTetromino(tetromino, -1, false, false);
        controlCounter = 0;
      }
      else if(joy1Right())
      { 
        updateTetromino(tetromino, 1, false, false);
        controlCounter = 0;
      }
      else if(joy1Down() && fastFallingEnabled)
      { 
        fallingByOneRowCounter = fallingByOneRowInterval - 1;
      }
      if(!joy1Down()){ fastFallingEnabled = true; }
    }

    // Check whether a row has been filled
    if(!tetromino.active)
    {
      updatePlayfield();
    }

    // Reset
    if(digitalRead(buttonReset) == LOW)
    {
      reset = true;
      break;
    }
    
    // Pause (only possible in 1-player game)
    if(digitalRead(buttonPause) == LOW && numberOfPlayers == 1)
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

    delay(20);
  }
  while(!gameOver && !reset);

  musicOn = false;
  if(numberOfPlayers == 2)
  { 
    xLinkRead();
    xLinkWrite(payload);
  }

  if(gameOver)
  {
    if(!(numberOfPlayers == 2 && twoPlayerResult == WINNER)) // Game Over animation is not shown to the winner in 2-player game
    { 
      gameOverAnimation();
    }
    else
    {
      delay(400);  
    }
    delay(2000);

    // Update high score
    if(gameType == GameA && numberOfPlayers == 1)
    {
      if(score > hiScoreA){ hiScoreA = score; }
    }
    else if(gameType == GameB)
    {
      if(score > hiScoreB && numberOfPlayers == 1){ hiScoreB = score; }
    }

    // Show results
    showResults();
  }
}

