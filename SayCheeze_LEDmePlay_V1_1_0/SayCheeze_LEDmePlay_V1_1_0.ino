// Say Cheeze!
// for the LEDmePlay
//
// www.mithotronic.de
//
// Version: 1.1.0
// Author: Thomas Laubach (2014)
//
// Thanks to Michael Rosskopf!
//
// Release Notes:
// V1.1.0: Support for LEDmePlay Joypad and LEDmePlayBoy (2018)
// V1.0.0: First release
// V0.9.0: Pre-release

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <Timer.h> // Enables timed events

#define CLK 50
#define OE  51
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

// Initialize audio interface
int audio = 2;
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
boolean joystick1Released = true;

// Joystick 2
int buttonU2 = 31;
int buttonD2 = 33;
int buttonL2 = 35;
int buttonR2 = 37;
int buttonFire2 = 39;
int analogX2 = 10;
int analogY2 = 11;
boolean joystick2Released = true;

// Sensitivity of analog thumb joysticks (of the LEDmePlay Joypad) in case of "digital usage" (detects movement if deviates from center position value of 512 by sensitivity value)
const int sensitivity = 192;

int buttonReset = 42;
int buttonPause = 43;

// Highest player ID
byte maxPlayer = 0;

// Level
// The higher this value, the more difficult the game gets
byte level = 0;

// Lanes
int lane[6][128];
int posOnLane[6] = {  0, 0, 0, 0, 0, 0 };

// x position of the lanes 1 to 6
int laneX[] = 
{
  3, 7, 11, 18, 22, 26
};

// screen buffer
unsigned int screen[32][48];

// player 1
byte player1NumAnimPhases = 2; // The player's sprite number of animation phases (currently two for moving, another one for standing)
byte player1AnimPhase = -1; // The player's sprite has two animation phases per directions N, S, E/NE/SE, W/NW/SW
byte player1Dir = 0; // Player looks in playerDirection NORTH. Directions are 0 - NORTH, 1 - NORTHEAST, ..., 4 - SOUTH, 5 - SOUTHWEST, ..., 7 - NORTWEST
byte player1_startX = 1;
byte player1_startY = 8;
byte player1X = player1_startX; // Set the player onto the starting position in the first playing area
byte player1Y = player1_startY;
byte player1X_old = player1X;
byte player1Y_old = player1Y;
// player 2
byte player2NumAnimPhases = 2; // The player's sprite number of animation phases (currently two for moving, another one for standing)
byte player2AnimPhase = -1; // The player's sprite has two animation phases per directions N, S, E/NE/SE, W/NW/SW
byte player2Dir = 0; // Player looks in playerDirection NORTH. Directions are 0 - NORTH, 1 - NORTHEAST, ..., 4 - SOUTH, 5 - SOUTHWEST, ..., 7 - NORTWEST
byte player2_startX = 30;
byte player2_startY = 21;
byte player2X = player2_startX; // Set the player onto the starting position in the first playing area
byte player2Y = player2_startY;
byte player2X_old = player2X;
byte player2Y_old = player2Y;

int player1;
int player2;
int score[100] = { 0 }; // A maximum of 100 players may attend the game
int scoreDisplayed = 0;
boolean idUsed[100] = { false };

byte lives = 6; // Both players share this number of lives

boolean bunnyIsMoving = false;
byte bunnyX = 15 + random(1);
byte bunnyY = random(28);
byte hasX_old = bunnyX - 1;
byte hasY_old = bunnyY - 1;
byte bunnyY_destination = 0;
int bunny_viewingDirection = 0; // 0 - not filmable, -1 - faces left, +1 - faces right
byte hase_animPhase = 0;
boolean bunnyWhistling = false;
boolean snapshotPossible = false;
boolean snapshotTaken = false;
int amountSnapshotsTaken = 0;
boolean cameraBusy = false;

boolean restart = false;
int idleCyclesPlayer1 = 0;
int idleCyclesPlayer2 = 0;
int maxIdleCycles = 1000; // After 1000 main loop cycles, the current game is interrupted

// Initialize the Timer object
Timer timer;
Timer scoreTimer;


// Not currently used - vehicles' colors
//int f[6] = {matrix.Color444(0, 0, 3), matrix.Color444(0, 3, 0), matrix.Color444(3, 0, 0), matrix.Color444(3, 3, 0), matrix.Color444(3, 0, 3), matrix.Color444(0, 3, 3) };

// A point on the screen is either OCCUPIED - 1 or FREE - 0
#define OCCUPIED 1
#define FREE 0

// *****************************************************************

void loop()
{
  displayTitle();
  player1 = 1;
  player2 = 1;
  enterPlayerIDs();
  restart = false;

  while (!restart)
  {
    // Game "Miami"
    playMiami();
    showScores();
  }
} // loop

// ****************************************************************

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

// ****************************************************************

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
} // setup

// ************************************************


void displayTitle()
{
  // Clear screen
  matrix.fillScreen(matrix.Color444(0, 0, 0));

  // Titel
  matrix.setCursor(0, 1);
  matrix.setTextColor(matrix.Color444(15, 9, 9));
  matrix.println("Say");
  matrix.setCursor(0, 9);
  matrix.setTextColor(matrix.Color444(14, 14, 2));
  matrix.println("Chee-");
  matrix.setCursor(4, 17);
  matrix.setTextColor(matrix.Color444(14, 14, 2));
  matrix.print("  ze!");
  
  do
  {
  }
  while ( !joy1Fire() && !joy2Fire() );
  tone(audio, 1024, 20);
  delay(200);

  // Clear screen
  matrix.fillScreen(matrix.Color333(0, 0, 0));
} // displayTitle

// **************************************************

void showScores()
{
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
  matrix.setCursor(0, 0);
  matrix.setTextColor(matrix.Color333(5, 0, 0));
  matrix.println("Scr");
  matrix.drawLine(0, 8, 31, 8, matrix.Color333(5, 0, 0));

  scoreTimer.every(1500, showNextScore);

  do
  {
    scoreTimer.update();
  }
  while ( !joy1Fire() && !joy2Fire() );
  tone(audio, 1024, 20);
  delay(200);
  scoreTimer.stop(0);
  // Clear screen
  matrix.fillScreen(matrix.Color333(0, 0, 0));
} // showScores

// ***************************************************

void showNextScore()
{
  if (scoreDisplayed < maxPlayer) 
  {
    scoreDisplayed++;
  } 
  else
  {
    scoreDisplayed = 1;
  }
  matrix.fillRect(0, 12, 32, 19, matrix.Color333(0, 0, 0));
  matrix.setTextColor(matrix.Color333(0, 0, 5));
  matrix.setCursor(0, 12);  matrix.print("Plr");
  matrix.setCursor(15, 12); matrix.print(scoreDisplayed);
  matrix.setTextColor(matrix.Color333(5, 5, 5));
  matrix.setCursor(0, 22);  matrix.print(score[scoreDisplayed - 1]);
}

void enterPlayerIDs()
{

  // Display the ID for player 1
  matrix.setCursor(0, 1);
  matrix.setTextColor(matrix.Color333(0, 0, 5));
  matrix.println("Plr A");
  matrix.setCursor(0, 9);
  matrix.setTextColor(matrix.Color333(7, 3, 3));
  matrix.println(player1);
  if (idUsed[1]) 
  {
    matrix.fillCircle(16, 12, 2, matrix.Color333(5, 1, 1));
  } 
  else 
  {
    matrix.fillCircle(16, 12, 2, matrix.Color333(1, 5, 1));
  }

  // Choice of player IDs

  // Player 1: // Check Joystick 1
  boolean writeIDs = false;
  do
  {
    if (joy1Right() && (player1 < 99))
    {
      player1++;
      writeIDs = true;
    }
    else if (joy1Left() && (player1 > 1))
    {
      player1--;
      writeIDs = true;
    }
    else
    {
      writeIDs = false;
    }

    if (writeIDs)
    {
      tone(audio, 768, 3);
      matrix.setCursor(0, 9);
      matrix.setTextColor(matrix.Color333(0, 0, 0));
      matrix.fillRect(0, 9, 32, 16, matrix.Color333(0, 0, 0));
      matrix.setCursor(0, 9);
      matrix.setTextColor(matrix.Color333(7, 3, 3));
      matrix.println(player1);
      if (idUsed[player1]) 
      {
        matrix.fillCircle(16, 12, 2, matrix.Color333(5, 1, 1));
      } 
      else 
      {
        matrix.fillCircle(16, 12, 2, matrix.Color333(1, 5, 1));
      }
      writeIDs = false;
      delay(100);
    }
  }
  while ( !joy1Fire() && !joy2Fire() );
  idUsed[player1] = true;
  if (idUsed[player1]) 
  {
    matrix.fillCircle(16, 12, 2, matrix.Color333(5, 1, 1));
  } 
  else 
  {
    matrix.fillCircle(16, 11, 2, matrix.Color333(1, 5, 1));
  }

  if (maxPlayer <= max(player1, player2)) 
  {
    maxPlayer = max(player1, player2);
  }

  // Display the ID for player 2
  matrix.setCursor(0, 17);
  matrix.setTextColor(matrix.Color333(0, 0, 5));
  matrix.println("Plr B");
  matrix.setTextColor(matrix.Color333(7, 3, 3));
  matrix.setCursor(0, 25);
  matrix.println(player2);
  //matrix.fillCircle(16, 28, 2, matrix.Color333(5, 1, 1));
  if (idUsed[2]) 
  {
    matrix.fillCircle(16, 28, 2, matrix.Color333(5, 1, 1));
  } 
  else 
  {
    matrix.fillCircle(16, 28, 2, matrix.Color333(1, 5, 1));
  }


  // Check Joystick 2
  do
  {
    do
    {
      if (joy2Right() && (player2 < 99))
      {
        player2++;
        writeIDs = true;
      }
      else if (joy2Left() && (player2 > 1))
      {
        player2--;
        writeIDs = true;
      }

      if (writeIDs)
      {
        tone(audio, 768, 3);
        matrix.setCursor(0, 25);
        matrix.setTextColor(matrix.Color333(0, 0, 0));
        matrix.fillRect(0, 25, 32, 32, matrix.Color333(0, 0, 0));
        matrix.setCursor(0, 25);
        matrix.setTextColor(matrix.Color333(7, 3, 3));
        matrix.println(player2);
        if (idUsed[player2]) {
          matrix.fillCircle(16, 28, 2, matrix.Color333(5, 1, 1));
        } else {
          matrix.fillCircle(16, 28, 2, matrix.Color333(1, 5, 1));
        }
        writeIDs = false;
        delay(100);
      }
    }
    while ( !joy1Fire() && !joy2Fire() );
  }
  while (player1 == player2);
  idUsed[player2] = true;

  // Maximum player id + 1 = highest assigned player id so far
  if (maxPlayer <= max(player1, player2)) 
  {
    maxPlayer = max(player1, player2);
  }

  // Clear screen
  matrix.fillScreen(matrix.Color333(0, 0, 0));

} // enterPlayerIDs


// ******************************************************************


void playMiami()
{
  if (random(10) < 6)
  {
    bunny_viewingDirection = -1;
  }
  else
  {
    bunny_viewingDirection = 1;
  }
  hase_animPhase = 0;
  bunnyWhistling = false;
  amountSnapshotsTaken = 0;
  snapshotPossible = false;
  snapshotTaken = false;
  cameraBusy = false;

  player1X = player1_startX;
  player1Y = player1_startY;
  player2X = player2_startX;
  player2Y = player2_startY;


  createLanes();
  drawScene();

  // Initialize screen buffer
  FlushScreenMemory();

  // Paint the vehicles on the various lanes
  timer.every(max(100, 240 - 10 * level), moveVehiclesOnLanes1And6);
  timer.every(max(80, 200 - 10 * level), moveVehiclesOnLanes2And5);
  timer.every(max(90, 220 - 10 * level), moveVehiclesOnLanes3And4);
  timer.every(200, steerBunny);
  timer.every(110, steerPlayers);  // Poll the joysticks

  do
  {
    timer.update();
    if ((idleCyclesPlayer1 > maxIdleCycles) && (idleCyclesPlayer2 > maxIdleCycles))
    {
      restart = true;
      idleCyclesPlayer1 = 0;
      idleCyclesPlayer2 = 0;
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
      lives = 0;
    }   
  }
  while ((lives > 0) && (amountSnapshotsTaken < 5) && (!restart));

  if (lives == 0)
  {
    tone(audio, NOTE_C3, 600);
    delay(600);
    tone(audio, NOTE_C3, 450);
    delay(450);
    tone(audio, NOTE_C3, 150);
    delay(150);
    tone(audio, NOTE_C3, 600);
    delay(600);
    tone(audio, NOTE_DS3, 450);
    delay(450);
    tone(audio, NOTE_D3, 150);
    delay(150);
    tone(audio, NOTE_D3, 450);
    delay(450);
    tone(audio, NOTE_C3, 150);
    delay(150);
    tone(audio, NOTE_C3, 450);
    delay(450);
    tone(audio, NOTE_B2, 150);
    delay(150);
    tone(audio, NOTE_C3, 800);
    delay(3000);

    level = 0;
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
    restart = true;
    lives = 6;
  }
  else if (amountSnapshotsTaken == 5)// Enough snapshots taken
  {
    tone(audio, NOTE_C5, 100);
    delay(100);
    tone(audio, NOTE_D5, 100);
    delay(100);
    tone(audio, NOTE_E5, 100);
    delay(100);
    tone(audio, NOTE_G5, 75);
    delay(220);
    tone(audio, NOTE_E5, 100);
    delay(125);
    tone(audio, NOTE_G5, 450);
    delay(3000);

    level++;
    // Both players receive one point for the collective success
    score[player1 - 1]++;
    score[player2 - 1]++;
  }

  timer.stop(0);
  timer.stop(1);
  timer.stop(2);
  timer.stop(3);
  timer.stop(4);

  // Initialize lanes
  for (int f = 0; f < 6; f++)
  {
    for (int r = 0; r < 127; r++)
    {
      lane[f][r] = 0;
    }
  }

  FlushScreenMemory();
}

// *******************************************************************

void FlushScreenMemory()
{
  for (byte i = 0; i <= 31; i++)
  {
    for (byte j = -8; j <= 40; j++)
    {
      screen[i][j] = FREE;
    }
  }
}

void steerBunny()
{
  if (bunnyIsMoving == false)
  {
    // With a given probability, the bunny starts moving on its own. If disturbed, it will also start moving.
    if ( (random(1000) > max(500, 990 - 2 * level)) || ((abs(player1X - bunnyX - 1) < 3) && (abs(player1Y - bunnyY - 1) < 3)) || ((abs(player2X - bunnyX - 1) < 3) && (abs(player2Y - bunnyY - 1) < 3)) || snapshotTaken )
    {
      // Assign a new destination on the middle strip. The bunny will hobble to it.
      int randomY;

      if ((abs(player1X - bunnyX - 1) < 3) && (abs(player1Y - bunnyY - 1) < 3))
      {
        if (player1Y - 1 < bunnyY - 1) // Player 1 chases the bunny downwards,...
        {
          if (player1Y < 15) // ...provided he is in the upper part of the screen
          {
            randomY = min(29, max(0, bunnyY - 1 + 1 + 5 + random(27 - bunnyY - 1)));
          }
          else
          {
            randomY = random(29);
          }
        }
        else // Player 1 chases the bunny upwards,...
        {
          if (player1Y >= 15) // ...provided the player is in the lower part of the screen
          {
            randomY = max( 0, min(29, bunnyY - 1 + 1 - 3 - random(bunnyY - 1)));
          }
          else
          {
            randomY = min(29, max(0, bunnyY - 1 + 1 + 5 + random(27 - bunnyY - 1)));
          }
        }
      }
      else if ((abs(player2X - bunnyX - 1) < 3) && (abs(player2Y - bunnyY - 1) < 3))
      {
        if (player2Y - 1 < bunnyY - 1) // Player 2 chases the bunny downwards,...
        {
          if (player2Y < 15) // provided the player is in the upper part of the screen
          {
            randomY = min(29, max(0, bunnyY - 1 + 1 + 5 + random(27 - bunnyY - 1)));
          }
          else
          {
            randomY = random(29);
          }
        }
        else // Player 2 chases the bunny upwards, ...
        {
          if (player2Y >= 15) // ...provided the player is in the lower part of the screen
          {
            randomY = max( 0, min(29, bunnyY - 1 + 1 - 3 - random(bunnyY - 1)));
          }
          else
          {
            randomY = min(29, max(0, bunnyY - 1 + 1 + 5 + random(27 - bunnyY - 1)));
          }
        }
      }
      else
      {
        do
        {
          randomY = random(29);
        }
        while ( abs(randomY - bunnyY - 1) < 6);
      }
      bunnyY_destination = min(29, max(0, randomY));

      bunnyIsMoving = true;
      bunny_viewingDirection = 0; // Bunny moves to the next destination and is not photogenic in the process

      snapshotTaken = false;
    }
    else // The Bunny could be photographed
    {
      if (bunny_viewingDirection == -1) // Bunny faces left
      {
        // Player 2
        if (( bunnyX - 1 - player1X < 10) && (bunnyX - 1 - player1X > 0) && (bunnyY - 1 == player1Y - 1) && (viewUnimpeded(player1X + 2, player1Y - 1, bunnyX - 1 - 1, bunnyY - 1 + 2))) // Is player 1 close enough to the bunny?
        {
          // Player is left from the bunny. The view of the bunny is unimpeded
          snapshotPossible = true;
          matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 0, 0));
          matrix.fillRect(bunnyX - 1, bunnyY - 1 + 1, 3, 2, matrix.Color444(5, 0, 0));
          matrix.drawPixel(bunnyX - 1, bunnyY - 1, matrix.Color444(5, 0, 0));
        }
        else if (( bunnyX - 1 - player2X < 10) && (bunnyX - 1 - player2X > 0) && (bunnyY - 1 == player2Y - 1) && (viewUnimpeded(player2X + 2, player2Y - 1, bunnyX - 1 - 1, bunnyY - 1 + 2))) // Is player 2 close enough to the bunny?
        {
          // Player is left from the bunny. The view of the bunny is unimpeded
          snapshotPossible = true;
          matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 0, 0));
          matrix.fillRect(bunnyX - 1, bunnyY - 1 + 1, 3, 2, matrix.Color444(5, 0, 0));
          matrix.drawPixel(bunnyX - 1, bunnyY - 1, matrix.Color444(5, 0, 0));
        }
        else
        {
          snapshotPossible = false;
          matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 0, 0));
          matrix.fillRect(bunnyX - 1, bunnyY - 1 + 1, 3, 2, matrix.Color444(7, 7, 7));
          matrix.drawPixel(bunnyX - 1, bunnyY - 1, matrix.Color444(7, 7, 7));
          matrix.drawPixel(bunnyX - 1, bunnyY - 1 + 1, matrix.Color444(3, 0, 0)); // Eye
        }
      }
      else if (bunny_viewingDirection == 1) // Bunny faces right
      {
        // Player 1
        if (( player1X - bunnyX - 1 < 12) && (player1X - bunnyX - 1 > 0) && (bunnyY - 1 == player1Y - 1) && (viewUnimpeded(bunnyX - 1 + 3, bunnyY - 1, player1X - 2, player1Y + 1))) // Is player 1 close enough to the bunny?
        {
          // Player is left from the bunny. The view of the bunny is unimpeded
          snapshotPossible = true;
          matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 0, 0));
          matrix.fillRect(bunnyX - 1, bunnyY - 1 + 1, 3, 2, matrix.Color444(5, 0, 0));
          matrix.drawPixel(bunnyX - 1 + 2, bunnyY - 1, matrix.Color444(5, 0, 0));
        } // Player 2
        else if (( player2X - bunnyX - 1 < 12) && (player2X - bunnyX - 1 > 0) && (bunnyY - 1 == player2Y - 1) && (viewUnimpeded(bunnyX - 1 + 3, bunnyY - 1, player2X - 2, player2Y + 1))) // Is player 2 close enough to the bunny?
        {
          // Player is left from the bunny. The view of the bunny is unimpeded
          snapshotPossible = true;
          matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 0, 0));
          matrix.fillRect(bunnyX - 1, bunnyY - 1 + 1, 3, 2, matrix.Color444(5, 0, 0));
          matrix.drawPixel(bunnyX - 1 + 2, bunnyY - 1, matrix.Color444(5, 0, 0));
        }
        else
        {
          snapshotPossible = false;
          matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 0, 0));
          matrix.fillRect(bunnyX - 1, bunnyY - 1 + 1, 3, 2, matrix.Color444(7, 7, 7));
          matrix.drawPixel(bunnyX - 1 + 2, bunnyY - 1, matrix.Color444(7, 7, 7));
          matrix.drawPixel(bunnyX - 1 + 2, bunnyY - 1 + 1, matrix.Color444(3, 0, 0)); // Eye
        }
      }
    } // Bunny has not started to run
  } // Bunny is not moving
  else // Bunny moves and cannot be photographed
  {
    // Clear the bunny sprite at its old position
    matrix.fillRect(14, hasY_old, 4, 3, matrix.Color333(0, 1, 0));

    if (bunnyY - 1 == bunnyY_destination)
    {
      bunnyIsMoving = false;

      // Decide whether the bunny turns to the other player
      if (random(10) < 5) {
        bunny_viewingDirection = 1;
      }
      else {
        bunny_viewingDirection = -1;
      }

      // Is one of the players in snapshot distance from the bunny?
      if (bunny_viewingDirection == -1) // Bunny poses for the left player
      {
        matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 4, 0));
        bunnyX = 15;
        matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 0, 0));
        matrix.fillRect(bunnyX - 1, bunnyY - 1 + 1, 3, 2, matrix.Color444(7, 7, 7));
        matrix.drawPixel(bunnyX - 1, bunnyY - 1, matrix.Color444(7, 7, 7));
      }
      else if (bunny_viewingDirection == 1) // Bunny poses for the right player
      {
        matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 4, 0));
        bunnyX = 15;
        matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 0, 0));
        matrix.fillRect(bunnyX - 1, bunnyY - 1 + 1, 3, 2, matrix.Color444(7, 7, 7));
        matrix.drawPixel(bunnyX - 1 + 2, bunnyY - 1, matrix.Color444(7, 7, 7));
      }

      bunnyWhistling = true;
      bunnyWhistles();
    }
    else // Bunny has not yet reached the newly assigned destination
    {
      hasX_old = bunnyX - 1;
      hasY_old = bunnyY - 1;
      if (bunnyY - 1 < bunnyY_destination)
      {
        if (  (!((abs(player1X - 1 - (bunnyX - 1)) < 3) && (abs(player1Y - 1 - (bunnyY - 1)) < 4))) && (!((abs(player2X - 1 - (bunnyX - 1)) < 3) && (abs(player2Y - 1 - (bunnyY - 1)) < 4)))) // Bunny must not move onto a player's position
        {
          bunnyY++;
        }
      }
      else
      {
        if (  (!((abs(player1X - 1 - (bunnyX - 1)) < 3) && (abs(player1Y - 1 - (bunnyY - 1)) < 4))) && (!((abs(player2X - 1 - (bunnyX - 1)) < 3) && (abs(player2Y - 1 - (bunnyY - 1)) < 4)))) // Bunny must not move onto a player's position
        {
          bunnyY--;
        }
      }

      if (hase_animPhase == 0)
      {
        matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 0, 0));

        matrix.fillRect(bunnyX - 1 + 1, bunnyY - 1, 2, 2, matrix.Color444(7, 7, 7));
        matrix.drawPixel(bunnyX - 1, bunnyY - 1 + 1, matrix.Color444(7, 7, 7));
        matrix.drawPixel(bunnyX - 1, bunnyY - 1 + 2, matrix.Color444(7, 7, 7));
        matrix.drawPixel(bunnyX - 1 + 2, bunnyY - 1 + 1, matrix.Color444(7, 7, 7));
        matrix.drawPixel(bunnyX - 1 + 2, bunnyY - 1 + 2, matrix.Color444(7, 7, 7));
        hase_animPhase = 1;
      }
      else
      {
        matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 0, 0));

        matrix.fillRect(bunnyX - 1, bunnyY - 1, 2, 2, matrix.Color444(7, 7, 7));
        matrix.drawPixel(bunnyX - 1, bunnyY - 1 + 2, matrix.Color444(7, 7, 7));
        matrix.drawPixel(bunnyX - 1, bunnyY - 1 + 2, matrix.Color444(7, 7, 7));
        matrix.drawPixel(bunnyX - 1 + 2, bunnyY - 1 + 1, matrix.Color444(7, 7, 7));
        matrix.drawPixel(bunnyX - 1 + 2, bunnyY - 1 + 2, matrix.Color444(7, 7, 7));
        hase_animPhase = 0;
      }
    }
  }
}

boolean viewUnimpeded(int x1, int y1, int x2, int y2)
{
  for (int x = x1; x < x2; x++)
  {
    for (int y = y1; y < y2; y++)
    {
      if (screen[x][y] == OCCUPIED) return false;
    }
  }
  return true;
}

void bunnyWhistles()
{
  for (int f = 1400; f < 3400; f = f + 5)
  {
    tone(audio, f, 1);
  }
  bunnyWhistling = false;
}

void createLanes()
{
  // The patterns of the vehicles per lane are four screens high. As each screen has 32 rows, the patterns restart after 4*32 = 128 steps.

  // Vehicle types
  // 1: Car, direction up
  // 2: Car, direction down
  // 3: Truck, direction up
  // 4: Truck, direction down
  // 5: Motorcycle, direction up
  // 6: Motorcycle, direction down

  int minimumDistance = 5;

  // lane 1: Direction up
  randomSeed(analogRead(5));
  int p = 0;
  do
  {
    int z1 = random(50);
    if (z1 > max(10, 45 - 5 * level)) // Place a car
    {
      int z2 = random(15);
      if (z2 < 3) // Place a motorcycle with the lowest probability
      {
        if (p < 121) // Prevents that vehicles are placed too close to each other
        {
          lane[0][p] = 5;
          p = p + minimumDistance + 32 - min(32, level);
        }
      }
      else if ((z2 >= 3) && (z2 < 6)) // Places a truck with a higher probability
      {
        if (p < 116) // Prevents that vehicles are placed too close to each other
        {
          lane[0][p] = 3;
          p = p + minimumDistance + 37 - min (32, level);
        }
      }
      else  // Place a car
      {
        if (p < 120) // Prevents that vehicles are placed too close to each other
        {
          lane[0][p] = 1;
          p = p + minimumDistance + 32 - min (32, level);
        }
      }
    }
    p++;
  }
  while (p < 128);

  // lane 2: Direction down
  randomSeed(analogRead(5));
  p = 0;
  do
  {
    int z1 = random(50);
    if (z1 > max(10, 45 - 5 * level)) // A car is placed: the higher the level, the more likely this gets
    {
      int z2 = random(15);
      if (z2 < 3) // A motorcycle is placed with the lowest probability
      {
        if (p < 121) // Prevents that vehicles are placed too close to each other
        {
          lane[1][p] = 5;
          p = p + minimumDistance + 32 - min (32, level);
        }
      }
      else if ((z2 >= 3) && (z2 < 6)) // Places a truck with higher probability
      {
        if (p < 116) // Prevents that vehicles are placed too close to each other
        {
          lane[1][p] = 3;
          p = p + minimumDistance + 37 - min (32, level);
        }
      }
      else  // Place a truck
      {
        if (p < 120) // Prevents that vehicles are placed too close to each other
        {
          lane[1][p] = 1;
          p = p + minimumDistance + 32 - min (32, level);
        }
      }
    }
    p++;
  }
  while (p < 128);

  // lane 3: Direction up
  randomSeed(analogRead(5));
  p = 0;
  do
  {
    int z1 = random(50);
    if (z1 > max(10, 45 - 5 * level)) // A car is placed. The higher the level, the higher the probability for this.
    {
      int z2 = random(15);
      if (z2 < 3) // A motorcycle is placed with the lowest probability
      {
        if (p < 121) // Prevents that vehicles are placed too close to each other
        {
          lane[2][p] = 5;
          p = p + minimumDistance + 32 - min (32, level);
        }
      }
      else if ((z2 >= 3) && (z2 < 6)) // Place a truck with a higher probability
      {
        if (p < 116) // Prevents that vehicles are placed too close to each other
        {
          lane[2][p] = 3;
          p = p + minimumDistance + 37 - min (32, level);
        }
      }
      else  // Place a truck
      {
        if (p < 120) // Prevents that vehicles are placed too close to each other
        {
          lane[2][p] = 1;
          p = p + minimumDistance + 32 - min (32, level);
        }
      }
    }
    p++;
  }
  while (p < 128);

  // Mirrored lanes

  // lane 6: Direction down
  for (byte p = 0; p < 127; p++) {
    lane[5][p] = lane[0][127 - p];
  }
  // lane 5: Direction up
  for (byte p = 0; p < 127; p++) {
    lane[4][p] = lane[1][127 - p];
  }
  // lane 4: Direction down
  for (byte p = 0; p < 127; p++) {
    lane[3][p] = lane[2][127 - p];
  }

} // createLanes

// ***************************************************************************

void drawScene()
{
  // Clear screen
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // Draw middle strip
  matrix.fillRect(14, 0, 4, 32, matrix.Color333(0, 1, 0));

  // Draw marginal strips
  matrix.fillRect(0, 0, 3, 32, matrix.Color333(0, 1, 0));
  matrix.fillRect(29, 0, 3, 32, matrix.Color333(0, 1, 0));

  // Draw bunny
  if (bunny_viewingDirection == -1) // Bunny poses for the left player
  {
    matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 4, 0));
    bunnyX = 15;
    matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 0, 0));
    matrix.fillRect(bunnyX - 1, bunnyY - 1 + 1, 3, 2, matrix.Color444(5, 5, 5));
    matrix.drawPixel(bunnyX - 1, bunnyY - 1, matrix.Color444(6, 6, 6));
  }
  else if (bunny_viewingDirection == 1) // Bunny poses for the right player
  {
    matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 4, 0));
    bunnyX = 15;
    matrix.fillRect(bunnyX - 1, bunnyY - 1, 3, 3, matrix.Color444(0, 0, 0));
    matrix.fillRect(bunnyX - 1, bunnyY - 1 + 1, 3, 2, matrix.Color444(5, 5, 5));
    matrix.drawPixel(bunnyX - 1 + 2, bunnyY - 1, matrix.Color444(6, 6, 6));
  }
} // drawScene

// ****************************************************************

void markZone(int num, int x1, int y1, int w, int h)
{
  for (int x = x1; x < x1 + w; x++)
  {
    for (int y = y1; y < y1 + h; y++)
    {
      screen[x][y] = num;
    }
  }
}

void drawCar_up(int x, int y, int c)
{
  matrix.fillRect(x, y + 4, 3, 2, matrix.Color333(0, 0, 0)); // Clear car at its old position on screen
  markZone(FREE, x, y + 1, 3, 4);
  matrix.fillRect(x, y + 1, 3, 4, matrix.Color444(0, 0, 3)); // Paint car at its new position
  markZone(OCCUPIED, x, y, 3, 4);
}

void drawCar_down(int x, int y, int c)
{
  matrix.fillRect(x + 0, y + 0, 3, 4, matrix.Color444(0, 0, 3));
  markZone(OCCUPIED, x, y, 3, 4);
}

void drawTruck_up(int x, int y, int c)
{
  matrix.fillRect(x + 0, y + 0, 3, 2, matrix.Color444(0, 0, 3));
  matrix.fillRect(x + 0, y + 3, 3, 5, matrix.Color444(0, 0, 3));
  markZone(OCCUPIED, x, y, 3, 8);

}

void drawTruck_down(int x, int y, int c)
{
  matrix.fillRect(x + 0, y + 0, 3, 5, matrix.Color444(0, 0, 3));
  matrix.fillRect(x + 0, y + 6, 3, 2, matrix.Color444(0, 0, 3));
  markZone(OCCUPIED, x, y, 3, 8);
}

void drawMotorcycle_up(int x, int y, int c)
{
  matrix.drawLine(x + 1, y, x + 1, y + 2, matrix.Color444(0, 0, 3));
  markZone(OCCUPIED, x + 1, y, 1, 3);
}

void drawMotorcycle_down(int x, int y, int c)
{
  matrix.drawLine(x + 1, y, x + 1, y + 2, matrix.Color444(0, 0, 3));
  markZone(OCCUPIED, x + 1, y, 1, 3);
}

void moveVehiclesOnLanes1And6()
{
  // Indicate an engine noise
  if ((!bunnyWhistling) && (!cameraBusy))
  {
    tone(audio, 450 + random(25), 2);
  }

  // Paint all vehicles on lane 1 including these that are partly hidden from view.
  for (int y = -8; y < 32 + 8; y++)
  {
    int c = matrix.Color444(7, 1, 1);

    switch (lane[0][(posOnLane[0] + y + 8) % 128])
    {
      case 1: // Car
        drawCar_up(3, y, c);
        break;
      case 3: // Truck
        matrix.fillRect(3, y + 1, 3, 8, matrix.Color333(0, 0, 0));
        markZone(FREE, 3, y + 1, 3, 8);
        drawTruck_up(3, y, c);
        break;
      case 5: // Motorcycle
        matrix.fillRect(3, y + 3, 3, 2, matrix.Color333(0, 0, 0)); // Clear at old position
        markZone(FREE, 3, y + 3, 3, 2);
        drawMotorcycle_up(3, y, c);
        break;
    }
  }
  if (posOnLane[0] == 128)
  {
    posOnLane[0] = 0;
  }
  else
  {
    posOnLane[0]++;
  }

  // Paint all vehicles on lane 6 including theses that are partly hidden from view (i. e., are entering or leaving the screen)
  for (int y = 0 - 8; y < 32 + 8; y++)
  {
    int c = matrix.Color444(7, 1, 1);

    switch (lane[5][(posOnLane[5] + y + 8) % 128])
    {
      case 1: // Car
        matrix.fillRect(26, y - 2, 3, 2, matrix.Color333(0, 0, 0));
        markZone(FREE, 26, y - 2, 3, 2);
        drawCar_down(26, y, c);

        break;
      case 3: // Truck
        matrix.fillRect(26, y - 2, 3, 8, matrix.Color333(0, 0, 0));
        markZone(FREE, 26, y - 2, 3, 8);
        drawTruck_down(26, y, c);
        break;
      case 5: // Motorcycle
        matrix.fillRect(26, y - 2, 3, 2, matrix.Color333(0, 0, 0));
        markZone(FREE, 26, y - 2, 3, 2);
        drawMotorcycle_down(26, y, c);
        break;
    }
  }
  if (posOnLane[5] == -1) {
    posOnLane[5] = 127;
  }
  else {
    posOnLane[5]--;
  }
} // moveVehiclesOnLanes1And6

void moveVehiclesOnLanes2And5()
{
  // Paint all vehicles on lane 2 including these which are partly hidden from view
  for (int y = 0 - 8; y < 32 + 8; y++)
  {
    int c = matrix.Color444(7, 1, 1);

    switch (lane[1][(posOnLane[1] + y + 8) % 128])
    {
      case 1: // Car
        matrix.fillRect(7, y - 2, 3, 2, matrix.Color333(0, 0, 0));
        markZone(FREE, 7, y - 2, 3, 2);
        drawCar_down(7, y, c);

        break;
      case 3: // Truck
        matrix.fillRect(7, y - 2, 3, 8, matrix.Color333(0, 0, 0));
        markZone(FREE, 7, y - 2, 3, 8);
        drawTruck_down(7, y, c);
        break;
      case 5: // Motorcylce
        matrix.fillRect(7, y - 2, 3, 2, matrix.Color333(0, 0, 0));
        markZone(FREE, 7, y - 2, 3, 2);
        drawMotorcycle_down(7, y, c);
        break;
    }
  }
  if (posOnLane[1] == -1) {
    posOnLane[1] = 127;
  }
  else {
    posOnLane[1]--;
  }

  // Paint all vehicles on lane 2 including these which are partly hidden from view
  for (int y = 0 - 8; y < 32 + 8; y++)
  {
    int c = matrix.Color444(7, 1, 1);

    switch (lane[4][(posOnLane[4] + y + 8) % 128])
    {
      case 1: // Car
        matrix.fillRect(22, y + 4, 3, 2, matrix.Color333(0, 0, 0));
        markZone(FREE, 22, y + 4, 3, 2);
        drawCar_up(22, y, c);

        break;
      case 3: // Truck
        matrix.fillRect(22, y + 1, 3, 8, matrix.Color333(0, 0, 0));
        markZone(FREE, 22, y + 1, 3, 8);
        drawTruck_up(22, y, c);
        break;
      case 5: // Motorcycle
        matrix.fillRect(22, y + 3, 3, 2, matrix.Color333(0, 0, 0));
        markZone(FREE, 22, y + 3, 3, 2);
        drawMotorcycle_up(22, y, c);
        break;
    }
  }

  if (posOnLane[4] == 128) 
  {
    posOnLane[4] = 0;
  }
  else 
  {
    posOnLane[4]++;
  }

} // moveVehiclesOnLanes2And5


// **********************************************************

void moveVehiclesOnLanes3And4()
{
  // Paint all vehicles on lane 3 including these which are partly hidden from view
  for (int y = 0 - 8; y < 32 + 8; y++)
  {
    int c = matrix.Color444(7, 0, 0);

    switch (lane[2][(posOnLane[2] + y + 8) % 128])
    {
      case 1: // Car
        matrix.fillRect(11, y + 4, 3, 2, matrix.Color333(0, 0, 0));
        markZone(FREE, 11, y + 4, 3, 2);
        drawCar_up(11, y, c);

        break;
      case 3: // Truck
        matrix.fillRect(11, y + 1, 3, 8, matrix.Color333(0, 0, 0));
        markZone(FREE, 11, y + 1, 3, 8);
        drawTruck_up(11, y, c);
        break;
      case 5: // Motorcycle
        matrix.fillRect(11, y + 3, 3, 2, matrix.Color333(0, 0, 0));
        markZone(FREE, 11, y + 3, 3, 2);
        drawMotorcycle_up(11, y, c);
        break;
    }
  }
  if (posOnLane[2] == 128) 
  {
    posOnLane[2] = 0;
  }
  else 
  {
    posOnLane[2]++;
  }

  // Paint all vehicles on lane 4 including these which are partly hidden from view
  for (int y = 0 - 8; y < 32 + 8; y++)
  {
    int c = matrix.Color444(7, 0, 0);

    switch (lane[3][(posOnLane[3] + y + 8) % 128])
    {
      case 1: // Car
        matrix.fillRect(18, y - 2, 3, 2, matrix.Color333(0, 0, 0));
        markZone(FREE, 18, y - 2, 3, 2);
        drawCar_down(18, y, c);

        break;
      case 3: // Truck
        matrix.fillRect(18, y - 2, 3, 8, matrix.Color333(0, 0, 0));
        markZone(FREE, 18, y - 2, 3, 8);
        drawTruck_down(18, y, c);
        break;
      case 5: // Motorcycle
        matrix.fillRect(18, y - 2, 3, 2, matrix.Color333(0, 0, 0));
        markZone(FREE, 18, y - 2, 3, 2);
        drawMotorcycle_down(18, y, c);
        break;
    }
  }

  if (posOnLane[3] == -1) 
  {
    posOnLane[3] = 127;
  }
  else 
  {
    posOnLane[3]--;
  }
} // moveVehiclesOnLanes3And4

// ********************************************************************************

void playerLostALife(int player, byte _x, byte _y)
{
  int col;
  int headCol = matrix.Color444(7, 4, 4);
  
  if (player == 1) 
  {
    col = matrix.Color333(7, 7, 0); // Default player sprite color
  }
  else 
  {
    col = matrix.Color333(0, 7, 7);
  } // Default player sprite color
  matrix.fillRect( player1X - 1, player1Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background

  // Play a sound effect
  for (int f = 3000; f > 500; f = f - 4)
  {
    tone(audio, f, 1);
  }
  for (int i = 0; i < 20; i++)
  {
    tone(audio, 200 + random(50), 1);
  }

  if (player == 1)
  {
    for (int i = 0; i < 3; i++)
    {
      matrix.fillRect( player1X - 1, player1Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // N
      matrix.drawPixel( player1X + 0, player1Y - 1, headCol);
      matrix.drawPixel( player1X + 0, player1Y + 0, col);
      matrix.drawPixel( player1X - 1, player1Y + 1, col);
      matrix.drawPixel( player1X + 1, player1Y + 1, col);
      delay(100);
      matrix.fillRect( player1X - 1, player1Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // NE
      matrix.drawPixel( player1X + 1, player1Y - 1, headCol);
      matrix.drawPixel( player1X + 0, player1Y + 0, col);
      matrix.drawPixel( player1X - 1, player1Y + 0, col);
      matrix.drawPixel( player1X + 0, player1Y + 1, col);
      delay(100);
      matrix.fillRect( player1X - 1, player1Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // W
      matrix.drawPixel( player1X + 1, player1Y + 0, headCol);
      matrix.drawPixel( player1X + 0, player1Y + 0, col);
      matrix.drawPixel( player1X - 1, player1Y - 1, col);
      matrix.drawPixel( player1X - 1, player1Y + 1, col);
      delay(100);
      matrix.fillRect( player1X - 1, player1Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // SE
      matrix.drawPixel( player1X + 1, player1Y + 1, headCol);
      matrix.drawPixel( player1X + 0, player1Y + 0, col);
      matrix.drawPixel( player1X - 1, player1Y + 0, col);
      matrix.drawPixel( player1X + 0, player1Y - 1, col);
      delay(100);
      matrix.fillRect( player1X - 1, player1Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // S
      matrix.drawPixel( player1X + 0, player1Y + 1, headCol);
      matrix.drawPixel( player1X + 0, player1Y + 0, col);
      matrix.drawPixel( player1X - 1, player1Y - 1, col);
      matrix.drawPixel( player1X + 1, player1Y + 1, col);
      delay(100);
      matrix.fillRect( player1X - 1, player1Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // SW
      matrix.drawPixel( player1X - 1, player1Y + 1, headCol);
      matrix.drawPixel( player1X + 0, player1Y + 0, col);
      matrix.drawPixel( player1X + 0, player1Y - 1, col);
      matrix.drawPixel( player1X + 1, player1Y + 0, col);
      delay(100);
      matrix.fillRect( player1X - 1, player1Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // W
      matrix.drawPixel( player1X - 1, player1Y + 0, headCol);
      matrix.drawPixel( player1X + 0, player1Y + 0, col);
      matrix.drawPixel( player1X + 1, player1Y - 1, col);
      matrix.drawPixel( player1X - 1, player1Y + 1, col);
      delay(100);
      matrix.fillRect( player1X - 1, player1Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // NW
      matrix.drawPixel( player1X - 1, player1Y - 1, headCol);
      matrix.drawPixel( player1X + 0, player1Y + 0, col);
      matrix.drawPixel( player1X + 1, player1Y + 0, col);
      matrix.drawPixel( player1X + 0, player1Y + 1, col);
      delay(100);
    }
  }
  else  // Player = 2
  {
    for (int i = 0; i < 3; i++)
    {
      matrix.fillRect( player2X - 1, player2Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // N
      matrix.drawPixel( player2X + 0, player2Y - 1, headCol);
      matrix.drawPixel( player2X + 0, player2Y + 0, col);
      matrix.drawPixel( player2X - 1, player2Y + 1, col);
      matrix.drawPixel( player2X + 1, player2Y + 1, col);
      delay(100);
      matrix.fillRect( player2X - 1, player2Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // NE
      matrix.drawPixel( player2X + 1, player2Y - 1, headCol);
      matrix.drawPixel( player2X + 0, player2Y + 0, col);
      matrix.drawPixel( player2X - 1, player2Y + 0, col);
      matrix.drawPixel( player2X + 0, player2Y + 1, col);
      delay(100);
      matrix.fillRect( player2X - 1, player2Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // W
      matrix.drawPixel( player2X + 1, player2Y + 0, headCol);
      matrix.drawPixel( player2X + 0, player2Y + 0, col);
      matrix.drawPixel( player2X - 1, player2Y - 1, col);
      matrix.drawPixel( player2X - 1, player2Y + 1, col);
      delay(100);
      matrix.fillRect( player2X - 1, player2Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // SE
      matrix.drawPixel( player2X + 1, player2Y + 1, headCol);
      matrix.drawPixel( player2X + 0, player2Y + 0, col);
      matrix.drawPixel( player2X - 1, player2Y + 0, col);
      matrix.drawPixel( player2X + 0, player2Y - 1, col);
      delay(100);
      matrix.fillRect( player2X - 1, player2Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // S
      matrix.drawPixel( player2X + 0, player2Y + 1, headCol);
      matrix.drawPixel( player2X + 0, player2Y + 0, col);
      matrix.drawPixel( player2X - 1, player2Y - 1, col);
      matrix.drawPixel( player2X + 1, player2Y + 1, col);
      delay(100);
      matrix.fillRect( player2X - 1, player2Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // SW
      matrix.drawPixel( player2X - 1, player2Y + 1, headCol);
      matrix.drawPixel( player2X + 0, player2Y + 0, col);
      matrix.drawPixel( player2X + 0, player2Y - 1, col);
      matrix.drawPixel( player2X + 1, player2Y + 0, col);
      delay(100);
      matrix.fillRect( player2X - 1, player2Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // W
      matrix.drawPixel( player2X - 1, player2Y + 0, headCol);
      matrix.drawPixel( player2X + 0, player2Y + 0, col);
      matrix.drawPixel( player2X + 1, player2Y - 1, col);
      matrix.drawPixel( player2X - 1, player2Y + 1, col);
      delay(100);
      matrix.fillRect( player2X - 1, player2Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
      // NW
      matrix.drawPixel( player2X - 1, player2Y - 1, headCol);
      matrix.drawPixel( player2X + 0, player2Y + 0, col);
      matrix.drawPixel( player2X + 1, player2Y + 0, col);
      matrix.drawPixel( player2X + 0, player2Y + 1, col);
      delay(100);
    }
  }
  delay(500);
}

// This subroutine is not directly called but within the timing framework
void steerPlayers()
{
  // PLAYER 1

  // Store the former position of player 1
  player1X_old = player1X;
  player1Y_old = player1Y;

  // Joystick 1
  joystick1Released = true;
  if      (joy1Up() && (player1Y > 1)) // N
  {
    idleCyclesPlayer1 = 0;
    if ((abs(player1X - bunnyX) < 3) && (player1Y - bunnyY < 4) && (player1Y - bunnyY > 0)) // Player must not run into the bunny which is situated north of him
    {
    }
    else
    {
      player1Y--;
      player1Dir = 0;
      joystick1Released = false;
      if (player1AnimPhase == player1NumAnimPhases - 1) {
        player1AnimPhase = 0;
      }
      else {
        player1AnimPhase++;
      } // Toggle the animation phase
      drawPlayer1();
    }
  }
  else if (joy1Right() && (player1X < 30)) // E
  {
    idleCyclesPlayer1 = 0;
    //if ((abs(player1X-bunnyX) < 4) && (player1Y-bunnyY < 3) && (bunnyX-player1X > 0)) // Player must not run into the bunny which is situated east of him
    //{
    //}
    //else
    {
      player1X++;
      joystick1Released = false;
      player1Dir = 2;
      if (player1AnimPhase == player1NumAnimPhases - 1) {
        player1AnimPhase = 0;
      }
      else {
        player1AnimPhase++;
      }
      drawPlayer1();
    }
  }
  else if (joy1Down() && (player1Y < 30)) // S
  {
    idleCyclesPlayer1 = 0;
    if ((abs(player1X - bunnyX) < 3) && (bunnyY - player1Y < 4) && (bunnyY - player1Y > 0)) // Player must not run into the bunny
    {
    }
    else
    {
      player1Y++;
      joystick1Released = false;
      player1Dir = 4;
      if (player1AnimPhase == player1NumAnimPhases - 1)
      {
        player1AnimPhase = 0;
      }
      else
      {
        player1AnimPhase++;
      }
      drawPlayer1();
    }
  }
  else if (joy1Left() && (player1X > 1))// W
  {
    idleCyclesPlayer1 = 0;
    //if (!((abs(player1Y-1-bunnyY-1) < 3) && (abs(player1X-1-bunnyX-1) < 4))) // Player must not move into the bunny
    {
      player1X--;
      joystick1Released = false;
      player1Dir = 6;
      if (player1AnimPhase == player1NumAnimPhases - 1) {
        player1AnimPhase = 0;
      }
      else {
        player1AnimPhase++;
      }
      drawPlayer1();
    }
  }

  if (joystick1Released == true) // Joystick was not touched. Player stands still
  {
    player1AnimPhase = -1; // Player stands still
    drawPlayer1();
    idleCyclesPlayer1++;
  }


  // Check on collision between player 1 and a vehicle
  if ((screen[player1X][player1Y] == OCCUPIED) || (screen[player1X - 1][player1Y - 1] == OCCUPIED) || (screen[player1X + 1][player1Y + 1] == OCCUPIED)
      || (screen[player1X - 1][player1Y + 1] == OCCUPIED) || (screen[player1X + 1][player1Y + 1] == OCCUPIED))
  {
    playerLostALife(1, player1X, player1Y);
    drawScene();
    player1X = player1_startX;
    player1Y = player1_startY;
    if (lives > 0) {
      lives--;
    }

    // Initialize screen buffer
    for (byte i = 0; i < 31; i++)
    {
      for (byte j = -8; j < 40; j++)
      {
        screen[i][j] = FREE;
      }
    }

  }

  // Player wants to take a snapshot
  if (joy1Fire())
  {
    idleCyclesPlayer1 = 0;
    if (snapshotPossible)
    {
      cameraBusy = true;
      for (int i = 0; i < 200; i++) {
        tone(audio, 400 + random(50), 1);
      }
      delay(200);
      for (int i = 0; i < 300; i++) {
        tone(audio, 600 + random(50), 1);
      }
      delay(300);
      for (int i = 0; i < 100; i++) {
        tone(audio, 800 + random(50), 1);
      }
      delay(100);
      snapshotTaken = true;
      score[player2 - 1]++;
      amountSnapshotsTaken++;
      snapshotPossible = false;
      cameraBusy = false;
    }
  }

  // Player 2
  // Store the former position of player 2
  player2X_old = player2X;
  player2Y_old = player2Y;

  // Joystick 2
  joystick2Released = true;
  if      (joy2Up() && (player2Y > 1)) // N
  {
    idleCyclesPlayer2 = 0;
    //if (!((abs(player2X-1-bunnyX-1) < 3) && (abs(player2Y-1-bunnyY-1) < 4))) // Player must not run into the bunny
    if ((abs(player2X - bunnyX) < 3) && (player2Y - bunnyY < 4) && (player2Y - bunnyY > 0)) // Player must not run into the bunny which is situated north of him
    {
    }
    else
    {
      player2Y--;
      player2Dir = 0;
      joystick2Released = false;
      if (player2AnimPhase == player2NumAnimPhases - 1) 
      {
        player2AnimPhase = 0;
      }
      else {
        player2AnimPhase++;
      } // Toggle the animation phase
      drawPlayer2();
    }
  }
  else if (joy2Right() && (player2X < 30)) // E
  {
    idleCyclesPlayer2 = 0;
    //if (!((abs(player2Y-1-bunnyY-1) < 3) && (abs(player2X-1-bunnyX-1) < 4))) // Player must not run into the bunny
    {
      player2X++;
      joystick2Released = false;
      player2Dir = 2;
      if (player2AnimPhase == player2NumAnimPhases - 1) {
        player2AnimPhase = 0;
      }
      else {
        player2AnimPhase++;
      }
      drawPlayer2();
    }
  }

  else if (joy2Down() && (player2Y < 30)) // S
  {
    idleCyclesPlayer2 = 0;
    //if (!((abs(player2X-1-bunnyX-1) < 3) && (abs(player2Y-1-bunnyY-1) < 4))) // Player must not run into the bunny
    if ((abs(player2X - bunnyX) < 3) && (bunnyY - player2Y < 4) && (bunnyY - player2Y > 0)) // Player must not run into the bunny
    {
    }
    else
    {
      player2Y++;
      joystick2Released = false;
      player2Dir = 4;
      if (player2AnimPhase == player2NumAnimPhases - 1) 
      {
        player2AnimPhase = 0;
      }
      else {
        player2AnimPhase++;
      }
      drawPlayer2();
    }
  }
  else if (joy2Left() && (player2X > 1))// W
  {
    idleCyclesPlayer2 = 0;
    {
      player2X--;
      joystick2Released = false;
      player2Dir = 6;
      if (player2AnimPhase == player2NumAnimPhases - 1) 
      {
        player2AnimPhase = 0;
      }
      else {
        player2AnimPhase++;
      }
      drawPlayer2();
    }
  }

  if (joystick2Released == true) // Joystick was not touched. Player stands still
  {
    player2AnimPhase = -1; // Player stands still
    drawPlayer2();
    idleCyclesPlayer2++;
  }

  // Check on collision between player 2 and a vehicle
  if ((screen[player2X][player2Y] == OCCUPIED) || (screen[player2X - 1][player2Y - 1] == OCCUPIED) || (screen[player2X + 1][player2Y + 1] == OCCUPIED)
      || (screen[player2X - 1][player2Y + 1] == OCCUPIED) || (screen[player2X + 1][player2Y + 1] == OCCUPIED))
  {
    playerLostALife(2, player2X, player2Y);
    drawScene();
    player2X = player2_startX;
    player2Y = player2_startY;
    if (lives > 0) 
    {
      lives--;
    }

    // Initialize screen buffer
    for (byte i = 0; i < 31; i++)
    {
      for (byte j = -8; j < 40; j++)
      {
        screen[i][j] = FREE;
      }
    }

  }

  // Player wants to take a snapshot
  if (joy2Fire())
  {
    if (snapshotPossible)
    {
      cameraBusy = true;
      for (int i = 0; i < 200; i++) {
        tone(audio, 400 + random(50), 1);
      }
      delay(200);
      for (int i = 0; i < 300; i++) {
        tone(audio, 600 + random(50), 1);
      }
      delay(300);
      for (int i = 0; i < 100; i++) {
        tone(audio, 800 + random(50), 1);
      }
      delay(100);
      snapshotTaken = true;
      score[player2 - 1]++;
      amountSnapshotsTaken++;
      snapshotPossible = false;
      cameraBusy = false;
    }
  }
}

// DrawPlayer gets called at the end of the controlPlayer subroutine
void drawPlayer1()
{
  int col = matrix.Color333(7, 7, 0); // Default player sprite color
  int headCol = matrix.Color444(7, 4, 4);

  // (playerX, playerY) is the midpoint of the player's sprite
  // Clear player sprite at old position
  matrix.fillRect( player1X_old - 1, player1Y_old - 1, 3, 3, matrix.Color333(0, 0, 0));

  if (joystick1Released == false) // Player is moving: update the animation phases
  {
    matrix.fillRect( player1X - 1, player1Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
    // Restore all strips
    if (player1X_old < 4) // Left strip
    {
      matrix.fillRect(0, player1Y_old - 1, 3, 3, matrix.Color333(0, 1, 0));
    }
    else if ((player1X_old > 12) && (player1X_old < 19)) // Middle strip
    {
      matrix.fillRect(14, player1Y_old - 1, 4, 3, matrix.Color333(0, 1, 0));
    }
    else if (player1X_old > 27) // Right strip
    {
      matrix.fillRect(29, player1Y_old - 1, 3, 3, matrix.Color333(0, 1, 0));
    }

    switch (player1AnimPhase)
    {
      case 0:

        if ((player1Dir == 0) || (player1Dir == 4)) // Player moves northwards or southwards
        {
          matrix.drawPixel( player1X + 0, player1Y - 1, headCol);
          matrix.drawPixel( player1X + 0, player1Y + 0, col);
          matrix.drawPixel( player1X + 1, player1Y + 1, col);
        }
        else if ((player1Dir == 5) || (player1Dir == 6) || (player1Dir == 7)) // Player moves to westwards
        {
          matrix.drawPixel( player1X + 0, player1Y - 1, headCol);
          matrix.drawPixel( player1X + 0, player1Y + 0, col);
          matrix.drawPixel( player1X - 1, player1Y + 1, col);
        }
        else if ((player1Dir == 1) || (player1Dir == 2) || (player1Dir == 3)) // Player moves to eastwards
        {
          matrix.drawPixel( player1X + 0, player1Y - 1, headCol);
          matrix.drawPixel( player1X + 0, player1Y + 0, col);
          matrix.drawPixel( player1X - 1, player1Y + 1, col);
          matrix.drawPixel( player1X + 1, player1Y + 0, col);
        }
        break;

      case 1:

        if ((player1Dir == 0) || (player1Dir == 4)) // Player moves northwards or southwards
        {
          matrix.drawPixel( player1X + 0, player1Y - 1, headCol);
          matrix.drawPixel( player1X + 0, player1Y + 0, col);
          matrix.drawPixel( player1X - 1, player1Y + 1, col);
        }
        else if ((player1Dir == 5) || (player1Dir == 6) || (player1Dir == 7)) // Player moves westwards
        {
          matrix.drawPixel( player1X + 0, player1Y - 1, headCol);
          matrix.drawPixel( player1X + 0, player1Y + 0, col);
          matrix.drawPixel( player1X + 1, player1Y + 1, col);
          matrix.drawPixel( player1X - 1, player1Y + 0, col);
        }
        else if ((player1Dir == 1) || (player1Dir == 2) || (player1Dir == 3)) // Player moves eastwards
        {
          matrix.drawPixel( player1X + 0, player1Y - 1, headCol);
          matrix.drawPixel( player1X + 0, player1Y + 0, col);
          matrix.drawPixel( player1X + 1, player1Y + 1, col);
        }
        break;

    } // switch
  }
  else // Player stands still: show the still image of the player's sprite
  {
    matrix.fillRect( player1X - 1, player1Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background

    matrix.drawPixel( player1X + 0, player1Y - 1, headCol);
    matrix.drawPixel( player1X + 0, player1Y + 0, col);
    matrix.drawPixel( player1X - 1, player1Y + 1, col);
    matrix.drawPixel( player1X + 1, player1Y + 1, col);
  }
}

// Draw player gets called at the end of the controlPlayer subroutine
void drawPlayer2()
{
  int col = matrix.Color333(0, 7, 7); // Default player sprite color
  int headCol = matrix.Color444(7, 4, 4);

  // (playerX, playerY) is the midpoint of the player's sprite
  // Clear player sprite at old position
  matrix.fillRect( player2X_old - 1, player2Y_old - 1, 3, 3, matrix.Color333(0, 0, 0));

  if (joystick2Released == false) // Player is moving: update the animation phases
  {
    matrix.fillRect( player2X - 1, player2Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background
    // Restore all strips
    if (player2X_old < 4) // Left strip
    {
      matrix.fillRect(0, player2Y_old - 1, 3, 3, matrix.Color333(0, 1, 0));
    }
    else if ((player2X_old > 12) && (player2X_old < 19)) // Mittelstreifen
    {
      matrix.fillRect(14, player2Y_old - 1, 4, 3, matrix.Color333(0, 1, 0));
    }
    else if (player2X_old > 27) // Right strip
    {
      matrix.fillRect(29, player2Y_old - 1, 3, 3, matrix.Color333(0, 1, 0));
    }

    switch (player2AnimPhase)
    {
      case 0:

        if ((player2Dir == 0) || (player2Dir == 4)) // Player moves northwards or southwards
        {
          matrix.drawPixel( player2X + 0, player2Y - 1, headCol);
          matrix.drawPixel( player2X + 0, player2Y + 0, col);
          matrix.drawPixel( player2X + 1, player2Y + 1, col);
        }
        else if ((player2Dir == 5) || (player2Dir == 6) || (player2Dir == 7)) // Player moves to westwards
        {
          matrix.drawPixel( player2X + 0, player2Y - 1, headCol);
          matrix.drawPixel( player2X + 0, player2Y + 0, col);
          matrix.drawPixel( player2X - 1, player2Y + 1, col);
        }
        else if ((player2Dir == 1) || (player2Dir == 2) || (player2Dir == 3)) // Player moves to eastwards
        {
          matrix.drawPixel( player2X + 0, player2Y - 1, headCol);
          matrix.drawPixel( player2X + 0, player2Y + 0, col);
          matrix.drawPixel( player2X - 1, player2Y + 1, col);
          matrix.drawPixel( player2X + 1, player2Y + 0, col);
        }
        break;

      case 1:

        if ((player2Dir == 0) || (player2Dir == 4)) // Player moves northwards or southwards
        {
          matrix.drawPixel( player2X + 0, player2Y - 1, headCol);
          matrix.drawPixel( player2X + 0, player2Y + 0, col);
          matrix.drawPixel( player2X - 1, player2Y + 1, col);
        }
        else if ((player2Dir == 5) || (player2Dir == 6) || (player2Dir == 7)) // Player moves westwards
        {
          matrix.drawPixel( player2X + 0, player2Y - 1, headCol);
          matrix.drawPixel( player2X + 0, player2Y + 0, col);
          matrix.drawPixel( player2X + 1, player2Y + 1, col);
          matrix.drawPixel( player2X - 1, player2Y + 0, col);
        }
        else if ((player2Dir == 1) || (player2Dir == 2) || (player2Dir == 3)) // Player moves eastwards
        {
          matrix.drawPixel( player2X + 0, player2Y - 1, headCol);
          matrix.drawPixel( player2X + 0, player2Y + 0, col);
          matrix.drawPixel( player2X + 1, player2Y + 1, col);
        }
        break;

    } // switch
  }
  else // Player stands still: show the still image of the player's sprite
  {
    matrix.fillRect( player2X - 1, player2Y - 1, 3, 3, matrix.Color333(0, 0, 0)); // Black background

    matrix.drawPixel( player2X + 0, player2Y - 1, headCol);
    matrix.drawPixel( player2X + 0, player2Y + 0, col);
    matrix.drawPixel( player2X - 1, player2Y + 1, col);
    matrix.drawPixel( player2X + 1, player2Y + 1, col);
  }
}


