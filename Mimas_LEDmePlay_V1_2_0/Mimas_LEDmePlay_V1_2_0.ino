// Mimas
// for the LEDmePlay
//
// www.mithotronic.de
//
// Version: 1.2.0
// Author: Michael Rosskopf (2014)
// 
// Thanks to Thomas Laubach!
// Thanks to Chris Huelsbeck who originally has composed the game melody.
// We spent hundreds of hours with Turrican and its great music!
//
// Release Notes:
// V1.2.0: Scrolling stars in level 1, different colors of bosses, fixes of a few glitches (2019)
// V1.1.0: Support for LEDmePlay Joypad and LEDmePlayBoy (2018)
// V1.0.4: Display shift problem with Arduino IDE 1.0.x fixed
// V1.0.3: Balancing of game improved
// V1.0.2: Random strange effects (caused by a wrong memory access) fixed
// V1.0.1: First release

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

// Enemy ships and bases
byte numberOfEnemies = 10;        // Number of enemies on the screen
byte enemyStatus[10];             // 0 = Enemy not active, 1 = Enemy active, 2 = Destroyed, 3-14 = Different explosion stages
double enemyX[10];                // x-position on the screen
double enemyY[10];                // y-position on the screen
byte enemyWidth[10];              // Width of the enemy
byte enemyHeight[10];             // Height of the enemy 
double enemyVectorX[10];          // Movement of enemy in x-direction
double enemyVectorY[10];          // Movement of enemy in y-direction
byte enemyType[10];               // Type of the enemy: Not every type makes sense in every level and in every situation. A level ends if the boss is destroyed.
                                  // 1 == Meteoroid
                                  // 2 == Meteroid
                                  // 3 == Fighter
                                  // 4 == Tower
                                  // 5 == Killer-Satellite
                                  // 6 == Cruise Missile
                                  // 7 == Submarine
                                  // 8 == End Boss (Level 1-5)
                                  // 9 == Hanging tower
                                  // 10 == Bouncing drones
                                  // 11 == Jumper
                                  // 12 == Heavy fighter
                                  // 13 == Bomber
                                  // 14 == Barricade
                                  // 15 == Core (Final end boss)
int enemyHitPoints[10];           // Hit points of enemy: If 0, enemy is destroyed
byte enemyFireRate[10];           // Value/1000 == Probability that enemy fires a every round
int enemyScore[10];               // Points for player if enemy is destroyed
byte enemyCounter;                // Index of the next approaching enemy in the different arrays containing the enemy values (enemyX[], enemyY[] etc.)

// Enemy shots
byte numberOfEnemyShots = 10; // Number of enemy shots on the screen
boolean enemyShotStatus[10];        // 0 == Shot not active, 1 == Shot fired
double enemyShotX[10];              // x-position of shot
double enemyShotY[10];              // y-position of shot
double enemyShotVectorX[10];        // Movement of shot in x-direction
double enemyShotVectorY[10];        // Movement of shot in y-direction
byte enemyShotCounter;              // Index of the next fired shot in the different arrays containing the shot values (enemyShotX[], enemyShotY[] etc.)

// Ship and shots
byte shipX;                    // x-position of player´s ship
byte shipY;                    // y-position of player´s ship
byte numberOfShots = 64;       // Number of fired shots on the screen
double shotX[64];              // x-position of player´s shot
double shotY[64];              // Y-position of player´s shot
double shotVectorX[64];        // Movement of player´s shot in x-direction
double shotVectorY[64];        // Movement of player´s shot in y-direction
byte shotFired[64];            // 0 == Not fired, 1 == Fired, 2 == Fired shot is power shot
byte shotPointer;              // Index of the next fired shot in the different arrays containing the player´s shot values (shotX[], shotY[] etc.)
byte shotLevel;                // Stage of extension of main weapon
byte weaponStatus;             // 0 == No shot fired, 1 == Shot fired, button pressed to charge power shot, 2 == Button pressed, power shot ready (fired if button released)
byte shotAngle;                // Used to switch between different angles
int powerShotChargeCounter;    // Counts the cycles till power shot level reached
int shieldEnergy;              // If > 0 shield (power-up) is active (collision with enemies and shots do not hurt)

// Power-ups
boolean powerUpActive; // 0 == No power up, 1 == Power up active (on screen)
double powerUpX;       // x-position of power-up
double powerUpY;       // y-position of power-up
byte powerUpType;      // 0 == Shot extension (violett), 1 == Shield (blue), 2 == Smart bomb (green), 3 == Extra life (red)

// Playfield (42 * 42 to have a 5 pixel border around the visible playfield)
byte playfield[42][42];

// Level settings
byte level;               // Current level
byte maxLevelReached;     // Highest level which has been reached (for level selection)
byte phase;               // Current phase (every level is subdivided into different phases)
int scrollCounter;        // Position in current phase
byte lives;               // Number of remaining lives
byte difficulty;          // Difficulty of current game
long score;               // Current score
long extraLifeScore;      // Score required for next extra life
long highScore;           // Current high score
boolean endBossActive;    // true if end boss active on screen
boolean endBossDestroyed; // true if end boss destroyed -> Jump to the next level

// Variables used for scrolling
float starX[24];         // x position of background star (Level 1)
float starY[24];         // y position of background star (Level 1)
float starSpeed[24];     // Speed of background star (Level 1)
int ceiling[32];         // y-positions of the ceiling of the visible level part
int ground[32];          // y-positions of the ground of the visible level part
byte scrollPointer = 31; // Index position in ceiling and ground array for the next screen element scrolling into the screen
int newCeiling = 0;      // Helper for y-positions of next ceiling element
int newGround= 0;        // Helper for y-positions of next ground element

// Event variables
boolean reset;       // if true, return to title screen
byte eventCounter;   // Counts cyclically from 0-7 since in the main loop to trigger cyclic events
byte collisionValue; // 0 == No collision, 1 == Collision with power-up (good), 2 == Collision with floor or ceiling (bad -> player´s ship explodes), 3 to (numberOfEnemies + 2), 127 == Enemy shots

// Soundtrack
int soundtrack[] = {523, 0, 523, 0, 784, 0, 784, 0, 880, 784, 698, 0, 784, 0, 0, 0, 932, 0, 932, 0, 880, 784, 698, 0, 698, 0, 784, 0, 622, 0, 587, 0, 523, 0, 523, 0, 784, 0, 784, 0, 880, 784, 698, 0, 784, 0, 0, 0, 932, 0, 932, 0, 880, 784, 698, 0, 698, 0, 784, 0, 622, 0, 587, 0, 784, 0, 659, 0, 523, 0, 659, 784, 880, 0, 698, 0, 523, 0, 698, 880, 932, 0, 784, 0, 622, 0, 784, 932, 1047, 0, 880, 0, 698, 0, 880, 1047, 784, 0, 659, 0, 523, 0, 659, 784, 880, 0, 698, 0, 523, 0, 698, 880, 932, 0, 784, 0, 622, 0, 784, 932, 1047, 0, 880, 0, 698, 0, 880, 1047};
int bossMelody[] = {196, 220, 233, 262}; // Special theme if boss approaches
int soundCounter; // Index position for the next tone to be played

// Game synchronization
unsigned long engineLoopStartPoint; // Contains millisconds since start of LEDmePlay at begin of each engine loop

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

  // Setup ship
  shipX = 4;
  shipY = 14;
  
  // Initialize high score
  highScore = 12345;
  
  // Set highest level reached to 1
  maxLevelReached = 1;

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

  byte i = 0;
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

// Draw title screen
void title()
{
  // Clear screen
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // Write 'MIMAS'
  matrix.setCursor(1, 2);
  matrix.setTextColor(matrix.Color333(0,0,3));
  matrix.println("MIMAS");

  // Line blow text
  matrix.drawLine(1, 10, 30, 10, matrix.Color333(3,3,0));

  // Flip between title screen and high score
  int i = 0;
  do
  {
    if(i == 0)
    {
      // Draw random stars
      matrix.fillRect(0, 11, 32, 21, matrix.Color333(0,0,0));
      for(i = 0; i < 24; i++)
      {
        matrix.drawPixel(random(32), random(20) + 12, matrix.Color333(1, 1, 3));
      }

      // Draw moon Mimas
      matrix.fillCircle(16, 21, 8, matrix.Color333(0,0,0)); 
      matrix.drawCircle(16, 21, 8, matrix.Color333(0,0,3)); 
      matrix.drawCircle(15, 18, 3, matrix.Color333(0,0,3));
    }
    if(i == 4000)
    {
      // Show highscore
      matrix.fillRect(0, 11, 32, 21, matrix.Color333(0,0,0));
      matrix.setTextColor(matrix.Color333(3,3,0));
      matrix.setCursor(1, 14);
      matrix.println("HiScr");
      matrix.setTextColor(matrix.Color333(0,0,3));
      matrix.setCursor(1, 22);
      matrix.println(highScore);    
    }
    random(100); // Draw random numbers for random gameplay
    i++;
    delay(2);
    if(i > 8000)
    {
      i = 0;
    }
  }
  while(!joy1Fire()); 
  tone(audio,1024,20);
}

// Draw dialog to select the difficulty
void selectDifficulty()
{
  // Clear screen
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // Write text
  matrix.setTextColor(matrix.Color333(0,0,3));
  matrix.setCursor(1, 0);
  matrix.println("Diff:");
  delay(200);
  matrix.setCursor(7, 8);
  matrix.println("Easy");
  matrix.setCursor(7, 16);
  matrix.println("Norm");
  matrix.setCursor(7, 24);
  matrix.println("Hard");

  // Select difficulty by moving joystick up and down
  difficulty = 2;
  matrix.setTextColor(matrix.Color333(3,3,0));
  do
  {
    matrix.setCursor(1, 8 * difficulty);
    matrix.println(">");
    if(joy1Up())
    {
      difficulty--;
      if(difficulty < 1)
      {
        difficulty = 1;
      }
      else
      {
        tone(audio,1024,20);
      }
      delay(200);
      matrix.fillRect(2, 8, 4, 23, matrix.Color333(0, 0, 0));
    }
    if(joy1Down())
    {
      difficulty++;
      if(difficulty > 3)
      {
        difficulty = 3;
      }
      else
      {
        tone(audio,1024,20);
      }
      delay(200);
      matrix.fillRect(2, 8, 4, 23, matrix.Color333(0, 0, 0));
    }
  }
  while(!joy1Fire());     
  tone(audio,1024,20);
}

void selectLevel()
{
  // Clear screen
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // Write text
  matrix.setTextColor(matrix.Color333(0,0,3));
  matrix.setCursor(7, 0);
  matrix.println("Set");
  matrix.setCursor(7, 8);
  matrix.println("Lvl");
  matrix.setTextColor(matrix.Color333(3,3,0));
  matrix.setCursor(7, 16);
  matrix.println("< >");
  delay(200);

  // Select level by moving joystick left and right
  level = 1;
  matrix.setTextColor(matrix.Color333(1,1,3));
  do
  {
    matrix.setCursor(1, 24);
    switch(level)
    {
    case 1:
      matrix.println("Space");
      break;
    case 2:
      matrix.println("Ocean");
      break;
    case 3:
      matrix.println("Hills");
      break;
    case 4:
      matrix.println("Rocks");
      break;
    case 5:
      matrix.println("Caves");
      break;
    case 6:
      matrix.println("Core");
      break;
    }

    if(joy1Left())
    {
      level--;
      if(level < 1)
      {
        level = 1;
      }
      else
      {
        tone(audio,1024,20);
        matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
      }
      delay(200);
    }
    
    if(joy1Right())
    {
      level++;
      if(level > maxLevelReached)
      {
        level = maxLevelReached;
      }
      else
      {
        tone(audio,1024,20);
        matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 0, 0));
      }
      delay(200);
    }
  }
  while(!joy1Fire());     
  tone(audio,1024,20);  
}

// Setup a new game by initializing different counters
void setupGame()
{
  level = 1;
  
  // Enable level selection if level > 1 has been reached before
  if(maxLevelReached > 1)
  {
    selectLevel();
  }
  
  shotLevel = 1;
  lives = 3;
  score = 0;
  extraLifeScore = 0;
}

// Setup a level by initializing different counters
void setupLevel()
{ 
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // Initialize playfield
  for(byte i = 0; i < 42; i++)
  {
    for(byte j = 0; j < 42; j++)
    {
      playfield[i][j] = 0;
    }
  }

  // Initialize ground and ceiling

  // Level 1: Space without ceiling or ground but with background stars
  if(level == 1)
  {
    for(byte i = 0; i < 32; i++){
      ceiling[i] = -2;
      ground[i] = 33;
    }
    for(byte i = 0; i < 24; i++){    
      starX[i] = random(128);
      starY[i] = random(32);
      starSpeed[i] = (random(4) + 1) / 4.0;
    }
  }

  // Level 2: Ocean (Ground is the water surface)
  if(level == 2)
  {
    for(byte i = 0; i < 32; i++){
      ceiling[i] = -2;
      ground[i] = 28;
    }
    matrix.fillRect(0, 28, 32, 4, matrix.Color333(0, 0, 3));
  }

  // Level 3: Hills (Ground is the surface)
  if(level == 3)
  {
    for(byte i = 0; i < 32; i++){
      ceiling[i] = -2;
      ground[i] = 24;
    }
    matrix.fillRect(0, 24, 32, 8, matrix.Color333(0, 1, 0));
  }

  // Level 4: Rocks (Ground is the surface)
  if(level == 4)
  {
    for(byte i = 0; i < 32; i++){
      ceiling[i] = -2;
      ground[i] = 24;
    }
    matrix.fillRect(0, 24, 32, 8, matrix.Color333(1, 1, 0));
  }

  // Level 5: Caves (Ceiling and ground)
  if(level == 5)
  {
    for(byte i = 0; i < 32; i++){
      ceiling[i] = 3;
      ground[i] = 28;
    }
    matrix.fillRect(0, 0, 32, 4, matrix.Color333(0, 0, 1));
    matrix.fillRect(0, 28, 32, 4, matrix.Color333(0, 0, 1));
  }

  // Level 6: Caves (Ceiling and ground but less space than in level 5)
  if(level == 6)
  {
    for(byte i = 0; i < 32; i++){
      ceiling[i] = 7;
      ground[i] = 24;
    }
    matrix.fillRect(0, 0, 32, 8, matrix.Color333(1, 0, 0));
    matrix.fillRect(0, 24, 32, 8, matrix.Color333(1, 0, 0));
  }

  // Set all enemy counter to 0
  enemyCounter = 0;
  for(byte i = 0; i < numberOfEnemies; i++)
  {
    enemyStatus[i] = 0;
  }

  // Set all enemy shot counter to 0
  enemyShotCounter = 0;
  for(byte i = 0; i < numberOfEnemyShots; i++)
  {
    enemyShotStatus[i] = 0;
  }

  // Prepare end boss for fight
  endBossActive = false;
  endBossDestroyed = false;

  // Set player´s shots to 0
  shotPointer = 0;
  for(byte i = 0; i < numberOfShots; i++)
  {
    shotFired[i] = 0;
  }

  // Update highest level which has been reached (for level selection)
  if(level > maxLevelReached)
  {
    maxLevelReached = level;
  }

  // Set additional variables to initial value
  scrollCounter = 0;
  soundCounter = 0;
  shipX = 0;
  shipY = (ceiling[(scrollPointer + 4) % 32] + ground[(scrollPointer + 4) % 32]) / 2;
  eventCounter = 0;
  shotPointer = 0;
  weaponStatus = 0;
  shotAngle = 0;
  powerShotChargeCounter = 0;
  powerUpActive = 0;
  shieldEnergy = 0;
}

// Returns the type of the next approaching enemy (depending on the current level and phase)
// In the most cases 0 is returned which means that no new enemy approaches.
byte getRandomEnemyType()
{
  switch(level)
  {
  // Level 1
  case 1:
    {
      switch(phase)
      {
      // Meteoroids
      case 1:
        if((random(1000) < 80))
        {
          return 1;
        }
        return 0;
      // Cruise Missiles and Bombers
      case 2:
        if((random(1000) < 40))
        {
          return 6;
        }
        if((random(1000) < 10))
        {
          return 13;
        }
        return 0;
      // Killer Satelites
      case 3:
        if((random(1000) < 40))
        {
          return 5;
        }
        return 0;
      // Meteoroids, Cruise Missiles, and Killer Satelites
      case 4:
        if((random(1000) < 20))
        {
          return 1;
        }
        if((random(1000) < 20))
        {
          return 6;
        }
        if((random(1000) < 20))
        {
          return 5;
        }
        return 0;
      case 5:
        return 0;
      case 6:
        return 0;
      case 7:
        return 8;
      default:
        return 0;
      }
    }
  // Level 2
  case 2:
    {
      switch(phase)
      {
      case 1:
        // Submarines and Bombers
        if((random(1000) < 40))
        {
          return 7;
        }
        if((random(1000) < 10))
        {
          return 13;
        }
        return 0;
      // Meteroids
      case 2:
        if((random(1000) < 100))
        {
          return 2;
        }
        return 0;
      // Cruise Missiles, Submarines, and Bombers
      case 3:
        if((random(1000) < 20))
        {
          return 6;
        }
        if((random(1000) < 20))
        {
          return 7;
        }
        if((random(1000) < 10))
        {
          return 13;
        }
        return 0;
      // Meteors, Cruise Missiles, Submarines,  
      case 4:
        if((random(1000) < 20))
        {
          return 7;
        }
        if((random(1000) < 20))
        {
          return 6;
        }
        if((random(1000) < 40))
        {
          return 2;
        }
        return 0;
      case 5:
        return 0;
      case 6:
        return 0;
      case 7:
        return 8;
      default:
        return 0;
      }
    }
  // Level 3
  case 3:
    {
      switch(phase)
      {
      // Fighters
      case 1:
        if((random(1000) < 20))
        {
          return 3;
        }
        return 0;
      // Fighters and Towers
      case 2:
        if((random(1000) < 10))
        {
          return 3;
        }
        if((random(1000) < 20))
        {
          return 4;
        }
        return 0;
      // Fighters and Jumpers
      case 3:
        if((random(1000) < 10))
        {
          return 3;
        }
        if((random(1000) < 30))
        {
          return 11;
        }
        return 0;
      // Fighters, Towers, and Jumpers
      case 4:
        if((random(1000) < 10))
        {
          return 3;
        }
        if((random(1000) < 20))
        {
          return 4;
        }
        if((random(1000) < 20))
        {
          return 11;
        }
        return 0;
      case 5:
        return 0;
      case 6:
        return 0;
      case 7:
        return 8;
      default:
        return 0;
      }
    }
  // Level 4
  case 4:
    {
      switch(phase)
      {
      // Heavy Fighters and Towers
      case 1:
        if((random(1000) < 30))
        {
          return 12;
        }
        if((random(1000) < 20))
        {
          return 4;
        }
        return 0;
      // Towers and Jumpers
      case 2:
        if((random(1000) < 20))
        {
          return 4;
        }
        if((random(1000) < 20))
        {
          return 11;
        }
        return 0;
      // Fighters and Towers
      case 3:
        if((random(1000) < 40))
        {
          return 3;
        }
        if((random(1000) < 20))
        {
          return 4;
        }
        return 0;
      // Heavy Fighters, Towers, and Jumpers
      case 4:
        if((random(1000) < 20))
        {
          return 4;
        }
        if((random(1000) < 20))
        {
          return 11;
        }
        if((random(1000) < 20))
        {
          return 12;
        }
        return 0;
      case 5:
        return 0;
      case 6:
        return 0;
      case 7:
        return 8;
      default:
        return 0;
      }
    }
  // Level 5
  case 5:
    {
      switch(phase)
      {
      // Barricades
      case 1:
        if((random(1000) < 40))
        {
          return 14;
        }
        return 0;
      // Towers and Hanging Towers
      case 2:
        if((random(1000) < 20))
        {
          return 4;
        }
        if((random(1000) < 20))
        {
          return 9;
        }
        return 0;
      // Barricades and Jumpers
      case 3:
        if((random(1000) < 20))
        {
          return 14;
        }
        if((random(1000) < 20))
        {
          return 11;
        }
        return 0;
      // Towers, Hanging Towers, and Barricades
      case 4:
        if((random(1000) < 20))
        {
          return 4;
        }
        if((random(1000) < 20))
        {
          return 9;
        }
        if((random(1000) < 20))
        {
          return 14;
        }
        return 0;
      case 5:
        return 0;
      case 6:
        return 0;
      case 7:
        return 8;
      default:
        return 0;
      }
    }
  // Level 6
  case 6:
    {
      switch(phase)
      {
      // Bouncing Drones
      case 1:
        if((random(1000) < 20))
        {
          return 10;
        }
        return 0;
      // Towers, Hanging Towers, and Cruise Missiles
      case 2:
        if((random(1000) < 20))
        {
          return 4;
        }
        if((random(1000) < 20))
        {
          return 9;
        }
        if((random(1000) < 20))
        {
          return 6;
        }
        return 0;
      // Towers, Hanging Towers, and Bouncing Drones
      case 3:
        if((random(1000) < 30))
        {
          return 4;
        }
        if((random(1000) < 30))
        {
          return 9;
        }
        if((random(1000) < 20))
        {
          return 10;
        }
        return 0;
      // Towers, Hanging Towers, Bouncing Drones, and Cruise Missiles
      case 4:
        if((random(1000) < 20))
        {
          return 4;
        }
        if((random(1000) < 20))
        {
          return 9;
        }
        if((random(1000) < 20))
        {
          return 10;
        }
        if((random(1000) < 20))
        {
          return 6;
        }
        return 0;
      case 5:
        return 0;
      case 6:
        return 0;
      case 7:
        return 15;
      default:
        return 0;
      }
    }
    default:
      return 0;
  }
}

// Initialize a new enemy which is mainly defined by the type of the enemy.
void initializeEnemy(byte i, byte _ememyType)
{  
  enemyType[i] = _ememyType;
  enemyStatus[i] = 1;
  
  // Meteoroid
  if(enemyType[i] == 1)
  {
    enemyX[i] = 32;
    enemyY[i] = random(32);
    enemyWidth[i] = 2;
    enemyHeight[i] = 2;
    enemyVectorX[i] = -0.5 -(double(random(500))/1000);
    enemyVectorY[i] = (double(random(1000))/1000) - 0.5;
    enemyHitPoints[i] = 2;
    enemyFireRate[i] = 0;
    enemyScore[i] = 50;
  }

  // Meteroid
  if(enemyType[i] == 2)
  {
    if(random(100) < 50)
    {
      enemyX[i] = random(32);
      enemyY[i] = -2;
    }
    else
    {
      enemyX[i] = 32;
      enemyY[i] = random(28) - 2;
    }
    enemyWidth[i] = 2;
    enemyHeight[i] = 2;
    enemyVectorX[i] = -0.5 -(double(random(500))/1000);
    enemyVectorY[i] = (double(random(1000))/1000);
    enemyHitPoints[i] = 2;
    enemyFireRate[i] = 0;
    enemyScore[i] = 50;
  }

  // Fighter
  if(enemyType[i] == 3)
  {
    enemyX[i] = 32;
    enemyY[i] = random(18);
    enemyWidth[i] = 3;
    enemyHeight[i] = 3;
    enemyVectorX[i] = -0.5 -(double(random(500))/1000);
    enemyVectorY[i] = (double(random(1000))/1000) - 0.5;
    enemyHitPoints[i] = 1;
    enemyFireRate[i] = 4 + (difficulty * 4);
    enemyScore[i] = 70;
  }

  // Tower
  if(enemyType[i] == 4)
  {
    enemyX[i] = 32;
    enemyY[i] = ground[scrollPointer] - 2;
    enemyWidth[i] = 3;
    enemyHeight[i] = 2;
    enemyVectorX[i] = -0.5;
    enemyVectorY[i] = 0.0;
    enemyHitPoints[i] = 1;
    enemyFireRate[i] = 4 + (difficulty * 4);
    enemyScore[i] = 40;
  }

  // Killer Satellite
  if(enemyType[i] == 5)
  {
    enemyX[i] = 32;
    enemyY[i] = random(28) - 2;
    enemyWidth[i] = 3;
    enemyHeight[i] = 3;
    enemyVectorX[i] = -0.05;
    enemyVectorY[i] = 0.0;
    enemyHitPoints[i] = 1;
    enemyFireRate[i] = 10 + (difficulty * 5);
    enemyScore[i] = 100;
  }

  // Cruise Missile
  if(enemyType[i] == 6)
  {
    enemyX[i] = 32;
    enemyY[i] = random((ground[scrollPointer] - 1) - (ceiling[scrollPointer] + 1)) + ceiling[scrollPointer] + 1;
    enemyWidth[i] = 4;
    enemyHeight[i] = 1;
    enemyVectorX[i] = -0.8 -(double(random(400))/1000);
    enemyVectorY[i] = (double(random(1000))/1000) - 0.5;
    enemyHitPoints[i] = 1;
    enemyFireRate[i] = 0;
    enemyScore[i] = 30;
  }

  // Submarine
  if(enemyType[i] == 7)
  {
    enemyX[i] = 32;
    enemyY[i] = 26;
    enemyWidth[i] = 3;
    enemyHeight[i] = 2;
    enemyVectorX[i] = -0.5 -(double(random(500))/1000);
    enemyVectorY[i] = 0;
    enemyHitPoints[i] = 2;
    enemyFireRate[i] = 5 + (difficulty * 5);
    enemyScore[i] = 120;
  }

  // End Boss
  if(enemyType[i] == 8)
  {
    enemyX[i] = 32;
    enemyY[i] = 12;
    enemyWidth[i] = 5;
    enemyHeight[i] = 7;
    enemyVectorX[i] = -0.1;
    enemyVectorY[i] = 0;
    enemyHitPoints[i] = 100 + level * 20;
    enemyFireRate[i] = 20 + (level * 10 * difficulty);
    enemyScore[i] = 500;
    endBossActive = true;
  }

  // Hanging Tower
  if(enemyType[i] == 9)
  {
    enemyX[i] = 32;
    enemyY[i] = ceiling[scrollPointer] + 1;
    enemyWidth[i] = 3;
    enemyHeight[i] = 2;
    enemyVectorX[i] = -0.5;
    enemyVectorY[i] = 0.0;
    enemyHitPoints[i] = 1;
    enemyFireRate[i] = 4 + (difficulty * 4);
    enemyScore[i] = 40;
  }

  // Bouncing Drones
  if(enemyType[i] == 10)
  {
    enemyX[i] = 32;
    enemyY[i] = random((ground[scrollPointer] - 3) - (ceiling[scrollPointer] + 1)) + ceiling[scrollPointer] + 1;
    enemyWidth[i] = 2;
    enemyHeight[i] = 3;
    enemyVectorX[i] = -0.3;
    enemyVectorY[i] = random(2) - 0.5;
    enemyHitPoints[i] = 2;
    enemyFireRate[i] = 4 + (difficulty * 4);
    enemyScore[i] = 80;
  }

  // Jumper
  if(enemyType[i] == 11)
  {
    enemyX[i] = 32;
    enemyY[i] = ground[scrollPointer] - 2;
    enemyWidth[i] = 2;
    enemyHeight[i] = 3;
    enemyVectorX[i] = -0.5;
    enemyVectorY[i] = -(random(75) / 50.0);
    enemyHitPoints[i] = 2;
    enemyFireRate[i] = 4 + (difficulty * 4);
    enemyScore[i] = 60;
  }

  // Heavy Fighter
  if(enemyType[i] == 12)
  {
    enemyX[i] = 32;
    enemyY[i] = random(ground[scrollPointer] - 3);
    enemyWidth[i] = 5;
    enemyHeight[i] = 3;
    enemyVectorX[i] = -0.2 -(double(random(200))/1000);
    enemyVectorY[i] = 0;
    enemyHitPoints[i] = 3;
    enemyFireRate[i] = 4 + (difficulty * 4);
    enemyScore[i] = 60;
  }

  // Bomber
  if(enemyType[i] == 13)
  {
    enemyX[i] = 32;
    enemyY[i] = random(ground[scrollPointer] - 3);
    enemyWidth[i] = 4;
    enemyHeight[i] = 3;
    enemyVectorX[i] = -0.15 -(double(random(150))/1000);
    enemyVectorY[i] = 0;
    enemyHitPoints[i] = 2;
    enemyFireRate[i] = 8 + (difficulty * 8);
    enemyScore[i] = 80;
  }

  // Barricade
  if(enemyType[i] == 14)
  {
    enemyX[i] = 32;
    enemyY[i] = random((ground[scrollPointer] - 4) - (ceiling[scrollPointer] + 1)) + ceiling[scrollPointer] + 1;
    enemyWidth[i] = 1;
    enemyHeight[i] = 4;
    enemyVectorX[i] = -0.5;
    enemyVectorY[i] = random(2) - 0.5;
    enemyHitPoints[i] = 2;
    enemyFireRate[i] = 0;
    enemyScore[i] = 20;
  }

  // Core
  if(enemyType[i] == 15)
  {
    enemyX[i] = 32;
    enemyY[i] = 12;
    enemyWidth[i] = 3;
    enemyHeight[i] = 7;
    enemyVectorX[i] = -0.1;
    enemyVectorY[i] = 0;
    enemyHitPoints[i] = 200;
    enemyFireRate[i] = 80 + (difficulty * 40);
    enemyScore[i] = 1000;
    endBossActive = true;
  }  
}

// Move enemies and additionally create new enemies
void moveEnemies()
{
  // If end boss is active, no new enemies will appear.
  if(!endBossActive && scrollCounter > 64)
  {
    // Check whether new enemy appears (probability of appearance and enemy type depends on level and phase; no enemies if end boss active)
    byte j = getRandomEnemyType();
    // j = 0 means no new enemy
    if(j > 0)
    {
      // Check if a free enemy exists. If so, initialize it.
      if(enemyStatus[enemyCounter] == 0)
      {
        initializeEnemy(enemyCounter, j);
        // Increase index pointing to the next enemy (set to 0 if == numberOfEnemies)
        enemyCounter++;
        if(enemyCounter == numberOfEnemies)
        {
          enemyCounter = 0;
        }
      }
    }
  }

  // Move existing enemies
  for(byte i = 0; i < numberOfEnemies; i++)
  {
    if(enemyStatus[i] == 1 || enemyStatus[i] == 2)
    {
      // Asteroids and meteors
      if(enemyType[i] == 1 || enemyType[i] == 2)
      {
        // Remove existing enemy
        matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = 0;

        // Move (following the initial direction vector)
        enemyX[i] = enemyX[i] + enemyVectorX[i];
        enemyY[i] = enemyY[i] + enemyVectorY[i];

        if(enemyX[i] < -enemyWidth[i] || enemyY[i] < -enemyHeight[i] || enemyY[i] > 32)
        {
          enemyStatus[i] = 0;
        }
        if(enemyStatus[i] == 2)
        {
          enemyStatus[i] = 3;
        }

        // Draw enemy
        if(enemyStatus[i] == 1)
        {
          matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(5, 3, 0));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i], matrix.Color333(4, 2, 0));
          matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(4, 2, 0));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(4, 2, 0));
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = i + 3;
        }

      }

      // Fighters
      if(enemyType[i] == 3)
      {
        // Remove existing enemy
        matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 7)] = 0;

        // Move (turns towards the y-level of player´s ship)
        enemyX[i] = enemyX[i] + enemyVectorX[i];
        if(shipY < enemyY[i])
        {
          enemyVectorY[i] = enemyVectorY[i] - 0.05;
        }
        else
        {
          enemyVectorY[i] = enemyVectorY[i] + 0.05;
        }
        enemyY[i] = enemyY[i] + enemyVectorY[i];

        if(enemyX[i] < -enemyWidth[i] || enemyY[i] < -enemyHeight[i] || enemyY[i] > 32)
        {
          enemyStatus[i] = 0;
        }
        if(enemyStatus[i] == 2)
        {
          enemyStatus[i] = 3;
        }

        // Draw enemy
        if(enemyStatus[i] == 1)
        {
          matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 0, 5));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 5, 0));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i], matrix.Color333(0, 1, 0));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 2, matrix.Color333(0, 1, 0));
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 7)] = i + 3;
        }
      }

      // Towers
      if(enemyType[i] == 4)
      {
        // Remove existing enemy
        matrix.drawPixel(enemyX[i] + 1, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 6)] = 0;

        // Move (with the same speed as the landscape -> immobile)
        enemyX[i] = enemyX[i] + enemyVectorX[i];

        if(enemyX[i] < -enemyWidth[i] || enemyY[i] < -enemyHeight[i] || enemyY[i] > 32)
        {
          enemyStatus[i] = 0;
        }
        if(enemyStatus[i] == 2)
        {
          enemyStatus[i] = 3;
        }

        // Draw enemy
        if(enemyStatus[i] == 1)
        {
          if(level != 5)
          {
            matrix.drawPixel(enemyX[i] + 1, enemyY[i], matrix.Color333(0, 0, 5));
            matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 0, 1));
            matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 0, 1));
            matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(0, 0, 1));
          }
          else
          {
            matrix.drawPixel(enemyX[i] + 1, enemyY[i], matrix.Color333(0, 5, 0));
            matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 1, 0));
            matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 1, 0));
            matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(0, 1, 0));
          }
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 6)] = i + 3;
        }
      }

      // Killer-Satellite
      if(enemyType[i] == 5)
      {
        // Remove existing enemy
        matrix.drawPixel(enemyX[i] + 1, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 7)] = 0;

        // Move (turns towards player´s ship)
        // Calculate directional vector of the shot
        double x = (double(shipX + 2) - enemyX[i] + 1.0);
        double y = (double(shipY + 1) - enemyY[i] + 1.0);
        // Calculate length of directional vector to normalize the shot speed
        double z = sqrt(sq(x) + sq(y));
        enemyVectorX[i] = enemyVectorX[i] + ((x / z) / 100.0);
        enemyVectorY[i] = enemyVectorY[i] + ((y / z) / 100.0);
        if(enemyVectorX[i] > 0.5)
        {
          enemyVectorX[i] = 0.5;
        }
        if(enemyVectorX[i] < -0.5)
        {
          enemyVectorX[i] = -0.5;
        }
        if(enemyVectorY[i] > 0.5)
        {
          enemyVectorY[i] = 0.5;
        }
        if(enemyVectorY[i] < -0.5)
        {
          enemyVectorY[i] = -0.5;
        }
        enemyX[i] = enemyX[i] + enemyVectorX[i];
        enemyY[i] = enemyY[i] + enemyVectorY[i];

        if(enemyX[i] < -enemyWidth[i] || enemyX[i] > 32 || enemyY[i] < -enemyHeight[i] || enemyY[i] > 32)
        {
          enemyStatus[i] = 0;
        }
        if(enemyStatus[i] == 2)
        {
          enemyStatus[i] = 3;
        }

        // Draw enemy
        if(enemyStatus[i] == 1)
        {
          matrix.drawPixel(enemyX[i] + 1, enemyY[i], matrix.Color333(1, 0, 0));
          matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(1, 0, 0));
          if((eventCounter % 4) == 0)
          {          
            matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 0, 7));
          }
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(1, 0, 0));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 2, matrix.Color333(1, 0, 0));
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 7)] = i + 3;
        }
      }

      // Cruise Missile
      if(enemyType[i] == 6)
      {
        // Remove existing enemy
        matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 3, enemyY[i], matrix.Color333(0, 0, 0));
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 5)] = 0;

        // Move (aims towards player´s ship)
        enemyX[i] = enemyX[i] + enemyVectorX[i];
        if(shipY < enemyY[i])
        {
          enemyVectorY[i] = -0.25;
        }
        else
        {
          enemyVectorY[i] = 0.25;
        }
        enemyY[i] = enemyY[i] + enemyVectorY[i];

        if(enemyX[i] < -enemyWidth[i] || enemyY[i] < -enemyHeight[i] || enemyY[i] > 32)
        {
          enemyStatus[i] = 0;
        }
        if(enemyStatus[i] == 2)
        {
          enemyStatus[i] = 3;
        }

        // Draw enemy
        if(enemyStatus[i] == 1)
        {
          matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(3, 3, 3));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i], matrix.Color333(3, 3, 3));
          if((eventCounter % 4) == 0)
          {          
            matrix.drawPixel(enemyX[i] + 2, enemyY[i], matrix.Color333(7, 7, 0));
          }
          if((eventCounter % 8) == 0)
          {          
            matrix.drawPixel(enemyX[i] + 3, enemyY[i], matrix.Color333(1, 1, 0));
          }
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 5)] = i + 3;
        }
      }

      // Submarine
      if(enemyType[i] == 7)
      {
        // Remove existing enemy
        matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 6)] = 0;

        // Move (with highspeed on the ocean surface)
        enemyX[i] = enemyX[i] + enemyVectorX[i];

        if(enemyX[i] < -enemyWidth[i])
        {
          enemyStatus[i] = 0;
        }
        if(enemyStatus[i] == 2)
        {
          enemyStatus[i] = 3;
        }

        // Draw enemy
        if(enemyStatus[i] == 1)
        {
          matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(0, 5, 0));
          matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 5, 0));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 1, 0));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(0, 1, 0));
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 6)] = i + 3;
        }
      }

      // End Boss
      if(enemyType[i] == 8)
      {
        // Remove existing enemy
        matrix.drawPixel(enemyX[i] + 3, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 4, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 4, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 3, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 3, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 4, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 4, enemyY[i] + 3, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 4, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 4, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 5, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 5, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 4, enemyY[i] + 5, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 6, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 4, enemyY[i] + 6, matrix.Color333(0, 0, 0));
        playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 8)] = 0;
        playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 8)] = 0;
        playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 8)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 9)] = 0;
        playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 9)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 10)] = 0;
        playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 10)] = 0;
        playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 10)] = 0;
        playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 11)] = 0;
        playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 11)] = 0;

        // Move (up and down towards player´s y-axis)
        if(enemyX[i] > 23)
        {
          enemyX[i] = enemyX[i] + enemyVectorX[i];
        }

        if((shipY + 2) < enemyY[i] + 3)
        {
          if(enemyY[i] > max((ceiling[(scrollPointer + byte(enemyX[i])) % 32] + 0.25), 0.25))
          {
            enemyY[i] = enemyY[i] - 0.25;
          }
        }
        else
        {
          if(enemyY[i] + enemyHeight[i] < ground[(scrollPointer + byte(enemyX[i])) % 32] - 1)
          {
            enemyY[i] = enemyY[i] + 0.25;
          }
        }

        if(enemyX[i] < -enemyWidth[i] || enemyY[i] < -enemyHeight[i] || enemyY[i] > 32)
        {
          enemyStatus[i] = 0;
        }
        if(enemyStatus[i] == 2)
        {
          enemyStatus[i] = 3;
        }

        // Draw enemy
        if(enemyStatus[i] == 1)
        {
          byte r, g, b;
          if(level == 1)     { r = 1; g = 0; b = 0; }
          else if(level == 2){ r = 0; g = 1; b = 0; }
          else if(level == 3){ r = 1; g = 1; b = 0; }
          else if(level == 4){ r = 0; g = 1; b = 1; }
          else if(level == 5){ r = 1; g = 0; b = 1; }
          
          matrix.drawPixel(enemyX[i] + 3, enemyY[i], matrix.Color333(r * 5, g * 5, b * 5));
          matrix.drawPixel(enemyX[i] + 4, enemyY[i], matrix.Color333(r, g, b));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(0, 0, 5));
          matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 1, matrix.Color333(0, 0, 1));
          matrix.drawPixel(enemyX[i] + 4, enemyY[i] + 1, matrix.Color333(r, g, b));
          matrix.drawPixel(enemyX[i], enemyY[i] + 2, matrix.Color333(r * 5, g * 5, b * 5));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 2, matrix.Color333(r, g, b));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 2, matrix.Color333(r, g, b));
          matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 2, matrix.Color333(r, g, b));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 3, matrix.Color333(r * 5, g * 5, b * 5));
          matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 3, matrix.Color333(r, g, b));
          if((eventCounter % 4) == 0)
          {
            matrix.drawPixel(enemyX[i] + 4, enemyY[i] + 2, matrix.Color333(5, 5, 0));
          }
          if((eventCounter % 8) == 0)
          {
            matrix.drawPixel(enemyX[i] + 4, enemyY[i] + 3, matrix.Color333(5, 5, 0));
          }
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 4, matrix.Color333(r, g, b));
          matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 4, matrix.Color333(r, g, b));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 5, matrix.Color333(r * 5, g * 5, b * 5));
          matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 5, matrix.Color333(r, g, b));
          matrix.drawPixel(enemyX[i] + 4, enemyY[i] + 5, matrix.Color333(r, g, b));
          matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 6, matrix.Color333(r * 5, g * 5, b * 5));
          matrix.drawPixel(enemyX[i] + 4, enemyY[i] + 6, matrix.Color333(r, g, b));
          playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 8)] = i + 3;
          playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 8)] = i + 3;
          playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 8)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 9)] = i + 3;
          playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 9)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 10)] = i + 3;
          playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 10)] = i + 3;
          playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 10)] = i + 3;
          playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 11)] = i + 3;
          playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 11)] = i + 3;
        }
      }
      
      // Hanging towers
      if(enemyType[i] == 9)
      {
        // Remove existing enemy
        matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = 0;

        // Move (with the same speed as the landscape -> immobile)
        enemyX[i] = enemyX[i] + enemyVectorX[i];

        if(enemyX[i] < -enemyWidth[i] || enemyY[i] < -enemyHeight[i] || enemyY[i] > 32)
        {
          enemyStatus[i] = 0;
        }
        if(enemyStatus[i] == 2)
        {
          enemyStatus[i] = 3;
        }

        // Draw enemy
        if(enemyStatus[i] == 1)
        {
          if(level != 5)
          {
            matrix.drawPixel(enemyX[i] + 2, enemyY[i], matrix.Color333(0, 0, 1));
            matrix.drawPixel(enemyX[i] + 1, enemyY[i], matrix.Color333(0, 0, 5));
            matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(0, 0, 1));
            matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 0, 1));
          }
          else
          {
            matrix.drawPixel(enemyX[i] + 2, enemyY[i], matrix.Color333(0, 1, 0));
            matrix.drawPixel(enemyX[i] + 1, enemyY[i], matrix.Color333(0, 5, 0));
            matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(0, 1, 0));
            matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 1, 0));
          }
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = i + 3;
        }
      }

      // Bouncing drones
      if(enemyType[i] == 10)
      {
        // Remove existing enemy
        matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 7)] = 0;

        // Move (up and down between ground and ceiling)
        enemyX[i] = enemyX[i] + enemyVectorX[i];
        if(enemyVectorY[i] < 0.0 && enemyY[i] < ceiling[(scrollPointer + byte(enemyX[i])) % 32] + 2)
        {
          enemyVectorY[i] = 0.5;
        }
        if(enemyVectorY[i] > 0.0 && enemyY[i] > ground[(scrollPointer + byte(enemyX[i])) % 32] - 3)
        {
          enemyVectorY[i] = -0.5;
        }
        enemyY[i] = enemyY[i] + enemyVectorY[i];

        if(enemyX[i] < -enemyWidth[i] || enemyY[i] < -enemyHeight[i] || enemyY[i] > 32)
        {
          enemyStatus[i] = 0;
        }
        if(enemyStatus[i] == 2)
        {
          enemyStatus[i] = 3;
        }

        // Draw enemy
        if(enemyStatus[i] == 1)
        {
        matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(7, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i], matrix.Color333(3, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 0, 3));
        matrix.drawPixel(enemyX[i], enemyY[i] + 2, matrix.Color333(7, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 2, matrix.Color333(3, 0, 0));
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 5)] = i + 3;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 5)] = i + 3;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = i + 3;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 7)] = i + 3;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 7)] = i + 3;
        }
      }

      // Jumper (with jumps over the ground)
      if(enemyType[i] == 11)
      {
        // Remove existing enemy
        matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 7)] = 0;

        // Move
        enemyX[i] = enemyX[i] + enemyVectorX[i];
        enemyVectorY[i] = enemyVectorY[i] + 0.1;
        if(enemyY[i] > ground[(scrollPointer + byte(enemyX[i])) % 32] - 3)
        {
          enemyVectorY[i] = -1.5;
        }
        enemyY[i] = enemyY[i] + enemyVectorY[i];

        if(enemyX[i] < -enemyWidth[i] || enemyY[i] < -enemyHeight[i] || enemyY[i] > 32)
        {
          enemyStatus[i] = 0;
        }
        if(enemyStatus[i] == 2)
        {
          enemyStatus[i] = 3;
        }

        // Draw enemy
        if(enemyStatus[i] == 1)
        {
          matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(0, 0, 3));
          matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(5, 0, 3));
          matrix.drawPixel(enemyX[i], enemyY[i] + 2, matrix.Color333(5, 0, 3));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 2, matrix.Color333(3, 0, 3));
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 7)] = i + 3;
        }
      }
      
       // Heavy fighter
      if(enemyType[i] == 12)
      {
        // Remove existing enemy
        matrix.drawPixel(enemyX[i] + 4, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 4, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 7)] = 0;

        // Move (slowly with a slight movement towards the player´s y-axis)
        enemyX[i] = enemyX[i] + enemyVectorX[i];
        if(shipY < enemyY[i])
        {
          enemyVectorY[i] = -0.1;
        }
        else
        {
          enemyVectorY[i] = 0.1;
        }
        enemyY[i] = enemyY[i] + enemyVectorY[i];

        if(enemyX[i] < -enemyWidth[i] || enemyY[i] < -enemyHeight[i] || enemyY[i] > 32)
        {
          enemyStatus[i] = 0;
        }
        if(enemyStatus[i] == 2)
        {
          enemyStatus[i] = 3;
        }

        // Draw enemy
        if(enemyStatus[i] == 1)
        {
          matrix.drawPixel(enemyX[i] + 4, enemyY[i], matrix.Color333(0, 2, 4));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(0, 0, 5));
          matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 1, matrix.Color333(3, 3, 0));
          matrix.drawPixel(enemyX[i] + 4, enemyY[i] + 1, matrix.Color333(3, 3, 0));
          matrix.drawPixel(enemyX[i], enemyY[i] + 2, matrix.Color333(0, 5, 5));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 2, matrix.Color333(0, 2, 4));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 2, matrix.Color333(0, 2, 4));
          matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 2, matrix.Color333(0, 2, 4));
          playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 9)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 7)] = i + 3;
        }
      }

       // Bomber
      if(enemyType[i] == 13)
      {
        // Remove existing enemy
        matrix.drawPixel(enemyX[i] + 2, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 3, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 7)] = 0;

        // Move (slowly through the screen)
        enemyX[i] = enemyX[i] + enemyVectorX[i];

        if(enemyX[i] < -enemyWidth[i] || enemyY[i] < -enemyHeight[i] || enemyY[i] > 32)
        {
          enemyStatus[i] = 0;
        }
        if(enemyStatus[i] == 2)
        {
          enemyStatus[i] = 3;
        }

        // Draw enemy
        if(enemyStatus[i] == 1)
        {
          matrix.drawPixel(enemyX[i] + 2, enemyY[i], matrix.Color333(5, 0, 5));
          matrix.drawPixel(enemyX[i] + 3, enemyY[i], matrix.Color333(3, 0, 3));
          matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 0, 3));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(3, 0, 3));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(3, 0, 3));
          matrix.drawPixel(enemyX[i] + 3, enemyY[i] + 1, matrix.Color333(3, 0, 3));
          matrix.drawPixel(enemyX[i], enemyY[i] + 2, matrix.Color333(5, 0, 5));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 2, matrix.Color333(3, 0, 3));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 2, matrix.Color333(3, 0, 3));
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 8)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 7)] = i + 3;
        }
      }

      // Barricade
      if(enemyType[i] == 14)
      {
        // Remove existing enemy
        matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 3, matrix.Color333(0, 0, 0));
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 8)] = 0;

        // Move (up and down between ground and ceiling)
        enemyX[i] = enemyX[i] + enemyVectorX[i];
        if(enemyVectorY[i] < 0.0 && enemyY[i] < ceiling[(scrollPointer + byte(enemyX[i])) % 32] + 2)
        {
          enemyVectorY[i] = 0.5;
        }
        if(enemyVectorY[i] > 0.0 && enemyY[i] > ground[(scrollPointer + byte(enemyX[i])) % 32] - 4)
        {
          enemyVectorY[i] = -0.5;
        }
        enemyY[i] = enemyY[i] + enemyVectorY[i];

        if(enemyX[i] < -enemyWidth[i] || enemyY[i] < -enemyHeight[i] || enemyY[i] > 32)
        {
          enemyStatus[i] = 0;
        }
        if(enemyStatus[i] == 2)
        {
          enemyStatus[i] = 3;
        }

        // Draw enemy
        if(enemyStatus[i] == 1)
        {
          matrix.drawPixel(enemyX[i], enemyY[i], matrix.Color333(3, 3, 3));
          matrix.drawPixel(enemyX[i], enemyY[i] + 1, matrix.Color333(3, 3, 3));
          matrix.drawPixel(enemyX[i], enemyY[i] + 2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(enemyX[i], enemyY[i] + 3, matrix.Color333(3, 3, 3));
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 8)] = i + 3;
        }
      }

      // Core
      if(enemyType[i] == 15)
      {
        // Remove existing enemy
        matrix.drawPixel(enemyX[i] + 2, enemyY[i], matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 3, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 3, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i], enemyY[i] + 4, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 4, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 4, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 5, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 5, matrix.Color333(0, 0, 0));
        matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 6, matrix.Color333(0, 0, 0));
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 5)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 6)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 7)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 8)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 8)] = 0;
        playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 9)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 9)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 9)] = 0;
        playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 10)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 10)] = 0;
        playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 11)] = 0;

        // Move (into the screen, then it stops)
        if(int(enemyX[i]) > 29)
        {
          enemyX[i] = enemyX[i] + enemyVectorX[i];
        }
        if(enemyStatus[i] == 2)
        {
          enemyStatus[i] = 3;
        }

        // Draw enemy
        if(enemyStatus[i] == 1)
        {
          matrix.drawPixel(enemyX[i] + 2, enemyY[i], matrix.Color333(3, 3, 3));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 1, matrix.Color333(3, 3, 3));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 1, matrix.Color333(3, 0, 0));
          matrix.drawPixel(enemyX[i], enemyY[i] + 2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 2, matrix.Color333(3, 0, 0));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 2, matrix.Color333(0, 0, 3));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 3, matrix.Color333(7, 7, 7));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 3, matrix.Color333(3, 0, 0));
          matrix.drawPixel(enemyX[i], enemyY[i] + 4, matrix.Color333(3, 3, 3));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 4, matrix.Color333(3, 0, 0));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 4, matrix.Color333(0, 0, 3));
          matrix.drawPixel(enemyX[i] + 1, enemyY[i] + 5, matrix.Color333(3, 3, 3));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 5, matrix.Color333(3, 0, 0));
          matrix.drawPixel(enemyX[i] + 2, enemyY[i] + 6, matrix.Color333(3, 3, 3));
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 5)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 6)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 7)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 8)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 8)] = i + 3;
          playfield[byte(enemyX[i] + 5)][byte(enemyY[i] + 9)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 9)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 9)] = i + 3;
          playfield[byte(enemyX[i] + 6)][byte(enemyY[i] + 10)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 10)] = i + 3;
          playfield[byte(enemyX[i] + 7)][byte(enemyY[i] + 11)] = i + 3;
        }
      }
    }

    // Enemy fires shot
    if((enemyStatus[i] == 1) && (random(1000) < enemyFireRate[i]))
    {
      if(enemyShotStatus[enemyShotCounter] == 0)
      {
        // Define new aimed shot fired from current enemy
        enemyShotStatus[enemyShotCounter] = 1;
        enemyShotX[enemyShotCounter] = enemyX[i] + double(enemyWidth[i]) / 2.0;
        enemyShotY[enemyShotCounter] = enemyY[i] + double(enemyHeight[i]) / 2.0;

        // Calculate directional vector of the shot (pointing to player´s ship)
        double x = (double(shipX + 2) - double(enemyShotX[enemyShotCounter]));
        double y = (double(shipY + 1) - double(enemyShotY[enemyShotCounter]));
        // Calculate length of directional vector to normalize the shot speed
        double z = sqrt(sq(x) + sq(y));
        enemyShotVectorX[enemyShotCounter] = (x / z) * (0.7 + 0.1 * float(difficulty));
        enemyShotVectorY[enemyShotCounter] = (y / z) * (0.7 + 0.1 * float(difficulty));

        // Update shot counter
        enemyShotCounter++;
        if(enemyShotCounter == numberOfEnemyShots)
        {
          enemyShotCounter = 0;
        }

        // Sound of fired shot
        tone(audio,256,20);
      }
    }

    // Enemy explosion  
    if(enemyStatus[i] > 2 && enemyStatus[i] < 9)
    {
      // Towers and submarines explode away from ground or ceiling (no point explosion, more a mushroom cloud)
      if(enemyType[i] == 4 || enemyType[i] == 7)
      {
        matrix.fillCircle(enemyX[i] + (enemyWidth[i] / 2.0), enemyY[i] + (enemyHeight[i] / 2.0) - int((enemyStatus[i] - 2) / 2.0), (enemyStatus[i] - 2) / 2.0, matrix.Color333(7, 7, 6 - (enemyStatus[i] - 3)));
      }
      else if(enemyType[i] == 9)
      {
        matrix.fillCircle(enemyX[i] + (enemyWidth[i] / 2.0), enemyY[i] + (enemyHeight[i] / 2.0) + int((enemyStatus[i] - 2) / 2.0), (enemyStatus[i] - 2) / 2.0, matrix.Color333(7, 7, 6 - (enemyStatus[i] - 3)));
      }
      else
      // Normal explosion
      {  
        matrix.fillCircle(enemyX[i] + (enemyWidth[i] / 2.0), enemyY[i] + (enemyHeight[i] / 2.0), (enemyStatus[i] - 2) / 2.0, matrix.Color333(7, 7, 6 - (enemyStatus[i] - 3)));
      }
      tone(audio, 2048 / pow(2,(enemyStatus[i] - 2)), 40);
      enemyStatus[i]++;
    }
    // Remove enemy explosion
    if(enemyStatus[i] > 8 && enemyStatus[i] < 15)
    {
      if(enemyType[i] == 4 || enemyType[i] == 7)
      {
        matrix.fillCircle(enemyX[i] + (enemyWidth[i] / 2.0), enemyY[i] + (enemyHeight[i] / 2.0) - int((enemyStatus[i] - 8) / 2.0), (enemyStatus[i] - 8) / 2.0, matrix.Color333(0, 0, 0));
      }
      else if(enemyType[i] == 9)
      {
        matrix.fillCircle(enemyX[i] + (enemyWidth[i] / 2.0), enemyY[i] + (enemyHeight[i] / 2.0) + int((enemyStatus[i] - 8) / 2.0), (enemyStatus[i] - 8) / 2.0, matrix.Color333(0, 0, 0));
      }
      else
      {
        matrix.fillCircle(enemyX[i] + (enemyWidth[i] / 2.0), enemyY[i] + (enemyHeight[i] / 2.0), (enemyStatus[i] - 8) / 2.0, matrix.Color333(0, 0, 0));
      }
      enemyStatus[i]++;
    }
    // Reset status of destroyed enemy
    if(enemyStatus[i] == 15)
    {
      if((enemyType[i] == 8 || enemyType[i] == 15) && endBossActive)
      {
        endBossDestroyed = true;
        // Remove all shots (including enemy shots) since it looks strange if they stop during big explosion
        for(byte j = 0; j < numberOfShots; j++)
        {
          if(shotFired[j] > 0)
          {
            matrix.drawPixel(shotX[j], shotY[j], matrix.Color333(0, 0, 0));
          }
        }
        for(byte j = 0; j < numberOfEnemyShots; j++)
        {
          if(enemyShotStatus[j] > 0)
          {
            matrix.drawPixel(enemyShotX[j], enemyShotY[j], matrix.Color333(0, 0, 0));
            enemyShotStatus[j] = 0;
          }
        } 
        // Big explosion       
        explosion(enemyX[i] + (enemyWidth[i] / 2.0), enemyY[i] + (enemyHeight[i] / 2.0)); 
      }
      score = score + enemyScore[i];
      extraLifeScore = extraLifeScore + enemyScore[i];
      enemyStatus[i] = 0;
    }
  }
}

void moveEnemyShots()
{
  // Move existing enemy shots
  for(byte i = 0; i < numberOfEnemyShots; i++)
  {
    if(enemyShotStatus[i] == 1)
    {
      // Remove existing enemy shots
      matrix.drawPixel(enemyShotX[i], enemyShotY[i], matrix.Color333(0, 0, 0));
      playfield[byte(enemyShotX[i] + 5)][byte(enemyShotY[i] + 5)] = 0;

      // Move
      enemyShotX[i] = enemyShotX[i] + enemyShotVectorX[i];
      enemyShotY[i] = enemyShotY[i] + enemyShotVectorY[i];

      // Remove shots outside of the screen or shots hitting ground or ceiling
      if(enemyShotX[i] < 0 || enemyShotX[i] > 31 || enemyShotY[i] < 0 || enemyShotY[i] > 31 || (enemyShotY[i] - 1 < ceiling[(scrollPointer + byte(enemyShotX[i])) % 32]) || (enemyShotY[i] + 1 > ground[(scrollPointer + byte(enemyShotX[i])) % 32]))
      {
        enemyShotStatus[i] = 0;
      }

      // Draw enemy
      if(enemyShotStatus[i] == 1)
      {
        matrix.drawPixel(enemyShotX[i], enemyShotY[i], matrix.Color333(7, 7, 0));
        playfield[byte(enemyShotX[i] + 5)][byte(enemyShotY[i] + 5)] = 15;
      }      
    }
  }
}

void moveShip()
{
  // Remove ship at old position
  matrix.drawPixel(shipX, shipY, matrix.Color333(0, 0, 0));
  matrix.drawPixel(shipX + 1, shipY, matrix.Color333(0, 0, 0));
  matrix.drawPixel(shipX, shipY + 1, matrix.Color333(0, 0, 0));
  matrix.drawPixel(shipX + 1, shipY + 1, matrix.Color333(0, 0, 0));
  matrix.drawPixel(shipX + 2, shipY + 1, matrix.Color333(0, 0, 0));
  matrix.drawPixel(shipX + 3, shipY + 1, matrix.Color333(0, 0, 0));
  matrix.drawPixel(shipX + 1, shipY + 2, matrix.Color333(0, 0, 0));
  matrix.drawPixel(shipX + 2, shipY + 2, matrix.Color333(0, 0, 0));
  matrix.drawPixel(shipX + 3, shipY + 2, matrix.Color333(0, 0, 0));
  matrix.drawPixel(shipX + 4, shipY + 2, matrix.Color333(0, 0, 0));
  matrix.drawPixel(shipX + 5, shipY + 2, matrix.Color333(0, 0, 0));

  // Check joystick 1 for player 1
  if(joy1Left() && (shipX > 0))
  {
    shipX--;
  }
  if(joy1Right() && (shipX < 27))
  {
    shipX++;
  }
  if(joy1Up() && (shipY > 0))
  {
    shipY--;
  }
  if(joy1Down() && (shipY < 29))
  {
    shipY++;
  }
  if(joy1Fire() && weaponStatus == 0)
  {
    tone(audio,128,20);

    // Depending on shot level, up to 6 shots fired at once
    for(byte i = 0; i < shotLevel; i++)
    {
      shotX[shotPointer] = shipX + 5;
      shotY[shotPointer] = shipY + 2;
      shotVectorX[shotPointer] = 1;
      if(i == 0)
      {
        shotVectorY[shotPointer] = 0;
      }
      if(i == 1)
      {
        if(shotAngle == 0)
        {
          shotVectorY[shotPointer] = 1;
        }
        else
        {
          shotVectorY[shotPointer] = 0.25;
        }
      }
      if(i == 2)
      {
        if(shotAngle == 0)
        {
          shotVectorY[shotPointer] = -0.25;
        }
        else
        {
          shotVectorY[shotPointer] = -1;
        }
      }
      if(i == 3)
      {
        shotX[shotPointer] = shipX + 1;
        shotVectorX[shotPointer] = -1;
        shotVectorY[shotPointer] = 0;
      }
      if(i == 4)
      {
        if(shotAngle == 0)
        {
          shotVectorY[shotPointer] = 0.25;
        }
        else
        {
          shotVectorY[shotPointer] = 1;
        }          
      }
      if(i == 5)
      {
        if(shotAngle == 0)
        {
          shotVectorY[shotPointer] = -1;
        }
        else
        {
          shotVectorY[shotPointer] = -0.25;
        }          
      }
      shotFired[shotPointer] = 1;
      shotPointer++;
      if(shotPointer == numberOfShots)
      {
        shotPointer = 0;
      }
    }
    // Weapon status is one after pressing the button and firing one fusillade (after that it is charged for power shot)
    weaponStatus = 1; 

    // Alternate shot angle
    shotAngle++;
    if(shotAngle > 1)
    {
      shotAngle = 0;  
    }  
  }
  
  if(!joy1Fire())
  {
    // Release fire button to stop power shot charging
    if(weaponStatus == 1)
    {
      weaponStatus = 0;
      powerShotChargeCounter = 0;
    }
    
    // Release fire button to fire power shot (after charging is ready which mean that weaponStatus == 2)
    if(weaponStatus == 2)
    {
      weaponStatus = 0;
      powerShotChargeCounter = 0;

      shotX[shotPointer] = shipX + 5;
      shotY[shotPointer] = shipY + 2;
      shotVectorX[shotPointer] = 0.75;
      shotVectorY[shotPointer] = 0;
      
      shotFired[shotPointer] = 2; // 2 is specific for power shot
      shotPointer++;
      if(shotPointer == numberOfShots)
      {
        shotPointer = 0;
      }
    }
  }

  // Charge power shot (if button is pressed constantly)
  if(weaponStatus == 1)
  {
    powerShotChargeCounter++;
    if(powerShotChargeCounter > 32)
    {
      weaponStatus = 2;
      powerShotChargeCounter = 0;   
    }
  }

  // Draw ship at new position
  if(shieldEnergy == 0 || ((shieldEnergy > 0) && (eventCounter % 4 == 0)))
  {
    matrix.drawPixel(shipX, shipY, matrix.Color333(5, 0, 0));
    matrix.drawPixel(shipX + 1, shipY, matrix.Color333(1, 0, 0));
    matrix.drawPixel(shipX, shipY + 1, matrix.Color333(5, 0, 0));
    matrix.drawPixel(shipX + 1, shipY + 1, matrix.Color333(1, 0, 0));
    matrix.drawPixel(shipX + 2, shipY + 1, matrix.Color333(1, 0, 0));
    matrix.drawPixel(shipX + 3, shipY + 1, matrix.Color333(0, 0, 5));
    matrix.drawPixel(shipX + 1, shipY + 2, matrix.Color333(5, 0, 0));
    matrix.drawPixel(shipX + 2, shipY + 2, matrix.Color333(1, 0, 0));
    matrix.drawPixel(shipX + 3, shipY + 2, matrix.Color333(1, 0, 0));
    matrix.drawPixel(shipX + 4, shipY + 2, matrix.Color333(5, 0, 0));
  }
  
  // Flashing in front of the ship means that power shot is charged
  if(weaponStatus == 2)
  {
    if((eventCounter % 4) == 0)
    {
      matrix.drawPixel(shipX + 5, shipY + 2, matrix.Color333(7, 7, 7));
      tone(audio, 1024, 10);
    }
  }
  
}

void moveShots()
{
  for(byte i = 0; i < numberOfShots; i++)
  {
    // Normal shot
    if(shotFired[i] == 1)
    {
      // Remove shot at old position
      matrix.drawPixel(shotX[i], shotY[i], matrix.Color333(0, 0, 0));

      // Remove shot if out of screen or if ceiling or ground has been hit (< 1 for shots fired backwards)
      if(shotX[i] < 1 || shotX[i] > 30 || (shotY[i] - 1 < ceiling[(scrollPointer + byte(shotX[i])) % 32]) || (shotY[i] + 1 > ground[(scrollPointer + byte(shotX[i])) % 32]))
      {
        shotFired[i] = 0;
      }
      // Move shot      
      else
      {
        shotX[i] = shotX[i] + shotVectorX[i];
        shotY[i] = shotY[i] + shotVectorY[i];

        // Enemy hit (but no enemy shots, floor, ceiling, and bonus items)
        byte j = playfield[byte(shotX[i] + 5)][byte(shotY[i] + 5)];
        if(j > 2 && j < 15)
        {
          // Find the correct enemy according to its ID and remove one hit point
          enemyHitPoints[j - 3]--;
          tone(audio,512,20);
          // If no more hit points remain, set enemy status = 2 (explosion)
          if(enemyHitPoints[j - 3] < 1)
          {
            enemyStatus[j - 3] = 2;
          }
          shotFired[i] = 0;
        }
        else
        // Draw shot at new position
        {
          matrix.drawPixel(shotX[i], shotY[i], matrix.Color333(3, 3, 3));
        }
      }
    }

    // Power shot    
    if(shotFired[i] == 2)
    {
      // Remove shot at old position
      matrix.drawPixel(shotX[i], shotY[i], matrix.Color333(0, 0, 0));
      matrix.drawPixel(shotX[i] + 1, shotY[i], matrix.Color333(0, 0, 0));
      matrix.drawPixel(shotX[i] + 2, shotY[i], matrix.Color333(0, 0, 0));
      matrix.drawPixel(shotX[i] + 3, shotY[i], matrix.Color333(0, 0, 0));

      // Remove shot if out of screen or if ceiling or ground has been hit
      if(shotX[i] > 30 || (shotY[i] - 1 < ceiling[(scrollPointer + byte(shotX[i] + 3)) % 32]) || (shotY[i] + 1 > ground[(scrollPointer + byte(shotX[i] + 3)) % 32]))
      {
        shotFired[i] = 0;
      }
      // Move shot      
      else
      {
        shotX[i] = shotX[i] + shotVectorX[i];
        shotY[i] = shotY[i] + shotVectorY[i];

        // Enemy hit (but no enemy shots, floor, ceiling, and bonus items)
        byte j = playfield[byte(shotX[i] + 8)][byte(shotY[i] + 5)];
        if(j > 2 && j < 15)
        {
          // Find the correct enemy according to its ID and remove 25 hit points
          enemyHitPoints[j - 3] = enemyHitPoints[j - 3] - 25;
          tone(audio,512,20);
          // If no more hit points remain, set enemy status = 2 (explosion)
          if(enemyHitPoints[j - 3] < 1)
          {
            enemyStatus[j - 3] = 2;
          }
          shotFired[i] = 0;
        }
        else
        // Draw shot at new position (with some flashing tail effects)
        {
          if((eventCounter % 8) == 0)
          {
            matrix.drawPixel(shotX[i], shotY[i], matrix.Color333(1,1,3));
          }
          if((eventCounter % 4) == 0)
          {
            matrix.drawPixel(shotX[i] + 1, shotY[i], matrix.Color333(1,1,3));
            tone(audio, 2048, 10);
          }
          matrix.drawPixel(shotX[i] + 2, shotY[i], matrix.Color333(3,3,7));
          matrix.drawPixel(shotX[i] + 3, shotY[i], matrix.Color333(7,7,7));
        }
      }
    }
  }
}

// Randomly creates a power up or moves an existing one (only one can be active at the same time)
void movePowerUps()
{
  if(powerUpActive == 0 && !endBossActive)
  {
    if((random(5000) < 10))
    {
      powerUpActive = 1;
      byte i = random(10);
      powerUpType = 0; // Shot extension (violett)
      if(i < 6)
      {
        powerUpType = 1; // Shield (blue)
      }
      if(i < 3)
      {
        powerUpType = 2; // Smart bomb (green)
      }
      if(i < 1)
      {
        powerUpType = 3; // Extra life (red)
      }
      if(shotLevel < 2) // Guarantees shot extension as first power up
      {
        powerUpType = 0;  
      }
      powerUpX = 32;
      powerUpY = random(ground[scrollPointer] - 4 - ceiling[scrollPointer]) + ceiling[scrollPointer] + 2;
    }
  }
  if(powerUpActive == 1);
  {
    // Remove shot at old position
    matrix.drawPixel(powerUpX, powerUpY, matrix.Color333(0, 0, 0));
    playfield[byte(powerUpX + 5)][byte(powerUpY + 5)] = 0;

    // Remove power up if it reaches the left screen border or if it has been collected
    if(powerUpX < 1)
    {
      powerUpActive = 0;
    }

    // Draw power up (color depends on type for differentiation)
    if(powerUpActive == 1)
    {
      powerUpX = powerUpX - 0.25;
      if((eventCounter % 4) == 0)
      {
        if(powerUpType == 0)
        {
          matrix.drawPixel(powerUpX, powerUpY, matrix.Color333(7, 0, 7));
        }
        if(powerUpType == 1)
        {
          matrix.drawPixel(powerUpX, powerUpY, matrix.Color333(0, 0, 7));
        }
        if(powerUpType == 2)
        {
          matrix.drawPixel(powerUpX, powerUpY, matrix.Color333(0, 7, 0));
        }
        if(powerUpType == 3)
        {
          matrix.drawPixel(powerUpX, powerUpY, matrix.Color333(7, 0, 0));
        }
      }
      else
      {
        matrix.drawPixel(powerUpX, powerUpY, matrix.Color333(0, 0, 0));
      }
      playfield[byte(powerUpX + 5)][byte(powerUpY + 5)] = 1;
    }
  }  
}

// Effect if player collects a power up
void powerUpCollected()
{
  powerUpActive = 0;
  
  // Shot extension collected
  if(powerUpType == 0)
  {
    if(shotLevel < 6)
    {
      shotLevel++;
    }
  }

  // Shield collected          
  if(powerUpType == 1)
  {
    shieldEnergy = 512;
  }
          
  // Smart bomb collected          
  if(powerUpType == 2)
  {
    for(byte i = 0; i < numberOfEnemies; i++)
    {
      if(enemyStatus[i] == 1 && !endBossActive)
      {
        // All active enemies explode (except level boss)
        enemyStatus[i] = 2;
      }
    }
  }
          
  // Extra life collected          
  if(powerUpType == 3)
  {
    lives++;
  }
          
  tone(audio, 512, 20);
}

// Method to perform the scrolling (drawing the following landscape is performed in a separate method) 
void scroll()
{
  // Helper
  int k;

  // Ground and ceiling positions are stored in two arrays of size 32 (screen width)
  // Instead of always moving every value within the array, only a "scrollPointer" is incremented pointing to the index to store the next ground and ceiling positions coming into the screen on the right hand side.
  if(scrollPointer < 31)
  {
    scrollPointer++;
  }
  else
  {
    scrollPointer = 0;
  }

  if(scrollPointer == 0)
  {
    k = 31;
  }
  else
  {
    k = scrollPointer - 1;
  }

  // Level 1: Screen borders define limits
  if(level == 1)
  {
    newCeiling = -2;
    newGround = 33;
  }

  // Level 2: Lower limit by ocean surface
  if(level == 2)
  {
    newCeiling = -2;
    newGround = 28;
  }

  // Level 1 & 2: Short break before end boss appears
  if((phase == 5 || phase == 6) && level < 3)
  {
    if(scrollCounter > 128)
    {
      scrollCounter = 0;
      phase++;
    }
  }

  // Level 3-6: Upper and lower limits are moved out of the screen (for end boss)
  if(phase > 4 && level > 2)
  {
    if(ceiling[k] > 0)
    {
      newCeiling = ceiling[k] - 1;
    }
    if(ground[k] < 31)
    {
      newGround = ground[k] + 1;
    }
    if(phase < 7 && scrollCounter > 128)
    {
      scrollCounter = 0;
      phase++;
    }
  }
  else
  { 
    // Level 3: Lower limit by hills
    if(level == 3)
    {
      newCeiling = -2;
      newGround = ground[k] + random(3) - 1;
      if(newGround < 24)
      {
        newGround = 24;
      }
      if(newGround > 31)
      {
        newGround = 31;
      }
    }

    // Level 4: Lower limit by rocks
    if(level == 4)
    {
      newCeiling = -2;
      newGround = ground[k] + random(3) - 1;
      if(newGround < 16)
      {
        newGround = 16;
      }
      if(newGround > 31)
      {
        newGround = 31;
      }
    }

    // Level 5: Flight through tunnel
    if(level == 5)
    {
      do
      {
        newCeiling = ceiling[k] + random(3) - 1;
        newGround =  ground[k] + random(3) - 1;
      } 
      while (newGround - newCeiling < 19);

      if(newCeiling < 1)
      {
        newCeiling = 1;
      }
      if(newGround > 31)
      {
        newGround = 31;
      }
    }

    // Level 6: Flight through tunnel
    if(level == 6)
    {
      do
      {
        newCeiling = ceiling[k] + random(3) - 1;
        newGround =  ground[k] + random(3) - 1;
      } 
      while (newGround - newCeiling < 15);
      
      if(newCeiling < 1)
      {
        newCeiling = 1;
      }
      if(newGround > 31)
      {
        newGround = 31;
      }
    }
  }

  ceiling[scrollPointer] = newCeiling;
  ground[scrollPointer] = newGround;    
}

// This draws scroling stars in the background (only in level 1)
void drawStars()
{
  if(level == 1)
  {
    // Move stars
    for(byte i = 0; i < 24; i++)
    {
      // Remove star at old position
      if(starX[i] < 32 
          && !((starY[i] == shipY) && (starX[i] >= shipX) && (starX[i] <= shipX + 1))
          && !((starY[i] == shipY + 1) && (starX[i] >= shipX) && (starX[i] <= shipX + 3))
          && !((starY[i] == shipY + 2) && (starX[i] >= shipX + 1) && (starX[i] <= shipX + 4))
          ){ matrix.drawPixel(byte(starX[i]), byte(starY[i]), matrix.Color333(0, 0, 0)); }

      // Set new star position
      starX[i] = starX[i] - starSpeed[i];

      if(starX[i] < 0){ starX[i] = 127; }

      // Draw star
      if(starX[i] < 32 
          && !((starY[i] == shipY) && (starX[i] >= shipX) && (starX[i] <= shipX + 1))
          && !((starY[i] == shipY + 1) && (starX[i] >= shipX) && (starX[i] <= shipX + 3))
          && !((starY[i] == shipY + 2) && (starX[i] >= shipX + 1) && (starX[i] <= shipX + 4))
          ){ matrix.drawPixel(byte(starX[i]), byte(starY[i]), matrix.Color333(1, 1, 1)); }
    }    
  }  
}

// This draws the landscape.
// Ground and ceiling and the points above and below are redrawn to save resoures but to overwrite all old points
void drawLandscape()
{
  byte j = scrollPointer + 1;  
  for(byte i = 0; i < 32; i++)
  {
    if(j > 31)
    {
      j = 0;
    }
    if(level == 1)
    {
      for(byte k = 0; k < 12; k++)
      {
        matrix.drawPixel(-1, -1, matrix.Color333(0, 0, 0));
      }
    }    
    else if(level == 2)
    {
      if((j % 4) == 0)
      {
        matrix.drawPixel(i, ground[j], matrix.Color333(0, 0, 1));
        matrix.drawPixel(i, ground[j] + 1, matrix.Color333(0, 0, 3));
        matrix.drawPixel(i, ground[j] + 2, matrix.Color333(0, 0, 1));
        matrix.drawPixel(i, ground[j] + 3, matrix.Color333(0, 0, 7));
      }
      else if((j % 4) == 1)
      {
        matrix.drawPixel(i, ground[j], matrix.Color333(0, 0, 3));
        matrix.drawPixel(i, ground[j] + 1, matrix.Color333(0, 0, 1));
        matrix.drawPixel(i, ground[j] + 2, matrix.Color333(0, 0, 7));
        matrix.drawPixel(i, ground[j] + 3, matrix.Color333(0, 0, 1));
      }
      else if((j % 4) == 2)
      {
        matrix.drawPixel(i, ground[j], matrix.Color333(0, 0, 1));
        matrix.drawPixel(i, ground[j] + 1, matrix.Color333(0, 0, 7));
        matrix.drawPixel(i, ground[j] + 2, matrix.Color333(0, 0, 1));
        matrix.drawPixel(i, ground[j] + 3, matrix.Color333(0, 0, 3));
      }
      else
      {
        matrix.drawPixel(i, ground[j], matrix.Color333(0, 0, 7));
        matrix.drawPixel(i, ground[j] + 1, matrix.Color333(0, 0, 1));
        matrix.drawPixel(i, ground[j] + 2, matrix.Color333(0, 0, 3));
        matrix.drawPixel(i, ground[j] + 3, matrix.Color333(0, 0, 1));
      }
    }
    else if(level == 3)
    {
      matrix.drawPixel(i, ground[j] - 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(i, ground[j], matrix.Color333(0, 5, 0));
      matrix.drawPixel(i, ground[j] + 1, matrix.Color333(0, 1, 0));
      for(byte k = 0; k < 3; k++)
      {
        matrix.drawPixel(-1, -1, matrix.Color333(0, 0, 0));
      }
    }
    else if(level == 4)
    {
      matrix.drawPixel(i, ground[j] - 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(i, ground[j], matrix.Color333(5, 3, 0));
      matrix.drawPixel(i, ground[j] + 1, matrix.Color333(1, 1, 0));
      for(byte k = 0; k < 3; k++)
      {
        matrix.drawPixel(-1, -1, matrix.Color333(0, 0, 0));
      }
    }
    else if(level == 5)
    {
      matrix.drawPixel(i, ceiling[j] - 1, matrix.Color333(0, 0, 1));
      matrix.drawPixel(i, ceiling[j], matrix.Color333(0, 0, 7));
      matrix.drawPixel(i, ceiling[j] + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(i, ground[j] - 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(i, ground[j], matrix.Color333(0, 0, 7));
      matrix.drawPixel(i, ground[j] + 1, matrix.Color333(0, 0, 1));
    }
    else if(level == 6)
    {
      matrix.drawPixel(i, ceiling[j] - 1, matrix.Color333(1, 0, 0));
      matrix.drawPixel(i, ceiling[j], matrix.Color333(1, 1, 1));
      matrix.drawPixel(i, ceiling[j] + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(i, ground[j] - 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(i, ground[j], matrix.Color333(1, 1, 1));
      matrix.drawPixel(i, ground[j] + 1, matrix.Color333(1, 0, 0));
    }
    j++;
  }
}

byte collision(){

  // Collisions with ceiling (only in later levels)
  if(level > 4)
  {  
    if(shipY <= ceiling[(scrollPointer + shipX + 1) % 32])
    {
      return 2;
    }
    if(shipY <= ceiling[(scrollPointer + shipX + 2) % 32])
    {
      return 2;
    }
    if(shipY + 1 <= ceiling[(scrollPointer + shipX + 3) % 32])
    {
      return 2;
    }
    if(shipY + 1 <= ceiling[(scrollPointer + shipX + 4) % 32])
    {
      return 2;
    }
    if(shipY + 2 <= ceiling[(scrollPointer + shipX + 5) % 32])
    {
      return 2;
    }
  }

  // Collision with ground
  if(shipY + 1 >= ground[(scrollPointer + shipX + 1) % 32])
  {
    return 2;
  }
  if(shipY + 2 >= ground[(scrollPointer + shipX + 2) % 32])
  {
    return 2;
  }
  if(shipY + 2 >= ground[(scrollPointer + shipX + 3) % 32])
  {
    return 2;
  }
  if(shipY + 2 >= ground[(scrollPointer + shipX + 4) % 32])
  {
    return 2;
  }
  if(shipY + 2 >= ground[(scrollPointer + shipX + 5) % 32])
  {
    return 2;
  }

  // Collision with enemies or enemy shots
  if(playfield[shipX + 5][shipY + 5] > 0)
  {
    return playfield[shipX + 5][shipY + 5];
  }
  if(playfield[shipX + 6][shipY + 5] > 0)
  {
    return playfield[shipX + 6][shipY + 5];
  }
  if(playfield[shipX + 5][shipY + 6] > 0)
  {
    return playfield[shipX + 5][shipY + 6];
  }
  if(playfield[shipX + 6][shipY + 6] > 0)
  {
    return playfield[shipX + 6][shipY + 6];
  }
  if(playfield[shipX + 7][shipY + 6] > 0)
  {
    return playfield[shipX + 7][shipY + 6];
  }
  if(playfield[shipX + 8][shipY + 6] > 0)
  {
    return playfield[shipX + 8][shipY + 6];
  }
  if(playfield[shipX + 6][shipY + 7] > 0)
  {
    return playfield[shipX + 6][shipY + 7];
  }
  if(playfield[shipX + 7][shipY + 7] > 0)
  {
    return playfield[shipX + 7][shipY + 7];
  }
  if(playfield[shipX + 8][shipY + 7] > 0)
  {
    return playfield[shipX + 8][shipY + 7];
  }
  if(playfield[shipX + 9][shipY + 7] > 0)
  {
    return playfield[shipX + 9][shipY + 7];
  }

  return 0;
}

// Remove a life and draw the explosion
void shipExplodes(byte _x, byte _y)
{
  lives--;
  if(shotLevel > 1)
  {
    shotLevel--;
  }
  explosion(_x, _y);
}

// Big explosion used for player´s ship
void explosion(byte _x, byte _y)
{
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

// Show level and score
void showLevelAndScore()
{

  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setTextColor(matrix.Color333(0,0,3));
  matrix.setCursor(1, 0);
  matrix.println("Lvl");
  matrix.setCursor(25, 0);
  matrix.println(level);
  matrix.setTextColor(matrix.Color333(1,1,3));
  matrix.setCursor(1, 8);
  switch(level)
  {
  case 1:
    matrix.println("Space");
    break;
  case 2:
    matrix.println("Ocean");
    break;
  case 3:
    matrix.println("Hills");
    break;
  case 4:
    matrix.println("Rocks");
    break;
  case 5:
    matrix.println("Caves");
    break;
  case 6:
    matrix.println("Core");
    break;
  }
  matrix.setTextColor(matrix.Color333(3,3,0));
  matrix.setCursor(1, 16);
  matrix.println("Liv");
  if(lives <= 9)
  {
    matrix.setCursor(25, 16);
  }
  else
  {
    matrix.setCursor(19, 16);    
  }
  matrix.println(lives);
  matrix.setTextColor(matrix.Color333(0,0,3));
  matrix.setCursor(1, 24);
  matrix.println(score);
  delay(2000);
  // Add an extra life for 5000 points
  if(extraLifeScore > 5000)
  {
    extraLifeScore = extraLifeScore - 5000;
    lives++;
    matrix.fillRect(0, 16, 32, 8, matrix.Color333(0,0,0));
    byte i = 0;
    do
    {
      matrix.setTextColor(matrix.Color333(3,3,0));
      matrix.setCursor(1, 16);
      matrix.println("Liv");
      if(lives <= 9)
      {
        matrix.setCursor(25, 16);
      }
      else
      {
        matrix.setCursor(19, 16);    
      }
      matrix.println(lives);
      delay(4);
      matrix.setTextColor(matrix.Color333(0,0,0));
      matrix.setCursor(1, 16);
      matrix.println("Liv");
      if(lives <= 9)
      {
        matrix.setCursor(25, 16);
      }
      else
      {
        matrix.setCursor(19, 16);    
      }
      matrix.println(lives);
      delay(4);
      tone(audio,(i % 25) * 32 + 50, 5);
      i++;
    } while(i < 125);
  }
  else
  {
    delay(2000);
  }
}

// Game Over
void gameOver()
{
  // Game over
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  for(int i = 100; i > 0; i--)
  {
    matrix.setCursor(4, 8);
    matrix.setTextColor(matrix.Color333(0, 0, 7 * (i % 2)));
    matrix.println("GAME");
    matrix.setCursor(4, 16);
    matrix.println("OVER");
    tone(audio,i * 5,10);
    delay(10);
  }
  matrix.setCursor(4, 8);
  matrix.setTextColor(matrix.Color333(0,0,3));
  matrix.println("GAME");
  matrix.setCursor(4, 16);
  matrix.println("OVER");
  delay(3000);
}

// Show score and highScore
void showScoreAndHighScore()
{
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setTextColor(matrix.Color333(0,0,3));
  matrix.setCursor(1, 0);
  matrix.println("Score");
  matrix.setTextColor(matrix.Color333(1,1,3));
  matrix.setCursor(1, 8);
  matrix.println(score);
  matrix.setTextColor(matrix.Color333(3,3,0));
  matrix.setCursor(1, 16);
  matrix.println("HiScr");
  matrix.setTextColor(matrix.Color333(0,0,3));
  matrix.setCursor(1, 24);
  matrix.println(highScore);
  delay(2000);
  if(highScore < score)
  {
    highScore = score;
    matrix.fillRect(0, 16, 32, 16, matrix.Color333(0,0,0));
    byte i = 0;
    do
    {
      matrix.setTextColor(matrix.Color333(3,3,0));
      matrix.setCursor(1, 16);
      matrix.println("HiScr");
      matrix.setTextColor(matrix.Color333(0,0,3));
      matrix.setCursor(1, 24);
      matrix.println(highScore);
      delay(4);
      matrix.setTextColor(matrix.Color333(0,0,0));
      matrix.setCursor(1, 16);
      matrix.println("HiScr");
      matrix.setCursor(1, 24);
      matrix.println(highScore);
      delay(4);
      tone(audio, i * 32, 5);
      i++;
    } while(i < 125);    
  }
  else
  {
    delay(2000);
  }
}

void endSequence()
{  
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // Draw random stars
  matrix.fillRect(0, 11, 32, 21, matrix.Color333(0,0,0));
  for(byte i = 0; i < 32; i++)
  {
    matrix.drawPixel(random(32), random(32), matrix.Color333(1, 1, 3));
  }

  // Draw moon Mimas
  matrix.fillCircle(16, 16, 8, matrix.Color333(0,0,0)); 
  matrix.drawCircle(16, 16, 8, matrix.Color333(0,0,3)); 
  matrix.drawCircle(15, 13, 3, matrix.Color333(0,0,3));
  delay(4000);
  
  // Explosion of Mimas
  explosion(16, 16);

  // Well done
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setTextColor(matrix.Color333(0,0,3));
  matrix.setCursor(4, 8);
  matrix.println("WELL");
  matrix.setCursor(4, 16);
  matrix.println("DONE");
  delay(4000);
}

// Main loop of the game
void loop() 
{
  // Mithotronic logo
  mithotronic(); 
  ledMePlay();
  
  do
  {

    // Title screen
    title();

    // Select difficulty
    selectDifficulty();

    // Start screen
    setupGame();
    phase = 1;

    do
    {
      // End sequence if level 7 has been reached <- it is not a real level which can be played
      if(level == 7)
      {
        endSequence();
        level = 1; // Set level back to 1 after end sequence
        difficulty++; // Increase difficulty
      }
      showLevelAndScore();
      setupLevel();
      
      reset = false; // Set reset indicator to false
      
      // Game engine loop
      do
      {
        // Get time of engine loop start point (important for a constant game speed)
        engineLoopStartPoint = millis();
        
        collisionValue = 0; // Set collision indicator to 0 (no collision)
        
        drawLandscape(); // Draw landscape

        // Stars are redrawn every second time
        if((eventCounter % 2) == 0)
        {
          drawStars();
        }

        // Enemies and enemy shots are redrawn every second time
        if((eventCounter % 2) == 0)
        {
          moveEnemies();
          moveEnemyShots();
        }
        // Move player´s shots      
        if(!endBossDestroyed)
        {
          moveShots();
        }
        // Move power up
        movePowerUps();
        // Move player´s ship every second time
        if((eventCounter % 2) == 0)
        {
          moveShip();
        }
        // Scroll every fourth time
        if((eventCounter % 4) == 0)
        {
          scroll();
        }
        
        // Play game music
        if((eventCounter % 8) == 0 && phase < 6)
        {
          tone(audio,soundtrack[soundCounter],50);
          soundCounter++;
          if(soundCounter > 127)
          {
            soundCounter = 0;
          }
        }
        // Play end boss melody
        if((eventCounter % 8) == 0 && phase == 7)
        {
          tone(audio, bossMelody[soundCounter % 4], 50);
          soundCounter++;
          if(soundCounter > 127)
          {
            soundCounter = 0;
          }
        }

        // Indicate approaching level boss
        if(phase == 6)
        {
          if(scrollCounter % 2 == 0)
          {
            matrix.setTextColor(matrix.Color333(5,0,0));
          }
          else
          {
            matrix.setTextColor(matrix.Color333(0,0,0));
          }
          matrix.setCursor(4, 8);
          matrix.println("BOSS");
          matrix.setCursor(4, 16);
          matrix.println("TIME");
          tone(audio,scrollCounter * 10,10);
          if(scrollCounter > 127)
          {
            matrix.setTextColor(matrix.Color333(0,0,0));
            matrix.setCursor(4, 8);
            matrix.println("BOSS");
            matrix.setCursor(4, 16);
            matrix.println("TIME");
            scrollCounter = 1024;
            soundCounter = 0;
          }
        }
        
        eventCounter++;
        if(eventCounter > 7)
        {
          eventCounter = 0;
        }
        
        // Reset
        if(digitalRead(buttonReset) == LOW)
        {
          reset = true;
          lives = 0;
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

        // Check for collision with enemies, enemy shots or power-ups
        collisionValue = collision();

        // Ignore collisions with shots or ships if shild is active
        if(shieldEnergy > 0 && collisionValue > 2)
        {
          collisionValue = 0;
        }
        
        // Power-up collected
        if(collisionValue == 1)
        {
          powerUpCollected();
        }
        
        // Reduce shield energy if > 0 (because of a shield power-up)
        if(shieldEnergy > 0)
        {
          shieldEnergy--;
          if(shieldEnergy < 128 && (eventCounter % 4) == 0)
          {
            tone(audio,512,10);
          }
        }
        
        // Increment scroll counter
        scrollCounter++;
        
        // Phase completed (go to next phase)
        if(scrollCounter > 1024)
        {
          scrollCounter = 0;
          if(phase < 7)
          {
            phase++;
          }
        }
        
        // Level completed (End boss destroyed)
        if(endBossDestroyed)
        {
          level++;
          phase = 1;
          break;        
        }

        // Loop until 20 milliseconds has passed since begin of engine loop
        do
        {
        }
        while(abs(millis() - engineLoopStartPoint) < (26 - (difficulty * 2)));

      }
      while(collisionValue < 2);  

      // Explosion if ship has been hit
      if(collisionValue >= 2)
      {    
        shipExplodes(shipX + 2, shipY + 1);
      }

      delay(1000);     
    }
    while(lives > 0);
    // If reset button has been used, it is jumped directly to start screen. Otherwise 'Game Over' and highscore are shown.
    if(!reset)
    {
      gameOver();
      showScoreAndHighScore();
    }
  }
  while(true);
}

