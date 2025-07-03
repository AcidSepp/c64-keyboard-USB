// Uncomment to debug using serial monitor at 115200 baud
#define DEBUG

#include <HID-Project.h>

#define KEYDELAY 30
#define KEY_COUNT 80

// TOP ROW
#define TAB_SCANCODE 71
#define HOME_SCANCODE 63
#define BACKSPACE_SCANCODE 0

// SECOND ROW
#define LEFT_CTRL_SCANCODE 72

// THIRD ROW
#define RUN_STOP_SCANCODE 77
#define RETURN_SCANCODE 1

// FOURTH ROW
#define C64_SCANCODE 75
#define LEFT_SHIFT_SCANCODE 17
#define SPACE_SCANCODE 74
#define RIGHT_SHIFT_SCANCODE 64
#define DOWN_ARROW_SCANCODE 7
#define RIGHT_ARROW_SCANCODE 2

// F-Keys
#define F1_SCANCODE 4
#define F3_SCANCODE 5
#define F5_SCANCODE 6
#define F7_SCANCODE 3

int debounceDelay = 5;
int RowPinMap[8] = { 9, 3, 4, 5, 6, 7, 8, 2 };
int ColPinMap[10] = { 10, 16, 14, 21, 18, 19, 20, 15, 1, 0 };

bool shifted = false;

int lastKeyState[KEY_COUNT];<
long lastDebounceTime[KEY_COUNT];
char directKeymap[KEY_COUNT];
char directKeymapShifted[KEY_COUNT];

void debug(String outstring) {
#ifdef DEBUG
  Serial.println(outstring);
#endif
}

void bootsetup() {

#ifdef DEBUG
  Serial.begin(115200);
#endif

  Keyboard.begin();
  Keyboard.releaseAll();

  for (int i = 0; i < KEY_COUNT; i++) {
    directKeymap[i] = NULL;
    directKeymapShifted[i] = NULL;
  }

  // TOP ROW
  directKeymap[70] = '1';
  directKeymapShifted[70] = '!';
  directKeymap[73] = '2';
  directKeymapShifted[73] = '"';
  directKeymap[10] = '3';
  directKeymapShifted[10] = '#';
  directKeymap[13] = '4';
  directKeymapShifted[13] = '$';
  directKeymap[20] = '5';
  directKeymapShifted[20] = '%';
  directKeymap[23] = '6';
  directKeymapShifted[23] = '&';
  directKeymap[30] = '7';
  directKeymapShifted[30] = '\'';
  directKeymap[33] = '8';
  directKeymapShifted[33] = '(';
  directKeymap[40] = '9';
  directKeymapShifted[40] = ')';
  directKeymap[43] = '0';
  directKeymapShifted[43] = '0';


  // SECOND ROW
  directKeymap[76] = 'q';
  directKeymapShifted[76] = 'Q';
  directKeymap[11] = 'w';
  directKeymapShifted[11] = 'W';
  directKeymap[16] = 'e';
  directKeymapShifted[16] = 'E';
  directKeymap[21] = 'r';
  directKeymapShifted[21] = 'R';
  directKeymap[26] = 't';
  directKeymapShifted[26] = 'T';
  directKeymap[31] = 'y';
  directKeymapShifted[31] = 'Y';
  directKeymap[36] = 'u';
  directKeymapShifted[36] = 'U';
  directKeymap[41] = 'i';
  directKeymapShifted[41] = 'I';
  directKeymap[46] = 'o';
  directKeymapShifted[46] = 'O';
  directKeymap[51] = 'p';
  directKeymapShifted[51] = 'P';
  directKeymap[56] = '@';
  directKeymap[61] = '*';
  // directKeymap[66] = KEY_UP_ARROW;
  //   TODO: RESTORE

  // THIRD ROW
  directKeymap[12] = 'a';
  directKeymapShifted[12] = 'A';
  directKeymap[15] = 's';
  directKeymapShifted[15] = 'S';
  directKeymap[22] = 'd';
  directKeymapShifted[22] = 'D';
  directKeymap[25] = 'f';
  directKeymapShifted[25] = 'F';
  directKeymap[32] = 'g';
  directKeymapShifted[32] = 'G';
  directKeymap[35] = 'h';
  directKeymapShifted[35] = 'H';
  directKeymap[42] = 'j';
  directKeymapShifted[42] = 'J';
  directKeymap[45] = 'k';
  directKeymapShifted[45] = 'K';
  directKeymap[52] = 'l';
  directKeymapShifted[52] = 'L';
  directKeymap[55] = ':';
  directKeymapShifted[55] = '(';
  directKeymap[62] = ';';
  directKeymapShifted[62] = ')';
  directKeymap[65] = '=';

  // BOTTOM ROW
  directKeymap[14] = 'z';
  directKeymapShifted[14] = 'Z';
  directKeymap[27] = 'x';
  directKeymapShifted[27] = 'X';
  directKeymap[24] = 'c';
  directKeymapShifted[24] = 'C';
  directKeymap[37] = 'v';
  directKeymapShifted[37] = 'V';
  directKeymap[34] = 'b';
  directKeymapShifted[34] = 'B';
  directKeymap[47] = 'n';
  directKeymapShifted[47] = 'N';
  directKeymap[44] = 'm';
  directKeymapShifted[44] = 'M';
  directKeymap[57] = ',';
  directKeymapShifted[57] = '<';
  directKeymap[54] = '.';
  directKeymapShifted[54] = '>';
  directKeymap[67] = '/';
  directKeymapShifted[67] = '?';
}


void setup() {

  bootsetup();
  for (int i = 0; i < 80; i++) lastKeyState[i] = false;
  for (int Row = 0; Row < 8; Row++) pinMode(RowPinMap[Row], INPUT_PULLUP);
  for (int Col = 0; Col < 10; Col++) pinMode(ColPinMap[Col], INPUT_PULLUP);
}

void specialKeyPress() {
}

void specialKeyRelease() {
}

void loop() {

  for (int Row = 0; Row < 8; Row++) {

    int RowPin = RowPinMap[Row];
    pinMode(RowPin, OUTPUT);
    digitalWrite(RowPin, LOW);

    for (int Col = 0; Col < 10; Col++) {

      int thisKey = Col + (Row * 10);
      bool isKeyDown = !digitalRead(ColPinMap[Col]);

      // Non-blocking delay
      if (millis() < lastDebounceTime[thisKey] + debounceDelay) continue;

      // Is the key currently down and wasn't before?
      if (isKeyDown && !lastKeyState[thisKey]) {

        // This is a C64 specific key, we need to map it to a char manually.
        if (shifted) {
          if (directKeymapShifted[thisKey] != NULL) {
            Keyboard.press(directKeymapShifted[thisKey]);
          }
        } else {
          if (directKeymap[thisKey] != NULL) {
            Keyboard.press(directKeymap[thisKey]);
          }
        }

        if (thisKey == LEFT_SHIFT_SCANCODE || thisKey == RIGHT_SHIFT_SCANCODE) {
          shifted = true;
        }

        // FIRST ROW
        if (thisKey == TAB_SCANCODE) {
          Keyboard.press(KEY_TAB);
          debug("<-: Press Tab");
        }
        if (thisKey == HOME_SCANCODE) {
          Keyboard.press(KEY_HOME);
          debug("CRL HOME: Press Home");
        }
        if (thisKey == BACKSPACE_SCANCODE) {
          Keyboard.press(KEY_BACKSPACE);
          debug("INST DEL: Press Backspace");
        }
        if (thisKey == LEFT_CTRL_SCANCODE) {
          Keyboard.press(KEY_LEFT_CTRL);
          debug("left CTRL: Press Left CRTL");
        }

        // THIRD ROW
        if (thisKey == RUN_STOP_SCANCODE) {
          Keyboard.press(KEY_ESC);
          debug("RUN STOP: Press ESC");
        }
        if (thisKey == RETURN_SCANCODE) {
          Keyboard.press(KEY_RETURN);
          debug("RETURN: Press RETURN");
        }


        // FOURTH ROW
        if (thisKey == C64_SCANCODE) {
          Keyboard.press(KEY_LEFT_WINDOWS);
          debug("C64: Press Left Windows");
        }
        if (thisKey == SPACE_SCANCODE) {
          Keyboard.press(KEY_SPACE);
          debug("Press SPACE");
        }
        if (thisKey == DOWN_ARROW_SCANCODE) {
          if (shifted) {
            Keyboard.press(KEY_UP_ARROW);
            debug("Press UP");
          } else {
            Keyboard.press(KEY_DOWN_ARROW);
            debug("Press DOWN");
          }
        }
        if (thisKey == RIGHT_ARROW_SCANCODE) {
          if (shifted) {
            Keyboard.press(KEY_LEFT_ARROW);
            debug("Press LEFT");
          } else {
            Keyboard.press(KEY_RIGHT_ARROW);
            debug("Press RIGHT");
          }
        }

        // F-KEYS
        if (thisKey == F1_SCANCODE) {
          if (shifted) {
            Keyboard.press(KEY_F2);
            debug("Press F2");
          } else {
            Keyboard.press(KEY_F1);
            debug("Press F1");
          }
        }
        if (thisKey == F3_SCANCODE) {
          if (shifted) {
            Keyboard.press(KEY_F4);
            debug("Press F4");
          } else {
            Keyboard.press(KEY_F3);
            debug("Press F3");
          }
        }
        if (thisKey == F5_SCANCODE) {
          if (shifted) {
            Keyboard.press(KEY_F6);
            debug("Press F6");
          } else {
            Keyboard.press(KEY_F5);
            debug("Press F5");
          }
        }
        if (thisKey == F7_SCANCODE) {
          if (shifted) {
            Keyboard.press(KEY_F8);
            debug("Press F8");
          } else {
            Keyboard.press(KEY_F7);
            debug("Press F7");
          }
        }

        lastKeyState[thisKey] = true;
        debug("Press: " + String(thisKey));
      }

      // The key is NOT down but WAS before
      if (!isKeyDown && lastKeyState[thisKey]) {

        // This is a C64 specific key, we need to map it to a char manually.
        if (directKeymap[thisKey] != NULL) {
          Keyboard.release(directKeymap[thisKey]);
          Keyboard.release(directKeymapShifted[thisKey]);
        }

        if (thisKey == LEFT_SHIFT_SCANCODE || thisKey == RIGHT_SHIFT_SCANCODE) {
          shifted = false;
        }

        // FIRST ROW
        if (thisKey == TAB_SCANCODE) {
          Keyboard.release(KEY_TAB);
          debug("<-: Release Tab");
        }
        if (thisKey == HOME_SCANCODE) {
          Keyboard.release(KEY_HOME);
          debug("CRL HOME: Release Home");
        }
        if (thisKey == BACKSPACE_SCANCODE) {
          Keyboard.release(KEY_BACKSPACE);
          debug("INST DEL: Release Backspace");
        }
        if (thisKey == LEFT_CTRL_SCANCODE) {
          Keyboard.release(KEY_LEFT_CTRL);
          debug("left CTRL: Release Left CRTL");
        }

        // THIRD ROW
        if (thisKey == RUN_STOP_SCANCODE) {
          Keyboard.release(KEY_ESC);
          debug("RUN STOP: Release ESC");
        }
        if (thisKey == RETURN_SCANCODE) {
          Keyboard.release(KEY_RETURN);
          debug("RETURN: Release Return");
        }


        // FOURTH ROW
        if (thisKey == C64_SCANCODE) {
          Keyboard.release(KEY_LEFT_WINDOWS);
          debug("C64: Release Left Windows");
        }
        if (thisKey == SPACE_SCANCODE) {
          Keyboard.release(KEY_SPACE);
          debug("Release SPACE");
        }
        if (thisKey == DOWN_ARROW_SCANCODE) {
            Keyboard.release(KEY_UP_ARROW);
            debug("Release UP");
            Keyboard.release(KEY_DOWN_ARROW);
            debug("Release DOWN");
        }
        if (thisKey == RIGHT_ARROW_SCANCODE) {
            Keyboard.release(KEY_LEFT_ARROW);
            debug("Release LEFT");
            Keyboard.release(KEY_RIGHT_ARROW);
            debug("Release RIGHT");
        }

        // F-KEYS
        if (thisKey == F1_SCANCODE) {
            Keyboard.release(KEY_F2);
            debug("Release F2");
            Keyboard.release(KEY_F1);
            debug("Release F1");
        }
        if (thisKey == F3_SCANCODE) {
            Keyboard.release(KEY_F4);
            debug("Release F4");
            Keyboard.release(KEY_F3);
            debug("Release F3");
        }
        if (thisKey == F5_SCANCODE) {
            Keyboard.release(KEY_F6);
            debug("Release F6");
            Keyboard.release(KEY_F5);
            debug("Release F5");
        }
        if (thisKey == F7_SCANCODE) {
            Keyboard.release(KEY_F8);
            debug("Release F8");
            Keyboard.release(KEY_F7);
            debug("Release F7");
        }

        lastKeyState[thisKey] = false;
        debug("Release: " + String(thisKey));
      }

      lastDebounceTime[thisKey] = millis();
    }

    digitalWrite(RowPin, HIGH);
    delay(1);
    pinMode(RowPin, INPUT_PULLUP);
  }
}
