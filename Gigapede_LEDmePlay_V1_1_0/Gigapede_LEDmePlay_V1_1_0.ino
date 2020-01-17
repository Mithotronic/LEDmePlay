// Gigapede
// for the LEDmePlay
//
// www.mithotronic.de
//
// Version: 1.1.0
// Author: Michael Rosskopf (2013)
// 
// Many thanks Thomas Laubach!
//
// Release Notes:
// V1.1.0: Support of LEDmePlay Joypad and LEDmePlayBoy (2018)
// V1.0.1: Smaller improvements (colors adapted for IDE 1.x, waiting in status screens can be skipped with fire button)
// V1.0.0: First release

// Include libraries for adafruit matrix
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

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

// Playfield
byte playfield[32][32]; // 0 == empty, 1 == centipede, 2 == mushroom, 3 == shot, 4 == spider, 5 == bug, 6 == dragonfly

// Enemies

// Centipede
byte lengthOfCentipede = 16;
byte centipedeX[24]; 
byte centipedeY[24];
byte centipedeDirection[24]; // 0 == killed, 1 == right down, 2 == left down, 3 == right up, 4 == left up
byte segmentsKilled;
boolean centipedeKilled;

// Spider
int spiderX;
int spiderY;
byte spiderDirection; // 0 == absent, 1 == right down, 2 == left down, 3 == right up, 4 == left up

// Bug
int bugX;
int bugY;
boolean bugPresent;

// Dragonfly (first appears in level 3)
int dragonflyX;
int dragonflyY;
byte dragonflyDirection; // 0 == absent, 1 == right down, 2 == left down

// Guns and shots
byte numberOfPlayers;
byte gunX[2];
byte gunY[2];
byte shotX[2];
byte shotY[2];
boolean shotFired[2];

// Level settings
byte level;
byte lives[2];
int score[2];
int extraLifeScore[2];
int highScore = 0;
int gameSpeed;
int enemyProbability;
boolean reset; // if true, return to title screen

// Colors
byte gun1R;
byte gun1G;
byte gun1B;
byte gun2R;
byte gun2G;
byte gun2B;
byte centipedeR;
byte centipedeG;
byte centipedeB;
byte mushroomR;
byte mushroomG;
byte mushroomB;
byte spiderR;
byte spiderG;
byte spiderB;
byte bugR;
byte bugG;
byte bugB;
byte colors[6];
byte newColor;
boolean newColorFound;

void setup() 
{
  // Initialize serial connection
  Serial.begin(9600);

  // Initialize random number generator
  randomSeed(analogRead(5));

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
  
  // Mithotronic logo
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

// Start screen and game options selection (difficulty and number of players)
void setupGame()
{
  // Reset set to false  
  reset = false;
  
  // Title screen
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setCursor(4, 0);
  matrix.setTextColor(matrix.Color333(0,5,0));
  matrix.println("Giga");
  matrix.setCursor(4, 8);
  matrix.setTextColor(matrix.Color333(0,5,0));
  matrix.println("pede");
  matrix.setCursor(1, 16);
  matrix.setTextColor(matrix.Color333(3,3,0));
  matrix.println("HiScr");
  matrix.setCursor(1, 24);
  matrix.println(highScore);
  
  // Initialize level score etc.
  score[0] = 0;
  score[1] = 0;
  lives[0] = 3;
  lives[1] = 3;
  extraLifeScore[0] = 0;
  extraLifeScore[1] = 0;
  level = 1;
  
  // Jingle on start screen
  tone(audio,523,200);
  delay(200);
  tone(audio,659,200);
  delay(200);
  tone(audio,784,200);
  delay(200);
  tone(audio,1047,400);
  delay(400);
  tone(audio,784,200);
  delay(200);
  tone(audio,1047,400);
  delay(400);
  
  // Wait until player 1 presses fire
  do
  {
  }
  while(!joy1Fire());
  tone(audio,1024,20);
  delay(200);

  // Select difficulty (defined by length of centipede)
  matrix.fillRect(0, 16, 32, 16, matrix.Color333(0, 0, 0));
  matrix.setCursor(1, 16);
  matrix.setTextColor(matrix.Color333(3,3,0));
  matrix.println("Diff");
  matrix.setCursor(1, 24);
  delay(200);

  do
  {
    if(joy1Left())
    {
      if(lengthOfCentipede > 12)
      {
        lengthOfCentipede = lengthOfCentipede - 4;
        matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
        tone(audio,1024,20);
        delay(200);
      }
    }
    if(joy1Right())
    {
      if(lengthOfCentipede < 24)
      {
        lengthOfCentipede = lengthOfCentipede + 4;
        matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
        tone(audio,1024,20);
        delay(200);
      }
    }
    matrix.setCursor(1, 24);
    switch(lengthOfCentipede)
    {
      case 12:
      matrix.println("Light");
      break;

      case 16:
      matrix.println("Med");
      break;

      case 20:
      matrix.println("Hard");
      break;

      case 24:
      matrix.println("Ultra");
      break;
    }
  }
  while(!joy1Fire());
  tone(audio,1024,20);
  delay(200);

  // Select number of players
  matrix.fillRect(0, 16, 32, 16, matrix.Color333(0, 0, 0));
  matrix.setCursor(1, 16);
  matrix.setTextColor(matrix.Color333(3,3,0));
  matrix.println("Plyrs");
  matrix.setCursor(1, 24);
  numberOfPlayers = 1;

  do
  {
    if(joy1Left() && numberOfPlayers != 1)
    {
      numberOfPlayers = 1;
      matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
      tone(audio,1024,20);
      delay(200);
    }
    if(joy1Right() && numberOfPlayers != 2)
    {
      numberOfPlayers = 2;
      matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
      tone(audio,1024,20);
      delay(200);
    }
    matrix.setCursor(14, 24);
    matrix.println(numberOfPlayers);
  }
  while(!joy1Fire());
  tone(audio,1024,20);
  delay(200);
}

// Setup a new or retried level
void setupLevel()
{
  // Set colors;
  if(level == 1) // Standard colors in level 1
  {
    setColor(1, gun1R, gun1G, gun1B);
    setColor(2, mushroomR, mushroomG, mushroomB);
    setColor(3, gun2R, gun2G, gun2B);
    setColor(4, spiderR, spiderG, spiderB);
    setColor(5, bugR, bugG, bugB);
    setColor(6, centipedeR, centipedeG, centipedeB);
  }
  
  if((level != 1) && centipedeKilled) // Random colors after level 1
  {
    for(byte i = 0; i < 6; i++)
    {
      colors[i] = 0;
      do
      {
        newColor = byte(random(6) + 1);
        newColorFound = true;
        for(byte j = 0; j < i; j++)
        {
          if(colors[j] == newColor)
          {
            newColorFound = false;
          }
        }
      }
      while(!newColorFound);
      colors[i] = newColor;
    }
    setColor(colors[0], gun1R, gun1G, gun1B);
    setColor(colors[1], mushroomR, mushroomG, mushroomB);
    setColor(colors[2], gun2R, gun2G, gun2B);
    setColor(colors[3], spiderR, spiderG, spiderB);
    setColor(colors[4], bugR, bugG, bugB);
    setColor(colors[5], centipedeR, centipedeG, centipedeB);
  }

  // Show score
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setCursor(1, 0);
  matrix.setTextColor(matrix.Color333(3,3,3));
  matrix.println("Score");
  matrix.setCursor(1, 8);
  matrix.setTextColor(matrix.Color333(gun1R, gun1G, gun1B));
  matrix.println(score[0]);
  if(numberOfPlayers == 2)
  {
    matrix.setCursor(1, 16);
    matrix.setTextColor(matrix.Color333(gun2R, gun2G, gun2B));
    matrix.println(score[1]);
  }
  matrix.setTextColor(matrix.Color333(3,3,3));
  matrix.setCursor(1, 24);
  matrix.println(highScore);
  wait(4000);

  // Extra life (every 2000 points)
  for(byte i = 0; i < numberOfPlayers; i++)
  {
    if(extraLifeScore[i] > 2000)
    {
      matrix.fillScreen(matrix.Color333(0, 0, 0));
      extraLifeScore[i] = extraLifeScore[i] - 2000;
      lives[i]++;
      for(byte j = 100; j > 0; j--)
      {
        matrix.setTextColor(matrix.Color333(7 * (j % 2), 0, 0));
        matrix.setCursor(1, 0);
        matrix.println("EXTRA");
        matrix.setCursor(4, 8);
        matrix.println("LIFE");
        matrix.setCursor(4, 16);
        matrix.println("Plyr");
        matrix.setCursor(14, 24);
        matrix.print(i + 1);
        tone(audio,j * 50,10);
        delay(10);
      }
      matrix.setTextColor(matrix.Color333(7,0,0));
      matrix.setCursor(1, 0);
      matrix.println("EXTRA");
      matrix.setCursor(4, 8);
      matrix.println("LIFE");
      matrix.setCursor(4, 16);
      matrix.println("Plyr");
      matrix.setCursor(14, 24);
      matrix.print(i + 1);
      delay(1000);    
    }
  }

  // Show level and remaining lives
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setCursor(1, 0);
  matrix.setTextColor(matrix.Color333(0,0,3));
  matrix.println("Level");
  matrix.setCursor(13, 8);
  matrix.println(level);
  matrix.setCursor(1, 16);
  matrix.setTextColor(matrix.Color333(3,0,0));
  matrix.println("Lives");
  if(numberOfPlayers == 1)
  {
    matrix.setCursor(13, 24);
    matrix.println(lives[0]);
  }
  else
  {
    matrix.setCursor(1, 24);
    matrix.println(lives[0]);
    matrix.setCursor(25, 24);
    matrix.println(lives[1]);
  }
  wait(4000);
  
  // Clear screen again
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // Initialize playfield
  for(byte i = 0; i < 32; i++)
  {
    for(byte j = 0; j < 32; j++)
    {
      playfield[i][j] = 0;
    }
  }
 
  // Initialize centipede
  for(byte i = 0; i < lengthOfCentipede; i++)
  {
    centipedeX[i] = lengthOfCentipede - 1 - i;
    centipedeY[i] = 0;
    centipedeDirection[i] = 1;
  }
  segmentsKilled = 0;
  centipedeKilled = false;
  
  // Initialize spider;
  spiderDirection = 0;
  
  // Initialize bug;
  bugPresent = false;
  
  // Initialize dragonfly;
  dragonflyDirection = 0;

  // Initialize guns
  gunY[0] = 30;
  gunY[1] = 30;
  shotFired[0] = false;
  shotFired[1] = false;
  if(numberOfPlayers == 1)
  {
    gunX[0] = 15;
  }
  if(numberOfPlayers == 2)
  {
    gunX[0] = 10;
    gunX[1] = 21;
  }
   
  // Setup level specific game play (increasing difficulty)

  // Initialize mushrooms (number increases with level)
  drawMushrooms(16 + (level * 4));
  delay(200);

  // Set game speed (speed increases with level)
  gameSpeed = 60 - ((level - 1) * 2);
  if(gameSpeed < 30)
  {
    gameSpeed = 30;
  }
  
  // Set probability that spider, bug or dragonfly appears (1000 == 100%)
  enemyProbability = 5 + ((level - 1) * 5);
  if(enemyProbability > 1000)
  {
    enemyProbability = 1000;
  }
}

// Method to set color values to referenced RGB values (specially used for random color assignment starting in level 2)
void setColor(byte color, byte& R, byte& G, byte& B)
{
  switch(color)
  {
    case 0:
    R = 0;
    G = 0;
    B = 0;
    break;

    case 1:
    R = 0;
    G = 0;
    B = 7;
    break;

    case 2:
    R = 0;
    G = 7;
    B = 0;
    break;

    case 3:
    R = 0;
    G = 7;
    B = 7;
    break;

    case 4:
    R = 7;
    G = 0;
    B = 0;
    break;

    case 5:
    R = 7;
    G = 0;
    B = 7;
    break;

    case 6:
    R = 7;
    G = 7;
    B = 0;
    break;

    case 7:
    R = 7;
    G = 7;
    B = 7;
    break;
  }
}

// Movement of the centipede
void moveCentipede()
{
  for(byte i = 0; i < lengthOfCentipede; i++)
  {
    // remove segment from old position
    if(centipedeDirection[i] > 0)
    {
      playfield[centipedeX[i]][centipedeY[i]] = 0;
      matrix.drawPixel(centipedeX[i], centipedeY[i], matrix.Color333(0, 0, 0));
    }
    // Algorithm to change the direction if there is a mushroom or the screen border
    switch(centipedeDirection[i])
    {
      case 1: // right down
      if((centipedeX[i] == 31) || (playfield[centipedeX[i] + 1][centipedeY[i]] > 0))
      {
        if(centipedeY[i] == 31)
        {
          centipedeDirection[i] = 4;
          centipedeY[i]--;
        } 
        else 
        {
          centipedeDirection[i] = 2;
          centipedeY[i]++;
        }
      }
      else
      {
        centipedeX[i]++;
      }
      break;

      case 2: // left down
      if((centipedeX[i] == 0) || (playfield[centipedeX[i] - 1][centipedeY[i]] > 0))
      {
        if(centipedeY[i] == 31)
        {
          centipedeDirection[i] = 3;
          centipedeY[i]--;
        } 
        else 
        {
          centipedeDirection[i] = 1;
          centipedeY[i]++;
        }
      }
      else
      {
        centipedeX[i]--;
      }      
      break;

      case 3: // right up
      if((centipedeX[i] == 31) || (playfield[centipedeX[i] + 1][centipedeY[i]] > 0))
      {
        if(centipedeY[i] == 0)
        {
          centipedeDirection[i] = 2;
          centipedeY[i]++;
        } 
        else 
        {
          centipedeDirection[i] = 4;
          centipedeY[i]--;
        }
      }
      else
      {
        centipedeX[i]++;
      }      
      break;

      case 4: // left up
      if((centipedeX[i] == 0) || (playfield[centipedeX[i] - 1][centipedeY[i]] > 0))
      {
        if(centipedeY[i] == 0)
        {
          centipedeDirection[i] = 1;
          centipedeY[i]++;
        } 
        else 
        {
          centipedeDirection[i] = 3;
          centipedeY[i]--;
        }
      }
      else
      {
        centipedeX[i]--;
      }      
      break;
    }
    
    // Draw segment at new position
    if(centipedeDirection[i] > 0)
    {
      playfield[centipedeX[i]][centipedeY[i]] = 1;
      matrix.drawPixel(centipedeX[i], centipedeY[i], matrix.Color333(centipedeR, centipedeG, centipedeB));
    }
  }
}

// Movement of spider
void moveSpider()
{
  // Activate spider randomly
  if(spiderDirection == 0)
  {
    if(random(1000) < enemyProbability)
    {
      if(random(100) < 50)
      {
        spiderDirection = 1;
        spiderX = -10;
        spiderY = random(7) + 24;
      }
      else
      {
        spiderDirection = 2;
        spiderX = 39;
        spiderY = random(7) + 24;
      }
    }
  }
  else
  {
    // Remove spider at old position
    if((spiderX > -2) && (spiderX < 31))
    {
      playfield[spiderX + 1][spiderY] = 0;
      matrix.drawPixel(spiderX + 1, spiderY, matrix.Color333(0, 0, 0));
    }
    if((spiderX > -1) && (spiderX < 32))
    {
      playfield[spiderX][spiderY + 1] = 0;
      matrix.drawPixel(spiderX, spiderY + 1, matrix.Color333(0, 0, 0));
    }
    if((spiderX > -3) && (spiderX < 30))
    {
      playfield[spiderX + 2][spiderY + 1] = 0;
      matrix.drawPixel(spiderX + 2, spiderY + 1, matrix.Color333(0, 0, 0));
    }
    
    switch(spiderDirection)
    {
      case 1: // right down
      if(spiderY == 30)
      {
        spiderDirection = 3;
      }
      else
      {
        spiderX++;
        spiderY++;
      }
      break;
      
      case 2: // left down
      if(spiderY == 30)
      {
        spiderDirection = 4;
      }
      else
      {
        spiderX--;
        spiderY++;
      }
      break;

      case 3: // right up
      if(spiderY == 24)
      {
        spiderDirection = 1;
      }
      else
      {
        spiderX++;
        spiderY--;
      }
      break;

      case 4: // left up
      if(spiderY == 24)
      {
        spiderDirection = 2;
      }
      else
      {
        spiderX--;
        spiderY--;
     }
      break;
    }
    
    // Spider moves out of the screen
    if((spiderX < -10 || spiderX > 39))
    {
      spiderDirection = 0;
    }
    else
    {    
      // Draw spider at new position
      if((spiderX > -2) && (spiderX < 31))
      {
        playfield[spiderX + 1][spiderY] = 4;
        matrix.drawPixel(spiderX + 1, spiderY, matrix.Color333(spiderR, spiderG, spiderB));
      }
      if((spiderX > -1) && (spiderX < 32))
      {
        playfield[spiderX][spiderY + 1] = 4;
        matrix.drawPixel(spiderX, spiderY + 1, matrix.Color333(spiderR, spiderG, spiderB));
      }
      if((spiderX > -3) && (spiderX < 30))
      {
        playfield[spiderX + 2][spiderY + 1] = 4;
        matrix.drawPixel(spiderX + 2, spiderY + 1, matrix.Color333(spiderR, spiderG, spiderB));
      }
    }
  }
}

// Movement of bug
void moveBug()
{
  // Activate bug randomly
  if(bugPresent == false)
  {
    if(random(1000) < enemyProbability)
    {
      bugPresent = true;
      bugX = random(32);
      bugY = -1;
    }
  }
  else
  {
    // Remove bug at old position
    if(bugY > -1)
    {
      if(random(8) == 0)
      {
        // Bug randomly drops a mushroom
        playfield[bugX][bugY] = 2;
        matrix.drawPixel(bugX, bugY, matrix.Color333(mushroomR, mushroomG, mushroomB));        
      }
      else
      {
        playfield[bugX][bugY] = 0;
        matrix.drawPixel(bugX, bugY, matrix.Color333(0, 0, 0));        
      }
    }
    
    // Move bug
    bugY++;

    if(bugY > 31)
    {
      bugPresent = false;
    }
    else
    {   
      //Draw bug at new position
      playfield[bugX][bugY] = 5;
      matrix.drawPixel(bugX, bugY, matrix.Color333(bugR, bugG, bugB));
    }
  }
}

// Movement of dragonfly
void moveDragonfly()
{
  // Dragonfly appears randomly (in level 3 or higher)
  if(dragonflyDirection == 0)
  {
    if((random(1000) < (enemyProbability) / 2) && level > 2)
    {
      if(random(100) < 50)
      {
        dragonflyDirection = 1;
      }
      else
      {
        dragonflyDirection = 2;
      }
      dragonflyX = random(30);
      dragonflyY = -3;
    }
  }
  else
  {
    // Remove dragonfly at old position
    if((dragonflyY > -1) && (dragonflyY < 32))
    {
      playfield[dragonflyX + 1][dragonflyY] = 0;
      matrix.drawPixel(dragonflyX + 1, dragonflyY, matrix.Color333(0, 0, 0));
    }
    if((dragonflyY > -2) && (dragonflyY < 31))
    {
      playfield[dragonflyX + 1][dragonflyY + 1] = 0;
      matrix.drawPixel(dragonflyX + 1, dragonflyY + 1, matrix.Color333(0, 0, 0));
    }
    if((dragonflyY > -3) && (dragonflyY < 30))
    {
      playfield[dragonflyX][dragonflyY + 2] = 0;
      matrix.drawPixel(dragonflyX, dragonflyY + 2, matrix.Color333(0, 0, 0));
    }
    if((dragonflyY > -3) && (dragonflyY < 30))
    {
      playfield[dragonflyX + 1][dragonflyY + 2] = 0;
      matrix.drawPixel(dragonflyX + 1, dragonflyY + 2, matrix.Color333(0, 0, 0));
    }
    if((dragonflyY > -3) && (dragonflyY < 30))
    {
      playfield[dragonflyX + 2][dragonflyY + 2] = 0;
      matrix.drawPixel(dragonflyX + 2, dragonflyY + 2, matrix.Color333(0, 0, 0));
    }
    
    switch(dragonflyDirection)
    {
      case 1: // right down
      if(dragonflyX > 27)
      {
        dragonflyDirection = 2;
      }
      else
      {
        dragonflyX = dragonflyX + 2;
        dragonflyY++;
      }
      break;
      
      case 2: // left down
      if(dragonflyX < 2)
      {
        dragonflyDirection = 1;
      }
      else
      {
        dragonflyX = dragonflyX - 2;
        dragonflyY++;
      }
      break;
    }
    
    // Dragonfly moves out of the screen
    if((dragonflyY > 31))
    {
      dragonflyDirection = 0;
    }
    else
    {    
      // Draw dragonfly at new position
      if((dragonflyY > -1) && (dragonflyY < 32))
      {
        playfield[dragonflyX + 1][dragonflyY] = 6;
        matrix.drawPixel(dragonflyX + 1, dragonflyY, matrix.Color333(3, 3, 3));
      }
      if((dragonflyY > -2) && (dragonflyY < 31))
      {
        playfield[dragonflyX + 1][dragonflyY + 1] = 6;
        matrix.drawPixel(dragonflyX + 1, dragonflyY + 1, matrix.Color333(3, 3, 3));
      }
      if((dragonflyY > -3) && (dragonflyY < 30))
      {
        playfield[dragonflyX][dragonflyY + 2] = 6;
        matrix.drawPixel(dragonflyX, dragonflyY + 2, matrix.Color333(3, 3, 3));
      }
      if((dragonflyY > -3) && (dragonflyY < 30))
      {
        playfield[dragonflyX + 1][dragonflyY + 2] = 6;
        matrix.drawPixel(dragonflyX + 1, dragonflyY + 2, matrix.Color333(3, 0, 0));
      }
      if((dragonflyY > -3) && (dragonflyY < 30))
      {
        playfield[dragonflyX + 2][dragonflyY + 2] = 6;
        matrix.drawPixel(dragonflyX + 2, dragonflyY + 2, matrix.Color333(3, 3, 3));
      }
    }
  }  
}

// Movement of guns
void moveGun()
{
  // Remove gun at old position
  matrix.drawPixel(gunX[0], gunY[0], matrix.Color333(0, 0, 0));
  matrix.drawPixel(gunX[0], gunY[0] + 1, matrix.Color333(0, 0, 0));
  
  // Check joystick 1 for player 1
  if(joy1Left() && (gunX[0] > 0))
  {
    gunX[0]--;
  }
  if(joy1Right() && (gunX[0] < 31))
  {
    gunX[0]++;
  }
  if(joy1Up() && (gunY[0] > 24))
  {
    gunY[0]--;
  }
  if(joy1Down() && (gunY[0] < 30))
  {
    gunY[0]++;
  }
  if(joy1Fire() && (shotFired[0] == false))
  {
    shotX[0] = gunX[0];
    shotY[0] = gunY[0];
    shotFired[0] = true;
    tone(audio,1024,20);
  }
    
  // Draw gun at new position
  matrix.drawPixel(gunX[0], gunY[0], matrix.Color333(gun1R, gun1G, gun1B));
  matrix.drawPixel(gunX[0], gunY[0] + 1, matrix.Color333(gun1R, gun1G, gun1B));
  
  if(numberOfPlayers == 2)
    {
    // Remove gun at old position
    matrix.drawPixel(gunX[1], gunY[1], matrix.Color333(0, 0, 0));
    matrix.drawPixel(gunX[1], gunY[1] + 1, matrix.Color333(0, 0, 0));
  
    // Check joystick 2 for player 2
    if(joy2Left() && (gunX[1] > 0))
    {
      gunX[1]--;
    }
    if(joy2Right() && (gunX[1] < 31))
    {
      gunX[1]++;
    }
    if(joy2Up() && (gunY[1] > 24))
    {
      gunY[1]--;
    }
    if(joy2Down() && (gunY[1] < 30))
    {
      gunY[1]++;
    }
    if(joy2Fire() && (shotFired[1] == false))
    {
      shotX[1] = gunX[1];
      shotY[1] = gunY[1];
      shotFired[1] = true;
      tone(audio,1024,20);
    }
    
    // Draw gun at new position
    matrix.drawPixel(gunX[1], gunY[1], matrix.Color333(gun2R, gun2G, gun2B));
    matrix.drawPixel(gunX[1], gunY[1] + 1, matrix.Color333(gun2R, gun2G, gun2B));    
  }
}

// Movements of shots and hit detection
void moveShot()
{
  for(byte i = 0; i < 3; i++)
  {
    for(byte j = 0; j < numberOfPlayers; j++)
    {
    
      if(shotFired[j])
      {
        // Remove shot at old position
        matrix.drawPixel(shotX[j], shotY[j], matrix.Color333(0, 0, 0));
      
        if(shotY[j] == 0)
        {
          shotFired[j] = false;
        }      
        else if(playfield[shotX[j]][shotY[j] - 1] == 0)
        {
          shotY[j]--;
          // Draw shot at new position
          matrix.drawPixel(shotX[j], shotY[j], matrix.Color333(7, 7, 7));
        }
        else
        {
          // Hit
          tone(audio,512,20);
          switch(playfield[shotX[j]][shotY[j] - 1])
          {
          
            case 1: // Centipede hit
            for(byte i = 0; i < lengthOfCentipede; i++)
            {
              if((centipedeX[i] == shotX[j]) && (centipedeY[i] == shotY[j] - 1) && (centipedeDirection[i] != 0))
              {
                centipedeDirection[i] = 0;
                segmentsKilled++;
                // Turn hit segment into a mushroom
                playfield[shotX[j]][shotY[j] - 1] = 2;
                matrix.drawPixel(shotX[j], shotY[j] - 1, matrix.Color333(mushroomR, mushroomG, mushroomB));
                shotFired[j] = false;
                score[j] = score[j] + 10;
                extraLifeScore[j] = extraLifeScore[j] + 10;
              }
            }
            break;
        
            case 2: // Mushroom hit
            playfield[shotX[j]][shotY[j] - 1] = 0;
            matrix.drawPixel(shotX[j], shotY[j] - 1, matrix.Color333(0, 0, 0));
            shotFired[j] = false;
            score[j] = score[j] + 1;
            extraLifeScore[j] = extraLifeScore[j] + 1;
            break;
          
            case 4: // Spider hit
            spiderDirection = 0;
            if((spiderX > -2) && (spiderX < 31))
            {
              playfield[spiderX + 1][spiderY] = 0;
              matrix.drawPixel(spiderX + 1, spiderY, matrix.Color333(0, 0, 0));
            }
            if((spiderX > -1) && (spiderX < 32))
            {
              playfield[spiderX][spiderY + 1] = 0;
              matrix.drawPixel(spiderX, spiderY + 1, matrix.Color333(0, 0, 0));
            }
            if((spiderX > -3) && (spiderX < 30))
            {
              playfield[spiderX + 2][spiderY + 1] = 0;
              matrix.drawPixel(spiderX + 2, spiderY + 1, matrix.Color333(0, 0, 0));
            }
            shotFired[j] = false;
            score[j] = score[j] + 40;
            extraLifeScore[j] = extraLifeScore[j] + 40;
            break;
 
            case 5: // Bug hit
            bugPresent = false;
            playfield[bugX][bugY] = 0;
            matrix.drawPixel(bugX, bugY, matrix.Color333(0, 0, 0));        
            shotFired[j] = false;
            score[j] = score[j] + 20;
            extraLifeScore[j] = extraLifeScore[j] + 20;
            break;
  
            case 6: // Dragonfly hit
            dragonflyDirection = 0;
            if((dragonflyY > -1) && (dragonflyY < 32))
            {
              playfield[dragonflyX + 1][dragonflyY] = 0;
              matrix.drawPixel(dragonflyX + 1, dragonflyY, matrix.Color333(0, 0, 0));
            }
            if((dragonflyY > -2) && (dragonflyY < 31))
            {
              playfield[dragonflyX + 1][dragonflyY + 1] = 0;
              matrix.drawPixel(dragonflyX + 1, dragonflyY + 1, matrix.Color333(0, 0, 0));
            }
            if((dragonflyY > -3) && (dragonflyY < 30))
            {
              playfield[dragonflyX][dragonflyY + 2] = 0;
              matrix.drawPixel(dragonflyX, dragonflyY + 2, matrix.Color333(0, 0, 0));
            }
            if((dragonflyY > -3) && (dragonflyY < 30))
            {
              playfield[dragonflyX + 1][dragonflyY + 2] = 0;
              matrix.drawPixel(dragonflyX + 1, dragonflyY + 2, matrix.Color333(0, 0, 0));
            }
            if((dragonflyY > -3) && (dragonflyY < 30))
            {
              playfield[dragonflyX + 2][dragonflyY + 2] = 0;
              matrix.drawPixel(dragonflyX + 2, dragonflyY + 2, matrix.Color333(0, 0, 0));
            }
            shotFired[j] = false;
            score[j] = score[j] + 100;
            extraLifeScore[j] = extraLifeScore[j] + 100;
            break;          
          }       
        }
      }
    }
  }
}

// Randomly drop mushrooms on the screen (used before a new level starts)
void drawMushrooms(int _numberOfMushrooms)
{
  for(int i = 0; i < _numberOfMushrooms; i++)
  {
    byte mushroomX = random(32);
    byte mushroomY = random(23) + 1;
    playfield[mushroomX][mushroomY] = 2;
    matrix.drawPixel(mushroomX, mushroomY, matrix.Color333(mushroomR, mushroomG, mushroomB));
  }
}

// Collision detection of players with enemies 
boolean collisionDetection()
{
  boolean collision = false;
  
  for(byte i = 0; i < numberOfPlayers; i++)
  {
    if((playfield[gunX[i]][gunY[i]] || playfield[gunX[i]][gunY[i] + 1]) > 0)
    {
      explosion(gunX[i], gunY[i]);
      lives[i]--;
      collision = true;
    }
  }
  if(collision)
  {
    return true;
  }
  return false;
}

// Creates an explosion if a player has been hit
void explosion(byte _x, byte _y){
  matrix.fillCircle(_x, _y, 1, matrix.Color333(7, 7, 7));
  tone(audio,2048,40);
  delay(40);
  matrix.fillCircle(_x, _y, 2, matrix.Color333(7, 7, 5));
  tone(audio,1024,40);
  delay(40);
  matrix.fillCircle(_x, _y, 3, matrix.Color333(7, 7, 3));
  tone(audio,512,40);
  delay(40);
  matrix.fillCircle(_x, _y, 4, matrix.Color333(5, 5, 3));
  tone(audio,256,40);
  delay(40);
  matrix.fillCircle(_x, _y, 5, matrix.Color333(4, 4, 2));
  tone(audio,128,40);
  delay(40);
  matrix.fillCircle(_x, _y, 6, matrix.Color333(3, 3, 2));
  tone(audio,64,40);
  delay(40);
  matrix.fillCircle(_x, _y, 7, matrix.Color333(2, 2, 1));
  tone(audio,32,40);
  delay(40);
  matrix.fillCircle(_x, _y, 8, matrix.Color333(1, 1, 0));
  tone(audio,16,40);
  delay(40);
  for(byte i = 1; i < 9; i++){
    matrix.fillCircle(_x, _y, i, matrix.Color333(0, 0, 0));
    delay(40);
  }
}

// Wait (which can be interrupted by pressing fire button)
void wait(int _waitingTime)
{
  int i;
  boolean firePressed = false;
  if(_waitingTime > 200){ i = _waitingTime - 200; }
  else{ i = _waitingTime; }
  delay(200);
  
  do
  {
    i--;
    delay(1);
    if(joy1Fire() || joy2Fire()){ firePressed = true; }
  }
  while(i > 0 && !firePressed);
  
  if(firePressed)
  { 
    tone(audio,1024,20); 
    delay(20);
  }
}

// Main loop of the game
void loop() 
{
  // Start screen
  setupGame();  
  do
  {
    // Start or retry a level
    setupLevel();
    do
    {
      // Sounds depending on enemies on screen
      tone(audio,200,20);
      if(bugPresent)
      {
        tone(audio,200,40);
      }
      if(spiderDirection > 0)
      {
        tone(audio,400,40);
      }
      if(dragonflyDirection > 0)
      {
        tone(audio,800,40);
      }
      
      // Moving enemies
      moveCentipede();
      moveSpider();
      moveBug();
      moveDragonfly();
      
      // Moving guns and shots
      moveGun();
      moveShot();
      
      // Delay depending on game speed
      delay(gameSpeed);
    
      // Killed
      if(collisionDetection())
      {
        break;
      }
      
      // Centipede killed
      if(segmentsKilled == lengthOfCentipede)
      {
        centipedeKilled = true;
        
        // Level completed sound
        for(int i = 0; i < 5; i++)
        {
          for(int j = 0; j < 100; j++)
          {
             tone(audio, j * 50, 2);
             delay(2);
          }
        }
        
        // Add level bonus
        score[0] = score[0] + (level * 50) * ((lengthOfCentipede / 4) - 2);
        extraLifeScore[0] = extraLifeScore[0] + (level * 50) * ((lengthOfCentipede / 4) - 2);
        if(numberOfPlayers == 2)
        {
          score[1] = score[1] + (level * 50) * ((lengthOfCentipede / 4) - 2);
          extraLifeScore[1] = extraLifeScore[1] + (level * 50) * ((lengthOfCentipede / 4) - 2);
        }
        
        level++; // Increase level
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
        while((!digitalRead(buttonPause) == LOW) && !joy1Fire() && !joy2Fire());
        tone(audio,1024,20);
        delay(500);
      }
    
      // Reset
      if(digitalRead(buttonReset) == LOW)
      {
        reset = true;
        break;
      }
      
    }
    while(true);
    delay(1000);
  }
  while((lives[0] > 0) && (lives[1] > 0) && !reset);

  if(!reset)
  {

    // Game over
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    for(int i = 0; i < 100; i++)
    {
      matrix.setCursor(4, 8);
      matrix.setTextColor(matrix.Color333(7 * (i % 2),0,0));
      matrix.println("GAME");
      matrix.setCursor(4, 16);
      matrix.println("OVER");
      tone(audio,i * 10,10);
      delay(10);
    }
    matrix.setCursor(4, 8);
    matrix.setTextColor(matrix.Color333(7,0,0));
    matrix.println("GAME");
    matrix.setCursor(4, 16);
    matrix.println("OVER");
    wait(3000);

    // Show winner (if two players)
    if((numberOfPlayers == 2) && (score[0] != score[1]))
    {
      matrix.fillScreen(matrix.Color333(0, 0, 0));
      matrix.setCursor(4, 4);
      matrix.setTextColor(matrix.Color333(0,0,7));
      matrix.println("Plyr");
      matrix.setCursor(14, 12);
      if(score[0] > score[1])
      {
        matrix.print(1);
      }
      else
      {
        matrix.print(2);
      }
      matrix.setCursor(2, 20);
      matrix.println("wins!");
      wait(4000);
    }
  
    // Set high score if reached
    if(score[0] > highScore)
    {
      highScore = score[0];
      matrix.fillScreen(matrix.Color333(0, 0, 0));
      for(byte i = 0; i < 5; i++)
      {
        explosion(random(24) + 4, random(24) + 4);
      }
      matrix.fillScreen(matrix.Color333(0, 0, 0));

      for(byte i = 100; i > 0; i--)
      {
        matrix.setCursor(7, 8);
        matrix.setTextColor(matrix.Color333(7 * (i % 2), 7 * (i % 2), 7 * (i % 2)));
        matrix.println("NEW");
        matrix.setCursor(1, 16);
        matrix.println("HiScr");
        tone(audio,i * 50,10);
        delay(10);
      }
      matrix.setCursor(7, 8);
      matrix.setTextColor(matrix.Color333(7,7,7));
      matrix.println("NEW");
      matrix.setCursor(1, 16);
      matrix.println("HiScr");
      wait(4000);    
    }

    // Show final score
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.setCursor(1, 0);
    matrix.setTextColor(matrix.Color333(3,3,3));
    matrix.println("Score");
    matrix.setCursor(1, 8);
    matrix.setTextColor(matrix.Color333(gun1R, gun1G, gun1B));
    matrix.println(score[0]);
    if(numberOfPlayers == 2)
    {
      matrix.setCursor(1, 16);
      matrix.setTextColor(matrix.Color333(gun2R, gun2G, gun2B));
      matrix.println(score[1]);
    }
    matrix.setTextColor(matrix.Color333(3,3,3));
    matrix.setCursor(1, 24);
    matrix.println(highScore);
    wait(4000);
  }
}

