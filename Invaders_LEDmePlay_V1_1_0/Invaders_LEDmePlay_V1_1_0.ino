// Invaders
// for the LEDmePlay
//
// www.mithotronic.de
//
// Version: 1.1.0
// Authors: Michael Rosskopf and Thomas Laubach (2016)
// 
// Release Notes:
// V1.1.0: Support for LEDmePlay Joypad and LEDmePlayBoy (2018)
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

// Other buttons
int buttonReset = 42;
int buttonPause = 43;

// Signals if reset button has been pressed
boolean reset;

// Game variables

// Playfield
byte playfield[32][32]; // 0 = empty, 1 = shield, 2 = mothership, 3 = boss, 4 ... 23 = enemy 0 - 19, 255 = bombs

// Player´s base
byte lives;
byte baseX;
byte level;
byte difficulty;
int waitTimerPlayer;
int currentWaitTimerPlayer;
byte playerShotX;
byte playerShotY;
byte playerShotStatus; // 0 = not fired, 1 = fired
int waitTimerPlayerShot;
int currentWaitTimerPlayerShot;
int score;
int extraLifeScore; // Extralife every 2000 points
int highscore;
int extraLifeCoughtTimer;
boolean baseDestroyed;
int waitCounterBaseDestroyed;

// Alien formation
int formationX;
int formationY;
boolean formationLanded; // true, if formation is so deep that player cannot win (level is restarted)
byte formationWidth = 5;
byte formationHeight = 4;
byte columnOffsetLeft;
byte columnOffsetRight;
byte animationPhase; // also used by bosses
int waitTimerFormation;
int currentWaitTimerFormation;
byte remainingAliens;
byte movementStatus;  // 0 = right, 1 = down, 2 = left, 3 = up
byte alienType[64];   // Variants of aliens (1 - 5)
byte alienStatus[64]; // 0 = absent, 1 = present, 2 = hit

// Alien bombs
int bombingRate;
int waitTimerBombs;
int currentWaitTimerBombs;
byte bombPointer;
float bombX[16];
byte bombY[16];
float bombDrift[16];
byte bombStatus[16]; // 0 = absent, 1 = present

// Alien mothership
int mothershipX;
byte mothershipStatus; // 0 = absent, 1 = present, 2 = hit
int mothershipRate;
int waitTimerMothership;
int currentWaitTimerMothership;

// Bosses
boolean bossActive;
byte bossType;
byte bossShield;
byte bossWidth;
byte bossX;
byte bossY;
byte bossMovementStatus;
int waitTimerBoss;
int currentWaitTimerBoss;
byte invader1[128] = {
  0, 0, 0, 1, 1, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 0,
  1, 1, 2, 1, 1, 2, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  0, 1, 0, 1, 1, 0, 1, 0,
  1, 0, 0, 0, 0, 0, 0, 1,
  0, 1, 0, 0, 0, 0, 1, 0,

  0, 0, 0, 1, 1, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 0,
  1, 1, 2, 1, 1, 2, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 1, 0, 0, 1, 0, 0,
  0, 1, 0, 1, 1, 0, 1, 0,
  1, 0, 1, 0, 0, 1, 0, 1
};

byte invader2[176] = {
  0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
  0, 1, 1, 2, 1, 1, 1, 2, 1, 1, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1,
  1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1,
  0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0,

  0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
  1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1,
  1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1,
  1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0,
  0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0
};

byte invader3[192] = {
  0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0,
  0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0,
  0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0,

  0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0,
  0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1
};

// Explosions
int waitTimerExplosions;
int currentWaitTimerExplosions;
byte explosionPointer;
byte explosionX[16];
byte explosionY[16];
byte explosionStatus[16]; // 0 = No explosion, 1 - 5 = Animation phases

// Title
byte title[150] = {
  1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1,
  1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0,
  1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1,
  1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1,
  1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1
};

#define RIGHT 0
#define DOWN 1
#define LEFT 2
#define UP 3

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

  // Initialize scores;
  highscore = 0;

  // Initialize difficulty;
  difficulty = 2;

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
  while (i <= 11);

  // Jingle on start screen
  tone(audio, NOTE_C4, 200);
  delay(400 + 20);
  tone(audio, NOTE_C4, 90);
  delay(200 - 20);
  tone(audio, NOTE_G4, 140);
  delay(400 + 20);
  tone(audio, NOTE_G4, 140);
  delay(200 - 20);
  tone(audio, NOTE_C5, 450);
  delay(600);
  tone(audio, NOTE_AS4, 140);
  delay(200 - 20);
  tone(audio, NOTE_A4, 130);
  delay(200 - 10);
  tone(audio, NOTE_F4, 120);
  delay(200);
  tone(audio, NOTE_G4, 1000);
  delay(3000);
}

// Sets color for the next character to show the LEDmePLay logo
void setLEDMePlayColor(int i)
{
  switch (i % 9)
  {
    case 0:
      matrix.setTextColor(matrix.Color333(5, 0, 0));
      break;
    case 1:
      matrix.setTextColor(matrix.Color333(5, 2, 0));
      break;
    case 2:
      matrix.setTextColor(matrix.Color333(2, 5, 0));
      break;
    case 3:
      matrix.setTextColor(matrix.Color333(0, 5, 0));
      break;
    case 4:
      matrix.setTextColor(matrix.Color333(0, 5, 2));
      break;
    case 5:
      matrix.setTextColor(matrix.Color333(0, 2, 5));
      break;
    case 6:
      matrix.setTextColor(matrix.Color333(0, 0, 5));
      break;
    case 7:
      matrix.setTextColor(matrix.Color333(2, 0, 5));
      break;
    case 8:
      matrix.setTextColor(matrix.Color333(5, 0, 2));
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
    if (i > 81)
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
  tone(audio, 1024, 20);
  delay(200);
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
}

void titleScreen()
{
  // Delete screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Draw text "INVADERS"
  for (byte j = 0; j < 5; j++)
  {
    for (byte i = 0; i < 30; i++)
    {
      if (title[(j * 30) + i] == 1)
      {
        matrix.drawPixel(i + 1, j + 3, matrix.Color333(1, 1, 0));
      }
    }
  }

  // Draw shield
  drawShields();

  // Wait for fire button to be pressed
  int timer = 2000;
  do
  {
    // Draw boss invader
    if (timer == 2000) {
      drawBoss(1, 1, 12, 12, 8, true);
    }
    if (timer == 1000) {
      drawBoss(1, 2, 12, 12, 8, true);
    }

    timer--;
    if (timer == 0) {
      timer = 2000;
    }
    delay(1);
  }
  while (!joy1Fire());
  tone(audio, 1024, 20);
  delay(100);

  // Delete screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Show highscore
  matrix.setCursor(1, 0);
  matrix.setTextColor(matrix.Color333(3, 3, 0));
  matrix.println("HiScr");
  matrix.setCursor(1, 8);
  matrix.println(highscore);
  delay(100);

  // Select difficulty
  int i = 0;
  do
  {
    matrix.setTextColor(matrix.Color888(i, i, 0));
    matrix.setCursor(1, 16);
    matrix.println("<Sel>");

    matrix.setTextColor(matrix.Color333(3, 3, 0));
    matrix.setCursor(4, 24);
    if (difficulty == 1)
    {
      matrix.println("Easy");
    }
    else if (difficulty == 2)
    {
      matrix.println("Norm");
    }
    else if (difficulty == 3)
    {
      matrix.println("Hard");
    }
    else
    {
      matrix.println("Ultr");
    }

    i = i + 2;
    if (i == 256) {
      i = 0;
    }

    if (joy1Left())
    {
      difficulty--;
      if (difficulty == 0) {
        difficulty = 4;
      }
      matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
      delay(200);
    }
    else if(joy1Right())
    {
      difficulty++;
      if (difficulty == 5) {
        difficulty = 1;
      }
      matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
      delay(200);
    }
  }
  while (!joy1Fire());
  tone(audio, 1024, 20);
  delay(100);

  if (difficulty == 4)
  {
    // Delete screen
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
    matrix.setTextColor(matrix.Color333(3, 3, 0));
    matrix.setCursor(1, 4);
    matrix.println("Sorry");
    matrix.setCursor(10, 12);
    matrix.println("no");
    matrix.setCursor(1, 20);
    matrix.println("cover");
    delay(2000);
  }
}

void setupGame()
{
  lives = 3;
  score = 0;
  extraLifeScore = 0;
  level = 1;
  reset = false;
}

void setupLevel()
{
  for (byte i = 0; i < 32; i++)
  {
    for (byte j = 0; j < 32; j++)
    {
      playfield[i][j] = 0;
    }
  }

  // Delete screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));

  // Draw shields
  // Difficulty specific definition: No shield with difficulty "Ultra"
  if (difficulty < 4) {
    drawShields();
  }

  // Setup alien formation
  for (byte i = 0; i < formationWidth * formationHeight; i++)
  {
    if (level > 7)
    {
      alienType[i] = random(5) + 1;
    }
    else
    {
      alienType[i] = (formationHeight - (i / formationWidth) + level) / 2;
      if (alienType[i] > 5) {
        alienType[i] = 5;
      }
    }
    alienStatus[i] = 1;
  }
  formationX = 0;
  formationY = 4;
  formationLanded = false;
  movementStatus = 0;
  columnOffsetLeft = 0;
  columnOffsetRight = 0;
  animationPhase = 1;
  remainingAliens = formationWidth * formationHeight;

  // Every second wave contains an extra invader (boss)
  if (level % 2 == 0) {
    remainingAliens = remainingAliens + 1;
  }

  // Level specific definition: Formation speed
  waitTimerFormation = remainingAliens * (50 - (level - 1) * 2);
  if (waitTimerFormation < 10) {
    waitTimerFormation = 10;
  }

  // Setup mothership
  mothershipStatus = 0;
  mothershipRate = 10;

  // Level specific definition: Mothership speed
  waitTimerMothership = 160 - (level * 10);
  if (waitTimerMothership < 10) {
    waitTimerMothership = 10;
  }

  // Bosses (approaching at the end of every second wave)
  bossActive = false;
  
  // Level specific definition: Boss speed
  waitTimerBoss = 110 - (5 * level);
  if(waitTimerBoss < 10){ waitTimerBoss = 10; }
        
  // Level specific definition: Boss shield
  bossShield = 6 + (level * 2);

  // Initialize bombs
  for (byte i = 0; i < 16; i++)
  {
    bombStatus[i] = 0;
    bombX[i] = 0.0;
    bombY[i] = 0;
    bombDrift[i] = 0;
  }
  // Level specific definition: Bombing rate
  bombingRate = 165 + (level * 15);
  if (bombingRate > 800) {
    bombingRate = 800;
  }

  // Difficulty specific definition: Speed of bombs
  waitTimerBombs = 60 - (difficulty * 10);
  bombPointer = 0;

  // Initialize explosions
  for (byte i = 0; i < 16; i++)
  {
    explosionStatus[i] = 0;
    explosionX[i] = 0;
    explosionY[i] = 0;
  }
  waitTimerExplosions = 40;
  explosionPointer = 0;

  // Initialize player
  baseX = 14;
  baseDestroyed = false;
  waitTimerPlayer = 100;
  waitTimerPlayerShot = 20;
  extraLifeCoughtTimer = 0;
  playerShotStatus = 0;
}

// Draws the shields on the screen and into the playfield
void drawShields()
{
  for (byte i = 0; i < 4; i++)
  {
    // draw into playfield
    playfield[4 + i * 7][25] = 1;
    playfield[5 + i * 7][25] = 1;
    playfield[6 + i * 7][25] = 1;
    playfield[3 + i * 7][26] = 1;
    playfield[4 + i * 7][26] = 1;
    playfield[5 + i * 7][26] = 1;
    playfield[6 + i * 7][26] = 1;
    playfield[7 + i * 7][26] = 1;
    playfield[3 + i * 7][27] = 1;
    playfield[4 + i * 7][27] = 1;
    playfield[5 + i * 7][27] = 1;
    playfield[6 + i * 7][27] = 1;
    playfield[7 + i * 7][27] = 1;
    playfield[3 + i * 7][28] = 1;
    playfield[7 + i * 7][28] = 1;
  }

  // Draw shield animation on screen
  for (byte i = 0; i < 4; i++)
  {
    matrix.drawPixel(7 + i * 7, 28, matrix.Color333(0, 1, 0));
    matrix.drawPixel(3 + i * 7, 28, matrix.Color333(0, 1, 0));
  }
  delay(100);
  for (byte i = 0; i < 4; i++)
  {
    matrix.drawLine(3 + i * 7, 27, 7 + i * 7, 27, matrix.Color333(0, 3, 0));
  }
  delay(100);
  for (byte i = 0; i < 4; i++)
  {
    matrix.drawLine(3 + i * 7, 26, 7 + i * 7, 26, matrix.Color333(0, 4, 0));
  }
  delay(100);
  for (byte i = 0; i < 4; i++)
  {
    matrix.drawLine(4 + i * 7, 25, 6 + i * 7, 25, matrix.Color333(0, 5, 0));
  }
  delay(100);
}

// Moves the complete formation of aliens
void moveFormation()
{
  byte k = 0; // Alien number in formation
  // Iterate over all columns and lines of formation
  for (byte j = 0; j < formationHeight; j++)
  {
    for (byte i = 0; i < formationWidth; i++)
    {
      // Draw aliens which are active (not destroyed)
      if (alienStatus[k] == 1)
      {
        // Remove a specific alien
        removeAlien((i * 5) + formationX, (j * 4) + formationY);

        // Draw a specific alien at a new position (depending on current movement direction)
        if (baseDestroyed) // No movement after base has been destroyed
        {
          drawAlien(alienType[k], animationPhase, k, (i * 5) + formationX, (j * 4) + formationY);
        }
        else if (movementStatus == RIGHT) // formation moves right
        {
          drawAlien(alienType[k], animationPhase, k, (i * 5) + formationX + 1, (j * 4) + formationY);
          if (bombingRate > random(1000) && !alienBelow(k))
          {
            dropBomb((i * 5) + formationX + 2 + random(2), (j * 4) + formationY + 3, 0.0);
          }
        }
        else if (movementStatus == DOWN) // formation moves down
        {
          drawAlien(alienType[k], animationPhase, k, (i * 5) + formationX, (j * 4) + formationY + 1);
          if (bombingRate > random(1000) && !alienBelow(k))
          {
            dropBomb((i * 5) + formationX + 1 + random(2), (j * 4) + formationY + 4, 0.0);
          }
        }
        else if (movementStatus == LEFT) // formation moves left
        {
          drawAlien(alienType[k], animationPhase, k, (i * 5) + formationX - 1, (j * 4) + formationY);
          if (bombingRate > random(1000) && !alienBelow(k))
          {
            dropBomb((i * 5) + formationX + random(2), (j * 4) + formationY + 3, 0.0);
          }
        }
        else // formation moves up
        {
          drawAlien(alienType[k], animationPhase, k, (i * 5) + formationX, (j * 4) + formationY - 1);
          if (bombingRate > random(1000) && !alienBelow(k))
          {
            dropBomb((i * 5) + formationX + 1 + random(2), (j * 4) + formationY + 2, 0.0);
          }
        }
      }
      k++;
    }
  }
  // Update formation position after movement of all aliens
  if (baseDestroyed)
  {
    // No movement after base has been destroyed
  }
  else if (movementStatus == RIGHT)
  {
    formationX++;
    // Change movement direction to down
    if (formationX >= 32 - (formationWidth * 5) + 1 + (columnOffsetRight * 5))
    {
      movementStatus = DOWN;
    }
  }
  else if (movementStatus == LEFT)
  {
    formationX--;
    // Change movement direction to down
    if (formationX <= 0 - (columnOffsetLeft * 5))
    {
      movementStatus = DOWN;
    }
  }
  else if (movementStatus == DOWN)
  {
    formationY++;
    // Change movement direction to right
    if (formationX <= 0 - (columnOffsetLeft * 5))
    {
      movementStatus = RIGHT;
    }
    // Change movement direction to left
    else
    {
      movementStatus = LEFT;
    }
  }
  if (animationPhase == 1) {
    animationPhase = 2;
  }
  else {
    animationPhase = 1;
  }
}

// Alien drops a bomb
void dropBomb(byte _x, byte _y, float _drift)
{
  // After base has been destroyed, no new bomb is thrown
  if (bombStatus[bombPointer] == 0 && !baseDestroyed)
  {
    bombStatus[bombPointer] = 1;
    bombX[bombPointer] = float(_x);
    bombY[bombPointer] = _y;
    bombDrift[bombPointer] = _drift;
    bombPointer++;
    if (bombPointer == 16)
    {
      bombPointer = 0;
    }
    // SOUND: Drop bomb
    tone(audio, 400, 20);
  }
}

// Move falling bombs
void moveBombs()
{
  // Iterate over all possible bombs
  for (byte i = 0; i < 16; i++)
  {
    // Move those bombs which are active
    if (bombStatus[i] == 1)
    {
      // Remove bomb at old position
      matrix.drawPixel(int(bombX[i]), bombY[i], matrix.Color333(0, 0, 0));
      playfield[int(bombX[i])][bombY[i]] = 0;

      // Move bomb
      bombY[i]++;

      // Bomb drift
      bombX[i] = float(bombX[i]) + float(bombDrift[i]);
      if(int(bombX[i]) < 0){ bombX[i] = 0.0; }
      if(int(bombX[i]) > 31){ bombX[i] = 31.0; }
      if(bombDrift[i] < 0.0)
      {
        bombDrift[i] = bombDrift[i] + 0.2;
      }
      else if(bombDrift[i] > 0.0)
      {
        bombDrift[i] = bombDrift[i] - 0.2;
      }

      // Bomb hits shield
      if(playfield[int(bombX[i])][bombY[i]] == 1)
      {
        bombStatus[i] = 0;

        // Remove part of shield
        matrix.drawPixel(int(bombX[i]), bombY[i], matrix.Color333(0, 0, 0));
        playfield[int(bombX[i])][bombY[i]] = 0;

        // SOUND: Shield hit
        tone(audio, 200, 20);
      }
      if(bombY[i] == 32)
      {
        bombStatus[i] = 0;        
      }

      // Draw bomb
      if (bombStatus[i] == 1)
      {
        matrix.drawPixel(int(bombX[i]), bombY[i], matrix.Color333(1, 1, 1));
        playfield[int(bombX[i])][bombY[i]] = 255;
      }
    }
  }
}

// Draws alien of a specific type with a specifc index in a defined movement phase at position (x, y)
void drawAlien(byte type, byte phase, byte index, byte x, byte y)
{
  if (type == 1)
  {
    matrix.drawPixel(x, y, matrix.Color333(1, 1, 1));
    matrix.drawPixel(x + 3, y, matrix.Color333(1, 1, 1));
    matrix.drawPixel(x, y + 1, matrix.Color333(3, 3, 3));
    matrix.drawPixel(x + 3, y + 1, matrix.Color333(3, 3, 3));
    matrix.drawPixel(x + 1, y + 2, matrix.Color333(1, 1, 1));
    matrix.drawPixel(x + 2, y + 2, matrix.Color333(1, 1, 1));
    playfield[x][y] = index + 4;
    playfield[x + 3][y] = index + 4;
    playfield[x][y + 1] = index + 4;
    playfield[x + 1][y + 1] = index + 4;
    playfield[x + 2][y + 1] = index + 4;
    playfield[x + 3][y + 1] = index + 4;
    playfield[x + 1][y + 2] = index + 4;
    playfield[x + 2][y + 2] = index + 4;
    if (phase == 1)
    {
      matrix.drawPixel(x + 1, y + 1, matrix.Color333(0, 0, 7));
      matrix.drawPixel(x + 2, y + 1, matrix.Color333(0, 0, 1));
    }
    else
    {
      matrix.drawPixel(x + 1, y + 1, matrix.Color333(0, 0, 1));
      matrix.drawPixel(x + 2, y + 1, matrix.Color333(0, 0, 7));
    }
  }
  else if (type == 2)
  {
    matrix.drawPixel(x + 1, y, matrix.Color333(0, 0, 3));
    matrix.drawPixel(x + 2, y, matrix.Color333(0, 0, 3));
    matrix.drawPixel(x + 1, y + 1, matrix.Color333(3, 0, 0));
    matrix.drawPixel(x + 2, y + 1, matrix.Color333(3, 0, 0));
    playfield[x + 1][y] = index + 4;
    playfield[x + 2][y] = index + 4;
    playfield[x + 1][y + 1] = index + 4;
    playfield[x + 2][y + 1] = index + 4;
    if (phase == 1)
    {
      matrix.drawPixel(x, y + 1, matrix.Color333(1, 0, 0));
      matrix.drawPixel(x + 3, y + 1, matrix.Color333(1, 0, 0));
      playfield[x][y + 1] = index + 4;
      playfield[x + 3][y + 1] = index + 4;
    }
    else
    {
      matrix.drawPixel(x, y + 2, matrix.Color333(1, 0, 0));
      matrix.drawPixel(x + 3, y + 2, matrix.Color333(1, 0, 0));
      playfield[x][y + 2] = index + 4;
      playfield[x + 3][y + 2] = index + 4;
    }
  }
  else if (type == 3)
  {
    matrix.drawPixel(x + 1, y, matrix.Color333(0, 1, 1));
    matrix.drawPixel(x + 2, y, matrix.Color333(0, 1, 1));
    matrix.drawPixel(x + 1, y + 2, matrix.Color333(0, 1, 1));
    matrix.drawPixel(x + 2, y + 2, matrix.Color333(0, 1, 1));
    playfield[x + 1][y] = index + 4;
    playfield[x + 2][y] = index + 4;
    playfield[x][y + 1] = index + 4;
    playfield[x + 1][y + 1] = index + 4;
    playfield[x + 2][y + 1] = index + 4;
    playfield[x + 3][y + 1] = index + 4;
    playfield[x + 1][y + 2] = index + 4;
    playfield[x + 2][y + 2] = index + 4;
    if (phase == 1)
    {
      matrix.drawPixel(x, y + 1, matrix.Color333(0, 1, 1));
      matrix.drawPixel(x + 1, y + 1, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x + 2, y + 1, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x + 3, y + 1, matrix.Color333(0, 1, 1));
    }
    else
    {
      matrix.drawPixel(x, y + 1, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x + 1, y + 1, matrix.Color333(0, 1, 1));
      matrix.drawPixel(x + 2, y + 1, matrix.Color333(0, 1, 1));
      matrix.drawPixel(x + 3, y + 1, matrix.Color333(0, 0, 3));
    }
  }
  else if (type == 4)
  {
    matrix.drawPixel(x, y, matrix.Color333(1, 1, 0));
    matrix.drawPixel(x + 1, y, matrix.Color333(3, 3, 0));
    matrix.drawPixel(x + 2, y, matrix.Color333(3, 3, 0));
    matrix.drawPixel(x + 3, y, matrix.Color333(1, 1, 0));
    playfield[x][y] = index + 4;
    playfield[x + 1][y] = index + 4;
    playfield[x + 2][y] = index + 4;
    playfield[x + 3][y] = index + 4;
    if (phase == 1)
    {
      matrix.drawPixel(x, y + 1, matrix.Color333(1, 1, 0));
      matrix.drawPixel(x + 1, y + 1, matrix.Color333(3, 3, 0));
      matrix.drawPixel(x + 2, y + 1, matrix.Color333(3, 3, 0));
      matrix.drawPixel(x + 3, y + 1, matrix.Color333(1, 1, 0));
      playfield[x][y + 1] = index + 4;
      playfield[x + 1][y + 1] = index + 4;
      playfield[x + 2][y + 1] = index + 4;
      playfield[x + 3][y + 1] = index + 4;
    }
    else
    {
      matrix.drawPixel(x + 1, y + 1, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x + 2, y + 1, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x, y + 2, matrix.Color333(1, 1, 0));
      matrix.drawPixel(x + 1, y + 2, matrix.Color333(3, 3, 0));
      matrix.drawPixel(x + 2, y + 2, matrix.Color333(3, 3, 0));
      matrix.drawPixel(x + 3, y + 2, matrix.Color333(1, 1, 0));
      playfield[x + 1][y + 1] = index + 4;
      playfield[x + 2][y + 1] = index + 4;
      playfield[x][y + 2] = index + 4;
      playfield[x + 1][y + 2] = index + 4;
      playfield[x + 2][y + 2] = index + 4;
      playfield[x + 3][y + 2] = index + 4;
    }
  }
  else
  {
    matrix.drawPixel(x, y, matrix.Color333(1, 0, 1));
    matrix.drawPixel(x + 1, y, matrix.Color333(3, 0, 3));
    matrix.drawPixel(x + 2, y, matrix.Color333(3, 0, 3));
    matrix.drawPixel(x + 3, y, matrix.Color333(1, 0, 1));
    matrix.drawPixel(x, y + 1, matrix.Color333(3, 0, 3));
    matrix.drawPixel(x + 1, y + 1, matrix.Color333(0, 0, 3));
    matrix.drawPixel(x + 2, y + 1, matrix.Color333(0, 0, 3));
    matrix.drawPixel(x + 3, y + 1, matrix.Color333(3, 0, 3));
    playfield[x][y] = index + 4;
    playfield[x + 1][y] = index + 4;
    playfield[x + 2][y] = index + 4;
    playfield[x + 3][y] = index + 4;
    playfield[x][y + 1] = index + 4;
    playfield[x + 1][y + 1] = index + 4;
    playfield[x + 2][y + 1] = index + 4;
    playfield[x + 3][y + 1] = index + 4;
    if (phase == 1)
    {
      matrix.drawPixel(x + 1, y + 2, matrix.Color333(1, 0, 1));
      matrix.drawPixel(x + 2, y + 2, matrix.Color333(1, 0, 1));
      playfield[x + 1][y + 2] = index + 4;
      playfield[x + 2][y + 2] = index + 4;
    }
    else
    {
      matrix.drawPixel(x, y + 2, matrix.Color333(1, 0, 1));
      matrix.drawPixel(x + 3, y + 2, matrix.Color333(1, 0, 1));
      playfield[x][y + 2] = index + 4;
      playfield[x + 3][y + 2] = index + 4;
    }
  }
}

void removeAlien(byte x, byte y)
{
  matrix.drawPixel(x, y, matrix.Color333(0, 0, 0));
  matrix.drawPixel(x + 1, y, matrix.Color333(0, 0, 0));
  matrix.drawPixel(x + 2, y, matrix.Color333(0, 0, 0));
  matrix.drawPixel(x + 3, y, matrix.Color333(0, 0, 0));
  matrix.drawPixel(x, y + 1, matrix.Color333(0, 0, 0));
  matrix.drawPixel(x + 1, y + 1, matrix.Color333(0, 0, 0));
  matrix.drawPixel(x + 2, y + 1, matrix.Color333(0, 0, 0));
  matrix.drawPixel(x + 3, y + 1, matrix.Color333(0, 0, 0));
  matrix.drawPixel(x, y + 2, matrix.Color333(0, 0, 0));
  matrix.drawPixel(x + 1, y + 2, matrix.Color333(0, 0, 0));
  matrix.drawPixel(x + 2, y + 2, matrix.Color333(0, 0, 0));
  matrix.drawPixel(x + 3, y + 2, matrix.Color333(0, 0, 0));
  playfield[x][y] = 0;
  playfield[x + 1][y] = 0;
  playfield[x + 2][y] = 0;
  playfield[x + 3][y] = 0;
  playfield[x][y + 1] = 0;
  playfield[x + 1][y + 1] = 0;
  playfield[x + 2][y + 1] = 0;
  playfield[x + 3][y + 1] = 0;
  playfield[x][y + 2] = 0;
  playfield[x + 1][y + 2] = 0;
  playfield[x + 2][y + 2] = 0;
  playfield[x + 3][y + 2] = 0;
}

// Moves the mothership;
void moveMothership()
{
  // Mothership appears randomly
  if (mothershipStatus == 0 && !bossActive)
  {
    if (mothershipRate > random(1000))
    {
      mothershipStatus = 1;
      mothershipX = - 5;
    }
  }
  if (mothershipStatus > 0)
  {
    // Remove mothership at old position
    matrix.drawPixel(mothershipX + 2, 0, matrix.Color333(0, 0, 0));
    matrix.drawPixel(mothershipX + 3, 0, matrix.Color333(0, 0, 0));
    matrix.drawPixel(mothershipX + 4, 0, matrix.Color333(0, 0, 0));
    matrix.drawPixel(mothershipX + 1, 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(mothershipX + 2, 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(mothershipX + 3, 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(mothershipX + 4, 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(mothershipX + 5, 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(mothershipX, 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(mothershipX + 1, 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(mothershipX + 3, 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(mothershipX + 5, 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(mothershipX + 6, 2, matrix.Color333(0, 0, 0));
    if (mothershipX >= 0 && mothershipX < 32)
    {
      playfield[mothershipX][2] = 0;
    }
    if (mothershipX >= -1 && mothershipX < 31)
    {
      playfield[mothershipX + 1][2] = 0;
      playfield[mothershipX + 1][1] = 0;
    }
    if (mothershipX >= -2 && mothershipX < 30)
    {
      playfield[mothershipX + 2][0] = 0;
      playfield[mothershipX + 2][1] = 0;
    }
    if (mothershipX >= -3 && mothershipX < 29)
    {
      playfield[mothershipX + 3][0] = 0;
      playfield[mothershipX + 3][1] = 0;
      playfield[mothershipX + 3][2] = 0;
    }
    if (mothershipX >= -4 && mothershipX < 28)
    {
      playfield[mothershipX + 4][0] = 0;
      playfield[mothershipX + 4][1] = 0;
    }
    if (mothershipX >= -5 && mothershipX < 27)
    {
      playfield[mothershipX + 5][1] = 0;
      playfield[mothershipX + 5][2] = 0;
    }
    if (mothershipX >= -6 && mothershipX < 26)
    {
      playfield[mothershipX + 6][2] = 0;
    }

    // Move mothership
    mothershipX++;
    if (mothershipX > 31 || mothershipStatus == 2)
    {
      mothershipStatus = 0;
    }

    // Draw mothership at new position
    if (mothershipStatus == 1)
    {
      matrix.drawPixel(mothershipX + 2, 0, matrix.Color333(1, 1, 1));
      matrix.drawPixel(mothershipX + 3, 0, matrix.Color333(1, 1, 1));
      matrix.drawPixel(mothershipX + 4, 0, matrix.Color333(1, 1, 1));
      matrix.drawPixel(mothershipX + 2, 1, matrix.Color333(1, 1, 1));
      matrix.drawPixel(mothershipX + 4, 1, matrix.Color333(1, 1, 1));
      matrix.drawPixel(mothershipX, 2, matrix.Color333(1, 1, 1));
      matrix.drawPixel(mothershipX + 1, 2, matrix.Color333(1, 1, 1));
      matrix.drawPixel(mothershipX + 3, 2, matrix.Color333(1, 1, 1));
      matrix.drawPixel(mothershipX + 5, 2, matrix.Color333(1, 1, 1));
      matrix.drawPixel(mothershipX + 6, 2, matrix.Color333(1, 1, 1));
      if(animationPhase == 1)
      {
        matrix.drawPixel(mothershipX + 1, 1, matrix.Color333(0, 0, 7));
        matrix.drawPixel(mothershipX + 3, 1, matrix.Color333(0, 0, 1));
        matrix.drawPixel(mothershipX + 5, 1, matrix.Color333(0, 0, 7));
      }
      else
      {
        matrix.drawPixel(mothershipX + 1, 1, matrix.Color333(0, 0, 1));
        matrix.drawPixel(mothershipX + 3, 1, matrix.Color333(0, 0, 7));
        matrix.drawPixel(mothershipX + 5, 1, matrix.Color333(0, 0, 1));
      }
      
      if (mothershipX >= 0 && mothershipX < 32)
      {
        playfield[mothershipX][2] = 2;
      }
      if (mothershipX >= -1 && mothershipX < 31)
      {
        playfield[mothershipX + 1][2] = 2;
        playfield[mothershipX + 1][1] = 2;
      }
      if (mothershipX >= -2 && mothershipX < 30)
      {
        playfield[mothershipX + 2][0] = 2;
        playfield[mothershipX + 2][1] = 2;
      }
      if (mothershipX >= -3 && mothershipX < 29)
      {
        playfield[mothershipX + 3][0] = 2;
        playfield[mothershipX + 3][1] = 2;
        playfield[mothershipX + 3][2] = 2;
      }
      if (mothershipX >= -4 && mothershipX < 28)
      {
        playfield[mothershipX + 4][0] = 2;
        playfield[mothershipX + 4][1] = 2;
      }
      if (mothershipX >= -5 && mothershipX < 27)
      {
        playfield[mothershipX + 5][1] = 2;
        playfield[mothershipX + 5][2] = 2;
      }
      if (mothershipX >= -6 && mothershipX < 26)
      {
        playfield[mothershipX + 6][2] = 2;
      }
    }
  }
}

// Move boss
void moveBoss()
{
  // Remove boss at old position
  matrix.fillRect(bossX, bossY, bossWidth, 8, matrix.Color333(0, 0, 0));
  for(byte j = 0; j < 8; j++)
  {
    for(byte i = 0; i < bossWidth; i++)
    {
      playfield[bossX + i][bossY + j] = 0;
    }
  }

  // Move boss
  if(bossType == 1)
  {
    if(bossMovementStatus == RIGHT)
    {
      if(bossX == 32 - bossWidth)
      {
        bossMovementStatus = DOWN;
      }
      else
      {      
        bossX++;
      }
    }
    else if(bossMovementStatus == LEFT)
    {
      if(bossX == 0)
      {
        bossMovementStatus = DOWN;
      }
      else
      {
        bossX--;        
      }
    }
    else if(bossMovementStatus == DOWN)
    {
      if(bossY == 17)
      {
        bossMovementStatus = UP;
      }
      else
      { 
        if(bossX == 0)
        {
          bossY++;     
          if(bossY % 2 == 0){ bossMovementStatus = RIGHT; }
        }
        else
        {
          bossY++;     
          if(bossY % 2 == 0){ bossMovementStatus = LEFT; }         
        }
      }
    }
    else if(bossMovementStatus == UP)
    {
      if(bossY == 3)
      {
        bossMovementStatus = RIGHT;
      }      
      else
      {
        bossY--;
      }
    }
  }
  else if(bossType == 2)
  {
    if(bossMovementStatus == RIGHT)
    {
      if(bossX == 32 - bossWidth)
      {
        bossMovementStatus = random(4);
      }
      else
      {      
        bossX++;
      }
    }
    else if(bossMovementStatus == DOWN)
    {
      if(bossY == 13)
      {
        bossMovementStatus = random(4);
      }
      else
      {      
        bossY++;
      }
    }
    else if(bossMovementStatus == LEFT)
    {
      if(bossX == 0)
      {
        bossMovementStatus = random(4);
      }
      else
      {
        bossX--;        
      }
    }
    else if(bossMovementStatus == UP)
    {
      if(bossY == 3)
      {
        bossMovementStatus = random(4);
      }      
      else
      {
        bossY--;
      }
    }
  }
  else
  {
    if(bossMovementStatus == RIGHT)
    {
      if(bossX == 32 - bossWidth)
      {
        bossMovementStatus = random(4);
      }
      else
      { 
        if(random(20) < 1)
        {
          bossMovementStatus = random(4);
        }
        else
        {     
          bossX++;
        }
      }
    }
    else if(bossMovementStatus == DOWN)
    {
      if(bossY == 17)
      {
        bossMovementStatus = random(4);
      }
      else
      {      
        if(random(20) < 1)
        {
          bossMovementStatus = random(4);
        }
        else
        {     
          bossY++;
        }
      }
    }
    else if(bossMovementStatus == LEFT)
    {
      if(bossX == 0)
      {
        bossMovementStatus = random(4);
      }
      else
      {
        if(random(20) < 1)
        {
          bossMovementStatus = random(4);
        }
        else
        {     
          bossX--;
        }
      }
    }
    else if(bossMovementStatus == UP)
    {
      if(bossY == 3)
      {
        bossMovementStatus = random(4);
      }      
      else
      {
        if(random(20) < 1)
        {
          bossMovementStatus = random(4);
        }
        else
        {     
          bossY--;
        }
      }
    }
  }
  // Drop bombs
  if (bombingRate > random(1000))
  {
    dropBomb(bossX + 3 + random(2), bossY + 6, (random(1 + (2 * level)) - level) / level);
  }    
  
  // Draw boss at new position
  drawBoss(bossType, animationPhase, bossX, bossY, bossWidth, true);

  // Update animation phase
  if (animationPhase == 1) {
    animationPhase = 2;
  }
  else 
  {
    animationPhase = 1;
  }
}

// Draw boss
void drawBoss(byte type, byte phase, int x, int y, byte bossWidth, boolean draw)
{
  byte pixel;
  for (byte j = 0; j < 8; j++)
  {
    for (byte i = 0; i < bossWidth; i++)
    {
      // Draw invader picture depending on type and phase
      if (type == 1) {
        pixel = invader1[(j * bossWidth) + i + (phase - 1) * 64];
      }
      else if (type == 2) {
        pixel = invader2[(j * bossWidth) + i + (phase - 1) * 88];
      }
      else if (type == 3) {
        pixel = invader3[(j * bossWidth) + i + (phase - 1) * 96];
      }

      if(draw)
      {
        if (pixel == 0) {
          matrix.drawPixel(x + i, y + j, matrix.Color333(0, 0, 0));
          playfield[x + i][y + j] = 0;
        }
        else if (pixel == 1) {
          matrix.drawPixel(x + i, y + j, matrix.Color333(1, 1, 1));
          playfield[x + i][y + j] = 3;
        }
        else if (pixel == 2) {
          matrix.drawPixel(x + i, y + j, matrix.Color333(0, 0, 7));
          playfield[x + i][y + j] = 3;
        }
      }
      else
      {
        if (pixel > 0) {
          matrix.drawPixel(x + i, y + j, matrix.Color333(0, 0, 0));
          playfield[x + i][y + j] = 0;
        }
      }
    }
  }
}

// Moves the base (player action)
void moveBase()
{
  // Move to the left
  if (joy1Left() && baseX > 0)
  {
    // Remove base at old postion
    matrix.drawPixel(baseX + 1, 30, matrix.Color333(0, 0, 0));
    matrix.drawPixel(baseX, 31, matrix.Color333(0, 0, 0));
    matrix.drawPixel(baseX + 1, 31, matrix.Color333(0, 0, 0));
    matrix.drawPixel(baseX + 2, 31, matrix.Color333(0, 0, 0));
    baseX--;
  }
  // Move to the right
  else if (joy1Right() && baseX < 29)
  {
    // Remove base at old postion
    matrix.drawPixel(baseX + 1, 30, matrix.Color333(0, 0, 0));
    matrix.drawPixel(baseX, 31, matrix.Color333(0, 0, 0));
    matrix.drawPixel(baseX + 1, 31, matrix.Color333(0, 0, 0));
    matrix.drawPixel(baseX + 2, 31, matrix.Color333(0, 0, 0));
    baseX++;
  }

  // Draw base at new position
  matrix.drawPixel(baseX + 1, 30, matrix.Color333(1, 1, 1));
  matrix.drawPixel(baseX, 31, matrix.Color333(1, 1, 1));
  matrix.drawPixel(baseX + 1, 31, matrix.Color333(3, 3, 3));
  matrix.drawPixel(baseX + 2, 31, matrix.Color333(1, 1, 1));
}

// Fires a new shot (player action)
void fire()
{
  if (joy1Fire() && playerShotStatus == 0)
  {
    playerShotStatus = 1;
    playerShotX = baseX + 1;
    playerShotY = 29;
    matrix.drawPixel(playerShotX, playerShotY, matrix.Color333(7, 7, 7));
    currentWaitTimerPlayerShot = waitTimerPlayerShot;

    // SOUND: Fire
    tone(audio, 300, 20);
  }
}

// Moves a fired shot and checks whether it hits something
void moveShot()
{
  // Remove shot at old position
  matrix.drawPixel(playerShotX, playerShotY, matrix.Color333(0, 0, 0));

  // Move shot
  playerShotY--;
  if (playerShotY == 255)
  {
    playerShotStatus = 0;
  }

  if (playerShotStatus == 1)
  {
    // Check for hit (checkForHit method procedure manages everything else)
    if (checkForHit())
    {
      // Inactivates shot after hit (not drawn again)
      playerShotStatus = 0;
    }
    else
    {
      // Draw shot at new position;
      matrix.drawPixel(playerShotX, playerShotY, matrix.Color333(7, 7, 7));
    }
  }
}

// Returns true if all aliens in column i of the formation have been destroyed
// (required to update left and right movement limits)
boolean columnEmpty(byte i)
{
  for (byte j = 0; j < formationHeight; j++)
  {
    if (alienStatus[(j * formationWidth) + i] != 0)
    {
      return false;
    }
  }
  return true;
}

// Returns lowest row which is not empty
byte getLowestRow()
{
  for (int j = formationHeight - 1; j > -1; j--)
  {
    for (int i = 0; i < formationWidth; i++)
    {
      if (alienStatus[(j * formationWidth) + i] != 0)
      {
        return j;
      }
    }
  }
}

// Returns true if there are aliens below alien k (so that k cannot drop a bomb)
boolean alienBelow(byte k)
{
  byte index = k + formationWidth;
  while (index < formationWidth * formationHeight)
  {
    if (alienStatus[index] != 0)
    {
      return true;
    }
    index = index + formationWidth;
  };
  return false;
}

// Creates a new explosion at position (_x, _y)
void explosion(byte _x, byte _y)
{
  // Check if a new explosion is possible (according to resources)
  if (explosionStatus[explosionPointer] == 0)
  {
    // Define new explosion
    explosionStatus[explosionPointer] = 1;
    explosionX[explosionPointer] = _x;
    explosionY[explosionPointer] = _y;
    explosionPointer++;

    // Update explosion pointer
    if (explosionPointer == 16)
    {
      explosionPointer = 0;
    }
  }
}

// Animates all existing explosion and
void animateExplosions()
{
  // Iterate over all possible explosion
  for (byte i = 0; i < 16; i++)
  {
    // Animate explosion if in an animation phase (explosionStatus[i] between 1 and 5)
    if (explosionStatus[i] > 0)
    {
      if (explosionStatus[i] == 1)
      {
        matrix.drawPixel(explosionX[i] + 1, explosionY[i] + 1, matrix.Color333(7, 7, 3));
        matrix.drawPixel(explosionX[i] + 2, explosionY[i] + 1, matrix.Color333(7, 7, 3));
        // SOUND: Explosion
        tone(audio, 800, 40);
      }
      else if (explosionStatus[i] == 2)
      {
        matrix.drawPixel(explosionX[i] + 1, explosionY[i], matrix.Color333(3, 3, 0));
        matrix.drawPixel(explosionX[i] + 2, explosionY[i], matrix.Color333(3, 3, 0));
        matrix.drawPixel(explosionX[i], explosionY[i] + 1, matrix.Color333(3, 3, 0));
        matrix.drawPixel(explosionX[i] + 1, explosionY[i] + 1, matrix.Color333(3, 1, 0));
        matrix.drawPixel(explosionX[i] + 2, explosionY[i] + 1, matrix.Color333(3, 1, 0));
        matrix.drawPixel(explosionX[i] + 3, explosionY[i] + 1, matrix.Color333(3, 3, 0));
        matrix.drawPixel(explosionX[i] + 1, explosionY[i] + 2, matrix.Color333(3, 3, 0));
        matrix.drawPixel(explosionX[i] + 2, explosionY[i] + 2, matrix.Color333(3, 3, 0));
        // SOUND: Explosion
        tone(audio, 400, 40);
      }
      else if (explosionStatus[i] == 3)
      {
        matrix.drawPixel(explosionX[i] + 1, explosionY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(explosionX[i] + 2, explosionY[i] + 1, matrix.Color333(0, 0, 0));
        // SOUND: Explosion
        tone(audio, 200, 40);
      }
      else if (explosionStatus[i] == 4)
      {
        matrix.fillRect(explosionX[i], explosionY[i], 4, 3, matrix.Color333(0, 0, 0));
        matrix.drawPixel(explosionX[i], explosionY[i], matrix.Color333(1, 1, 0));
        matrix.drawPixel(explosionX[i] + 3, explosionY[i], matrix.Color333(1, 1, 0));
        matrix.drawPixel(explosionX[i], explosionY[i] + 2, matrix.Color333(1, 1, 0));
        matrix.drawPixel(explosionX[i] + 3, explosionY[i] + 2, matrix.Color333(1, 1, 0));
        // SOUND: Explosion
        tone(audio, 100, 40);
      }
      else if (explosionStatus[i] == 5)
      {
        matrix.fillRect(explosionX[i], explosionY[i], 4, 3, matrix.Color333(0, 0, 0));
        // SOUND: Explosion
        tone(audio, 50, 80);
      }
      explosionStatus[i]++;

      // Explosion ends
      if (explosionStatus[i] == 6) {
        explosionStatus[i] = 0;
      }
    }
  }
}

// Returns true if an alien from the formation has been hit
// If so, a destroyed alien is removed and left and right column of the formation are updated
boolean checkForHit()
{
  // Check if there is a hit (bombs cannot be hit)
  if (playfield[playerShotX][playerShotY] > 0 && playfield[playerShotX][playerShotY] != 255)
  {
    // SOUND: Hit
    tone(audio, 200, 40);
    
    // Shield hit
    if (playfield[playerShotX][playerShotY] == 1)
    {
      playfield[playerShotX][playerShotY] = 0;
      matrix.drawPixel(playerShotX, playerShotY, matrix.Color333(0, 0, 0));
    }
    // Mothership hit
    else if (playfield[playerShotX][playerShotY] == 2)
    {
      score = score + 200;
      extraLifeScore = extraLifeScore + 200;
      mothershipStatus = 2;

      // Mothership explosion;
      explosion(mothershipX - 1, 0);
      explosion(mothershipX + 1, 0);
      explosion(mothershipX + 2, 0);
      explosion(mothershipX + 4, 0);
    }
    // Boss hit
    else if (playfield[playerShotX][playerShotY] == 3)
    {
      bossShield--;
      if(bossShield == 0)
      {
        score = score + 500;
        extraLifeScore = extraLifeScore + 500;
        currentWaitTimerBoss = 1000;

        // Remove boss
        matrix.fillRect(bossX, bossY, bossWidth, 8, matrix.Color333(0, 0, 0));
        for(byte j = 0; j < 8; j++)
        {
          for(byte i = 0; i < bossWidth; i++)
          {
            playfield[bossX + i][bossY + j] = 0;
          }
        }
        
        // Boss explosion
        for(byte i = 0; i < 16; i++)
        {
          explosion(bossX + random(bossWidth - 3), bossY + random(6));
        }
      }
    }
    // Alien hit
    else
    {
      // Get the hit alien and its position in formation
      byte alienIndex = playfield[playerShotX][playerShotY] - 4;
      alienStatus[alienIndex] = 0;
      byte i = alienIndex % formationWidth;
      byte j = alienIndex / formationWidth;

      // Remove the alien from screen
      removeAlien((i * 5) + formationX, (j * 4) + formationY);
      waitTimerFormation = waitTimerFormation - (50 - (level - 1) * 2); // Formation speeds up with every loss
      if (waitTimerFormation < 0) {
        waitTimerFormation = 0;
      }
      remainingAliens--;

      // Update score
      switch (alienType[alienIndex])
      {
        case 1:
          score = score + 10;
          extraLifeScore = extraLifeScore + 10;
          break;
        case 2:
          score = score + 20;
          extraLifeScore = extraLifeScore + 20;
          break;
        case 3:
          score = score + 30;
          extraLifeScore = extraLifeScore + 30;
          break;
        case 4:
          score = score + 40;
          extraLifeScore = extraLifeScore + 40;
          break;
        case 5:
          score = score + 50;
          extraLifeScore = extraLifeScore + 50;
          break;
        default:
          break;
      }

      // Create explosion
      explosion((i * 5) + formationX, (j * 4) + formationY);

      // Update left and right column of formation in case the most left or most right column has been destroyed completely
      byte k;
      k = i;
      while (columnEmpty(k))
      {
        if (k == columnOffsetLeft)
        {
          columnOffsetLeft++;
          k++;
        }
        else
        {
          break;
        }
      };
      k = i;
      while (columnEmpty(k))
      {
        if (k == formationWidth - 1 - columnOffsetRight)
        {
          columnOffsetRight++;
          k--;
        }
        else
        {
          break;
        }
      };
    }
    return true;
  }
  else
  {
    return false;
  }
}

// Show the Game Over screen
void gameOver()
{
  // Game over
  matrix.fillRect(0, 0, 32, 25, matrix.Color333(0, 0, 0));
  for (int i = 0; i < 50; i++)
  {
    matrix.setCursor(4, 4);
    matrix.setTextColor(matrix.Color333(7 * (i % 2), 7 * (i % 2), 0));
    matrix.println("Game");
    matrix.setCursor(4, 12);
    matrix.println("Over");

    // SOUND: Game Over
    tone(audio, i * 10, 5);
    delay(10);
  }
  matrix.setCursor(4, 4);
  matrix.setTextColor(matrix.Color333(3, 3, 0));
  matrix.println("Game");
  matrix.setCursor(4, 12);
  matrix.println("Over");
  delay(3000);

  // Check for new highscore
  if (score > highscore)
  {
    highscore = score;
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
    for (int i = 0; i < 50; i++)
    {
      matrix.setCursor(7, 4);
      matrix.setTextColor(matrix.Color333(7 * (i % 2), 7 * (i % 2), 0));
      matrix.println("New");
      matrix.setCursor(1, 12);
      matrix.println("HiScr");

      // SOUND: New High Score
      tone(audio, 500 + i * 40, 5);
      delay(10);
    }
    matrix.setCursor(7, 4);
    matrix.setTextColor(matrix.Color333(3, 3, 0));
    matrix.println("New");
    matrix.setCursor(1, 12);
    matrix.println("HiScr");
    delay(3000);
  }

  // Show final score
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
  matrix.setCursor(1, 4);
  matrix.setTextColor(matrix.Color333(3, 3, 0));
  matrix.println("Score");
  matrix.setCursor(1, 12);
  matrix.println(score);
  delay(2000);
}

// Show the number of remaining bases at the bottom of the screen
void showLives(byte _lives)
{
  matrix.fillRect(0, 30, 32, 2, matrix.Color333(0, 0, 0));
  for (byte i = 0; i < _lives; i++)
  {
    if (i < 4)
    {
      matrix.drawPixel(4 + (i * 7) + 1, 30, matrix.Color333(1, 1, 0));
      matrix.drawPixel(4 + (i * 7), 31, matrix.Color333(1, 1, 0));
      matrix.drawPixel(4 + (i * 7) + 1, 31, matrix.Color333(3, 3, 0));
      matrix.drawPixel(4 + (i * 7) + 2, 31, matrix.Color333(1, 1, 0));
    }
  }
}

void loop() {

  // Draw title screen
  titleScreen();

  // Setup a new game
  setupGame();

  // Loop over levels
  do
  {
    // Initialize all variables for a new level
    setupLevel();

    // Show score
    matrix.setCursor(1, 4);
    matrix.setTextColor(matrix.Color333(3, 3, 0));
    matrix.println("Score");
    matrix.setCursor(1, 12);
    matrix.println(score);
    delay(2000);

    // Clear screen from score
    matrix.fillRect(0, 0, 32, 24, matrix.Color333(0, 0, 0));

    // Show level
    matrix.setCursor(4, 4);
    matrix.setTextColor(matrix.Color333(3, 3, 0));
    matrix.println("Wave");
    matrix.setCursor(13, 12);
    matrix.println(level);

    // Show remaining bases
    showLives(lives);
    delay(2000);

    // Clear screen from level and base visualization
    matrix.fillRect(0, 30, 32, 2, matrix.Color333(0, 0, 0));
    matrix.fillRect(0, 4, 32, 16, matrix.Color333(0, 0, 0));

    // Set timers
    currentWaitTimerBombs = waitTimerBombs;
    currentWaitTimerExplosions = waitTimerExplosions;
    currentWaitTimerFormation = waitTimerFormation;
    currentWaitTimerMothership = waitTimerMothership;
    currentWaitTimerBoss = waitTimerBoss;
    currentWaitTimerPlayerShot = waitTimerPlayerShot;
    currentWaitTimerPlayer = waitTimerPlayer;

    // Main game loop
    do
    {
      // Move falling bombs immediately after timer has elapsed
      currentWaitTimerBombs--;
      if (currentWaitTimerBombs == 0)
      {
        moveBombs();
        currentWaitTimerBombs = waitTimerBombs;
      }

      // Animate explosions immediately after timer has elapsed
      currentWaitTimerExplosions--;
      if (currentWaitTimerExplosions == 0)
      {
        animateExplosions();
        currentWaitTimerExplosions = waitTimerExplosions;
      }

      // Move formation immediately after timer has elapsed
      currentWaitTimerFormation--;
      if (currentWaitTimerFormation == 0)
      {
        // SOUND: Move formation (only if formation is moving)
        if (!baseDestroyed && !bossActive)
        {
          tone(audio, 50 + (animationPhase * 50), 40);
        }

        moveFormation();
        currentWaitTimerFormation = waitTimerFormation;
      }

      // Move mothership immediately after timer has elapsed
      currentWaitTimerMothership--;
      if (currentWaitTimerMothership == 0)
      {
        // SOUND: Move mothership (only if formation is moving)
        if (mothershipStatus != 0 && !baseDestroyed && !bossActive)
        {
          tone(audio, 200 + (animationPhase * 200), 40);
        }
          
        moveMothership();
        currentWaitTimerMothership = waitTimerMothership;
      }

      // Move boss immediately after timer has elapsed
      if(bossActive)
      {
        currentWaitTimerBoss--;
        if (currentWaitTimerBoss == 0)
        {
          // SOUND: Move formation (only if formation is moving)
          if (!baseDestroyed)
          {
            tone(audio, 100 + (animationPhase * 100), 40);
          }
          if(bossShield > 0)
          {
            moveBoss();
          }
          else
          {
            bossActive = false;            
            remainingAliens--;
          }
          currentWaitTimerBoss = waitTimerBoss;
        }        
      }

      // Move shots by player immediately after timer has elapsed
      currentWaitTimerPlayerShot--;
      if (currentWaitTimerPlayerShot == 0)
      {
        if (playerShotStatus == 1)
        {
          moveShot();
        }
        currentWaitTimerPlayerShot = waitTimerPlayerShot;
      }

      // Allow player actions after timer has elapsed
      if (currentWaitTimerPlayer > 0)
      {
        currentWaitTimerPlayer--;
      }
      if(currentWaitTimerPlayer == 0)
      {
        if (!baseDestroyed)
        {
          moveBase();
          fire();
        }
        else
        {
          // Procedure after a hit
          waitCounterBaseDestroyed--;
          if (waitCounterBaseDestroyed == 40)
          {
            // Show remaining lives
            showLives(lives - 1);
          }
          else if (waitCounterBaseDestroyed == 20)
          {
            // Remove display of remaining lives
            matrix.fillRect(0, 30, 32, 2, matrix.Color333(0, 0, 0));
          }
          else if (waitCounterBaseDestroyed < 1)
          {
            // Decrease number of lives and go back to normal mode
            lives--;
            baseDestroyed = false;
            baseX = 14;
          }
        }
        currentWaitTimerPlayer = waitTimerPlayer;

        // Show extra life
        if (extraLifeCoughtTimer > 0)
        {
          if (extraLifeCoughtTimer > 1)
          {
            matrix.drawPixel(15, 1, matrix.Color333(1, 1, 0));
            matrix.drawPixel(14, 2, matrix.Color333(1, 1, 0));
            matrix.drawPixel(15, 2, matrix.Color333(3, 3, 0));
            matrix.drawPixel(16, 2, matrix.Color333(1, 1, 0));

            // SOUND: Extra life
            tone(audio, 100 + (extraLifeCoughtTimer * 10), 20);
          }
          else
          {
            matrix.drawPixel(15, 1, matrix.Color333(0, 0, 0));
            matrix.drawPixel(14, 2, matrix.Color333(0, 0, 0));
            matrix.drawPixel(15, 2, matrix.Color333(0, 0, 0));
            matrix.drawPixel(16, 2, matrix.Color333(0, 0, 0));
          }
          extraLifeCoughtTimer--;
        }
      }

      // Check whether base has been hit
      if (playfield[baseX + 1][30] != 0 || playfield[baseX][31] || playfield[baseX + 1][31] || playfield[baseX + 2][31])
      {
        if (!baseDestroyed)
        {
          baseDestroyed = true;
          waitCounterBaseDestroyed = 60;

          // Base explodes
          explosion(baseX - 1, 29);
          explosion(baseX + 1, 29);
        }
      }

      // Check whether formation is so deep, that player cannot win (level is restarted in this case)
      if ((formationY + (getLowestRow() * 4)) > 27) {
        formationLanded = true;
      }

      // Check for extra life (every 2000 points)
      if (extraLifeScore > 2000)
      {
        extraLifeScore = extraLifeScore - 2000;
        lives++;
        extraLifeCoughtTimer = 20;
      }

      // Activate boss
      if ((level % 2 == 0) && remainingAliens == 1 && !bossActive)
      {
        bossActive = true;
        currentWaitTimerBoss = 500;

        // Boss type depends on level
        if ((level / 2) % 3 == 1){ bossType = 1; bossWidth = 8; }
        if ((level / 2) % 3 == 2){ bossType = 2; bossWidth = 11; }
        if ((level / 2) % 3 == 0){ bossType = 3; bossWidth = 12; }

        bossX = 16 - (bossWidth / 2);
        bossY = 3;
        bossMovementStatus = RIGHT;
      }

      // Pause
      if (digitalRead(buttonPause) == LOW)
      {
        tone(audio, 1024, 20);
        delay(500);
        do
        {
        }
        while ((!digitalRead(buttonPause) == LOW) && (!joy1Fire()));
        tone(audio, 1024, 20);
        delay(500);
      }

      // Reset
      if (digitalRead(buttonReset) == LOW)
      {
        reset = true;
        break;
      }

      delay(1);
    }
    while (lives > 0 && remainingAliens > 0 && !formationLanded && !reset);

    // Increase Level
    if (remainingAliens == 0)
    {
      level++;
    }
  }
  while (lives > 0 && !reset);

  if (!reset)
  {
    // Game Over
    gameOver();
  }
}

