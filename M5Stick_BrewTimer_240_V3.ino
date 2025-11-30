

// M5 StickC Plus Brew Timer v3
// Written By u/David_R_Carroll C 2023
// 
//  Use Board ESP32 Arduino > M5Stick-C

#include <M5StickCPlus.h>
#include <M5Display.h>
#include  <sys/time.h>

#include "Free_Fonts.h"
#include "Brew_Background.h"
#include "HF_Icons.h"

TFT_eSprite Sprite_1 = TFT_eSprite(&M5.Lcd);

char      chBuffer[128]; 

const int buttonA = 37;
int lastButtonA = 1;
int currentButtonA;

const int buttonB = 39;
int lastButtonB = 1;
int currentButtonB;

int CurrentIcon = 0;
bool OverTime = false;
bool TestMode = false;

int Loops = 0;


void setup()  {
   // Serial init.
   Serial.begin(115200);
   while (!Serial)
   {
      Serial.print('.');
   }

   M5.begin();
   M5.Lcd.setRotation(3);

   Sprite_1.createSprite(240, 135);
   Sprite_1.setRotation(3);

   struct tm {
      int tm_sec;
      int tm_min;
      int tm_hour;
      int tm_mday;
      int tm_mon;
      int tm_year;
      int tm_wday;
      int tm_yday;
      int tm_isdst;
   };
}

void  loop() {

   struct  timeval tvTimeValue = {0, 0};
   struct tm* ptr;
   time_t t;
   int Tmp1;

   currentButtonA = digitalRead(buttonA);
   if (currentButtonA != lastButtonA) {
      if (currentButtonA == 0) { // Button was pressed so reset the timer.
         CurrentIcon = 0;                    // Set Icon to Happy Face
         Loops = 0;                          // Reset test counter 
         tvTimeValue = {0, 0};               // Reset time of day to 0:00
         settimeofday(& tvTimeValue, NULL);
         OverTime = false;                   // Reset OverTime flag
         TestMode = false;
      }
   }
   lastButtonA = currentButtonA;

   currentButtonB = digitalRead(buttonB);
   if (currentButtonB != lastButtonB) {
      if (currentButtonB == 0) {             // Button was pressed so reset the timer.
         CurrentIcon = 0;                    // Set Icon to Happy Face
         Loops = 0;                          // Reset test counter 
         tvTimeValue = {0, 0};               // Reset time of day to 0:00
         settimeofday(& tvTimeValue, NULL);
         OverTime = false;                   // Reset OverTime flag
         TestMode = true;                    // Turn on test mode (quickly advance by 5 min)
      }
   }
   lastButtonB = currentButtonB;

   Sprite_1.setTextColor(BLACK);

   t = time(NULL);
   ptr = localtime(&t);
   
   strftime(chBuffer, sizeof(chBuffer), "%H:%M",  ptr);
   // If the hour is from 01 to 09, remove the 0.
   if (chBuffer[0] == 48)  {
      for (Tmp1 = 0; Tmp1 <= 5; Tmp1++) {
         chBuffer[Tmp1] = chBuffer[Tmp1 + 1];
      }
   }

   if (chBuffer[0] >= 48 + 3)  { // At 3 or more hours change icon to straight face.
      CurrentIcon = 1;
   }
   if (chBuffer[0] >= 48 + 6)  { // At 6 or more hours change icon to bad face.
      CurrentIcon = 2;
   }

   // if time is >= 9:55, set OverTime = true. That will force display to always read 9:55+
   if (chBuffer[0] >= 48 + 9 && chBuffer[2] >= 48 + 5 && chBuffer[3] >= 48 + 5) {
      OverTime = true;
   }

   Sprite_1.pushImage(1, 1, 240, 135, Brew_Background[0] );

   if (OverTime)  {                                         // If true, always display 9:55+
      Sprite_1.setFreeFont(FSSB18);
      Sprite_1.pushImage(108, 24+22, 70, 70, HF_Icons[2] );
      Sprite_1.drawString("9:55", 15, 45+22, GFXFF);
      Sprite_1.drawString("+", 82, 40+22, GFXFF);
   } else {                                                // If false display the time of day
      Sprite_1.setFreeFont(FSSB24);
      Sprite_1.pushImage(108, 24+22, 70, 70, HF_Icons[CurrentIcon] );
      Sprite_1.drawString(chBuffer, 10, 40+22, GFXFF);
   }

   if (TestMode) { // Count quickly by 5 minutes for testing
      Loops++;           
      tvTimeValue = {Loops * 300, 0}; // Increment time of day by 5 min.
      settimeofday(& tvTimeValue, NULL);
      Sprite_1.setFreeFont(FSSB9);
      Sprite_1.drawString("Test", 125, 120, GFXFF);
      delay(100);
   }else{
      delay(100);
   }

   Sprite_1.pushSprite(0, 0);
}
