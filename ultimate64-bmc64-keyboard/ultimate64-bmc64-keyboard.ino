// Uncomment to debug using serial monitor at 115200 baud
#define DEBUG

#include <HID-Project.h>

/* =======================================================================

 Commodore 64 USB HID for Pro Micro by Chris Garrett @makerhacks   
 
 Inspired by and based on code by DJ Sures (Synthiam.com)          


 ----------------------
 CONNECTIONS
 ----------------------

     C64   |  Arduino
    ==================
       20     2 - SDA
       19     3 - SCL
       18     4 - A6
       17     5
       16     6 - A7
       15     7 -
       14     8 - A8
       13     9 - A9
       12     10 - A10
       11     16 - MOSI
       10     14 - MISO
       9      15 - SCLK
       8      18 - A0
       7      19 - A1
       6      20 - A2
       5      21 - A3
       4      -N/C-
       3      1
       2      -N/C-
       1      0
    ==================

    

        ------------------------------------
        Commodore 64 keyboard matrix layout                                                                
        ------------------------------------
                                                                
        Del      Return   curl/r   F7       F1       F3       F5       curup
        3        W        A        4        Z        S        E        lSh
        5        R        D        6        C        F        T        X
        7        Y        G        8        B        H        U        V
        9        I        J        0        M        K        O        N
        +        P        L        –        .        :        @        ,
        £        *        ;        Home     rshift   =        ↑        /
        1        ←        Ctrl     2        Space    C=       Q        Stop
       

*/

#define KEYDELAY 30

int  thisKey;
bool isKeyDown;
int lastKeyState[80];
long lastDebounceTime[80];
int debounceDelay = 100;
int RowPinMap[8] = {9, 3, 4, 5, 6, 7, 8, 2};
int ColPinMap[10] = {10, 16, 14, 21, 18, 19, 20, 15, 1, 0};

KeyboardKeycode keymap[80];
  
char printchar;

void SerialPrintLine(String outstring)
{
  #ifdef DEBUG
  Serial.println(outstring);
  #endif
}

void SerialPrint(String outstring)
{
  #ifdef DEBUG
  Serial.print(outstring);
  #endif
}


void SerialWrite(char outstring)
{
  #ifdef DEBUG
  Serial.write(outstring);
  #endif
}


void bootsetup() {

  #ifdef DEBUG
  Serial.begin(115200);
  #endif
  
  Keyboard.begin();
  Keyboard.releaseAll();

  for (int i = 0; i < 80; i++) {
    keymap[i] = NULL;
  }

  // TOP ROW
  keymap[71] = KEY_LEFT_ARROW;
  keymap[70] = KEY_1; 
  keymap[73] = KEY_2;
  keymap[10] = KEY_3;
  keymap[13] = KEY_4;
  keymap[20] = KEY_5;
  keymap[23] = KEY_6;
  keymap[30] = KEY_7;
  keymap[33] = KEY_8;
  keymap[40] = KEY_9;
  keymap[43] = KEY_0;
  //keymap[50] = '+';
  //keymap[53] = '-';
  keymap[60] = KEY_BACKSLASH;
  keymap[63] = KEY_HOME;
  keymap[0] = KEY_BACKSPACE;


  // SECOND ROW
  keymap[72] = KEY_LEFT_CTRL;
  keymap[76] = KEY_Q;
  keymap[11] = KEY_W;
  keymap[16] = KEY_E;
  keymap[21] = KEY_R;
  keymap[26] = KEY_T;
  keymap[31] = KEY_Y;
  keymap[36] = KEY_U;
  keymap[41] = KEY_I;
  keymap[46] = KEY_O;
  keymap[51] = KEY_P;
//   keymap[56] = '@';
//   keymap[61] = '*';
  keymap[66] = KEY_UP_ARROW;
//   TODO: RESTORE

  // THIRD ROW

  keymap[77] = KEY_ESC;
  keymap[17] = KEY_LEFT_SHIFT;
  keymap[12] = KEY_A;
  keymap[15] = KEY_S;
  keymap[22] = KEY_D;
  keymap[25] = KEY_F;
  keymap[32] = KEY_G;
  keymap[35] = KEY_H;
  keymap[42] = KEY_J;
  keymap[45] = KEY_K;
  keymap[52] = KEY_L;
  keymap[55] = ':';
  keymap[62] = ';';
  keymap[65] = '=';
  keymap[1] = KEY_ENTER;

  // BOTTOM ROW

  keymap[75] = KEY_LEFT_WINDOWS;
  keymap[14] = KEY_Z;
  keymap[27] = KEY_X;
  keymap[24] = KEY_C;
  keymap[37] = KEY_V;
  keymap[34] = KEY_B;
  keymap[47] = KEY_N;
  keymap[44] = KEY_M;
  keymap[57] = ',';
  keymap[54] = '.';
  keymap[67] = KEY_SLASH;
  keymap[64] = KEY_RIGHT_SHIFT;
  keymap[7] = KEY_DOWN_ARROW;
  keymap[2] = KEY_RIGHT_ARROW;

  // SPACE AND F1-F8

  keymap[74] = KEY_SPACE;
  keymap[4] = KEY_F1;
  keymap[5] = KEY_F3;
  keymap[6] = KEY_F5;
  keymap[3] = KEY_F7;

}  


void setup() {

  bootsetup();
  for (int i = 0; i < 80; i++) lastKeyState[i] = false;
  for (int Row = 0; Row < 8; Row++) pinMode(RowPinMap[Row], INPUT_PULLUP);
  for (int Col = 0; Col < 10; Col++) pinMode(ColPinMap[Col], INPUT_PULLUP);

}

void loop() {

  thisKey   = NULL;
  isKeyDown = NULL;

  for (int Row = 0; Row < 8; Row++) {

    int RowPin = RowPinMap[Row];
    pinMode(RowPin, OUTPUT);
    digitalWrite(RowPin, LOW);

    for (int Col = 0; Col < 10; Col++) {

      thisKey    = Col + (Row * 10);
      isKeyDown = !digitalRead(ColPinMap[Col]);

      // Non-blocking delay
      if (millis() < lastDebounceTime[thisKey] + debounceDelay) continue;

      // Is the key currently down and was before too?
      if (isKeyDown && lastKeyState[thisKey]) {
       
        // if(shifted()) SerialPrint(" SHIFT ");

      }
      
      // Is the key currently down and wasn't before?
      if (isKeyDown && !lastKeyState[thisKey]) {

        Keyboard.press(keymap[thisKey]);

        // Toggle the key state
        lastKeyState[thisKey] = true;

        SerialPrint("Press: ");
        SerialPrint(String(thisKey));
        SerialPrint("('");
        SerialWrite(keymap[thisKey]);               
        SerialPrint("')");
        SerialPrint("\n\r");
      }

      // The key is NOT down but WAS before
      if (!isKeyDown && lastKeyState[thisKey]) {
        
        Keyboard.release(keymap[thisKey]);

        // Toggle the key state
        lastKeyState[thisKey] = false;

        SerialPrint("Release: ");
        SerialPrint(String(thisKey));
        SerialPrint("('");
        SerialWrite(keymap[thisKey]);               
        SerialPrint("')");
        SerialPrint("\n\r");
      }
    }

    lastDebounceTime[thisKey] = millis();
    digitalWrite(RowPin, HIGH);
    delay(1);
    pinMode(RowPin, INPUT_PULLUP);
  }
}
