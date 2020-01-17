// Self Test
// for the LEDmePlay
//
// www.mithotronic.de
//
// Version: 1.0.0
// Author: Michael Rosskopf (2017)
// 
// Thanks to Thomas Laubach! 
// 
// Release Notes:
// V1.0.0: First release

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

// Joystick 2 (2 player mode not supported)
int buttonU2 = 31;
int buttonD2 = 33;
int buttonL2 = 35;
int buttonR2 = 37;
int buttonFire2 = 39;
int analogX2 = 10;
int analogY2 = 11;

// Other buttons
int buttonReset = 42;
int buttonPause = 43;

byte joy1VisualX = 15;
byte joy1VisualY = 15;
byte joy2VisualX = 15;
byte joy2VisualY = 15;

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
  int counter = 0;
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
    counter++;
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
    while(j < 250 && counter < 18);
  }
  while(counter < 18);
  delay(200);
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
}

void clearScreen()
{
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 0));
}

void colorChannelTest()
{
  clearScreen();
  matrix.setTextColor(matrix.Color333(3, 3, 0));
  matrix.setCursor(4, 8);
  matrix.println("Test");
  matrix.setCursor(1, 16);
  matrix.println("Color");
  delay(4000);

  clearScreen();
  matrix.setTextColor(matrix.Color333(3, 0, 0));
  matrix.setCursor(1, 12);
  matrix.println(" Red");
  delay(2000);
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(1, 0, 0));
  delay(2000);
  
  clearScreen();
  matrix.setTextColor(matrix.Color333(0, 3, 0));
  matrix.setCursor(1, 12);
  matrix.println("Green");
  delay(2000);
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 1, 0));
  delay(2000);

  clearScreen();
  matrix.setTextColor(matrix.Color333(0, 0, 3));
  matrix.setCursor(4, 12);
  matrix.println("Blue");
  delay(2000);
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 0, 1));
  delay(2000);

  clearScreen();
}

void paletteTest()
{
  byte x = 0;
  byte y = 0;
  clearScreen();
  for(byte b = 0; b < 8; b++)
  {
    for(byte g = 0; g < 8; g++)
    {
      for(byte r = 0; r < 8; r++)
      {
        matrix.drawPixel(x, y, matrix.Color444(r, g, b));
        matrix.drawPixel(x, y + 1, matrix.Color444(r, g, b));
        x++;
        if(x == 32)
        {
          x = 0;
          y = y + 2;
        }
        delay(25);
      }    
    }    
  }
  delay(4000);  
  clearScreen();
}

void soundTest()
{
  clearScreen();
  matrix.setTextColor(matrix.Color333(3, 3, 0));
  matrix.setCursor(4, 8);
  matrix.println("Test");
  matrix.setCursor(1, 16);
  matrix.println("Sound");
  delay(2000);

  for(int i = 50; i < 2000; i = i + 10)
  {
    tone(audio, i, 18);
    delay(20);
  }

  delay(1000);

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

  clearScreen();
}

void joystickTest()
{
  clearScreen();
  matrix.setTextColor(matrix.Color333(3, 3, 0));
  matrix.setCursor(1, 0);
  matrix.println("Move");
  matrix.setCursor(1, 8);
  matrix.println("Joys");
  matrix.setCursor(1, 16);
  matrix.println("Press");
  matrix.setCursor(1, 24);
  matrix.println("Bttns");
  delay(2000);
  clearScreen();

  boolean fireAHasBeenUsed = false; // Leave test and perform other tests again
  
  do
  {
    // Draw corners
    matrix.drawLine(0, 0, 3, 0, matrix.Color333(1, 1, 0));
    matrix.drawLine(28, 0, 31, 0, matrix.Color333(1, 1, 0));
    matrix.drawLine(31, 0, 31, 3, matrix.Color333(1, 1, 0));
    matrix.drawLine(31, 28, 31, 31, matrix.Color333(1, 1, 0));
    matrix.drawLine(31, 31, 28, 31, matrix.Color333(1, 1, 0));
    matrix.drawLine(3, 31, 0, 31, matrix.Color333(1, 1, 0));
    matrix.drawLine(0, 31, 0, 28, matrix.Color333(1, 1, 0));
    matrix.drawLine(0, 3, 0, 0, matrix.Color333(1, 1, 0));
    
    // Buttons

    // Reset
    if(digitalRead(buttonReset) == LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(3, 3, 0));
      matrix.setCursor(1, 12);
      matrix.println("Reset");
      delay(2000);
      clearScreen();
    }
    
    // Pause
    if(digitalRead(buttonPause) == LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(3, 3, 0));
      matrix.setCursor(1, 12);
      matrix.println("Pause");
      delay(2000);
      clearScreen();
    }
    
    
    // Joysick 1
    
    // Joystick 1 Up
    if(digitalRead(buttonU1) == LOW && digitalRead(buttonD1) != LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(0, 3, 0));
      matrix.setCursor(1, 8);
      matrix.println("Joy 1");
      matrix.setCursor(10, 16);
      matrix.println("Up");
      delay(2000);
      clearScreen();
    }
    
    // Joystick 1 Down
    if(digitalRead(buttonU1) != LOW && digitalRead(buttonD1) == LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(0, 3, 0));
      matrix.setCursor(1, 8);
      matrix.println("Joy 1");
      matrix.setCursor(4, 16);
      matrix.println("Down");
      delay(2000);
      clearScreen();
    }

    // Joystick 1 Left
    if(digitalRead(buttonL1) == LOW && digitalRead(buttonR1) != LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(0, 3, 0));
      matrix.setCursor(1, 8);
      matrix.println("Joy 1");
      matrix.setCursor(4, 16);
      matrix.println("Left");
      delay(2000);
      clearScreen();
    }
    
    // Joystick 1 Right
    if(digitalRead(buttonL1) != LOW && digitalRead(buttonR1) == LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(0, 3, 0));
      matrix.setCursor(1, 8);
      matrix.println("Joy 1");
      matrix.setCursor(1, 16);
      matrix.println("Right");
      delay(2000);
      clearScreen();
    }

    // Joystick 1 Fire A (leave mode and perform other tests again)
    if(digitalRead(buttonFire1) == LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(0, 3, 0));
      matrix.setCursor(1, 8);
      matrix.println("Joy 1");
      matrix.setCursor(1, 16);
      matrix.println("FireA");
      delay(2000);
      clearScreen();
      fireAHasBeenUsed = true;
    }

    // Joystick 1 Fire B      
    if(digitalRead(buttonU1) == LOW && digitalRead(buttonD1) == LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(0, 3, 0));
      matrix.setCursor(1, 8);
      matrix.println("Joy 1");
      matrix.setCursor(1, 16);
      matrix.println("FireB");
      delay(2000);
      clearScreen();
    }

    // Joystick 1 Fire C
    if(digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(0, 3, 0));
      matrix.setCursor(1, 8);
      matrix.println("Joy 1");
      matrix.setCursor(1, 16);
      matrix.println("FireC");
      delay(2000);
      clearScreen();
    }

    // Joystick 1 Analog X/Y
    if(!fireAHasBeenUsed)
    {
      matrix.drawRect(joy1VisualX, joy1VisualY, 2, 2, matrix.Color333(0, 0, 0));
      joy1VisualX = 32 - (analogRead(analogX1) / 32) - 1;
      joy1VisualY = 32 - (analogRead(analogY1) / 32) - 2;
      matrix.drawRect(joy1VisualX, joy1VisualY, 2, 2, matrix.Color333(0, 3, 0));
    }


    // Joysick 2
    
    // Joystick 2 Up
    if(digitalRead(buttonU2) == LOW && digitalRead(buttonD2) != LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(0, 0, 3));
      matrix.setCursor(1, 8);
      matrix.println("Joy 2");
      matrix.setCursor(10, 16);
      matrix.println("Up");
      delay(2000);
      clearScreen();
    }
    
    // Joystick 2 Down
    if(digitalRead(buttonU2) != LOW && digitalRead(buttonD2) == LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(0, 0, 3));
      matrix.setCursor(1, 8);
      matrix.println("Joy 2");
      matrix.setCursor(4, 16);
      matrix.println("Down");
      delay(2000);
      clearScreen();
    }

    // Joystick 2 Left
    if(digitalRead(buttonL2) == LOW && digitalRead(buttonR2) != LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(0, 0, 3));
      matrix.setCursor(1, 8);
      matrix.println("Joy 2");
      matrix.setCursor(4, 16);
      matrix.println("Left");
      delay(2000);
      clearScreen();
    }
    
    // Joystick 2 Right
    if(digitalRead(buttonL2) != LOW && digitalRead(buttonR2) == LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(0, 0, 3));
      matrix.setCursor(1, 8);
      matrix.println("Joy 2");
      matrix.setCursor(1, 16);
      matrix.println("Right");
      delay(2000);
      clearScreen();
    }

    // Joystick 2 Fire A (leave mode and perform other tests again)
    if(digitalRead(buttonFire2) == LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(0, 0, 3));
      matrix.setCursor(1, 8);
      matrix.println("Joy 2");
      matrix.setCursor(1, 16);
      matrix.println("FireA");
      delay(2000);
      clearScreen();
      fireAHasBeenUsed = true;
    }

    // Joystick 2 Fire B      
    if(digitalRead(buttonU2) == LOW && digitalRead(buttonD2) == LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(0, 0, 3));
      matrix.setCursor(1, 8);
      matrix.println("Joy 2");
      matrix.setCursor(1, 16);
      matrix.println("FireB");
      delay(2000);
      clearScreen();
    }

    // Joystick 2 Fire C
    if(digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW)
    {
      clearScreen();
      matrix.setTextColor(matrix.Color333(0, 0, 3));
      matrix.setCursor(1, 8);
      matrix.println("Joy 2");
      matrix.setCursor(1, 16);
      matrix.println("FireC");
      delay(2000);
      clearScreen();
    }

    // Joystick 2 Analog X/Y
    if(!fireAHasBeenUsed)
    {
      matrix.drawRect(joy2VisualX, joy2VisualY, 2, 2, matrix.Color333(0, 0, 0));
      joy2VisualX = 32 - (analogRead(analogX2) / 32) - 1;
      joy2VisualY = 32 - (analogRead(analogY2) / 32) - 2;
      matrix.drawRect(joy2VisualX, joy2VisualY, 2, 2, matrix.Color333(0, 0, 3));
    }
  }
  while(!fireAHasBeenUsed);

  clearScreen();
  matrix.setTextColor(matrix.Color333(3, 3, 0));
  matrix.setCursor(1, 8);
  matrix.println("Again");
  matrix.setCursor(1, 16);
  matrix.println(" ... ");
  delay(2000);
  clearScreen();
}

void loop() {

  // Check whether the color channels are wired correctly
  colorChannelTest();

  // Show the complete palette
  paletteTest();

  // Check the sound
  soundTest();

  // Check joystick wiring
  joystickTest();
}
